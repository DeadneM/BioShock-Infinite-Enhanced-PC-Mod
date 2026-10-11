// Include after aim.cpp and the test CHECK macro.
static void testAim() {
    using namespace aim_detail;
    State s;
    auto step=[&](bool down,unsigned flags,bool wanted=true,bool allowed=true) {
        return s.sample(0x1000,0x2000,VK_MBUTTON,wanted,allowed,down,flags);
    };
    CHECK(step(false,0)==Action::None);
    CHECK(step(true,0)==Action::Start);
    CHECK(step(true,heldMask)==Action::None);
    CHECK(step(false,heldMask)==Action::Stop);
    CHECK(step(false,0)==Action::None);
    // A native toggle from a click too short for the poll must not stick.
    CHECK(step(false,0x10000)==Action::Stop);
    CHECK(step(true,0x10000)==Action::Start);
    CHECK(step(true,zoomMask,true,false)==Action::Stop);
    CHECK(step(true,0)==Action::None); // still down after focus comes back
    CHECK(step(false,0)==Action::None);
    CHECK(step(true,0)==Action::Start);
    CHECK(step(true,heldMask,false)==Action::Stop);
    CHECK(step(false,0x10000,false)==Action::None); // original toggle stays intact
    CHECK(step(true,0)==Action::None); // enabling while held needs a release
    CHECK(step(false,0)==Action::None);
    CHECK(step(true,0)==Action::Start);
    CHECK(s.sample(0x3000,0x4000,VK_MBUTTON,true,true,true,0)==Action::None);
    CHECK(s.sample(0x3000,0x4000,VK_MBUTTON,true,true,false,0)==Action::None);
    CHECK(s.sample(0x3000,0x4000,VK_MBUTTON,true,true,true,0)==Action::Start);
    CHECK(s.sample(0x3000,0x5000,VK_MBUTTON,true,true,true,heldMask)==Action::Stop);
    CHECK(s.sample(0x3000,0x5000,VK_RBUTTON,true,true,true,heldMask)==Action::Stop);
    CHECK(s.sample(0x3000,0x5000,VK_XBUTTON1,false,true,false,heldMask)==Action::Stop);
    // Verify signature rejection and acceptance without running engine code.
    BYTE *image=(BYTE *)VirtualAlloc(nullptr,0x650000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);CHECK(image);
    CHECK(!installAim(image));CHECK(!startZoom && !stopZoom);
    memcpy(image+0x4e4990,startSignature,sizeof(startSignature));
    memcpy(image+0x649e60,stopSignature,sizeof(stopSignature));
    CHECK(installAim(image));CHECK(startZoom && stopZoom);
    image[0x4e4990+1]^=1;CHECK(!installAim(image));CHECK(!startZoom && !stopZoom);
    VirtualFree(image,0,MEM_RELEASE);
    CHECK(!installAim(nullptr));
    puts("PASS: hold aim press/release, short clicks, focus/menu blocking, enable/key/world changes and signature drift.");
}
