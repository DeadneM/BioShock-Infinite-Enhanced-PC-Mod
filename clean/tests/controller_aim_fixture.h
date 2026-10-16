static XINPUT_STATE controllerStates[4];static bool controllerConnected[4];
static DWORD WINAPI controllerFixtureQuery(DWORD index,XINPUT_STATE *state) {
    if(index>=4 || !controllerConnected[index])return ERROR_DEVICE_NOT_CONNECTED;
    *state=controllerStates[index];return ERROR_SUCCESS;
}
static void testControllerAim() {
    using namespace controller_aim;
    CHECK(!down(nullptr,0,30));CHECK(!down(controllerFixtureQuery,0,30));
    controllerConnected[2]=true;
    controllerStates[2].Gamepad.bLeftTrigger=30;CHECK(!down(controllerFixtureQuery,0,30));
    controllerStates[2].Gamepad.bLeftTrigger=31;CHECK(down(controllerFixtureQuery,0,30));
    CHECK(!down(controllerFixtureQuery,1,30));
    controllerStates[2].Gamepad.bRightTrigger=255;CHECK(down(controllerFixtureQuery,1,30));
    for(int i=2;i<(int)count;++i) {
        const WORD masks[]={XINPUT_GAMEPAD_LEFT_SHOULDER,XINPUT_GAMEPAD_RIGHT_SHOULDER,XINPUT_GAMEPAD_LEFT_THUMB,XINPUT_GAMEPAD_RIGHT_THUMB,
            XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_X,XINPUT_GAMEPAD_Y};
        controllerStates[2].Gamepad.wButtons=masks[i-2];CHECK(down(controllerFixtureQuery,i,30));
        controllerStates[2].Gamepad.wButtons=0;CHECK(!down(controllerFixtureQuery,i,30));
    }
    CHECK(!down(controllerFixtureQuery,10,30));CHECK(!down(controllerFixtureQuery,0,255));
    controllerConnected[2]=false;CHECK(!down(controllerFixtureQuery,0,30));
    for(unsigned i=0;i<4;++i){controllerStates[i].Gamepad.bLeftTrigger=255;controllerConnected[i]=true;CHECK(down(controllerFixtureQuery,0,30));controllerConnected[i]=false;CHECK(!down(controllerFixtureQuery,0,30));}
    wchar_t path[MAX_PATH],cwd[MAX_PATH];CHECK(GetCurrentDirectoryW(MAX_PATH,cwd));
    CHECK(swprintf(path,MAX_PATH,L"%ls\\controller-binding-%lu.ini",cwd,GetCurrentProcessId())>0);
    CHECK(load(path)==0);volatile LONG setting=0;
    for(unsigned i=0;i<count;++i){CHECK(save(path,setting,i));CHECK(setting==(LONG)i && load(path)==(int)i);}
    CHECK(!save(path,setting,10) && setting==9);CHECK(!save(L"",setting,0) && setting==9);
    CHECK(DeleteFileW(path));
    aim_detail::State state;
    CHECK(state.sample(1,2,0x104,true,true,false,0)==aim_detail::Action::None);
    CHECK(state.sample(1,2,0x104,true,true,true,0)==aim_detail::Action::Start);
    CHECK(state.sample(1,2,0x104,true,true,false,aim_detail::heldMask)==aim_detail::Action::Stop);
    CHECK(state.sample(1,2,0x204,true,true,true,0)==aim_detail::Action::None); // Rebind held: release gate.
    puts("PASS: XInput LT default, all four users, trigger threshold, ten binding choices, disconnect/release and rebind gate.");
}
