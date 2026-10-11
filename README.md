# InfiniteMod 0.18.0

ASI mod for **BioShock Infinite GOG x86**: overlay, weapons and progression,
player settings, and configurable shortcuts.

**BUILT and CHECKED. The player confirmed vitals refill and shortcut reassignment in-game.**
It starts from the latest user-validated base, 0.15. The FOV fixes from
0.16–0.17.1 were rejected and are kept on a separate branch.
**Weapon/hand FOV and aiming zoom at high FOV still need correction.**

## Installation

The installable ZIP contains README, scripts, licenses, and
`Binaries/Win32` directly, without an enclosing folder. **Extract it to a
separate folder outside the game**, close the game normally, then run this
from the extracted folder:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Installer.ps1 -GameDirectory 'C:\Path\BioShock Infinite'
```

Replace the path with your game directory. The installer checks the executable,
package files, and loader, then backs up the existing ASI and INI.
**Your existing INI is preserved**, along with any compatible loader already
installed. It refuses to replace a different `dinput8.dll`. After installation,
it displays the backup path and rollback command.

The scripts do not replace the executable, save files, or native game INIs.
At launch, the mod applies the HUD/logo preferences to the native INIs.
Granted items may be stored by normal game saves; rolling back the module
does not undo them.

See [installation and rollback instructions](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/blob/experiment/v0.18.0-bindings/distribution/INSTALLATION.md)
in the repository; the ZIP provides that document at its root as
`INSTALLATION.md`.
Repository: [DeadneM/BioShock-Infinite-Enhanced-PC-Mod](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod).
The ZIP and its availability are announced only after delivery checks.

## Default shortcuts

**Insert opens the main menu.** Use the commands below with this menu open,
Booker playable, and the game in the foreground. The game continues while
the menu is displayed. **Ctrl+F8 opens the shortcut menu directly.**

| Shortcut | Action |
| --- | --- |
| Insert | Open/close the main menu |
| Ctrl+F8 | Open/close the shortcut menu |
| F1 | Refill health, shield, and salts |
| F2 | Fill the magazine and reserve of both weapons |
| F3 / F4 | Select the previous/next weapon |
| F5 | Give the selected weapon in the active slot |
| F6 | Give the selected weapon in the secondary slot |
| F7 | Speed x1, x1.25, x1.5, x1.75, x2 |
| F12 | Enable/disable automatic logo skipping for the next launch |
| Ctrl+F1 | All Vigors |
| Ctrl+F2 | All gear with bonuses |
| Ctrl+F3 | All weapon upgrades |
| Ctrl+F4 | All Vigor upgrades |
| Ctrl+F5 | Maximize health/salts/shield infusions |
| Ctrl+F6 | Money up to the native cap |
| Ctrl+F7 | Lockpicks up to the native cap |
| F8 | Show/hide crosshairs |
| F9 | Interface size for the next launch |
| F10 | Enable/disable hold-to-aim |
| F11 | Select the aim key matching the game's controls |
| - / + | World FOV -/+5, from 80 to 130 |

F5 and F6 select their destination slot directly; the old slot-switching key
is no longer needed. Ctrl combinations are distinct from their unmodified
equivalents. Holding a key does not repeat the command.

## Change shortcuts

Press Ctrl+F8, then **Up/Down** to select an entry and **Enter** to edit it.
Release the keys, then press the new shortcut. Ctrl, Alt, or Shift can be
included. **Escape** cancels or goes back.

All **23 actions** can be reassigned, including Insert and Ctrl+F8.
The menu shows eight entries per page; Up/Down moves through all three pages.
A shortcut already assigned to another action is rejected. No mod commands
execute during capture. If saving fails, the previous shortcut is retained.

Changes take effect immediately and persist in the `[Keybinds]` section of
`Binaries/Win32/InfiniteMod.ini`. Main-menu labels follow the new shortcuts.
The game's own bindings remain those selected in its control settings.

## Weapons and progression

The eight main weapons can be given in either slot. An owned weapon is reused;
requests that would create a duplicate are rejected. F2 fills ammunition for
both weapons, magazine and reserve.

Grants cover eight Vigors, 49 recognized fixed gear definitions, 48 upgrades
for the twelve campaign weapons/variants, and 16 Vigor upgrades. Previously
acquired promotional equivalents are recognized. Existing items are not added
again. An unknown catalog, unconfirmed effect, or incompatible inventory
produces a warning in the menu.

Money and lockpicks follow the character's caps: **1,000,000 dollars** and
**30 lockpicks** observed in this campaign. Values above those caps are not
reduced. Ctrl+F5 adds missing infusions up to **10/10 for each of the three
gauges**. The initial shield must already have been obtained in the campaign.

**F1 fills current values up to their capacities; Ctrl+F5 increases those
capacities through infusions.** F1 does not revive a dead character or grant
the initial shield. Values and maxima are read again after the refill.
The player confirmed this refill in-game on October 11, 2026.

No items are granted automatically at launch. Commands may be retained by
an autosave; keep an earlier save to restore progression from before testing.

## HUD, aiming, speed, and FOV

F9 selects the native interface size: compact, x1.25, x1.5, or x2.
The engine applies it on the **next launch**, to the HUD and menus, through
`MaxResolutionUpScale`. Native aspect-ratio and resolution handling are kept.
Vigor effects are not resized separately; their alignment still needs a
visual check.

F8 uses native crosshair visibility. This feature and infusions at 10/10
were user-validated on 0.15.

For hold-to-aim, enable F10, then use F11 to select **the same aim key as in
the game's controls**: middle mouse, right mouse, Z, or a side button.
Close the menu and release the key before testing. Aiming ends when the key
is released, a menu is opened, or the game loses foreground focus.

F7 changes the local player's walking/running and sprint speed.
Returning to x1 restores native speed. -/+ sets world FOV from 80 to 130,
including keypad aliases while the default shortcut is retained.
Preferences are saved in the INI.

**This version does not fix weapon/hand perspective.** It keeps the camera
and native aiming behavior from the 0.15 base; zoom may therefore remain
too strong at high FOV. The next work targets two global FOVs, for the world
and the character's complete first-person rendering, without per-weapon fixes.

## Logos and configuration

F12 enables/disables automatic skipping of the four logos on the next launch.
The loading loop is preserved. This automatic behavior was accepted and
user-validated; the mod currently does not provide manual logo skipping.

Configuration: `Binaries/Win32/InfiniteMod.ini`.
Log: `Binaries/Win32/InfiniteMod.log`.

| INI option | Value and effect |
| --- | --- |
| [General] Enabled | 0 disables the module on the next launch |
| [Overlay] Enabled | 0 disables the overlay on the next launch |
| [Weapons] Enabled | 0 disables weapon/ammunition commands |
| [Progression] Enabled | 0 disables progression grants |
| [Display] HideCrosshair | 0 shown, 1 requests hiding |
| [Display] HUDScale | 1.00 compact, 1.25, 1.50, or 2.00 |
| [Display] FOV | 80 to 130 degrees |
| [Player] SpeedMultiplier | 1.00, 1.25, 1.50, 1.75, or 2.00 |
| [Controls] HoldAim | 0 disabled, 1 enabled |
| [Controls] AimKey | Windows key code matching the game's aim key |
| [Intros] SkipIntroLogos | 0 show, 1 skip automatically |
| [Keybinds] | Shortcuts saved through Ctrl+F8 |

The supplied INI is an example for a fresh installation. The installer
preserves an existing INI in full; a missing shortcut entry uses the new
default. Native INI backups created by the module can restore HUD/logo
settings; see the technical notebook and rollback instructions.

## Status and tracking

0.18.0 is **BUILT** and **CHECKED**: x86 compilation and the integrated test
suite passed, including new commands, conflicts/persistence, foreground guards,
and regressions. The graphics fixture checks menus at 720p, 4K, and ultrawide.
These checks remain distinct from testing in an actual game session.

**USER-VALIDATED for F1 refill and shortcut reassignment.** Additional checks: F1 with a partially
empty gauge, F2/F5/F6 for both weapons, then reassign commands and menu-opening
shortcuts and confirm they persist after restarting.

- [Build 0.18 objective, results, and test](docs/BUILD-v018.md)
- [Cumulative history and statuses](docs/HISTORY.md)
- [Technical notebook](docs/TECHNICAL-NOTEBOOK.md)
- [Remaining work](docs/TODO.md)

Stable and experimental branches are separate. Only visual results confirmed
by the player are retained.

## Build from source

The compiler requires 64-bit Windows; the resulting module is **x86**.
Active sources are in `clean/`. The bootstrap downloads **Zig 0.13.0** from
the official site and verifies its hash; the compiler and its caches are not
included in the installable ZIP.

```powershell
.\distribution\bootstrap.ps1
.\test.ps1
.\build.ps1
```

An existing Zig 0.13.0 can be selected through `INFINITEMOD_ZIG`, or the
`-ZigPath` parameter of `clean/build.ps1`. `bootstrap.ps1 -IncludeLoader`
also downloads the pinned official loader to prepare an installable package.

Checked compatibility: GOG executable SHA256
`CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A`.
Compatibility with Steam or other versions is not claimed.

Dependencies with their licenses retained:
[Dear ImGui 1.91.9b](https://github.com/ocornut/imgui),
[MinHook 1.3.4](https://github.com/TsudaKageyu/minhook), and
[Ultimate ASI Loader 9.7.4](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/tag/v9.7.4).