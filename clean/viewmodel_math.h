#pragma once
#include <math.h>
#include <float.h>

// UE3 uses row-vector transforms. The optical correction retains the camera's
// forward axis and expands its two perpendicular axes equally. Applying it
// around the camera (including translation) preserves native weapon framing.
namespace viewmodel_math {
struct Vec {float x,y,z;};
struct Matrix {float m[4][4];};
struct Bounds {Vec origin,extent;float radius;};
struct Pose {Vec origin;int pitch,yaw,roll;};
struct Affine {double a[3][3],camera[3],factor;};
inline bool finite(double value) {return value==value && value<=DBL_MAX && value>=-DBL_MAX;}
inline bool scalar(double value) {return finite(value) && fabs(value)<1e12;}
inline bool affineForward(const Pose &pose,const double forward[3],float native,float world,Affine &out) {
    if(!scalar(pose.origin.x) || !scalar(pose.origin.y) || !scalar(pose.origin.z)
        || !finite(native) || !finite(world) || native<=1 || native>=179 || world<=1 || world>=179)return false;
    double norm=0;
    for(int i=0;i<3;++i){if(!scalar(forward[i]))return false;norm+=forward[i]*forward[i];}
    if(norm<1e-12)return false;
    norm=sqrt(norm);double direction[3];for(int i=0;i<3;++i)direction[i]=forward[i]/norm;
    const double half=3.14159265358979323846/360.0;
    const double s=tan(world*half)/tan(native*half);
    if(!finite(s) || s<0.2 || s>5)return false;
    Affine result={};result.factor=s;
    result.camera[0]=pose.origin.x;result.camera[1]=pose.origin.y;result.camera[2]=pose.origin.z;
    for(int i=0;i<3;++i)for(int j=0;j<3;++j)
        result.a[i][j]=(i==j?s:0)+(1-s)*direction[i]*direction[j];
    out=result;return true;
}
inline bool affine(const Pose &pose,float native,float world,Affine &out) {
    const double unit=6.28318530717958647692/65536.0;
    const double pitch=(pose.pitch&65535)*unit,yaw=(pose.yaw&65535)*unit;
    const double cp=cos(pitch),forward[]={cp*cos(yaw),cp*sin(yaw),sin(pitch)};
    return affineForward(pose,forward,native,world,out);
}
inline bool affineDrift(const Pose &pose,const Matrix &drift,float native,float world,Affine &out) {
    for(int i=0;i<4;++i)for(int j=0;j<4;++j)if(!scalar(drift.m[i][j]))return false;
    for(int i=0;i<3;++i) {
        if(fabs(drift.m[i][3])>0.001)return false;
        for(int j=0;j<3;++j) {
            double dot=0;for(int k=0;k<3;++k)dot+=drift.m[i][k]*drift.m[j][k];
            if(fabs(dot-(i==j?1:0))>0.002)return false;
        }
    }
    if(fabs(drift.m[3][3]-1)>0.001)return false;
    // Native GetPlayerViewPoint composes rotation with this drift matrix, but
    // leaves camera location unchanged. Its translation must not be applied.
    const double unit=6.28318530717958647692/65536.0;
    const double pitch=(pose.pitch&65535)*unit,yaw=(pose.yaw&65535)*unit;
    const double cp=cos(pitch),forward[]={cp*cos(yaw),cp*sin(yaw),sin(pitch)};
    double finalForward[3]={};
    for(int j=0;j<3;++j)for(int k=0;k<3;++k)finalForward[j]+=forward[k]*drift.m[k][j];
    return affineForward(pose,finalForward,native,world,out);
}
inline bool floatValue(double value,float &out) {
    if(!finite(value) || fabs(value)>FLT_MAX)return false;
    out=(float)value;return finite(out);
}
inline bool transform(Matrix &matrix,Bounds &bounds,float &determinant,const Affine &affine) {
    // Calculate into private copies. A rejected value leaves the whole native
    // packet untouched; partial matrix/bounds writes could invalidate culling.
    Matrix changed={};Bounds enlarged={};float det=0;
    if(!finite(determinant) || fabs(determinant)<1e-12 || !finite(bounds.radius) || bounds.radius<0
        || !finite(affine.factor) || affine.factor<0.2 || affine.factor>5)return false;
    for(int i=0;i<4;++i)for(int j=0;j<4;++j)if(!scalar(matrix.m[i][j]))return false;
    if(fabs(matrix.m[0][3])>1e-5 || fabs(matrix.m[1][3])>1e-5 || fabs(matrix.m[2][3])>1e-5
        || fabs(matrix.m[3][3]-1)>1e-5)return false;
    const float origin[]={bounds.origin.x,bounds.origin.y,bounds.origin.z};
    const float extent[]={bounds.extent.x,bounds.extent.y,bounds.extent.z};
    float newOrigin[3]={},newExtent[3]={};
    for(int i=0;i<3;++i)if(!scalar(origin[i]) || !scalar(extent[i]) || extent[i]<0 || !scalar(affine.camera[i]))return false;
    for(int i=0;i<3;++i)for(int j=0;j<3;++j)if(!finite(affine.a[i][j]))return false;
    changed=matrix;
    for(int i=0;i<3;++i)for(int j=0;j<3;++j) {
        double value=0;for(int k=0;k<3;++k)value+=matrix.m[i][k]*affine.a[k][j];
        if(!floatValue(value,changed.m[i][j]))return false;
    }
    for(int j=0;j<3;++j) {
        double translation=affine.camera[j],center=affine.camera[j],radius=0;
        for(int k=0;k<3;++k) {
            translation+=(matrix.m[3][k]-affine.camera[k])*affine.a[k][j];
            center+=(origin[k]-affine.camera[k])*affine.a[k][j];
            radius+=extent[k]*fabs(affine.a[k][j]);
        }
        if(!floatValue(translation,changed.m[3][j]) || !floatValue(center,newOrigin[j])
            || !floatValue(radius,newExtent[j]))return false;
    }
    enlarged.origin={newOrigin[0],newOrigin[1],newOrigin[2]};
    enlarged.extent={newExtent[0],newExtent[1],newExtent[2]};
    if(!floatValue(bounds.radius*(affine.factor>1?affine.factor:1),enlarged.radius)
        || !floatValue(determinant*affine.factor*affine.factor,det) || fabs(det)<1e-12)return false;
    matrix=changed;bounds=enlarged;determinant=det;return true;
}
}
