#pragma once
#include "ui_config.h"
#include <stdio.h>

inline bool readNativeSliderPreference(const wchar_t *path,int &percent) {
    wchar_t text[32],*end=nullptr;
    DWORD n=GetPrivateProfileStringW(L"Display",L"NativeFOVSlider",L"-1",text,32,path);
    if(!n || n>=31)return false;
    errno=0;long value=wcstol(text,&end,10);
    if(errno || end==text || *end || value< -1 || value>100)return false;
    percent=(int)value;return true;
}

// Preserve the old angle preference as a range request against the observed
// native 75-degree reference. The game still owns its actual base and slider.
inline double nativeFOVPercent(int preference) {
    return preference>=80 && preference<=130 ? (preference/75.0-1.0)*100.0 : -1;
}
inline bool nativeFOVValue(const std::string &s,double maximum=100) {
    if(s.empty())return false;
    char *end=nullptr;errno=0;double n=strtod(s.c_str(),&end);
    return end!=s.c_str() && !*end && !errno && std::isfinite(n) && n>=0 && n<=maximum;
}
inline bool nativeFOVToken(const std::string &text,std::string &value,size_t &begin,size_t &length,const char *option="maxuserfovoffsetpercent",double maximum=100) {
    bool section=false;unsigned sections=0,keys=0;
    for(size_t pos=0;pos<text.size();) {
        size_t end=text.find('\n',pos);if(end==std::string::npos)end=text.size();
        std::string line=text.substr(pos,end-pos),plain=trim(line);
        if(!pos && plain.compare(0,3,"\xef\xbb\xbf")==0)plain.erase(0,3);
        if(!plain.empty() && plain[0]=='[') {
            section=lower(plain)=="[xcore.xuseroptionsmanager]";
            if(section && ++sections>1)return false;
        } else if(section && !plain.empty() && plain[0]!=';' && plain[0]!='#') {
            size_t eq=line.find('=');
            std::string key=lower(trim(line.substr(0,eq))),bare=key;
            if(!bare.empty() && strchr("+-.!",bare[0]))bare.erase(0,1);
            if(bare==option) {
                if(eq==std::string::npos || key!=bare || ++keys>1)return false;
                size_t a=line.find_first_not_of(" \t\r",eq+1);
                if(a==std::string::npos)return false;
                size_t b=line.find_first_of(" \t\r;#",a);if(b==std::string::npos)b=line.size();
                if(!nativeFOVValue(line.substr(a,b-a),maximum))return false;
                std::string rest=trim(line.substr(b));
                if(!rest.empty() && rest[0]!=';' && rest[0]!='#')return false;
                begin=pos+a;length=b-a;value=line.substr(a,b-a);
            }
        }
        pos=end<text.size()?end+1:end;
    }
    return sections==1 && keys==1;
}
inline bool editNativeFOV(const std::string &text,const std::string &target,std::string &edited,const char *option="maxuserfovoffsetpercent",double maximum=100) {
    std::string current;size_t begin=0,length=0;
    if(!nativeFOVValue(target,maximum) || !nativeFOVToken(text,current,begin,length,option,maximum))return false;
    edited=text;
    if(strtod(current.c_str(),nullptr)!=strtod(target.c_str(),nullptr))edited.replace(begin,length,target);
    return true;
}
// This transaction edits one existing native option. No reset/generated INI,
// camera call, shader hook or live object write. Slider mode manages FieldOfView;
// legacy mode manages only MaxUserFOVOffsetPercent. Backups are independent.
inline int applyNativeFOV(const wchar_t *path,int preference,bool restore=false,bool slider=false) {
    if(!path || wcslen(path)>MAX_PATH-100 || (!restore && (slider?(preference<0 || preference>100):nativeFOVPercent(preference)<0)))return -1;
    const char *option=slider?"fieldofview":"maxuserfovoffsetpercent";
    const double maximum=slider?1.0:100.0;
    DWORD attributes=GetFileAttributesW(path);
    if(attributes==INVALID_FILE_ATTRIBUTES || (attributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)))return -1;
    HANDLE original=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
    if(original==INVALID_HANDLE_VALUE)return -1;
    int result=-1;std::wstring temp;
    do {
        std::string raw,text,previous;bool wide=false;size_t a=0,n=0;BY_HANDLE_FILE_INFORMATION identity={};
        if(!GetFileInformationByHandle(original,&identity) || !readUIHandle(original,raw) || !decodeUIConfig(raw,text,wide)
            || !nativeFOVToken(text,previous,a,n,option,maximum))break;
        std::wstring backup=std::wstring(path)+(slider?L".InfiniteMod-native-slider-backup":L".InfiniteMod-native-fov-backup");
        DWORD savedAttributes=GetFileAttributesW(backup.c_str());
        if(savedAttributes!=INVALID_FILE_ATTRIBUTES && (savedAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)))break;
        if(!restore) {
            HANDLE saved=CreateFileW(backup.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(saved!=INVALID_HANDLE_VALUE) {
                bool ok=writeUIHandle(saved,raw);CloseHandle(saved);
                if(!ok){DeleteFileW(backup.c_str());break;}
            } else if(GetLastError()!=ERROR_FILE_EXISTS && GetLastError()!=ERROR_ALREADY_EXISTS)break;
        }
        HANDLE saved=CreateFileW(backup.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        if(saved==INVALID_HANDLE_VALUE)break;
        std::string savedRaw,savedText,savedValue;bool savedWide=false;
        bool valid=readUIHandle(saved,savedRaw) && decodeUIConfig(savedRaw,savedText,savedWide)
            && nativeFOVToken(savedText,savedValue,a,n,option,maximum);CloseHandle(saved);
        if(!valid)break;
        char percentage[32];snprintf(percentage,sizeof(percentage),"%.6f",slider?preference/100.0:nativeFOVPercent(preference));
        std::string edited,bytes,target=restore?savedValue:percentage;
        if(!editNativeFOV(text,target,edited,option,maximum) || !encodeUIConfig(edited,wide,bytes))break;
        if(bytes==raw){result=0;break;}
        wchar_t suffix[80];swprintf(suffix,80,L".InfiniteMod-fov-tmp-%lu-%lu",GetCurrentProcessId(),GetTickCount());
        temp=std::wstring(path)+suffix;
        HANDLE out=CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(out==INVALID_HANDLE_VALUE)break;
        bool ok=writeUIHandle(out,bytes);CloseHandle(out);if(!ok)break;
        HANDLE check=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
        if(check==INVALID_HANDLE_VALUE)break;
        BY_HANDLE_FILE_INFORMATION current={};std::string currentRaw;
        ok=GetFileInformationByHandle(check,&current) && current.dwVolumeSerialNumber==identity.dwVolumeSerialNumber
            && current.nFileIndexHigh==identity.nFileIndexHigh && current.nFileIndexLow==identity.nFileIndexLow
            && readUIHandle(check,currentRaw) && currentRaw==raw;CloseHandle(check);
        if(!ok || !ReplaceFileW(path,temp.c_str(),nullptr,REPLACEFILE_IGNORE_MERGE_ERRORS,nullptr,nullptr))break;
        result=1;
    }while(false);
    CloseHandle(original);if(!temp.empty())DeleteFileW(temp.c_str());return result;
}
