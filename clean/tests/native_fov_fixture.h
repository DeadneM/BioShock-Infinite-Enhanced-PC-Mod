#pragma once
static void testNativeFOVConfig() {
    std::string input="; keep\r\n[XCore.XUserOptionsManager]\r\nFieldOfView=0.625000\r\n MaxUserFOVOffsetPercent = 15.000000 ; keep comment\r\nDisplayMode=1\r\n[Other]\r\nMaxUserFOVOffsetPercent=3\r\n",edited;
    CHECK(editNativeFOV(input,"46.666667",edited));
    CHECK(edited=="; keep\r\n[XCore.XUserOptionsManager]\r\nFieldOfView=0.625000\r\n MaxUserFOVOffsetPercent = 46.666667 ; keep comment\r\nDisplayMode=1\r\n[Other]\r\nMaxUserFOVOffsetPercent=3\r\n");
    std::string again;CHECK(editNativeFOV(edited,"46.666667",again) && edited==again);
    CHECK(!editNativeFOV(input,"nan",edited));CHECK(!editNativeFOV(input,"101",edited));
    CHECK(!editNativeFOV(input+"[XCore.XUserOptionsManager]\nMaxUserFOVOffsetPercent=15\n","25",edited));
    CHECK(!editNativeFOV("[XCore.XUserOptionsManager]\nMaxUserFOVOffsetPercent=15\nMaxUserFOVOffsetPercent=15\n","25",edited));
    CHECK(!editNativeFOV("[XCore.XUserOptionsManager]\n+MaxUserFOVOffsetPercent=15\n","25",edited));
    CHECK(!editNativeFOV("[XCore.XUserOptionsManager]\nMaxUserFOVOffsetPercent=bad\n","25",edited));
    CHECK(!editNativeFOV("[Other]\nMaxUserFOVOffsetPercent=15\n","25",edited));
    wchar_t path[MAX_PATH],cwd[MAX_PATH];CHECK(GetCurrentDirectoryW(MAX_PATH,cwd));
    CHECK(swprintf(path,MAX_PATH,L"%ls\\native-fov-%lu.ini",cwd,GetCurrentProcessId())>0);
    for(bool wide:{false,true}) {
        std::string raw;CHECK(encodeUIConfig(input,wide,raw));
        HANDLE f=CreateFileW(path,GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);CHECK(f!=INVALID_HANDLE_VALUE);
        CHECK(writeUIHandle(f,raw));CloseHandle(f);
        CHECK(applyNativeFOV(path,110)==1);CHECK(applyNativeFOV(path,110)==0);
        std::string changed;CHECK(readConfigBytes(path,changed));std::string text;bool encoded=false;CHECK(decodeUIConfig(changed,text,encoded));
        CHECK(encoded==wide && text.find("FieldOfView=0.625000")!=std::string::npos && text.find("46.666667 ; keep comment")!=std::string::npos);
        std::wstring backup=std::wstring(path)+L".InfiniteMod-native-fov-backup";std::string saved;CHECK(readConfigBytes(backup.c_str(),saved) && saved==raw);
        // A later native slider change belongs to the player, including on rollback.
        text.replace(text.find("FieldOfView=0.625000"),strlen("FieldOfView=0.625000"),"FieldOfView=0.500000");
        CHECK(encodeUIConfig(text,wide,changed));
        f=CreateFileW(path,GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);CHECK(f!=INVALID_HANDLE_VALUE);
        CHECK(writeUIHandle(f,changed));CloseHandle(f);
        CHECK(applyNativeFOV(path,130)==1);CHECK(readConfigBytes(backup.c_str(),saved) && saved==raw);
        HANDLE blocker=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);CHECK(blocker!=INVALID_HANDLE_VALUE);
        CHECK(applyNativeFOV(path,100)==-1);CloseHandle(blocker);
        std::string restored=input;restored.replace(restored.find("FieldOfView=0.625000"),strlen("FieldOfView=0.625000"),"FieldOfView=0.500000");
        CHECK(encodeUIConfig(restored,wide,changed));
        CHECK(applyNativeFOV(path,80,true)==1);CHECK(readConfigBytes(path,saved) && saved==changed);
        CHECK(applyNativeFOV(path,80,true)==0);
        std::string sliderBackup=changed;
        CHECK(applyNativeFOV(path,65,false,true)==1);
        CHECK(applyNativeFOV(path,65,false,true)==0);
        CHECK(readConfigBytes(path,saved));CHECK(decodeUIConfig(saved,text,encoded));
        std::string sliderExpected=restored;sliderExpected.replace(sliderExpected.find("FieldOfView=0.500000"),strlen("FieldOfView=0.500000"),"FieldOfView=0.650000");
        CHECK(text==sliderExpected);
        CHECK(applyNativeFOV(path,101,false,true)==-1);
        CHECK(applyNativeFOV(path,0,true,true)==1);CHECK(readConfigBytes(path,saved) && saved==sliderBackup);
        std::wstring sliderBackupPath=std::wstring(path)+L".InfiniteMod-native-slider-backup";
        CHECK(readConfigBytes(sliderBackupPath.c_str(),saved) && saved==sliderBackup);CHECK(DeleteFileW(sliderBackupPath.c_str()));
        CHECK(DeleteFileW(path));CHECK(DeleteFileW(backup.c_str()));
    }
    CHECK(applyNativeFOV(path,110)==-1);CHECK(nativeFOVPercent(79)==-1);
    puts("PASS: native FOV single-key edits, slider/unrelated-byte preservation, ambiguous/invalid refusal, UTF-8/UTF-16LE, immutable backup, writer exclusion, idempotence and native-range restore.");
}
