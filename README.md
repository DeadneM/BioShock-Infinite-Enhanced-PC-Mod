# BioShock Infinite - Enhanced PC Mod

![InfiniteMod banner](assets/infinitemod-banner.png)

An unofficial, English-language ASI mod for **BioShock Infinite GOG 1.1.25.5165 (Win32)**. It adds an in-game overlay, configurable shortcuts, weapons and progression commands, separate sensitivity profiles, live world FOV and native arms/weapon positioning.

**Current public version: [v0.31.0 experimental](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/tag/v0.31.0-experimental).** The player approved this version. Support for other executable versions, Steam editions and DLC campaigns is not established.

[Download the installable ZIP](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.31.0-experimental/InfiniteMod-v0.31.0-signed-distance-experimental.zip) | [SHA256 file](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.31.0-experimental/InfiniteMod-v0.31.0-signed-distance-experimental.zip.sha256) | [16:9 cover artwork](assets/infinitemod-cover-16x9.png)

## Features

- **Overlay and shortcuts:** open the mod menu or a separate key editor; rebind all 34 actions, including both menu keys. Commands work with the mod menu closed by default and can be restricted to the menu.
- **Weapons and ammunition:** select from eight main weapons, give the selected weapon directly to either slot, and refill both weapons' magazines and reserve ammunition.
- **Progression commands:** grant Vigors, gear, weapon upgrades and Vigor upgrades; maximize health/salts/shield infusions, money and lockpicks through their native limits. Commands are explicit; nothing is granted automatically at launch.
- **Current-vitals refill:** restore health, shield and salts to their existing capacities. This is separate from increasing capacities with infusions.
- **Player speed:** x1, x1.25, x1.5, x1.75 or x2.
- **Live world FOV:** 80-130, adjustable immediately from the mod. Optional proportional aim zoom retains the native transition. No special launcher is needed.
- **Native arms/weapon position:** signed camera distance from **-50 to +50**. Negative brings the complete owned first-person rig closer; positive moves it farther away; zero is original. This is a position adjustment, not an independent viewmodel FOV or projection correction.
- **Six sensitivity profiles:** independent normal, ADS and sniper multipliers for mouse and controller, each defaulting to x1.
- **Hold-to-aim:** configurable mouse key and XInput button; **LT** is the default controller button. Enable hold-to-aim explicitly and match the game's own aim binding.
- **Interface controls:** hide/show weapon and power crosshairs; select compact native HUD/menu size or ceilings up to x1.25, x1.5 and x2.
- **Startup logos:** automatically skip the four logos while retaining the loading loop.

## Default shortcuts

Existing custom shortcuts take precedence over the defaults shown here. Open the shortcut editor to see your current assignments.

| Shortcut | Action |
| --- | --- |
| Insert | Open/close the mod menu |
| Ctrl+F8 | Open/close the shortcut editor |
| F1 | Refill current health, shield and salts |
| F2 | Refill magazine and reserve ammo for both weapons |
| F3 / F4 | Select the previous/next weapon |
| F5 / F6 | Give selected weapon in the main/secondary slot |
| F7 | Cycle player speed |
| F8 | Show/hide crosshairs |
| F9 | Cycle HUD/menu size - next launch |
| F10 | Enable/disable hold-to-aim |
| F11 | Select the mouse aim key |
| F12 | Enable/disable automatic logo skipping - next launch |
| - / + | Decrease/increase live world FOV by 5 |
| Ctrl+- / Ctrl++ | Decrease/increase arms/weapon distance by 2 |
| Ctrl+F1 | All Vigors |
| Ctrl+F2 | All gear |
| Ctrl+F3 | All weapon upgrades |
| Ctrl+F4 | All Vigor upgrades |
| Ctrl+F5 | Maximize health, salts and shield infusions |
| Ctrl+F6 / Ctrl+F7 | Money / lockpicks up to native caps |
| Ctrl+F9 | Proportional aim zoom ON/OFF |
| Ctrl+F10 | Hotkeys without the mod menu ON/OFF - default ON |
| Ctrl+F11 | Select the controller hold-aim button |
| Ctrl+1 / Ctrl+2 / Ctrl+3 | Mouse normal / ADS / sniper sensitivity |
| Ctrl+4 / Ctrl+5 / Ctrl+6 | Controller normal / ADS / sniper sensitivity |

Keypad Minus/Plus aliases work while their default bindings are retained. Ctrl+Shift+Plus is also accepted for distance increase on keyboards that need Shift to type `+`. Ctrl+0 was retired in v0.31. Commands trigger once per press.

### Rebind any action

Open **Ctrl+F8**, select an action with **Up/Down**, then press **Enter**. Release the keys and press the new shortcut; **Escape** cancels or goes back. Conflicts are rejected during editing. Changes persist in `[Keybinds]` in `Binaries/Win32/InfiniteMod.ini` and immediately update the menu labels. The editor is paginated automatically; all 34 actions are reachable. Mod commands are suppressed during key capture.

### Position, FOV and sensitivity

World FOV uses plain Minus/Plus. Arms/weapon distance uses Ctrl+Minus/Plus and clamps at -50/+50 instead of wrapping. The initial distance is zero on a fresh install. A saved v0.30.1 `BackOffset=10` loads as `DistanceOffset=-10`, preserving the same position without rewriting the old key. A saved `DistanceOffset` takes precedence, including zero.

