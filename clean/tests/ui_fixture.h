#pragma once
#include "../ui_config.h"

static void uiFixtureWrite(const wchar_t *path,const std::string &bytes,DWORD creation=CREATE_ALWAYS) {
    HANDLE file=CreateFileW(path,GENERIC_WRITE,0,nullptr,creation,FILE_ATTRIBUTE_NORMAL,nullptr);
    CHECK(file!=INVALID_HANDLE_VALUE);CHECK(writeUIHandle(file,bytes));CloseHandle(file);
}
static void testUIScale() {
    const std::string original="; preserve comments\r\n[GFxUIEngine]\r\n  mAxResolutionUpScale \t=\t 1.25  \r\nKeep=7\r\n[Other]\r\nMaxResolutionUpScale=8\r\n";
    std::string edited,again,value;
    CHECK(editUIScale(original,"0.0",edited));CHECK(edited.find("  mAxResolutionUpScale \t=\t 0.0  \r\n")!=std::string::npos);
    CHECK(editUIScale(edited,"0.0",again));CHECK(again==edited);
    CHECK(editUIScale(edited,"1.25",again));CHECK(again==original);
    CHECK(uiScaleValue(edited,value));CHECK(value=="0.0");
    CHECK(!nativeUIScale(0) && !nativeUIScale(63) && !nativeUIScale(175));
    CHECK(!editUIScale(original,"NaN",again));CHECK(!editUIScale(original,"1 junk",again));
    const char *invalid[]={
        "[GFxUIEngine]\nMaxResolutionUpScale=1\nMaxResolutionUpScale=1\n",
        "[GFxUIEngine]\nMaxResolutionUpScale=1\n[gfxuiengine]\n",
        "[GFxUIEngine]\n+MaxResolutionUpScale=1\n",
        "[GFxUIEngine]\nMaxResolutionUpScale=1\n!MaxResolutionUpScale=ClearArray\n",
        "[GFxUIEngine]\nMaxResolutionUpScale=\n",
        "[GFxUIEngine]\nMaxResolutionUpScale=-1\n",
        "[Other]\nMaxResolutionUpScale=1\n"
    };
    for(const char *bad:invalid)CHECK(!uiScaleValue(bad,value));
    CHECK(uiScaleValue("\xef\xbb\xbf[GFxUIEngine]\n;MaxResolutionUpScale=0\nMaxResolutionUpScale=1\n",value));
    for(int encoding=0;encoding<3;encoding++) {
        wchar_t path[MAX_PATH];swprintf(path,MAX_PATH,L"ui-fixture-%lu-%lu-%d.ini",GetCurrentProcessId(),GetTickCount(),encoding);
        std::wstring backup=std::wstring(path)+L".InfiniteMod-hud-backup";
        std::string fixture=(encoding==1?"\xef\xbb\xbf":"")+original+"Label="+(encoding==0?"caf\xe9":"caf\xc3\xa9")+"\r\n";
        std::string bytes;CHECK(encodeUIConfig(fixture,encoding==2,bytes));uiFixtureWrite(path,bytes,CREATE_NEW);
        CHECK(enforceUIScale(path,0)==-1);CHECK(GetFileAttributesW(backup.c_str())==INVALID_FILE_ATTRIBUTES);
        CHECK(enforceUIScale(path,100)==1);CHECK(enforceUIScale(path,100)==0);
        std::string read;CHECK(readConfigBytes(backup.c_str(),read));CHECK(read==bytes);
        for(int percent:{125,150,200,100}) {
            CHECK(enforceUIScale(path,percent)==1);CHECK(enforceUIScale(path,percent)==0);
            CHECK(readConfigBytes(path,read));std::string expected;
            CHECK(editUIScale(fixture,nativeUIScale(percent),edited));CHECK(encodeUIConfig(edited,encoding==2,expected));CHECK(read==expected);
        }
        // Native reserialization drops comments and changes unrelated settings.
        std::string rewritten="[GFxUIEngine]\nMaxResolutionUpScale=63.000000\nKeep=99\n[Other]\nMaxResolutionUpScale=9\n";
        std::string serialized;CHECK(encodeUIConfig(rewritten,encoding==2,serialized));uiFixtureWrite(path,serialized);
        CHECK(enforceUIScale(path,100)==1);CHECK(enforceUIScale(path,100)==0);
        CHECK(readConfigBytes(path,read));std::string expected;
        CHECK(editUIScale(rewritten,"0.0",edited));CHECK(encodeUIConfig(edited,encoding==2,expected));CHECK(read==expected);
        CHECK(readConfigBytes(backup.c_str(),read));CHECK(read==bytes); // Backup never replaced.
        // A corrupt existing backup must leave the current config untouched.
        uiFixtureWrite(backup.c_str(),"[Broken]\n");CHECK(enforceUIScale(path,125)==-1);
        CHECK(readConfigBytes(path,read));CHECK(read==expected);
        CHECK(DeleteFileW(backup.c_str()));
        HANDLE writer=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);CHECK(writer!=INVALID_HANDLE_VALUE);
        CHECK(enforceUIScale(path,125)==-1);CloseHandle(writer);
        CHECK(readConfigBytes(path,read));CHECK(read==expected);
        CHECK(GetFileAttributesW(backup.c_str())==INVALID_FILE_ATTRIBUTES);
        CHECK(DeleteFileW(path));
    }
    bool wide=false;
    CHECK(!decodeUIConfig(std::string("\xff\xfeX",3),edited,wide));
    CHECK(!decodeUIConfig(std::string("\xfe\xff\0X",4),edited,wide));
    CHECK(!decodeUIConfig(std::string("[GFxUIEngine]\0garbage",20),edited,wide));
    puts("UI scale config tests passed (all four ceilings, repair legacy 63, ANSI, UTF-8, UTF-16, preserve backup, conflicting writes)");
}
