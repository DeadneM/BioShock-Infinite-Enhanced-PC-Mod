#pragma once
#include "../keybinds.h"

static SHORT keybindFixtureKeys[256];
static SHORT keybindFixtureQuery(int key) {return keybindFixtureKeys[key];}
static void keybindFixtureClear() {for(auto &key:keybindFixtureKeys)key=0;}
static void keybindFixtureDown(int key,bool down=true) {keybindFixtureKeys[key]=down?static_cast<SHORT>(0x8000):0;}
static void testKeybinds() {
    using namespace keybinds;
    using Chord=keybinds::Chord; // Windows GDI also exports a global Chord function.
    for(unsigned i=0;i<count;++i) {
        CHECK(validChord(definitions[i].defaultChord));
        Chord parsed;CHECK(parseChord(chordLabel(definitions[i].defaultChord),parsed));CHECK(parsed==definitions[i].defaultChord);
        for(unsigned j=i+1;j<count;++j)CHECK(!chordsConflict(static_cast<Action>(i),definitions[i].defaultChord,static_cast<Action>(j),definitions[j].defaultChord));
    }
    Chord candidate{99,0};
    CHECK(parseChord("  ctrl + SHIFT + f24 ",candidate) && (candidate==Chord{VK_F24,Control|Shift}));
    CHECK(parseChord("Alt+Mouse5",candidate) && (candidate==Chord{VK_XBUTTON2,Alt}));
    CHECK(parseChord("VK_0xBA",candidate) && candidate.key==VK_OEM_1);
    for(const char *value:{"","Ctrl","Ctrl++F1","Ctrl+Ctrl+F1","F1+Ctrl","F0","F25","VK_0xFF","Ctrl+nope","None"}) {
        candidate={99,0};CHECK(!parseChord(value,candidate));CHECK((candidate==Chord{99,0}));
    }

    keybindFixtureClear();Engine engine;
    auto poll=[&](bool focused=true,bool menu=true,bool block=false) {return engine.pollEdges(focused,menu,block,keybindFixtureQuery);};
    CHECK(poll().bits==0); // A focus transition never invents an input.
    keybindFixtureDown(VK_F1);Events first=poll();CHECK(first.has(Action::Vitals) && !first.has(Action::Powers));
    CHECK(poll().bits==0); // Held keys have no autorepeat.
    keybindFixtureDown(VK_CONTROL);CHECK(poll().bits==0); // Modifiers cannot convert a held F1 into a new press.
    keybindFixtureDown(VK_F1,false);poll();keybindFixtureDown(VK_F1);
    first=poll();CHECK(first.has(Action::Powers) && !first.has(Action::Vitals));
    keybindFixtureDown(VK_SHIFT);CHECK(poll().bits==0);
    keybindFixtureDown(VK_F1,false);poll();keybindFixtureDown(VK_F1);CHECK(poll().bits==0); // Extra modifiers require an exact chord.
    keybindFixtureDown(VK_SHIFT,false);CHECK(poll().bits==0);
    keybindFixtureClear();poll();keybindFixtureDown(VK_RCONTROL);keybindFixtureDown(VK_F1);
    CHECK(poll().has(Action::Powers)); // Left/right modifier keys work independently of the generic VK.
    keybindFixtureClear();poll();keybindFixtureDown(VK_F2);CHECK(poll(true,false).bits==0);
    CHECK(poll(true,true).bits==0); // Opening a menu does not trigger a previously held action.
    keybindFixtureClear();poll();keybindFixtureDown(VK_F2);CHECK(poll(true,true,true).bits==0);
    CHECK(poll().bits==0); // A blocked press stays consumed.
    keybindFixtureClear();poll();keybindFixtureDown(VK_F2);CHECK(poll(false).bits==0);
    CHECK(poll(true).bits==0);CHECK(poll(true).bits==0); // Focus regain requires release.
    keybindFixtureClear();poll();keybindFixtureDown(VK_F2);CHECK(poll().has(Action::Ammo));
    keybindFixtureClear();poll();keybindFixtureDown(VK_CONTROL);keybindFixtureDown(VK_F8);
    first=poll(true,false);CHECK(first.has(Action::BindingsMenu) && !first.has(Action::Crosshair));
    keybindFixtureClear();poll();keybindFixtureDown(VK_INSERT);CHECK(poll(true,false).has(Action::MainMenu));
    keybindFixtureClear();poll();keybindFixtureDown(VK_SUBTRACT);CHECK(poll().has(Action::FovDecrease));
    keybindFixtureClear();poll();keybindFixtureDown(VK_ADD);CHECK(poll().has(Action::FovIncrease));
    keybindFixtureClear();poll();keybindFixtureDown(VK_SHIFT);keybindFixtureDown(VK_OEM_PLUS);CHECK(poll().has(Action::FovIncrease));
    RebindResult conflict=engine.rebind(Action::Vitals,{VK_F2,0});
    CHECK(!conflict.ok && conflict.conflict==Action::Ammo && (engine.binding(Action::Vitals)==Chord{VK_F1,0}));
    conflict=engine.rebind(Action::Vitals,{VK_SUBTRACT,0});CHECK(!conflict.ok && conflict.conflict==Action::FovDecrease);
    conflict=engine.rebind(Action::Vitals,{VK_OEM_PLUS,Shift});CHECK(!conflict.ok && conflict.conflict==Action::FovIncrease);
    CHECK(engine.rebind(Action::FovDecrease,{'J',0}).ok);
    CHECK(engine.rebind(Action::Vitals,{VK_SUBTRACT,0}).ok); // A custom FOV binding releases its default keypad alias.
    CHECK(!engine.rebind(Action::Count,{'A',0}).ok);
    CHECK(!engine.rebind(Action::Ammo,{VK_CONTROL,0}).ok);

    // Exercise actual Windows profile APIs in a private, absolute fixture path.
    wchar_t directory[MAX_PATH],path[MAX_PATH];CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf(path,MAX_PATH,L"%ls\\keybinds-%lu-%lu.ini",directory,GetCurrentProcessId(),GetTickCount())>0);
    CHECK(WritePrivateProfileStringW(L"Other",L"Preserve",L"unchanged",path));
    Engine saved;
    for(unsigned i=0;i<count;++i)CHECK(saved.rebind(static_cast<Action>(i),{i<26?static_cast<unsigned>('A'+i):static_cast<unsigned>(VK_F1+i-26),Control|Shift}).ok);
    CHECK(saved.save(path));Engine loaded;LoadReport report=loaded.load(path);CHECK(!report.invalid && !report.conflicts);
    for(unsigned i=0;i<count;++i)CHECK(loaded.binding(static_cast<Action>(i))==saved.binding(static_cast<Action>(i)));
    wchar_t other[32];CHECK(GetPrivateProfileStringW(L"Other",L"Preserve",L"",other,32,path));CHECK(!wcscmp(other,L"unchanged"));
    CHECK(loaded.rebindAndSave(Action::MainMenu,{'Z',Alt},path).ok);
    Engine persisted;persisted.load(path);CHECK((persisted.binding(Action::MainMenu)==Chord{'Z',Alt}));
    Chord previous=persisted.binding(Action::MainMenu);
    CHECK(!persisted.rebindAndSave(Action::MainMenu,{'Q',Win},L"").ok);CHECK(persisted.binding(Action::MainMenu)==previous);
    CHECK(WritePrivateProfileSectionW(L"Keybinds",L"Vitals=F2\0Ammo=F1\0\0",path));
    report=loaded.load(path);CHECK(!report.invalid && !report.conflicts);
    CHECK((loaded.binding(Action::Vitals)==Chord{VK_F2,0}) && (loaded.binding(Action::Ammo)==Chord{VK_F1,0})); // Legitimate swaps survive loading.
    CHECK(WritePrivateProfileSectionW(L"Keybinds",L"Vitals=Ctrl++F1\0Ammo=NotAKey\0MainMenu=\u00e9\0\0",path));
    report=loaded.load(path);CHECK(report.invalid==3);
    CHECK((loaded.binding(Action::Vitals)==Chord{VK_F1,0}) && (loaded.binding(Action::Ammo)==Chord{VK_F2,0}));
    CHECK(WritePrivateProfileSectionW(L"Keybinds",L"Vitals=F2\0\0",path));
    report=loaded.load(path);CHECK(report.conflicts==2);
    CHECK((loaded.binding(Action::Vitals)==Chord{VK_F1,0}) && (loaded.binding(Action::Ammo)==Chord{VK_F2,0}));
    CHECK(WritePrivateProfileSectionW(L"Keybinds",L"MainMenu=F2\0Vitals=Insert\0Ammo=Insert\0\0",path));
    report=loaded.load(path);CHECK(report.conflicts==3);
    for(unsigned i=0;i<count;++i)CHECK(loaded.binding(static_cast<Action>(i))==definitions[i].defaultChord); // Cascading fallback does not leave duplicate shortcuts.
    CHECK(WritePrivateProfileSectionW(L"Keybinds",L"Vitals=NumPlus\0\0",path));
    report=loaded.load(path);CHECK(report.conflicts==2);
    CHECK(DeleteFileW(path));

    // Entering capture consumes the menu's Enter/Ctrl+F8 release, rejects
    // conflicts by action, supports rebinding Insert, and consumes completion.
    keybindFixtureClear();Engine capture;
    capture.pollEdges(true,false,false,keybindFixtureQuery);
    keybindFixtureDown(VK_RETURN);keybindFixtureDown(VK_CONTROL);capture.beginCapture(Action::MainMenu);
    auto scan=[&](bool focused=true) {return capture.pollCapture(focused,keybindFixtureQuery);};
    CHECK(scan().status==CaptureStatus::Waiting);
    keybindFixtureDown(VK_RETURN,false);CHECK(scan().status==CaptureStatus::Waiting);
    keybindFixtureDown(VK_CONTROL,false);CHECK(scan().status==CaptureStatus::Waiting);
    keybindFixtureDown(VK_F2);CaptureResult result=scan();CHECK(result.status==CaptureStatus::Conflict && result.conflict==Action::Ammo);
    CHECK(capture.binding(Action::MainMenu).key==VK_INSERT && capture.capturing());
    keybindFixtureClear();scan();keybindFixtureDown(VK_CONTROL);scan();keybindFixtureDown('K');
    result=scan();CHECK(result.status==CaptureStatus::Captured && (result.candidate==Chord{'K',Control}));
    CHECK(capture.binding(Action::MainMenu).key==VK_INSERT); // A candidate is not yet a committed binding.
    CHECK(capture.rebind(result.action,result.candidate).ok);capture.cancelCapture();
    CHECK(capture.pollEdges(true,false,false,keybindFixtureQuery).bits==0); // Captured press cannot immediately toggle the menu.
    keybindFixtureClear();capture.pollEdges(true,false,false,keybindFixtureQuery);
    keybindFixtureDown(VK_CONTROL);keybindFixtureDown('K');CHECK(capture.pollEdges(true,false,false,keybindFixtureQuery).has(Action::MainMenu));
    keybindFixtureClear();capture.beginCapture(Action::Ammo);scan();keybindFixtureDown(VK_ESCAPE);
    result=scan();CHECK(result.status==CaptureStatus::Cancelled && !capture.capturing());
    CHECK(capture.binding(Action::Ammo).key==VK_F2);
    keybindFixtureClear();capture.beginCapture(Action::BindingsMenu);scan();
    keybindFixtureDown('B');CHECK(scan(false).status==CaptureStatus::Waiting);
    CHECK(scan(true).status==CaptureStatus::Waiting); // Returning to focus cannot capture a background key.
    keybindFixtureClear();scan();keybindFixtureDown('C');result=scan();CHECK(result.status==CaptureStatus::Captured && result.candidate.key=='C');
    capture.cancelCapture();
    keybindFixtureClear();puts("PASS: configurable exact chords, defaults, menu/focus gates, physical press edges, conflict-safe INI persistence and release-gated capture.");
}
