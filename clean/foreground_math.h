#pragma once
#include <math.h>
namespace foreground_math {
struct Matrix {float v[4][4];};
inline bool finite(double x){return x==x && x>-1e30 && x<1e30;}
inline bool valid(const Matrix &m){for(auto &r:m.v)for(float x:r)if(!finite(x))return false;return true;}
inline Matrix multiply(const Matrix &a,const Matrix &b){
    Matrix o={};
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            double x=0;
            for(int k=0;k<4;k++)x+=(double)a.v[i][k]*b.v[k][j];
            o.v[i][j]=(float)x;
        }
    }
    return o;
}
inline bool inverse(const Matrix &m,Matrix &out){
    if(!valid(m))return false;double a[4][8]={};
    for(int i=0;i<4;i++)for(int j=0;j<4;j++){a[i][j]=m.v[i][j];a[i][j+4]=i==j;}
    for(int col=0;col<4;col++){
        int row=col;for(int i=col+1;i<4;i++)if(fabs(a[i][col])>fabs(a[row][col]))row=i;
        if(fabs(a[row][col])<1e-12)return false;
        if(row!=col)for(int j=0;j<8;j++){double x=a[col][j];a[col][j]=a[row][j];a[row][j]=x;}
        double divisor=a[col][col];for(int j=0;j<8;j++)a[col][j]/=divisor;
        for(int i=0;i<4;i++)if(i!=col){double x=a[i][col];for(int j=0;j<8;j++)a[i][j]-=x*a[col][j];}
    }
    Matrix r={};for(int i=0;i<4;i++)for(int j=0;j<4;j++)r.v[i][j]=(float)a[i][j+4];
    if(!valid(r))return false;out=r;return true;
}
struct Fields {Matrix projection,translatedVP,inverseTranslatedVP,VP,inverseProjection,inverseVP,screenToWorld;};
inline bool adapt(const Matrix &view,const Matrix &translatedView,const Fields &raw,double ratio,Fields &out){
    if(!valid(view)||!valid(translatedView)||!valid(raw.projection)||!finite(ratio)||ratio<0.2||ratio>5)return false;
    // Keep depth, off-axis center and temporal jitter authored by the renderer.
    if(fabs(raw.projection.v[3][3])>1e-5||fabs(raw.projection.v[2][3]-1)>1e-5
        ||raw.projection.v[0][0]<=0||raw.projection.v[1][1]<=0)return false;
    Fields r=raw;r.projection.v[0][0]*=(float)ratio;r.projection.v[1][1]*=(float)ratio;
    r.VP=multiply(view,r.projection);r.translatedVP=multiply(translatedView,r.projection);
    if(!inverse(r.projection,r.inverseProjection)||!inverse(r.VP,r.inverseVP)
        ||!inverse(r.translatedVP,r.inverseTranslatedVP))return false;
    Matrix screen={};screen.v[0][0]=screen.v[1][1]=1;
    screen.v[2][2]=r.projection.v[2][2];screen.v[2][3]=1;screen.v[3][2]=r.projection.v[3][2];
    r.screenToWorld=multiply(screen,r.inverseTranslatedVP);
    if(!valid(r.VP)||!valid(r.translatedVP)||!valid(r.screenToWorld))return false;
    out=r;return true;
}
}
