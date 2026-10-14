# 0.23.0 - Native proportional aiming

October 14, 2026. Experimental candidate from accepted English 0.18.1 source
`99a75aa9e1a47543c418b2dc37b43a8883898c72`, isolated on
`experiment/native-zoom-v0.23`. Panini 0.22 and first-person/GPU/ADS 0.21
experiments are rejected and excluded. No player validation is claimed.

## One objective

Retain the game's native zoom magnification relative to the selected world
FOV, instead of dropping to an unadapted native angle during aiming. This is
a native camera calculation change. It does not change world perspective,
independently fix arms/weapons, or make distant NPCs larger at the same lens.

The accepted CalcFOV hook already receives the unmodified native calculation
at the qualified XCamera callsite. The new optional policy uses the last
unzoomed native result as its baseline, not a modified camera cache:

```text
tan(adaptedAim / 2) = tan(nativeAim / 2) * tan(requestedHip / 2) / tan(nativeHip / 2)
```

This preserves optical zoom magnification. For native hip82.5/aim40 and
requested hip120, the result is approximately71.421 degrees. It is not
40*120/82.5; multiplying angle labels does not preserve optical magnification.
Native Hor+ applies the same aspect factor to both tangents, so this ratio
remains valid on other viewport aspects. Existing saved FOV semantics remain
the horizontal 16:9 reference; no second aspect multiplier is introduced.

## Scope and controls

- Existing native CalcFOV signature/callsite, current playable local controller,
  pawn and owned camera guards are retained. Only the returned camera angle
  changes; no saved native properties, bones, transforms or GPU buffers are written.
- Native hip-fire output remains the requested80-130. Native aim transitions
  use their current unmodified lens and a known native hip baseline. Without
  that baseline, aiming passes through until the player has unzoomed.
- `[Display] ProportionalAim=0` is the default, including missing keys. OFF
  returns the accepted native-zoom policy. Ctrl+F9 toggles it with the main
  menu open. The new 24th action can be reassigned with all previous actions.
- Gameplay/camera identity changes discard the native baseline. A locked
  camera is bypassed: native AdvanceFOV can select LockedFOV instead of the
  calculated cache, and this adapter does not control that other source.
- Camera interpolation is native. Offline GOG review places the accepted
  callsite inside native XCamera interpolation at VA D38CA0, ending ret12;
  its call to controller CalcFOV is at D38D7C. Private disassembly is not shipped.
- Inventory, vitals, progression, infusions, crosshair hiding, native HUD,
  automatic logos, speed, hold-to-aim and existing shortcuts remain retained.

## Success criterion and player test

1. Load a playable save and keep the same weapon, scene and World FOV120.
2. Open the existing main-menu key (the player's custom Backtick is preserved).
   Ctrl+F9 compares Aim zoom: Native / Proportional (experimental).
3. Close the menu and aim/release repeatedly. ON must give a visibly gentler
   zoom, recover the chosen hip-fire view, and avoid a stuck or doubled lens.
4. Change weapon and repeat at100 and120. OFF must immediately restore the
   accepted aiming behavior. Confirm hip-fire framing, HUD/power effect/menu
   placement and retained commands remain unchanged.

The existing first-person size/distortion issue is outside this build's success
criterion and remains open. A successful return-value log or fixture is not
proof of the final rendered view; the player must confirm the visible result.
If ON is ineffective or introduces a defect, keep OFF and investigate that
specific native route before another build. Do not patch weapons individually.

## Status and checks

**BUILT, CHECKED, then REJECTED and ROLLED BACK.**
The player observed no ON/OFF difference. The game's ordinary aiming uses
LockedFOV and bypasses this adapter. See [REJECTED-v023.md](REJECTED-v023.md).
The objective/checks below are historical and do not establish a retained fix.
Checked ASI SHA256:
`67B8E723113C969C0B55496031B23646DBD2C77F92ACFA79C3DC502244B490F3`.

The retained integrated suite passed. New checks cover independent expected
angles, zoom-magnification invariance at4:3/16:9/21:9, native release transitions,
live FOV changes, missing baseline/identity reset and malformed inputs. The
actual x86 native prologue/trampoline fixture exercises2000 converted camera
returns, unmodified unrelated callers, locked-camera bypass and immediate OFF.
The menu/shortcuts fixture checks Ctrl+F9 versus plain F9; all24 actions remain
reachable/rebindable. Actual graphics/menu bounds/state/resize tests pass at
720p,4K and ultrawide. The720p menu image was visually inspected.

Production x86 compilation completed without warnings/errors. All six isolated
installation/rollback scenarios passed with this final ASI, using private
copies only. This establishes the stated technical checks, not in-game visual
success or compatibility outside the supported GOG executable.

The root-layout ZIP and extracted payload were verified, then installed with
an accepted-ASI backup and complete existing INI/loader preservation. The game
was relaunched for the player comparison. See the exact receipt below.

The final build/check/installation hashes and outcomes are recorded in
[DELIVERY-v023.md](DELIVERY-v023.md). No public 0.23 release is claimed.
The installer preserves the complete existing INI and loader and backs up the
accepted ASI. Normal closure is required before installation or rollback.

Next, after the aiming result is confirmed, establish one native shared
first-person placement/lens boundary for arms, weapons, reload props and
attached effects. This build does not claim that boundary has been found.
