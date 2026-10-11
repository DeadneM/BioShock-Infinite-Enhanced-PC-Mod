# Build 0.18.0 — shortcuts and gauge refill

Preparation date: October 10, 2026.

## Single objective

Use the requested mapping, allow every mod shortcut to be changed and saved,
and add health/shield/salts refill on F1. Existing commands remain available
through their new shortcuts. The build returns to the 0.15 camera policy;
the 0.16–0.17.1 FOV experiments are not part of this objective.

## Base and status

- Validated base: 0.15 ASI, SHA256
  `0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
- Recovered stable source and checked base suite: stable/v0.15,
  commit `0d331468ac6f987b1742d8a95fbb60fc34ef4c11`.
- This build's branch: experiment/v0.18.0-bindings; delivery commit to be
  finalized and checked after compilation/packaging.
- Status: **BUILT**, **CHECKED**, **INSTALLED**; F1 refill and shortcut reassignment are **USER-VALIDATED**. The FOV work remains separate.
- x86 compilation and integrated tests: passed, without warnings.
- This build's ASI SHA256:
  `61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
- Installation: completed with a verified backup on October 11, 2026; the existing mod INI, loader and game EXE were preserved.
- ZIP and release: to be produced and verified; no download link claimed
  before actual publication.

The previously installed 0.17.1 ASI is not validated. Its technical rollback
and the validated 0.15 ASI are distinct checkpoints; returning to 0.17.1 is
not a return to visual stability.

## Changes

| Default shortcut | Action |
| --- | --- |
| Insert | Open/close the main menu |
| Ctrl+F8 | Open/close the shortcut menu |
| F1 | Refill health, shield, and salts up to current capacities |
| F2 | Fill the magazine and reserve of both weapons |
| F3 / F4 | Select the previous/next weapon |
| F5 | Give the selected weapon in the active slot |
| F6 | Give the selected weapon in the secondary slot |
| F7 | Speed x1, x1.25, x1.5, x1.75, x2 |
| F12 | Enable/disable automatic logo skipping for the next launch |
| Ctrl+F1 | All Vigors |
| Ctrl+F2 | All gear |
| Ctrl+F3 | All weapon upgrades |
| Ctrl+F4 | All Vigor upgrades |
| Ctrl+F5 | Maximize health/salts/shield infusions |
| Ctrl+F6 | Money up to the native cap |
| Ctrl+F7 | Lockpicks up to the native cap |
| F8 | Show/hide crosshairs |
| F9 | Native interface size for the next launch |
| F10 | Enable/disable hold-to-aim |
| F11 | Select the aim key matching the game's controls |
| - / + | World FOV -/+5, from 80 to 130 |

Grant and setting commands retain their usage conditions: main menu open and
game in the foreground. Ctrl+F8 opens configuration directly without first
opening Insert. All 23 actions are configurable.

In the shortcut menu: Up/Down selects an entry, Enter begins capture, then
release and press the new shortcut. Escape cancels capture/goes back.
Conflicts are reported, and failed saving keeps the old key. The main menu
shows current bindings; they are saved in the INI's [Keybinds] section.

F1 fills current values only. It does not replace Ctrl+F5, increase capacities,
or grant the initial shield. A dead character or unrecognized context rejects
refill. An unconfirmed effect produces a visible warning.

## Preserved features and preferences

Overlay, automatic logos, weapons/ammunition, inventory/progression, crosshair,
native HUD ceiling, hold-to-aim, speed, and world FOV from the 0.15 base.
The game keeps its aspect-ratio adaptation. Refill is explicit, and no items
are granted at launch. The new installation must keep the user INI for
preferences other than the requested mapping.

Retained limitations: weapon/hand FOV and native zoom at high FOV are not
fixed by this build; Vigor effects are not resized separately. No visual
result from 0.17.1 is presented as validated.

## Checks to record before delivery

| Check | State at preparation |
| --- | --- |
| Full suite for the recovered 0.15 camera source | Passed; distinct from validation of a newly built ASI |
| Integrated 0.18 tests | Passed without warnings |
| x86 0.18 compilation | Passed; ASI identified by SHA256 |
| Menus at 720p/4K/ultrawide and all three page bounds | Checked by the graphics fixture |
| Navigation after reassigning menu openings to Up/Down | Regression reproduced, then fixed; check passed |
| Actual menu display in the game | Pending after installation |
| F1 on partially empty values | Awaiting player |
| New shortcuts and persistent reassignment | Awaiting player |
| ZIP without an enclosing folder and exact hashes | Pending |
| GitHub references and direct ZIP link | Pending |