**ADS and zoom recovery intentionally retain native arms/weapon placement.** This helps preserve sights alignment. Large offsets may need a smaller value if they expose geometry or clipping; not every weapon, animation or transient effect has been exhaustively validated.

Sensitivity values cycle through x0.10, x0.25, x0.50, x0.75, x1.00, x1.25, x1.50 and x2.00. Each is relative to the game's sensitivity. Mouse and controller use separate sets; ordinary ADS and sniper ADS have separate entries. Fresh defaults leave input unchanged.

Hold-to-aim supports LT, RT, LB, RB, L3, R3, A, B, X and Y. The selected mouse/controller input must match the game's control assignment. The mod releases its aim state when the menu opens or the game loses focus.

## Installation and rollback

1. Close the game normally.
2. Extract the release ZIP into a separate folder. It has `README.md`, installation scripts, licenses and `Binaries/Win32` at its root, with no enclosing folder.
3. From that extracted folder, run:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Installer.ps1 -GameDirectory 'P:\Games\BioShock Infinite'
```

Replace the example path with your game root. The installer verifies the supported EXE and package checksums, backs up existing mod files under `InfiniteMod-Backups/install-*`, and preserves an existing personal INI and compatible shared loader. A different `dinput8.dll` stops installation. Saves and the game EXE are not replaced. Launch normally using your usual shortcut; **v0.31 does not require `Launch-Mod.ps1` or `NativeFOV.exe`.**

To roll back, close the game and use the backup directory printed by the installer:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Rollback.ps1 -GameDirectory 'P:\Games\BioShock Infinite' -BackupDirectory 'P:\Games\BioShock Infinite\InfiniteMod-Backups\install-<timestamp-id>'
```

Rollback verifies the previous module and preserves recovery copies. It does not undo progression already written by a game autosave. Keep a prior save before using grants.

## Configuration and limits

- Mod preferences and bindings: `Binaries/Win32/InfiniteMod.ini`.
- Runtime log: `Binaries/Win32/InfiniteMod.log`.
- HUD/menu size and startup-logo changes apply at the **next launch**. World FOV, sensitivity and position changes are live.
- HUD scaling uses the game's native UI ceiling and aspect/resolution handling, not a fixed 4K layout. Power-effect/UI alignment is not separately corrected.
- Money and lockpicks are filled to live character caps; 1,000,000 dollars and 30 lockpicks were observed in the tested campaign. Above-cap values are not reduced.
- Infusions target 10/10 for health, salts and shield. Obtain the initial shield through the story first. F1 neither grants it nor revives a dead player.
- The catalog includes eight Vigors, 49 recognized fixed gear definitions, 48 retail upgrades for twelve campaign weapon variants and 16 Vigor upgrades. An unrecognized catalog, inventory or unconfirmed grant is refused or reported in the menu.
- This is a set of progression commands, **not a complete scripted New Game+ mode**.
- No Panini/lens filter, green-shader diagnostic, per-weapon projection patch, or bob/sway removal is active. Native high-FOV perspective is not presented as fixed.

## Validation

The player reported the sensitivity parameters working, confirmed the arms and weapon move together, accepted the closer position at 10 in v0.30.1 and approved v0.31. Earlier confirmed behavior includes the English menus, saved bindings, refill, crosshair hiding, infusions and automatic logo skip. Local tests cover native ABI/identity guards, restoration, bounds, migration, controls, UI bounds and installer/rollback behavior. These checks do not substitute for every hardware configuration or extreme-position visual test.

Published ZIP SHA256:

```text
FCB9A7DF21C9A7BF097A6F48E372C1B4615300490F635E686B39A42782F2657D
```

The published v0.31 payload and tag are retained unchanged. Documentation/artwork maintenance does not alter your installed ASI or personal INI.

## Source, tests and project records

Active release source: [`experiment/firstperson-distance`](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/tree/experiment/firstperson-distance). The immutable release tag is [`v0.31.0-experimental`](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/tree/v0.31.0-experimental). Stable and experimental branches remain separate.

The default branch retains its existing gameplay source checkpoint; switch to the active experimental source before building the features described here.

```powershell
git switch experiment/firstperson-distance
.\distribution\bootstrap.ps1
.\test.ps1
.\build.ps1
```

Zig 0.13.0 is pinned; ImGui and MinHook sources/licenses are included. Set `INFINITEMOD_ZIG` to an existing matching compiler if needed. Test outputs are isolated under ignored `tools/test-runs/`. The historical native-FOV utility is opt-in with `-BuildLegacyNativeFOV`; it is not part of the current runtime or package.

[Current build scope](docs/BUILD-v031.md) | [Verified delivery](docs/DELIVERY-v031.md) | [Maintenance audit](docs/AUDIT-20261026.md) | [History](docs/HISTORY.md) | [Technical notebook](docs/TECHNICAL-NOTEBOOK.md) | [TODO](docs/TODO.md) | [Historical README snapshot](docs/archive/README-through-v031.md) | [Artwork details](assets/README.md)

Community mod by DeadneM. Unofficial artwork; not affiliated with or endorsed by the game's publisher. Third-party dependency licenses are included in release ZIPs.
