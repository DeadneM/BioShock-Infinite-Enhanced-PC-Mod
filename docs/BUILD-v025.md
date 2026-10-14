# 0.25.0 - native FOV slider experiment

October 14, 2026. **EXPERIMENTAL. Build, package and gameplay status are recorded separately in DELIVERY-v025.md. No independent viewmodel correction is claimed.**

## Base, authorized change and one objective

Source starts from retained checkpoint `63f87fea42141b0055c3d126dfd853120390837f` on `experiment/native-fov-slider`. English 0.18.1 remains the broader accepted gameplay fallback. The player explicitly requested abandoning further draw observations and testing the game's own FOV option instead of our camera-result/aim-target overrides. The confirmed 0.24.1 aim control/effect is intentionally retired in this experiment, not silently promoted or described as broken. Its saved preference and binding remain intact for rollback.

One objective: widen the native FOV slider range while the game alone calculates camera FOV and aiming transitions. Success requires the player to confirm that the native slider changes the view, aiming/release recover normally, and revolver fire/reload plus retained gameplay/HUD/menus remain usable. The player must separately judge proportions; this build cannot promise that high-FOV perspective or arms/weapon distortion disappears.

## Exact behavior

- No CalcFOV result hook, weapon aim-target hook, GPU projection patch, draw observer, Panini or per-object/bone/attachment changes are compiled into the runtime.
- The retained speed attribute hook remains. Other gameplay grants, refill, infusions, held aiming, crosshair toggle, HUD preference, logos, overlay and editable bindings remain.
- Existing `Display.FOV` (80..130) is preserved as a range preference. The configured native percentage is `(FOV / 75 - 1) * 100`, using the previously observed native 75-degree base only as a migration/reference convention. For saved110, the cap is46.666667%; for120,60%; for130,73.333333%. This does not force an actual camera angle or assume every camera state has the same base.
- Only `[XCore.XUserOptionsManager] MaxUserFOVOffsetPercent` in the existing user `XUserOptions.ini` is edited. Native `FieldOfView` slider position, display mode, input and all unrelated settings are preserved. Native limits and context-specific camera/aim behavior still apply.
- The main menu displays `Native FOV range: +...%`. Existing plus/minus bindings save the range preference; restart through the included mod launcher to apply it before native option loading. The game options slider controls the actual framing within that range.
- Aiming is native. The old proportional-aim binding remains reassignable but reserved; pressing it does not change the saved `NativeAimZoom` preference. Old viewmodel keys remain inert and are not advertised as functional controls.

## Installation, launch and recovery

Extract the ZIP outside the game, close the game normally and run `Installer.ps1 -GameDirectory ...`. The installer preserves existing mod INI/keys and loader, backs up the previous ASI, checks the native helper hash and configures the single native option before launch. A missing, duplicate, malformed or unwritable native setting is refused; no new/reset native INI is generated.

The helper keeps the original full native file at `XUserOptions.ini.InfiniteMod-native-fov-backup`, never overwriting that first backup. It uses checked same-file identity, writer exclusion and atomic replacement. UTF-8 and UTF-16LE are supported. Restore changes only the managed percentage back to the backup's value; later native slider/other settings survive.

After a range change, use the supplied launcher:

```powershell
.\Launch-Mod.ps1 -GameDirectory 'C:\Path\BioShock Infinite'
```

It refuses while the game is running, verifies the supported EXE, installed candidate ASI and helper, configures the range, then starts the original game EXE. The runtime only reads the native setting for menu feedback; it does not race the game's option loading or rewrite that file while playing.

Use the rollback command printed by the installer after normal closure. For a 0.25 transaction, it also invokes the verified native helper to restore the original percentage before restoring module files. A helper/configuration failure stops before module restoration and preserves the backups. The previous actual installation was diagnostic0.24.2; the retained0.24.1 and accepted18.1 backups are distinct older checkpoints.

## Test once

Use the native FOV slider in game options; compare low and high positions, then shoot/reload the revolver and aim/release. Check the HUD/Vigor effect, Objectives menu and one retained gameplay shortcut. No trace, timed firing session or new live memory observation is requested. Record the native method as player-validated only after explicit confirmation; do not equate a successful build/configuration write with a visual fix.
