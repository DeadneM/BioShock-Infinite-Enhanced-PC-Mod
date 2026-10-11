#pragma once
#include "config.h"
#include <cmath>
#include <cstdlib>
#include <cerrno>

// This is the native Scaleform ceiling, shared by the HUD AND full-screen menus.
// The caller must apply it only before UI creation at process startup. It does
// not change separately rendered particle effects; it is not a HUD-only hook.
static bool validUIScale(const std::string &value) {
    if(value.empty())return false;
    char *end=nullptr;errno=0;double number=strtod(value.c_str(),&end);
    return !errno && end==value.c_str()+value.size() && std::isfinite(number) && number>=0.0;
}
static bool uiScaleValue(const std::string &input,std::string &value,size_t *begin=nullptr,size_t *length=nullptr) {
    bool section=false;unsigned sections=0,keys=0;size_t valueBegin=0,valueLength=0;
    for(size_t pos=0;pos<input.size();) {
        size_t end=input.find('\n',pos);if(end==std::string::npos)end=input.size();
        std::string original=input.substr(pos,end-pos),line=trim(original);
        if(pos==0 && line.compare(0,3,"\xef\xbb\xbf")==0)line.erase(0,3);
        if(!line.empty() && line[0]=='[') {
            section=lower(line)=="[gfxuiengine]";
            if(section && ++sections!=1)return false;
        } else if(section && !line.empty() && line[0]!=';' && line[0]!='#') {
            size_t eq=original.find('=');
            if(eq!=std::string::npos) {
                std::string key=lower(trim(original.substr(0,eq))),plain=key;
                if(!plain.empty() && (plain[0]=='+' || plain[0]=='-' || plain[0]=='.' || plain[0]=='!'))plain.erase(0,1);
                if(plain=="maxresolutionupscale") {
                    if(key!=plain || ++keys!=1)return false;
                    size_t first=original.find_first_not_of(" \t\r",eq+1),last=original.find_last_not_of(" \t\r");
                    if(first==std::string::npos || first>last)return false;
                    valueBegin=pos+first;valueLength=last-first+1;
                }
            } else if(lower(trim(line))=="maxresolutionupscale")return false;
        }
        pos=end<input.size()?end+1:end;
    }
    if(keys!=1)return false;
    std::string parsed=input.substr(valueBegin,valueLength);
    if(!validUIScale(parsed))return false;
    value=parsed;if(begin)*begin=valueBegin;if(length)*length=valueLength;return true;
}
static bool editUIScale(const std::string &input,const std::string &value,std::string &output) {
    std::string previous;size_t begin=0,length=0;
    if(!validUIScale(value) || !uiScaleValue(input,previous,&begin,&length))return false;
    output=input;output.replace(begin,length,value);return true;
}
static bool decodeUIConfig(const std::string &raw,std::string &text,bool &wide) {
    // Unsupported byte order/NUL-containing narrow files must never be rewritten.
    if(raw.compare(0,2,"\xfe\xff")==0 || !decodeConfig(raw,text,wide))return false;
    return text.find('\0')==std::string::npos;
}
static bool encodeUIConfig(const std::string &text,bool wide,std::string &bytes) {
    bytes=text;if(!wide)return true;
    int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),(int)text.size(),nullptr,0);
    if(!count)return false;
    bytes.assign(2+count*2,'\0');bytes[0]=(char)255;bytes[1]=(char)254;
    return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),(int)text.size(),(wchar_t *)(bytes.data()+2),count)==count;
}
static bool readUIHandle(HANDLE file,std::string &raw) {
    LARGE_INTEGER size={};DWORD read=0;
    if(!GetFileSizeEx(file,&size) || size.QuadPart<1 || size.QuadPart>4*1024*1024)return false;
    raw.resize((size_t)size.QuadPart);
    return ReadFile(file,raw.data(),(DWORD)raw.size(),&read,nullptr) && read==raw.size();
}
static bool writeUIHandle(HANDLE file,const std::string &bytes) {
    DWORD written=0;
    return WriteFile(file,bytes.data(),(DWORD)bytes.size(),&written,nullptr) && written==bytes.size() && FlushFileBuffers(file);
}
static const char *nativeUIScale(int percent) {
    switch(percent) {case 100:return "0.0";case 125:return "0.25";case 150:return "0.5";case 200:return "1.0";default:return nullptr;}
}
static int enforceUIScale(const wchar_t *path,int percent) {
    const char *target=nativeUIScale(percent);if(!target)return -1;
    // Deny other file writers for the whole transaction. Delete sharing is
    // required by ReplaceFile; check identity immediately before replacement
    // to detect a path replaced by another actor while the snapshot was read.
    HANDLE original=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
    if(original==INVALID_HANDLE_VALUE)return -1;
    int result=-1;std::wstring temp;
    do {
        std::string raw,text,value,edited;bool wide=false;BY_HANDLE_FILE_INFORMATION identity={};
        if(!GetFileInformationByHandle(original,&identity) || !readUIHandle(original,raw) || !decodeUIConfig(raw,text,wide) || !uiScaleValue(text,value))break;
        std::wstring backup=std::wstring(path)+L".InfiniteMod-hud-backup";
        {
            HANDLE saved=CreateFileW(backup.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(saved!=INVALID_HANDLE_VALUE) {
                bool ok=writeUIHandle(saved,raw);CloseHandle(saved);
                if(!ok){DeleteFileW(backup.c_str());break;}
            } else if(GetLastError()!=ERROR_FILE_EXISTS && GetLastError()!=ERROR_ALREADY_EXISTS)break;
        }
        HANDLE saved=CreateFileW(backup.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        if(saved==INVALID_HANDLE_VALUE)break;
        std::string savedRaw,savedText,savedValue;bool savedWide=false;
        bool valid=readUIHandle(saved,savedRaw) && decodeUIConfig(savedRaw,savedText,savedWide) && uiScaleValue(savedText,savedValue);
        CloseHandle(saved);
        if(!valid)break; // Preserve a valid original for manual restoration.
        value=target;
        if(!editUIScale(text,value,edited))break;
        if(edited==text){result=0;break;}
        std::string bytes;if(!encodeUIConfig(edited,wide,bytes))break;
        HANDLE file=INVALID_HANDLE_VALUE;static volatile LONG serial=0;
        for(int attempt=0;attempt<8 && file==INVALID_HANDLE_VALUE;attempt++) {
            wchar_t suffix[100];swprintf(suffix,100,L".InfiniteMod-hud.%lu.%lu.%ld.tmp",GetCurrentProcessId(),GetTickCount(),InterlockedIncrement(&serial));
            std::wstring candidate=std::wstring(path)+suffix;
            file=CreateFileW(candidate.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(file!=INVALID_HANDLE_VALUE)temp=candidate;
            else if(GetLastError()!=ERROR_FILE_EXISTS && GetLastError()!=ERROR_ALREADY_EXISTS)break;
        }
        if(file==INVALID_HANDLE_VALUE)break;
        bool ok=writeUIHandle(file,bytes);CloseHandle(file);if(!ok)break;
        HANDLE current=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
        if(current==INVALID_HANDLE_VALUE)break;
        BY_HANDLE_FILE_INFORMATION check={};
        ok=GetFileInformationByHandle(current,&check) && check.dwVolumeSerialNumber==identity.dwVolumeSerialNumber &&
           check.nFileIndexHigh==identity.nFileIndexHigh && check.nFileIndexLow==identity.nFileIndexLow;
        CloseHandle(current);
        if(!ok || !ReplaceFileW(path,temp.c_str(),nullptr,0,nullptr,nullptr))break;
        temp.clear();result=1;
    } while(false);
    CloseHandle(original);if(!temp.empty())DeleteFileW(temp.c_str());return result;
}
