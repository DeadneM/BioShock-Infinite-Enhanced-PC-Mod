#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

// Only our marked additions are removed on disable. Native startup/loading entries
// and every other byte outside that block remain unchanged.
static const char *configBegin="; InfiniteMod managed skippable intros BEGIN";
static const char *configEnd="; InfiniteMod managed skippable intros END";
static const char *startupMarker="; InfiniteMod skipped logo: ";
static std::string trim(std::string s) {
    size_t a=s.find_first_not_of(" \t\r\n"), b=s.find_last_not_of(" \t\r\n");
    return a==std::string::npos ? "" : s.substr(a,b-a+1);
}
static std::string lower(std::string s) {
    for(char &c:s) if(c>='A' && c<='Z') c+=32;
    return s;
}
static std::string manageStartupLogos(const std::string &input,bool skip) {
    std::string output;bool section=false;
    const char *movies[]={"2klogosweep720p2997","irrationallogotest","ue3animatedlogo_prores","amdlogo720p"};
    for(size_t pos=0;pos<input.size();) {
        size_t end=input.find('\n',pos);end=end==std::string::npos?input.size():end+1;
        std::string original=input.substr(pos,end-pos);
        if(original.compare(0,strlen(startupMarker),startupMarker)==0) original.erase(0,strlen(startupMarker));
        std::string line=trim(original);
        if(pos==0 && line.size()>=3 && line.substr(0,3)=="\xef\xbb\xbf") line.erase(0,3);
        if(!line.empty() && line[0]=='[') section=lower(line)=="[fullscreenmovie]";
        if(skip && section) {
            size_t eq=line.find('=');
            if(eq!=std::string::npos) {
                std::string key=lower(trim(line.substr(0,eq))),value=lower(trim(line.substr(eq+1)));
                if(key=="startupmovies" || key=="+startupmovies")
                    for(const char *movie:movies) if(value==movie) {original=startupMarker+original;break;}
            }
        }
        output+=original;pos=end;
    }
    return output;
}
static bool editConfig(const std::string &input, bool enabled, std::string &output, bool skipLogos=false) {
    output=manageStartupLogos(input,enabled && skipLogos);
    size_t a=output.find(configBegin), b=output.find(configEnd);
    if ((a==std::string::npos)!=(b==std::string::npos)) return false;
    if(a!=std::string::npos) {
        if(b<a || output.find(configBegin,a+1)!=std::string::npos) return false;
        b=output.find('\n',b); b=b==std::string::npos ? output.size() : b+1;
        output.erase(a,b-a);
    }
    if(!enabled) return true;
    const char *movies[]={"2KLogoSweep720p2997","IrrationalLogoTest","UE3AnimatedLogo_ProRes","AMDLogo720p"};
    bool seen[2][4]={}; bool section=false, found=false;
    size_t insert=std::string::npos;
    for(size_t pos=0;pos<output.size();) {
        size_t end=output.find('\n',pos); if(end==std::string::npos) end=output.size();
        std::string line=trim(output.substr(pos,end-pos));
        if(pos==0 && line.size()>=3 && line.substr(0,3)=="\xef\xbb\xbf") line.erase(0,3);
        if(!line.empty() && line[0]=='[') {
            if(section) insert=pos;
            section=lower(line)=="[fullscreenmovie]";
            if(section) found=true;
        } else if(section && !line.empty() && line[0]!=';' && line[0]!='#') {
            size_t eq=line.find('=');
            if(eq!=std::string::npos) {
                std::string key=lower(trim(line.substr(0,eq))), value=lower(trim(line.substr(eq+1)));
                bool removal=!key.empty() && key[0]=='-';
                if(!key.empty() && (key[0]=='+' || key[0]=='-' || key[0]=='.')) key.erase(0,1);
                for(int k=0;k<2;k++) if(key==(k ? "skippablemoviespc" : "skippablemovies"))
                    for(int m=0;m<4;m++) if(value==lower(movies[m])) seen[k][m]=!removal;
            }
        }
        pos=end<output.size()?end+1:end;
    }
    if(section) insert=output.size();
    if(!found || insert==std::string::npos) return false; // Never generate/reset a missing game config.
    std::string nl=output.find("\r\n")!=std::string::npos ? "\r\n" : "\n", block;
    for(int k=0;k<2;k++) for(int m=0;m<4;m++) if(!seen[k][m])
        block+=std::string(k?"SkippableMoviesPC=":"SkippableMovies=")+movies[m]+nl;
    if(!block.empty()) {
        block=std::string(configBegin)+nl+block+configEnd+nl;
        if(insert && output[insert-1]!='\n') block=nl+block;
        output.insert(insert,block);
    }
    return true;
}
static bool readConfigBytes(const wchar_t *path,std::string &bytes) {
    HANDLE file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr);
    if(file==INVALID_HANDLE_VALUE) return false;
    DWORD size=GetFileSize(file,nullptr),read=0;
    if(size==INVALID_FILE_SIZE || size>4*1024*1024) {CloseHandle(file);return false;}
    bytes.resize(size);
    bool ok=ReadFile(file,bytes.data(),size,&read,nullptr) && read==size;
    CloseHandle(file);return ok;
}
static bool decodeConfig(const std::string &raw,std::string &text,bool &wide) {
    wide=raw.size()>=2 && (unsigned char)raw[0]==255 && (unsigned char)raw[1]==254;
    if(!wide) {text=raw;return true;}
    if((raw.size()-2)%2) return false;
    int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,(const wchar_t *)(raw.data()+2),(int)(raw.size()-2)/2,nullptr,0,nullptr,nullptr);
    if(!n) return false;
    text.resize(n);return WideCharToMultiByte(CP_UTF8,0,(const wchar_t *)(raw.data()+2),(int)(raw.size()-2)/2,text.data(),n,nullptr,nullptr)==n;
}
static bool restoreRewrittenLogos(std::string &text,const std::string &backup) {
    // UE3 drops comments when saving XEngine.ini. Recover only startup logos that
    // were present in our original backup, never the rest of the old configuration.
    const char *movies[]={"2KLogoSweep720p2997","IrrationalLogoTest","UE3AnimatedLogo_ProRes","AMDLogo720p"};
    bool present[2][4]={};size_t loading=std::string::npos;
    for(int source=0;source<2;source++) {
        const std::string &input=source?backup:text;bool section=false;
        for(size_t pos=0;pos<input.size();) {
            size_t end=input.find('\n',pos);if(end==std::string::npos)end=input.size();
            std::string line=trim(input.substr(pos,end-pos));
            if(!line.empty() && line[0]=='[') section=lower(line)=="[fullscreenmovie]";
            size_t eq=line.find('=');
            if(section && eq!=std::string::npos) {
                std::string key=lower(trim(line.substr(0,eq))),value=lower(trim(line.substr(eq+1)));
                if(key=="startupmovies" || key=="+startupmovies") {
                    for(int i=0;i<4;i++)if(value==lower(movies[i]))present[source][i]=true;
                    if(!source && value=="loopingmovieuntilstartupfinishes")loading=pos;
                }
            }
            pos=end<input.size()?end+1:end;
        }
    }
    std::string missing,nl=text.find("\r\n")!=std::string::npos?"\r\n":"\n";
    for(int i=0;i<4;i++)if(present[1][i] && !present[0][i])missing+=std::string("StartupMovies=")+movies[i]+nl;
    if(missing.empty())return true;
    if(loading==std::string::npos)return false;
    text.insert(loading,missing);return true;
}
// Result: 0 unchanged, 1 changed, -1 failure. Preserve UTF-8/ANSI or UTF-16LE.
static int enforceConfig(const wchar_t *path,bool enabled,bool skipLogos=false) {
    std::string raw; if(!readConfigBytes(path,raw)) return -1;
    bool wide=false;std::string text;if(!decodeConfig(raw,text,wide))return -1;
    std::string edited;if(!editConfig(text,enabled,edited,skipLogos)) return -1;
    std::wstring backup=std::wstring(path)+L".InfiniteMod-v09-backup";
    if(!enabled && skipLogos) {
        std::string saved,decoded;bool savedWide=false;
        if(readConfigBytes(backup.c_str(),saved)) {
            if(!decodeConfig(saved,decoded,savedWide) || !restoreRewrittenLogos(edited,decoded))return -1;
        }
    }
    if(edited==text) return 0;
    std::string bytes=edited;
    if(wide) {
        int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,edited.data(),(int)edited.size(),nullptr,0);
        if(!n) return -1;
        bytes.assign(2+n*2,'\0');bytes[0]=(char)255;bytes[1]=(char)254;
        MultiByteToWideChar(CP_UTF8,0,edited.data(),(int)edited.size(),(wchar_t *)(bytes.data()+2),n);
    }
    if(!CopyFileW(path,backup.c_str(),TRUE) && GetLastError()!=ERROR_FILE_EXISTS) return -1;
    std::string check;if(!readConfigBytes(path,check) || check!=raw) return -1;
    std::wstring temporary=std::wstring(path)+L".InfiniteMod.tmp";
    HANDLE file=CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE) return -1;
    DWORD written=0;
    bool ok=WriteFile(file,bytes.data(),(DWORD)bytes.size(),&written,nullptr) && written==bytes.size() && FlushFileBuffers(file);
    CloseHandle(file);
    if(ok) ok=ReplaceFileW(path,temporary.c_str(),nullptr,0,nullptr,nullptr)!=FALSE;
    if(!ok) DeleteFileW(temporary.c_str());
    return ok?1:-1;
}
