# InfiniteMod - Accepted English base and native FOV research

ASI mod for **BioShock Infinite GOG x86**: overlay, weapons and progression,
player settings, and configurable shortcuts.

**Current runtime/source:** accepted English0.18.1. The complete current INI
and custom keys are preserved. [Panini0.22](docs/REJECTED-v022.md) and the
ineffective [CalcFOV aiming0.23](docs/REJECTED-v023.md) are rejected and removed.
This branch starts directly from the accepted source, carrying cumulative
documentation but no rejected runtime changes. All23 accepted actions remain
available; the experimental Ctrl+F9 action is not retained.

**Native zoom target research: PREPARED, no new build or functional fix.**
The next objective is one native aiming target before interpolation. Existing
world FOV/native Hor+ and commands are unchanged. No scene remap, GPU projection
or per-weapon profile is introduced. Independent arms/weapon FOV and placement
remain unfinished. See [the current objective](docs/NATIVE-ZOOM-TARGET.md).
Rejected [0.21](docs/REJECTED-v021.md) GPU/ADS layers are also excluded.

**0.18.1 is BUILT, CHECKED, and INSTALLED. English menus and preserved custom
menu keys are USER-VALIDATED.** The v0.18.1 public prerelease and downloaded
payload are verified.
The localization starts from the accepted **0.18 controls base**. The player
confirmed F1 vitals refill and shortcut reassignment in-game. Menus, shortcuts,
weapon labels, and gameplay status messages are now in English. The retained
tests and x86 compilation passed on October 12, 2026. The local installable
ZIP and installation backup were verified, and the complete existing INI was
preserved. The player then confirmed both English menus and existing menu keys.
Unreported gameplay/restart checks remain separate from those confirmations.
See [the verified delivery](docs/DELIVERY-v0181.md).
**Weapon/hand FOV and proportional aiming remain unfinished.**

## Accepted download and rollback

[Installable v0.18.1 English ZIP](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.1-experimental/InfiniteMod-v0.18.1-English.zip) · [Release notes](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/tag/v0.18.1-experimental)

The downloaded ZIP was checked against SHA256
`1853FEF39BAFE9CCC6739B1E66A30A959E44999E86A956A4C82BF831B9EAA545`.
All 18 downloaded entries match the package used for the confirmed local
installation. See [release verification](docs/DELIVERY-v0181.md).
The release is a preview of an unfinished mod; its English menus and saved
menu keys are confirmed, while the first-person FOV work remains unfinished.

The [accepted v0.18 package](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.0-experimental/InfiniteMod-v0.18.0.zip)
remains available for rollback. The v0.18.1 ZIP's 18 entries have no enclosing
folder. Its bundled documentation records the pre-installation checkpoint;
this repository's delivery record and published
release notes supersede those pending-status statements. The ZIP is retained
unchanged. See [the English localization objective](docs/BUILD-v0181.md).

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

The English package uses `Rollback.ps1` for that command and retains
`Retour.ps1` as a legacy compatibility entry. Use the command printed by the
installer for the package you downloaded.

The scripts do not replace the executable, save files, or native game INIs.
At launch, the mod applies the HUD/logo preferences to the native INIs.
Granted items may be stored by normal game saves; rolling back the module
does not undo them.

See [installation and rollback instructions](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/blob/main/distribution/INSTALLATION.md)
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

The English localization preserves the accepted 0.18 camera and native aiming
behavior, inherited from 0.15. Weapon/hand perspective and zoom at high FOV
remain unresolved. The 0.19 diagnostic did not produce a functional FOV fix;
the 0.20 renderer experiment was **REJECTED** after HUD flicker and no weapon
correction. The separate FOV goal remains two global views, for the world and
the character's complete first-person rendering.

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

0.18.1 is **BUILT**, **CHECKED**, and **INSTALLED**, with English main/shortcut
menus and preserved custom menu keys **USER-VALIDATED**. Its objective is English text throughout the mod and
project documentation while preserving the accepted 0.18 behavior. The runtime
changes are confined to string literals, the version label, and equivalent
English fixture expectations. Default shortcuts, custom bindings, INI keys,
native asset identifiers, formulas, and gameplay conditions are preserved.

Success requires English menus and messages that fit, passing regression
checks, a verified installable package, and player confirmation without losing
accepted features. The checked ASI SHA256 is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
The retained suite passed, including native command guards, key reassignment,
INI persistence, and the module's own D3D11 menu fixture at 720p/4K/ultrawide.
The 720p main menu and shortcut menu images were also inspected. These checks
and six isolated installer/rollback scenarios remain distinct from the
player's menu confirmation. No test of every command or restart persistence
is inferred. The checked English source on `main` is
`cd0873936a92f0a0e2c00dca88557c7724b7b1a6`, tree
`cca6c50943f84d2c88b1f8eeebcfd2b3e92b0c35`, identical to the checked local
build snapshot `eab12be926efbfb286051087e95749138d0fb067`.
The stable 0.18 controls checkpoint remains
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`. The public tag
`v0.18.1-experimental` points to the checked English source commit; both
uploaded assets and the downloaded ZIP/checksum were verified. FOV research
is paused and remains a separate
unfinished experiment.

The accepted 0.18.0 is **BUILT**, **CHECKED**, and **USER-VALIDATED for F1 refill
and shortcut reassignment**. Its integrated checks covered conflicts,
INI reload, foreground guards, regressions, and menus at 720p/4K/ultrawide.
The player has not separately confirmed persistence after restarting or every
remapped action; those remain additional checks.

- [Build 0.18.1 English objective, checks, and player test](docs/BUILD-v0181.md)
- [Current 0.18.1 installation, source, and player checkpoint](docs/DELIVERY-v0181.md)
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
