# 0.28.0 live FOV and controller hold aim

Objective: retain the0.27 gameplay the player reports as good, restore immediate
world FOV and proportional aim, and extend hold-to-aim to configurable XInput.
Success: world FOV responds without restart; aim transitions/recovery follow the
selected lens; LT press/release works with no stuck aim after menus, rebinds or
disconnect. Actual player confirmation is separate from fixtures/publication.

## Base and deliberate scope

Base is0.27 source baa1715, whose remaining behavior the player reports good.
Restore only the camera-result/zoom-target policy from retained0.24.1 checkpoint
63f87fe. That checkpoint had a player-visible ON/OFF effect, not proof of exact
held zoom. This release is EXPERIMENTAL pending renewed game testing.

No Panini, lens distortion, green masks, per-weapon profiles or viewmodel/render
projection adapter. Arms/weapon perspective is not repaired or claimed repaired.
The requested live world lens is a camera adapter, not the native options slider.

## Controls

- Minus/Plus: live world FOV80..130 by5 degrees; existing custom shortcuts and
  keypad aliases retained. Saves Display.FOV. No restart/special launcher.
- Ctrl+F9: proportional aim zoom ON/OFF; saves Display.NativeAimZoom. Existing
  setting preserved; default for missing/new configuration ON.
- F10: existing hold-to-aim toggle now covers mouse and controller together.
- F11: existing keyboard/mouse aim-key choice.
- Ctrl+F11: controller aim button cycle LT,RT,LB,RB,L3,R3,A,B,X,Y. Default LT.
- Ctrl+F10: retain global commands ON by default. Existing bindings editor can
  remap the new ControllerAimKey action and every existing keyboard action.
- F9: retain native HUD/menu scaling for the next launch; not a live HUD rewrite.

HoldAim ON is required for hold enforcement on both input types; the player's
existing choice is preserved. ControllerAimButton is stored by name in Controls.
ControllerAimThreshold defaults30; triggers count pressed above the threshold.
All four XInput user slots are polled; release occurs when neither the mouse aim
key nor a matching controller button remains down. Hold aim does not intercept
native controller bindings: select the same button in the game's controls.
DirectInput-only devices require an XInput-compatible mapping; no vendor drivers
or third-party remapper is bundled. XInput DLLs load only from Windows System32.

## Mechanism and safeguards

Reuse exact supported native signatures, thread/actor ownership/reflection guards,
active-weapon and camera callsite checks. Return-value policies preserve native
weapon properties and engine interpolation. Proportional target obeys
tan(target/2)/tan(requestedHip/2)=tan(nativeTarget/2)/tan(nativeHip/2).
An unavailable native hip baseline retains the original target until qualified.
No repeated per-weapon edits or shared GPU matrix mutation.

Gamepad physical state joins the existing hold-state machine. Unfocused/open-mod-
menu, rebind and actor changes require release before rearming. Disconnect turns
the controller input into released; original native start/stop methods run on the
already verified game-thread update seam. No controller messages are synthesized.

Legacy NativeFOVSlider is ignored/preserved. No native-option preparer/launcher is
needed or included in this package. Use the normal game launch after installation.
Existing native options, mod INI and custom bindings are preserved byte-for-byte
at installation; new defaults require no rewrite. No game launch is automatic.

## Validation, installation and rollback

Build, ABI/math/state fixtures, UI checks, ZIP checks, installation and actual
gameplay are distinct statuses. See DELIVERY-v028.md. The user authorized a public
experimental release, not a claim that these new changes are user-validated.

Install only with the game closed. The transactional installer saves the actual
previous module and preserves settings/loader. After normal closure, Rollback.ps1
with that backup restores the preceding module; later INI edits are retained.
Old native slider/cap backups are not rewritten by0.28.
