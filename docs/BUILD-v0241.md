# 0.24.1 explicit aiming control

Base: accepted English0.18.1. This remains the same isolated native aiming
experiment as0.24; its lens behavior has not been accepted. Branch:
`experiment/native-zoom-target`. Status: BUILT, CHECKED, EXPERIMENTAL;
INSTALLED; not user-validated. See [actual delivery](DELIVERY-v0241.md).

## One objective

Make the aiming option's saved setting and menu feedback explicit. The player
clarifies that the line stays NATIVE after Ctrl+F9. The real INI has
NativeAimZoom=1 and earlier runtime target conversions exist; these observations
do not establish what was displayed at the time or why the player saw NATIVE.
No contradictory report is dismissed and no camera fix is inferred.

The aim row now reads `Proportional aim zoom: ON` in green or
`Proportional aim zoom: OFF` in grey. OFF uses the original aiming behavior.
The unrelated generic `Native camera retained`
message becomes `Player settings active`. Ctrl+F9 keeps its default and remains
rebindable. Each recognized request, persistence result and displayed state
is journaled. A save failure keeps the previous live state. The test harness
now invokes the same persistence implementation as production instead of an
always-successful stub, and reads back the real test INI.

The native lens policy and all other functions, existing bindings and settings
are unchanged. This is control/feedback work, not another camera adapter.
Health, grants, weapons/ammo, HUD, reticle, intros, speed and menus are retained.

## Success criterion and boundaries

With the main menu open, Ctrl+F9 must visibly alternate green ON / grey OFF;
the corresponding INI value and log must agree, including after restarting.
Tests must cover held presses, plainF9 separation, persistence failure and
all retained controls/menu bounds. The player's real visual result is required.
Only after this UI check can eventual held-aim FOV be compared meaningfully.

No independent first-person correction or successful proportional zoom is
claimed. The accepted18.1 backup remains the rollback reference. Preserve the
entire current INI and keys on installation. Do not reuse a rejected-binary
artifact based on its branch name; compile and verify the explicit new payload.

## Completed checks

Final production x86 ASI compiles without warnings/errors; SHA256
`61523562EF1018A9F271EE259C287248CE21A966952A53C19E19D5F99F245F8A`.
The retained native target/transition, grants, vitals, controls, UI and graphics
suite passes. Real production profile APIs now cover ON/OFF writes and restart
reads, held-key behavior, plainF9 separation, empty/directory-path save refusal
and unchanged live state after failure. Main and shortcut menus fit720p/4K/
ultrawide; ON/OFF720p previews are visually inspected. Private receipts:
tools/v0241-build.log, v0241-tests-build.log and v0241-tests.log.
The25-entry root-layout ZIP and extracted payloads are checked. All six final
installer/rollback fixtures pass after confirmed closure. Actual installation
preserves the complete current INI and has an independently verified accepted
18.1 ASI backup. See DELIVERY-v0241 for the exact receipt and rollback command.
