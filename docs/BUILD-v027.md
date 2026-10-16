# 0.27.0 global hotkeys and native display preferences

Objective: use existing actions with the mod menu closed and provide shortcuts
for native FOV-slider/HUD preferences, retiring the green rendering diagnostic.
Success: default ON outside-menu commands; persistent/rebindable toggle; no
commands while unfocused or editing bindings; next-launch native FOV/HUD values
with unrelated configuration preserved. In-game confirmation remains separate.

Base: accepted English0.18.1 gameplay through retained63f87fe and authorized
native-camera restart. Existing0.26 evidence is retained in documentation, but
the green include, hooks, controls and mask replay are absent from this runtime.
No camera/ADS override, lens, Panini, projection correction or per-weapon patch.

## Controls and persistence

- Ctrl+F10: Hotkeys without menu ON/OFF. Missing Controls.GlobalHotkeys defaults
  ON. The key can be reassigned in the existing shortcut editor.
- Minus/Plus (and retained keypad aliases): native FieldOfView slider preference
  by5 percentage points, bounded0..100. This is not a degree measurement.
- F9: existing native HUD/menu scaling ceiling cycle, applied on next launch.
- Existing menu/editor keys and all prior custom assignments are preserved.

The shortcut editor still blocks gameplay commands. Unfocused/held key presses
are consumed; activation requires a new press with the game focused. Native game
key handling is unchanged. HUD and FOV shortcuts obey the global toggle as other
actions do. The toggle itself remains available when global shortcuts are OFF.

NativeFOVSlider=-1 or a missing preference leaves the game option alone. Runtime
reads the current normalized native FieldOfView only for initial display; an
explicit +/- change saves the mod preference. An unavailable native value does
not invent an angle. Legacy Display.FOV and NativeAimZoom are retained untouched.

## Applying display preferences

Close the game normally. Launch-Mod.ps1 -GameDirectory '<game path>' uses the
checked NativeFOV.exe apply-slider command before starting the original game.
Only existing FieldOfView in XCore.XUserOptionsManager is changed to slider/100.
MaxUserFOVOffsetPercent and every unrelated token are preserved. A normal launch
without this launcher does not apply a newly saved slider preference.

No live native-option write or camera hook is installed. HUDScale continues to
use the already retained native UI configuration preparation at startup. The
menu explicitly states that both display settings require the next launch.
No automatic game launch is authorized by installation or this document.

The native helper rejects a running game, malformed/missing/duplicate options,
invalid preferences and unsafe file paths. Slider backup is independently named
XUserOptions.ini.InfiniteMod-native-slider-backup; legacy cap backup is unchanged.
NativeFOV.exe restore-slider '<game path>' restores only the original slider token
while preserving later unrelated changes. The slider preference in the mod INI
must also be disabled/set to-1 if the launcher should stop reapplying it.

## Installation and rollback

Root ZIP includes installer, rollback, INI, ASI, verified loader, native helper,
launcher and docs. Existing mod INI/loader remain byte-identical at install.
The installer does not edit native options or start the game. It saves the actual
previous module. Rollback restores that module and preserves the player's latest
preferences; it does not reset the user-selected native slider. Use restore-slider
explicitly after normal closure if restoring that token is desired as well.

Status: EXPERIMENTAL. Build/fixture/package/install results are recorded separately
in DELIVERY-v027.md. No user validation or GitHub publication is implied.
