#pragma once

// Included by the existing DX11 harness after overlay.cpp. Uses its simulated
// foreground window and key array; all input passes through the real Present.
static void testOverlayBindings(IDXGISwapChain *chain,HWND window) {
    wchar_t originalIni[MAX_PATH],directory[MAX_PATH],fixtureIni[MAX_PATH];
    wcscpy(originalIni,modIni);CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf(fixtureIni,MAX_PATH,L"%ls\\overlay-bindings-%lu-%lu.ini",directory,GetCurrentProcessId(),GetTickCount())>0);
    CHECK(WritePrivateProfileStringW(L"Other",L"Preserve",L"unchanged",fixtureIni));wcscpy(modIni,fixtureIni);
    CHECK(readGlobalHotkeys(fixtureIni));
    CHECK(persistGlobalHotkeys(fixtureIni,false));CHECK(!readGlobalHotkeys(fixtureIni));
    for(auto &key:keys)key=0;
    overlayBindings.resetDefaults();bindingsLoaded=false;bindingsOpen=false;menuOpen=false;bindingsReturnToMain=false;
    selectedWeapon=0;selectedBinding=0;*bindingsNotice=0;bindingsError=false;settingError=false;
    navigationFocused=false;for(bool &held:navigationHeld)held=false;
    configWanted=1;hudWanted=hudApplied=100;aimWanted=0;aimKey=VK_MBUTTON;crosshairHidden=0;speedPercent=100;fovDegrees=80;nativeAimZoom=0;
    available=false;pending=-1;busy=false;grantsEnabled=progressionEnabled=true;lastSeen=0;publishMenuState();
    auto frame=[&]() {CHECK(SUCCEEDED(chain->Present(0,0)));};
    auto press=[&](int key) {keys[key]=static_cast<SHORT>(0x8000);frame();};
    auto release=[&](int key) {keys[key]=0;frame();};
    auto tapBinding=[&](int key) {press(key);release(key);};
    focus=nullptr;tapBinding(VK_INSERT);CHECK(!menuOpen && !bindingsOpen && !gui);
    focus=window;frame();tapBinding(VK_INSERT);CHECK(menuOpen && !bindingsOpen && gui && modMenuOpen==1);
    // Invalid player state prevents every grant; F1 no longer toggles logos.
    tapBinding(VK_F1);tapBinding(VK_F2);tapBinding(VK_F5);tapBinding(VK_F6);CHECK(pending==-1 && configWanted==1);
    tapBinding(VK_F3);CHECK(selectedWeapon==weaponChoiceCount-1);tapBinding(VK_F4);CHECK(selectedWeapon==0);
    press(VK_F4);frame();CHECK(selectedWeapon==1);release(VK_F4);
    // Verify the exact requests, including the two direct destination keys.
    available=true;lastSeen=GetTickCount();press(VK_F1);CHECK(pending==-10);
    pending=-1;frame();CHECK(pending==-1);release(VK_F1);
    lastSeen=GetTickCount();press(VK_F2);CHECK(pending==-2);pending=-1;frame();CHECK(pending==-1);release(VK_F2);
    lastSeen=GetTickCount();tapBinding(VK_F5);CHECK(pending==selectedWeapon && pendingDestination==0);pending=-1;
    lastSeen=GetTickCount();tapBinding(VK_F6);CHECK(pending==selectedWeapon && pendingDestination==1);pending=-1;
    const int progressionRequests[]={-3,-4,-5,-8,-9,-6,-7};
    int oldWeapon=selectedWeapon;keys[VK_CONTROL]=static_cast<SHORT>(0x8000);frame();
    for(int i=0;i<7;++i) {
        lastSeen=GetTickCount();press(VK_F1+i);CHECK(pending==progressionRequests[i]);
        pending=-1;frame();CHECK(pending==-1);release(VK_F1+i);
        CHECK(selectedWeapon==oldWeapon && crosshairHidden==0 && speedPercent==100 && configWanted==1);
    }
    // Ctrl+F8 now opens configuration and cannot grant infusions or toggle F8.
    press(VK_F8);CHECK(bindingsOpen && !menuOpen && pending==-1 && crosshairHidden==0 && modMenuOpen==1);
    release(VK_F8);keys[VK_CONTROL]=0;frame();
    for(int key:{VK_F1,VK_F2,VK_F5,VK_F6,VK_F7,VK_F12})tapBinding(key);
    CHECK(pending==-1 && speedPercent==100 && configWanted==1);
    // Each accepted action and the appended experimental action is reachable.
    CHECK(selectedBinding==0);
    for(unsigned i=1;i<keybinds::count;++i){tapBinding(VK_DOWN);CHECK(selectedBinding==i);}
    CHECK(selectedBinding/bindingsPageSize==(keybinds::count-1)/bindingsPageSize);tapBinding(VK_DOWN);CHECK(selectedBinding==0);
    tapBinding(VK_UP);CHECK(selectedBinding==keybinds::count-1);tapBinding(VK_DOWN);CHECK(selectedBinding==0);
    // Rebind Insert. A conflicting F2 is rejected while keeping capture open;
    // a held grant key during capture cannot execute the grant behind the UI.
    tapBinding(VK_RETURN);CHECK(overlayBindings.capturing());
    press(VK_F2);CHECK(bindingsError && overlayBindings.capturing() && pending==-1);
    CHECK(strstr(bindingsNotice,"ammo") && overlayBindings.binding(keybinds::Action::MainMenu).key==VK_INSERT);release(VK_F2);
    press('K');CHECK(!overlayBindings.capturing() && !bindingsError && bindingsOpen && !menuOpen && modMenuOpen==1);
    CHECK(overlayBindings.binding(keybinds::Action::MainMenu).key=='K');frame();CHECK(bindingsOpen && !menuOpen);release('K');
    tapBinding(VK_ESCAPE);CHECK(menuOpen && !bindingsOpen);tapBinding(VK_INSERT);CHECK(menuOpen);
    tapBinding('K');CHECK(!menuOpen && !bindingsOpen);tapBinding('K');CHECK(menuOpen);
    // Rebind the configuration menu itself, and verify persistent reload.
    keys[VK_CONTROL]=static_cast<SHORT>(0x8000);tapBinding(VK_F8);keys[VK_CONTROL]=0;frame();CHECK(bindingsOpen);
    tapBinding(VK_DOWN);CHECK(selectedBinding==1);tapBinding(VK_RETURN);CHECK(overlayBindings.capturing());
    tapBinding('J');CHECK(!overlayBindings.capturing() && overlayBindings.binding(keybinds::Action::BindingsMenu).key=='J');
    tapBinding(VK_ESCAPE);CHECK(menuOpen && !bindingsOpen);
    keys[VK_CONTROL]=static_cast<SHORT>(0x8000);tapBinding(VK_F8);keys[VK_CONTROL]=0;frame();CHECK(!bindingsOpen);
    tapBinding('J');CHECK(bindingsOpen && !menuOpen);tapBinding('J');CHECK(menuOpen && !bindingsOpen);
    tapBinding('K');CHECK(!menuOpen);tapBinding('J');CHECK(bindingsOpen && !menuOpen);tapBinding(VK_ESCAPE);CHECK(!bindingsOpen && !menuOpen && modMenuOpen==0);
    bindingsLoaded=false;frame();CHECK(overlayBindings.binding(keybinds::Action::MainMenu).key=='K' && overlayBindings.binding(keybinds::Action::BindingsMenu).key=='J');
    frame();tapBinding('K');CHECK(menuOpen);
    wchar_t preserved[32];CHECK(GetPrivateProfileStringW(L"Other",L"Preserve",L"",preserved,32,fixtureIni));CHECK(!wcscmp(preserved,L"unchanged"));
    // Focus recovery consumes background and held keys; a closed main menu
    // likewise consumes grants rather than queueing them for later opening.
    focus=nullptr;lastSeen=GetTickCount();press(VK_F1);CHECK(pending==-1);
    focus=window;frame();frame();CHECK(pending==-1);release(VK_F1);lastSeen=GetTickCount();tapBinding(VK_F1);CHECK(pending==-10);pending=-1;
    tapBinding('K');CHECK(!menuOpen);lastSeen=GetTickCount();press(VK_F2);CHECK(pending==-1);release(VK_F2);tapBinding('K');CHECK(menuOpen && pending==-1);
    keys[VK_SHIFT]=static_cast<SHORT>(0x8000);lastSeen=GetTickCount();tapBinding(VK_F1);CHECK(pending==-1);keys[VK_SHIFT]=0;frame();
    available=false;lastSeen=0;
    press(VK_F8);frame();CHECK(crosshairHidden==1);release(VK_F8);tapBinding(VK_F8);CHECK(crosshairHidden==0);
    for(int expected:{125,150,200,100}){tapBinding(VK_F9);CHECK(hudWanted==expected && hudApplied==100);}
    CHECK(WritePrivateProfileStringW(L"Display",L"NativeAimZoom",L"1",fixtureIni));
    keys[VK_CONTROL]=static_cast<SHORT>(0x8000);tapBinding(VK_F9);
    CHECK(nativeAimZoom==1 && hudWanted==100 && !strcmp(aimZoomLabel(),"ON"));
    CHECK(GetPrivateProfileIntW(L"Display",L"NativeAimZoom",-1,fixtureIni)==1);
    press(VK_F9);frame();release(VK_F9);
    CHECK(nativeAimZoom==0);
    CHECK(nativeAimZoom==0 && GetPrivateProfileIntW(L"Display",L"NativeAimZoom",-1,fixtureIni)==0);
    keys[VK_CONTROL]=0;frame();
    keys[VK_CONTROL]=(SHORT)0x8000;frame();
    LONG beforeFov=fovDegrees;
    tapBinding(VK_OEM_MINUS);CHECK(weaponDistance==-2 && fovDegrees==beforeFov);
    tapBinding(VK_SUBTRACT);CHECK(weaponDistance==-4 && firstperson_setting::load(fixtureIni)==-4);
    tapBinding(VK_OEM_PLUS);CHECK(weaponDistance==-2);
    tapBinding(VK_ADD);CHECK(weaponDistance==0 && fovDegrees==beforeFov);
    keys[VK_SHIFT]=(SHORT)0x8000;tapBinding(VK_OEM_PLUS);keys[VK_SHIFT]=0;frame();CHECK(weaponDistance==2);
    for(int i=0;i<30;++i)tapBinding(VK_OEM_PLUS);CHECK(weaponDistance==50 && firstperson_setting::load(fixtureIni)==50);
    for(int i=0;i<55;++i)tapBinding(VK_OEM_MINUS);CHECK(weaponDistance==-50 && firstperson_setting::load(fixtureIni)==-50 && fovDegrees==beforeFov);
    CHECK(saveWeaponDistance(0));
    for(unsigned i=0;i<6;++i){tapBinding('1'+i);CHECK(inputGains[i]==125 && input_profiles::load(fixtureIni,i)==125);CHECK(saveInputGain(i,100));}
    keys[VK_CONTROL]=0;frame();
    tapBinding(VK_F10);CHECK(aimWanted==1);tapBinding(VK_F10);CHECK(aimWanted==0);tapBinding(VK_F11);CHECK(aimKey==VK_RBUTTON);
    tapBinding(VK_F12);CHECK(configWanted==0);tapBinding(VK_F12);CHECK(configWanted==1);
    for(int expected:{125,150,175,200,100}){tapBinding(VK_F7);CHECK(speedPercent==expected);}
    press(VK_F7);frame();CHECK(speedPercent==125);release(VK_F7);
    keys[VK_CONTROL]=(SHORT)0x8000;frame();tapBinding(VK_F11);keys[VK_CONTROL]=0;frame();
    CHECK(controllerAimButton==1 && controller_aim::load(fixtureIni)==1);
    tapBinding(VK_SUBTRACT);CHECK(fovDegrees==80);press(VK_ADD);frame();CHECK(fovDegrees==85);release(VK_ADD);
    for(int i=0;i<12;++i)tapBinding(VK_ADD);CHECK(fovDegrees==130);
    for(int i=0;i<12;++i)tapBinding(VK_SUBTRACT);CHECK(fovDegrees==80);
    tapBinding(VK_OEM_PLUS);CHECK(fovDegrees==85);tapBinding(VK_OEM_MINUS);CHECK(fovDegrees==80);
    keys[VK_SHIFT]=static_cast<SHORT>(0x8000);tapBinding(VK_OEM_PLUS);CHECK(fovDegrees==85);keys[VK_SHIFT]=0;frame();tapBinding(VK_OEM_MINUS);CHECK(fovDegrees==80);
    focus=nullptr;tapBinding(VK_ADD);tapBinding(VK_F7);CHECK(fovDegrees==80 && speedPercent==125);focus=window;frame();
    // Cancelled capture keeps the key and doesn't close its menu on the same
    // Escape; saving failure leaves the old binding functional and visible.
    tapBinding('J');CHECK(bindingsOpen);selectedBinding=0;tapBinding(VK_RETURN);tapBinding(VK_ESCAPE);
    CHECK(bindingsOpen && !overlayBindings.capturing() && overlayBindings.binding(keybinds::Action::MainMenu).key=='K');
    tapBinding(VK_RETURN);modIni[0]=0;tapBinding('Q');CHECK(bindingsError && overlayBindings.capturing() && overlayBindings.binding(keybinds::Action::MainMenu).key=='K');
    wcscpy(modIni,fixtureIni);tapBinding(VK_ESCAPE);CHECK(!overlayBindings.capturing() && bindingsOpen);tapBinding(VK_ESCAPE);CHECK(menuOpen && !bindingsOpen);
    // Menu bindings may be navigation keys. Within the editor, its plain
    // navigation takes priority so all rows and Escape remain reachable.
    CHECK(overlayBindings.rebindAndSave(keybinds::Action::MainMenu,{VK_UP,0},modIni).ok);
    CHECK(overlayBindings.rebindAndSave(keybinds::Action::BindingsMenu,{VK_DOWN,0},modIni).ok);frame();
    tapBinding(VK_DOWN);CHECK(bindingsOpen && !menuOpen);selectedBinding=0;
    for(unsigned i=1;i<keybinds::count;++i){tapBinding(VK_DOWN);CHECK(bindingsOpen && !menuOpen && selectedBinding==i);}
    tapBinding(VK_DOWN);CHECK(selectedBinding==0 && bindingsOpen);tapBinding(VK_UP);CHECK(selectedBinding==keybinds::count-1 && bindingsOpen);
    tapBinding(VK_RETURN);CHECK(overlayBindings.capturing());tapBinding(VK_ESCAPE);CHECK(bindingsOpen && !overlayBindings.capturing());
    tapBinding(VK_ESCAPE);CHECK(menuOpen && !bindingsOpen);tapBinding(VK_UP);CHECK(!menuOpen);tapBinding(VK_DOWN);CHECK(bindingsOpen && !menuOpen);
    tapBinding(VK_ESCAPE);CHECK(!menuOpen && !bindingsOpen);
    // Global toggle is available even with the menu closed and global actions OFF.
    keys[VK_CONTROL]=(SHORT)0x8000;frame();tapBinding(VK_F10);keys[VK_CONTROL]=0;frame();
    CHECK(globalHotkeys && readGlobalHotkeys(fixtureIni) && !menuOpen);
    available=true;lastSeen=GetTickCount();tapBinding(VK_F2);CHECK(pending==-2);pending=-1;
    lastSeen=GetTickCount();tapBinding(VK_F5);CHECK(pending==selectedWeapon && pendingDestination==0);pending=-1;
    focus=nullptr;tapBinding(VK_F2);CHECK(pending==-1);focus=window;frame();
    keys[VK_CONTROL]=(SHORT)0x8000;frame();tapBinding(VK_F10);keys[VK_CONTROL]=0;frame();
    CHECK(!globalHotkeys && !readGlobalHotkeys(fixtureIni));tapBinding(VK_F2);CHECK(pending==-1);
    // Leave stable defaults for the existing rendering/state-restoration tests.
    CHECK(DeleteFileW(fixtureIni));wcscpy(modIni,originalIni);overlayBindings.resetDefaults();bindingsLoaded=true;
    selectedBinding=0;menuOpen=true;bindingsOpen=false;bindingsReturnToMain=false;bindingsError=false;*bindingsNotice=0;
    navigationFocused=false;for(bool &held:navigationHeld)held=false;for(auto &key:keys)key=0;
    available=false;pending=-1;busy=false;crosshairHidden=0;hudWanted=hudApplied=100;aimWanted=0;aimKey=VK_RBUTTON;fovDegrees=80;speedPercent=125;
    focus=window;publishMenuState();frame();
    CHECK(menuOpen && modMenuOpen==1);
    puts("PASS: overlay requests/default keys, independent paginated bindings menu, conflict/cancel/save errors, Insert and bindings-menu rebinds, capture suppression, persistent reload and focus/menu gating.");
}
