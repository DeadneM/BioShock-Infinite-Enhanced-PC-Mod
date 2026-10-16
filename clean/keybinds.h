#pragma once
#include <windows.h>
#include <string>
#include <cwchar>
#include <cstdint>
#include <cstdlib>
#include <cctype>
#include <cstdio>

// The engine owns input history. Use one Engine instance for the overlay; this
// header has no mutable global state shared between translation units.
namespace keybinds {
enum class Action : unsigned {
    MainMenu, BindingsMenu, Vitals, Ammo, WeaponPrevious, WeaponNext, GiveMain,
    GiveSecondary, Speed, SkipIntros, Powers, Gear, WeaponUpgrades, VigorUpgrades,
    Infusions, Money, Lockpicks, Crosshair, HUD, HoldAim, AimKey, FovDecrease,
    FovIncrease, NativeAimZoom, GlobalHotkeys, Count
};
constexpr unsigned count=static_cast<unsigned>(Action::Count);
enum Modifier : unsigned { Control=1, Alt=2, Shift=4, Win=8 };
struct Chord { unsigned key=0,modifiers=0; };
inline bool operator==(Chord a,Chord b) {return a.key==b.key && a.modifiers==b.modifiers;}
inline bool operator!=(Chord a,Chord b) {return !(a==b);}
struct Definition { const wchar_t *iniName; const char *label; Chord defaultChord; bool requiresMenu; };
inline constexpr Definition definitions[count]={
    {L"MainMenu","Open / close the main menu",{VK_INSERT,0},false},
    {L"BindingsMenu","Open / close the shortcut menu",{VK_F8,Control},false},
    {L"Vitals","Refill health, shield, and salts",{VK_F1,0},true},
    {L"Ammo","Refill ammo",{VK_F2,0},true},
    {L"WeaponPrevious","Previous weapon",{VK_F3,0},true},
    {L"WeaponNext","Next weapon",{VK_F4,0},true},
    {L"GiveMain","Give weapon in the main slot",{VK_F5,0},true},
    {L"GiveSecondary","Give weapon in the secondary slot",{VK_F6,0},true},
    {L"Speed","Change speed",{VK_F7,0},true},
    {L"SkipIntros","Skip startup logos",{VK_F12,0},true},
    {L"Powers","All Vigors",{VK_F1,Control},true},
    {L"Gear","All gear",{VK_F2,Control},true},
    {L"WeaponUpgrades","All weapon upgrades",{VK_F3,Control},true},
    {L"VigorUpgrades","All Vigor upgrades",{VK_F4,Control},true},
    {L"Infusions","Maximize infusions",{VK_F5,Control},true},
    {L"Money","Maximize money",{VK_F6,Control},true},
    {L"Lockpicks","Maximize lockpicks",{VK_F7,Control},true},
    {L"Crosshair","Show / hide crosshairs",{VK_F8,0},true},
    {L"HUD","Change interface size",{VK_F9,0},true},
    {L"HoldAim","Enable / disable hold-to-aim",{VK_F10,0},true},
    {L"AimKey","Change aim key",{VK_F11,0},true},
    {L"FovDecrease","Decrease native FOV slider (restart)",{VK_OEM_MINUS,0},true},
    {L"FovIncrease","Increase native FOV slider (restart)",{VK_OEM_PLUS,0},true},
    {L"NativeAimZoom","Reserved aim option (native zoom active)",{VK_F9,Control},true},
    {L"GlobalHotkeys","Hotkeys without the menu: ON / OFF",{VK_F10,Control},false}
};
inline bool validAction(Action action) {return static_cast<unsigned>(action)<count;}
struct ChordList {Chord chords[3];unsigned size;};
inline ChordList effectiveChords(Action action,Chord chord) {
    // Preserve the two keypad shortcuts that the validated overlay accepted.
    // A custom FOV binding has only its selected chord, without a hidden alias.
    if(action==Action::FovDecrease && chord==definitions[static_cast<unsigned>(action)].defaultChord)
        return {{chord,{VK_SUBTRACT,0},{}},2};
    if(action==Action::FovIncrease && chord==definitions[static_cast<unsigned>(action)].defaultChord)
        return {{chord,{VK_ADD,0},{VK_OEM_PLUS,Shift}},3};
    return {{chord,{},{}},1};
}
inline bool chordsConflict(Action a,Chord first,Action b,Chord second) {
    ChordList aa=effectiveChords(a,first),bb=effectiveChords(b,second);
    for(unsigned i=0;i<aa.size;++i)for(unsigned j=0;j<bb.size;++j)if(aa.chords[i]==bb.chords[j])return true;
    return false;
}
inline bool modifierKey(unsigned key) {
    return key==VK_CONTROL || key==VK_LCONTROL || key==VK_RCONTROL ||
        key==VK_MENU || key==VK_LMENU || key==VK_RMENU || key==VK_SHIFT ||
        key==VK_LSHIFT || key==VK_RSHIFT || key==VK_LWIN || key==VK_RWIN;
}
inline bool validChord(Chord chord) {
    return chord.key>0 && chord.key<255 && !modifierKey(chord.key) && (chord.modifiers&~15u)==0;
}
inline std::string trimToken(const std::string &value) {
    size_t a=value.find_first_not_of(" \t\r\n"),b=value.find_last_not_of(" \t\r\n");
    return a==std::string::npos ? std::string() : value.substr(a,b-a+1);
}
inline std::string lowerToken(std::string value) {
    for(char &c:value)c=static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}
struct NamedKey {unsigned key;const char *name;};
inline constexpr NamedKey namedKeys[]={
    {VK_INSERT,"Insert"},{VK_DELETE,"Delete"},{VK_HOME,"Home"},{VK_END,"End"},
    {VK_PRIOR,"PageUp"},{VK_NEXT,"PageDown"},{VK_UP,"Up"},{VK_DOWN,"Down"},
    {VK_LEFT,"Left"},{VK_RIGHT,"Right"},{VK_RETURN,"Enter"},{VK_ESCAPE,"Escape"},
    {VK_BACK,"Backspace"},{VK_TAB,"Tab"},{VK_SPACE,"Space"},{VK_CAPITAL,"CapsLock"},
    {VK_SNAPSHOT,"PrintScreen"},{VK_SCROLL,"ScrollLock"},{VK_PAUSE,"Pause"},
    {VK_NUMLOCK,"NumLock"},{VK_APPS,"Menu"},{VK_LBUTTON,"Mouse1"},
    {VK_RBUTTON,"Mouse2"},{VK_MBUTTON,"Mouse3"},{VK_XBUTTON1,"Mouse4"},{VK_XBUTTON2,"Mouse5"},
    {VK_OEM_MINUS,"Minus"},{VK_OEM_PLUS,"Plus"},{VK_SUBTRACT,"NumMinus"},
    {VK_ADD,"NumPlus"},{VK_MULTIPLY,"NumMultiply"},{VK_DIVIDE,"NumDivide"},
    {VK_DECIMAL,"NumDecimal"},{VK_OEM_COMMA,"Comma"},{VK_OEM_PERIOD,"Period"},
    {VK_OEM_1,"Semicolon"},{VK_OEM_2,"Slash"},{VK_OEM_3,"Backtick"},
    {VK_OEM_4,"LeftBracket"},{VK_OEM_5,"Backslash"},{VK_OEM_6,"RightBracket"},
    {VK_OEM_7,"Quote"},{VK_OEM_102,"OEM102"}
};
inline std::string keyName(unsigned key) {
    for(const auto &entry:namedKeys)if(entry.key==key)return entry.name;
    if((key>='A' && key<='Z') || (key>='0' && key<='9'))return std::string(1,static_cast<char>(key));
    if(key>=VK_F1 && key<=VK_F24)return "F"+std::to_string(key-VK_F1+1);
    if(key>=VK_NUMPAD0 && key<=VK_NUMPAD9)return "Num"+std::to_string(key-VK_NUMPAD0);
    char output[16];std::snprintf(output,sizeof(output),"VK_0x%02X",key);return output;
}
inline bool parseKey(const std::string &input,unsigned &key) {
    std::string value=lowerToken(trimToken(input));
    for(const auto &entry:namedKeys)if(value==lowerToken(entry.name)){key=entry.key;return true;}
    if(value.size()==1 && ((value[0]>='a' && value[0]<='z') || (value[0]>='0' && value[0]<='9'))) {
        key=static_cast<unsigned>(std::toupper(static_cast<unsigned char>(value[0])));return true;
    }
    if(value.size()>=2 && value[0]=='f') {
        char *end=nullptr;unsigned long number=std::strtoul(value.c_str()+1,&end,10);
        if(end!=value.c_str()+1 && !*end && number>=1 && number<=24){key=VK_F1+static_cast<unsigned>(number)-1;return true;}
    }
    if(value.size()==4 && value.compare(0,3,"num")==0 && value[3]>='0' && value[3]<='9') {
        key=VK_NUMPAD0+static_cast<unsigned>(value[3]-'0');return true;
    }
    if(value.compare(0,5,"vk_0x")==0 && value.size()==7) {
        char *end=nullptr;unsigned long number=std::strtoul(value.c_str()+5,&end,16);
        if(end!=value.c_str()+5 && !*end && number>0 && number<255){key=static_cast<unsigned>(number);return true;}
    }
    return false;
}
inline std::string chordLabel(Chord chord) {
    if(!validChord(chord))return "Invalid";
    std::string value;
    if(chord.modifiers&Control)value+="Ctrl+";
    if(chord.modifiers&Alt)value+="Alt+";
    if(chord.modifiers&Shift)value+="Shift+";
    if(chord.modifiers&Win)value+="Win+";
    return value+keyName(chord.key);
}
inline bool parseChord(const std::string &input,Chord &output) {
    Chord candidate;std::string value=trimToken(input);bool haveKey=false;
    if(value.empty())return false;
    for(size_t pos=0;pos<=value.size();) {
        size_t end=value.find('+',pos);if(end==std::string::npos)end=value.size();
        std::string token=lowerToken(trimToken(value.substr(pos,end-pos)));unsigned mod=0;
        if(token=="ctrl" || token=="control")mod=Control;
        else if(token=="alt")mod=Alt;
        else if(token=="shift")mod=Shift;
        else if(token=="win")mod=Win;
        if(token.empty() || haveKey)return false;
        if(mod) {if(candidate.modifiers&mod)return false;candidate.modifiers|=mod;}
        else {if(!parseKey(token,candidate.key))return false;haveKey=true;}
        if(end==value.size())break;
        pos=end+1;
    }
    if(!haveKey || !validChord(candidate))return false;
    output=candidate;return true;
}
using KeyQuery=SHORT (*)(int);
inline SHORT systemQuery(int key) {return GetAsyncKeyState(key);}
struct Snapshot {
    bool down[256]={};unsigned modifiers=0;
    explicit Snapshot(KeyQuery query) {
        for(unsigned key=1;key<255;++key)down[key]=(query(static_cast<int>(key))&0x8000)!=0;
        if(down[VK_CONTROL] || down[VK_LCONTROL] || down[VK_RCONTROL])modifiers|=Control;
        if(down[VK_MENU] || down[VK_LMENU] || down[VK_RMENU])modifiers|=Alt;
        if(down[VK_SHIFT] || down[VK_LSHIFT] || down[VK_RSHIFT])modifiers|=Shift;
        if(down[VK_LWIN] || down[VK_RWIN])modifiers|=Win;
    }
    bool anyDown()const {for(unsigned key=1;key<255;++key)if(down[key])return true;return false;}
};
struct Events {
    uint64_t bits=0;
    bool has(Action action)const {return validAction(action) && (bits&(uint64_t(1)<<static_cast<unsigned>(action)))!=0;}
};
struct RebindResult {bool ok=false;Action conflict=Action::Count;const char *reason="Invalid action or shortcut";};
struct LoadReport {unsigned invalid=0,conflicts=0;};
enum class CaptureStatus {Waiting,Captured,Cancelled,Conflict,Invalid};
struct CaptureResult {CaptureStatus status=CaptureStatus::Waiting;Action action=Action::Count;Chord candidate;Action conflict=Action::Count;};
class Engine {
    Chord bindings_[count];bool previous_[256]={},capturePrevious_[256]={};
    bool focused_=false,releaseGate_=false,capturing_=false,captureReady_=false;
    Action captureAction_=Action::Count;
    void remember(const Snapshot &snapshot,bool *target) {for(unsigned i=0;i<256;++i)target[i]=snapshot.down[i];}
public:
    Engine() {resetDefaults();}
    void resetDefaults() {
        for(unsigned i=0;i<count;++i)bindings_[i]=definitions[i].defaultChord;
        for(unsigned i=0;i<256;++i)previous_[i]=capturePrevious_[i]=false;
        focused_=false;releaseGate_=false;capturing_=false;captureReady_=false;captureAction_=Action::Count;
    }
    Chord binding(Action action)const {return validAction(action)?bindings_[static_cast<unsigned>(action)]:Chord{};}
    std::string bindingLabel(Action action)const {return chordLabel(binding(action));}
    RebindResult checkRebind(Action action,Chord chord)const {
        if(!validAction(action) || !validChord(chord))return {};
        for(unsigned i=0;i<count;++i)if(i!=static_cast<unsigned>(action) && chordsConflict(action,chord,static_cast<Action>(i),bindings_[i]))
            return {false,static_cast<Action>(i),"Shortcut already assigned"};
        return {true,Action::Count,""};
    }
    RebindResult rebind(Action action,Chord chord) {
        RebindResult result=checkRebind(action,chord);
        if(result.ok){bindings_[static_cast<unsigned>(action)]=chord;releaseGate_=true;}
        return result;
    }
    // Save before publishing the new binding, so a write failure leaves the
    // current shortcut usable and the screen can show an explicit error.
    RebindResult rebindAndSave(Action action,Chord chord,const wchar_t *modIni) {
        RebindResult result=checkRebind(action,chord);if(!result.ok)return result;
        std::string value=chordLabel(chord);std::wstring wide(value.begin(),value.end());
        if(!modIni || !*modIni || !WritePrivateProfileStringW(L"Keybinds",definitions[static_cast<unsigned>(action)].iniName,wide.c_str(),modIni))
            return {false,Action::Count,"Shortcut could not be saved"};
        bindings_[static_cast<unsigned>(action)]=chord;releaseGate_=true;return result;
    }
    LoadReport load(const wchar_t *modIni) {
        resetDefaults();LoadReport result;
        if(!modIni || !*modIni)return result;
        for(unsigned i=0;i<count;++i) {
            wchar_t value[128]={};DWORD length=GetPrivateProfileStringW(L"Keybinds",definitions[i].iniName,L"",value,128,modIni);
            if(!length)continue;
            std::string narrow;bool ascii=true;
            for(DWORD j=0;j<length;++j){if(value[j]>127)ascii=false;narrow+=static_cast<char>(value[j]);}
            Chord candidate;
            if(length>=127 || !ascii || !parseChord(narrow,candidate)){++result.invalid;continue;}
            bindings_[i]=candidate;
        }
        // Read the complete section first: legitimate key swaps are accepted.
        // Duplicate chords fall back together. Repeat because a fallback may
        // expose a conflict with another custom chord. Unique defaults converge.
        bool reported[count]={};
        for(unsigned round=0;round<count;++round) {
            bool duplicate[count]={},found=false;
            for(unsigned i=0;i<count;++i)for(unsigned j=i+1;j<count;++j)
                if(chordsConflict(static_cast<Action>(i),bindings_[i],static_cast<Action>(j),bindings_[j]))duplicate[i]=duplicate[j]=found=true;
            if(!found)break;
            for(unsigned i=0;i<count;++i)if(duplicate[i]) {
                bindings_[i]=definitions[i].defaultChord;
                if(!reported[i]){reported[i]=true;++result.conflicts;}
            }
        }
        return result;
    }
    bool save(const wchar_t *modIni)const {
        if(!modIni || !*modIni)return false;
        std::wstring section;
        for(unsigned i=0;i<count;++i) {
            std::string value=chordLabel(bindings_[i]);
            section+=definitions[i].iniName;section+=L'=';section.append(value.begin(),value.end());section+=L'\0';
        }
        section+=L'\0';return WritePrivateProfileSectionW(L"Keybinds",section.c_str(),modIni)!=FALSE;
    }
    Events pollEdges(bool focused,bool mainMenuOpen,bool blockActions=false,KeyQuery query=systemQuery) {
        Snapshot snapshot(query);Events events;
        if(!focused) {focused_=false;remember(snapshot,previous_);return events;}
        if(!focused_) {focused_=true;releaseGate_=snapshot.anyDown();remember(snapshot,previous_);return events;}
        if(releaseGate_) {
            if(!snapshot.anyDown())releaseGate_=false;
            remember(snapshot,previous_);return events;
        }
        if(!blockActions && !capturing_)for(unsigned i=0;i<count;++i) {
            ChordList options=effectiveChords(static_cast<Action>(i),bindings_[i]);
            if(!mainMenuOpen && definitions[i].requiresMenu)continue;
            for(unsigned n=0;n<options.size;++n) {
                const Chord chord=options.chords[n];
                if(snapshot.down[chord.key] && !previous_[chord.key] && snapshot.modifiers==chord.modifiers)
                    events.bits|=uint64_t(1)<<i;
            }
        }
        remember(snapshot,previous_);return events;
    }
    bool capturing()const {return capturing_;}
    Action captureAction()const {return captureAction_;}
    void beginCapture(Action action) {
        if(!validAction(action))return;
        capturing_=true;captureReady_=false;captureAction_=action;releaseGate_=true;
        for(bool &value:capturePrevious_)value=false;
    }
    void cancelCapture() {capturing_=false;captureReady_=false;captureAction_=Action::Count;releaseGate_=true;}
    // Capture returns a validated candidate without committing it. The caller
    // can use rebindAndSave, then cancelCapture only after a successful save.
    CaptureResult pollCapture(bool focused,KeyQuery query=systemQuery) {
        CaptureResult result;result.action=captureAction_;
        if(!capturing_)return result;
        Snapshot snapshot(query);
        if(!focused){captureReady_=false;remember(snapshot,capturePrevious_);return result;}
        if(!captureReady_) {
            if(!snapshot.anyDown())captureReady_=true;
            remember(snapshot,capturePrevious_);return result;
        }
        if(snapshot.down[VK_ESCAPE] && !capturePrevious_[VK_ESCAPE]) {
            result.status=CaptureStatus::Cancelled;cancelCapture();remember(snapshot,previous_);return result;
        }
        unsigned key=0,pressed=0;
        for(unsigned i=1;i<255;++i)if(!modifierKey(i) && snapshot.down[i] && !capturePrevious_[i]){key=i;++pressed;}
        remember(snapshot,capturePrevious_);
        if(!pressed)return result;
        result.candidate={key,snapshot.modifiers};
        if(pressed>1){result.status=CaptureStatus::Invalid;captureReady_=false;return result;}
        RebindResult check=checkRebind(captureAction_,result.candidate);
        result.conflict=check.conflict;
        if(!check.ok){result.status=check.conflict==Action::Count?CaptureStatus::Invalid:CaptureStatus::Conflict;captureReady_=false;return result;}
        result.status=CaptureStatus::Captured;captureReady_=false;return result;
    }
};
} // namespace keybinds
