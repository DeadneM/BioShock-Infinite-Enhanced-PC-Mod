# 0.31 signed native arms / weapon distance

Base: 0.30.1, implementationa212850 and delivery144ee62. The player confirms the closer position at10 is good and reports all sensitivity parameters appear functional. This build extends that accepted native placement path only.

Objective: signed distance -50 through+50, decrease/increase hotkeys, preserving the existing closer setting and native ADS behavior.
Success: Ctrl+Minus reduces camera distance, Ctrl+Plus increases it, endpoints clamp, and the old10 closer position is unchanged at-10. Both arms and weapon must continue to move together. Extended positions and effect alignment require player testing.

## Controls and storage

Ctrl+Minus: decrease signed distance by2 (closer). Ctrl+Plus: increase by2 (farther). Ctrl+NumMinus / Ctrl+NumPlus work with default bindings; Ctrl+Shift+Plus is an accepted default alias. Remapping either action removes its default aliases. Plain Minus/Plus retain world FOV control. Ctrl+0 is retired. Both new actions remain reconfigurable in the existing shortcut editor. Other bindings are retained.

Negative=closer, positive=farther, zero=native. Range includes all integer values -50..50 for INI preferences, with keyboard steps2 and clamping at endpoints. New preference [FirstPerson] DistanceOffset; default0. If absent, a valid old0.30.1 BackOffset is read as its negative without rewriting the personal INI. Old10 becomes-10 and produces identical local-X displacement. Once DistanceOffset is explicitly saved, it takes precedence even when zero. Legacy values remain untouched for rollback.

No FOV, projection, mesh-scale, camera animation, HUD, input-profile or per-weapon changes. Existing native root identity/thread/living-pawn/cinematic guards, transient offset restoration and ADS/zoom recovery bypass remain in place. This extends the user-confirmed joint rig movement; it does not establish every temporary effect's alignment.

Player test: keep the same FOV. The initial menu value should be-10 if old BackOffset was10. Try Ctrl+Minus toward-12/-14, then Ctrl+Plus back to-10 and toward0. Fire/reload/aim and verify both arms and weapon, attached parts and flash. Larger signed offsets are experimental. Native ADS placement remains deliberate.

Rollback restores the exact0.30.1 module while keeping personal settings. No automatic game launch or public release. See DELIVERY-v031.md for actual checks.