Integrated checks cover Ctrl commands without triggering their unmodified
equivalents, held keys, loss of foreground focus, conflicts and write failures,
suppression of commands during capture, the F1 native queue, and preservation
of capacities. The graphics fixture checks menus at multiple resolutions and
regressions of base features. Images of the 720p menu, shortcut pages 1 and 3
at 720p, and the 4K menu were also inspected. None of these checks is presented
as user testing inside the game.

## Player test and success criterion

1. Load a playable save with at least one gauge below full. Insert, then F1,
   must fill health/shield/salts up to current capacities. The menu must
   confirm values, and capacities must not increase.
2. F2 must fill both weapons. F3/F4 selects a weapon, F5 puts it in the active
   slot, and F6 puts it in the secondary. Confirm both slots.
3. Check F7, F12, and Ctrl+F1 through Ctrl+F7; Ctrl commands must not trigger
   their unmodified actions.
4. Ctrl+F8, reassign a command, then both menu-opening shortcuts. Capturing
   a shortcut must not execute its command. Confirm the new menu openings,
   restart, and check persistence.
5. Check validated features: logos/loading, crosshair, hold-to-aim, HUD, and
   inventory. A defect prevents promotion of the entire build.

The build succeeds only when these actions and their persistence are
confirmed without losing validated features. Simulated tests do not replace
this result.

## Installation and rollback

Close the game normally before replacing the ASI. Copy the current ASI,
INI, and files required for rollback; keep game saves before progression
tests. Installing the new module must not replace the user's configuration
with the example INI.

To return to the previous module, with the game closed, restore **the ASI and
INI from the same backup**: the old ASI does not use the new mapping.
To return to the expressly validated base, select the 0.15 backup identified
by its hash. Restoring a module does not remove items already saved in-game;
that requires an earlier game save.

## Delivery and next step

Public repository created:
[BioShock-Infinite-Enhanced-PC-Mod](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod).
This build's commit, ASI/ZIP hashes, direct link, and download result must
be recorded after their actual checks.

After shortcut validation, resume FOV on a separate branch with one objective:
consistent world and complete first-person rendering. The technical notebook
defines the evidence needed before another attempt; no per-weapon fix should
be added to 0.18.
## Installation checkpoint - October 11, 2026

The controlled v0.18 ASI was installed at08:17 CEST, after a normal game shutdown.
The transaction and backup hashes were verified; the existing mod INI, loader, and game EXE were unchanged.
The initial local installation ZIP was checked for its16 entries and exact payload hashes.
The game was relaunched; its log confirms v0.18, ready command/aim/camera hooks, a3840x2160 swap chain,
and keybind loading with zero invalid entries or conflicts. These observations are not user validation.
F1 on partially empty gauges, actual menu operation, and persistent remapping await the player.
Six isolated installer/rollback scenarios passed, including custom INI preservation and shared-loader handling.
The public release ZIP and final remote commit IDs are recorded separately once they are verified.

## Public-source build check

The isolated publication checkout built successfully with `dist/` absent, using official Zig0.13.0.
The build created the ASI and default INI without requiring game assets or changing any source or Git ref.
The freshly linked output was SHA256160B010A9899DEA3B47BE162D3F735A16BEFB506A2A545059F5F9CD6226799FF.
It is a separate reproducibility check, not the packaged/installed binary. The installable package pins
SHA25661F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2 from the fully tested build.
Compiler/dependency warnings appeared while populating the fresh compiler cache; the build exited0.

## Player confirmation - October 11, 2026

The player confirmed that current-value vitals refill and shortcut reassignment work in-game
("Recharge et raccourcis OK", translated: "Vitals refill and shortcuts OK"). These two features
are USER-VALIDATED. A restart-persistence test and every remapped action were not separately reported;
those remain distinguished from the integrated routing/INI-reload checks. Weapon/hand FOV remains unresolved.
The accepted controls build can serve as the next retained base without importing rejected FOV hooks.
