# 0.30.1 native first-person position toward camera

Base: 0.30 native placement implementation, source ce7b196 and delivery9f96b8f. The player reports all sensitivity parameters appear to work, and confirms both arms and weapon move together. This does not independently validate all temporary firing/reload effects. The player requests the opposite direction: move the rig closer, not farther away.

Objective: reverse the existing root offset toward the camera while preserving its bounded controls, native ADS position, restoration and accepted gameplay/input features.
Success criterion: the same positive offset value visibly moves both arms and weapon closer than0, with acceptable clipping and aligned reload/firing effects. ADS remains at original position.

Controls unchanged: Ctrl+0 cycles0,2,4,6,8,10,0; the key editor still remaps WeaponBack. The menu now says Toward camera. Existing [FirstPerson] BackOffset values and custom keybinds are preserved. Zero remains original. The new default remains0. Start at2 before trying4; higher values can cause clipping.

Only the sign of local X changes: subtract the selected amount during the same qualified native root-placement call, then restore it. No per-weapon, mesh-scale, projection, FOV, HUD, animation, input-profile or save changes. ADS and zoom recovery bypass the offset exactly as before. Missing/invalid settings still default0; nested calls do not accumulate; native field changes are not overwritten.

Local checks must repeat native aligned ret8 trampoline, reflected owner/Vector guards, toward-camera sign, restoration/reentrancy, ADS/cinematic/death/thread gates, persistence, remapping and retained UI checks. These are not a player validation of this new direction.

Player test: launch normally when ready, load the same save and keep FOV unchanged. Open the usual menu, set Toward camera to2 or4, close it, then compare0. Fire/reload the revolver and aim/release. Report whole-rig movement, clipping and alignment of hands, cartridges, speedloader and flash.

Rollback must restore the exact installed0.30 ASI and preserve the personal INI. No automatic launch or public release. Actual final results are in DELIVERY-v0301.md.
