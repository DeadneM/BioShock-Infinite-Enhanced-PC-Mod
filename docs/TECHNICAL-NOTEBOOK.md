# Cumulative technical notebook

## Scope and base

x86 ASI mod for the GOG executable checked by SHA256:
`CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A`.
Compatibility with other executables is not established. The project does not
distribute the game executable. Native signatures are checked before adapters
are installed; an unknown schema rejects the command.

The current accepted controls base is **0.18.0**, ASI SHA256
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
Its F1 vitals refill and shortcut reassignment were confirmed by the player,
who also confirmed the restored base was correct except for weapon/hand FOV.
The accepted source checkpoint is
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`. Restart persistence and every
remapped action have not been separately player-confirmed.

The original user-validated predecessor is 0.15:
`0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
The stable/v0.15 checkpoint contains recovered and checked sources, not a
promise of binary identity with that ASI. Rejected FOV experiments remain in
experiment/fov-v0.17.1. The accepted 0.18 controls step keeps the 0.15 camera
policy. English localization starts from 0.18, without rejected FOV hooks.

## Preserved architecture

The overlay attaches to the factory and swapchain actually created by the
game. The D3D11 device and context used to display the menu belong to that
swapchain. Graphics state is restored, and the menu does not pause the game.
Commands require the game to be in the foreground.

Gameplay requests are queued from the overlay, then executed on the game
thread through the existing camera connection. The queue checks the requesting
character, playable context, and a two-second expiry. Operations use native
functions and then verify their effects. Acquisitions may subsequently be
retained by normal game saves; no items are granted automatically at launch.

External diagnostic tools are read-only. Reading an inventory or matrix
does not validate the rendered result.

## Retained features and decisions

- **Intros**: the four logo StartupMovies entries are disabled through
  configuration; LoopingMovieUntilStartupFinishes remains active. The player
  accepted and validated automatic skipping. Manual-skipping experiments
  did not achieve the requested result and are not presented as active.
- **Weapons**: the eight selectable main weapons use native functions.
  Active and secondary slots are read again after execution. Magazine and
  reserve are filled through their two separate operations.
- **Progression**: eight Vigors, 49 recognized fixed gear definitions,
  48 upgrades for the twelve weapons/variants, and 16 Vigor upgrades.
  Complete identity checks and no duplicates. Recognized promotional
  variants replace their standard equivalents; unused variants are not
  additional items to invent.
- **Resources**: money and lockpicks use the caps read from the character;
  observed caps are 1,000,000 and 30. Legitimate values above a cap are not
  reduced.
- **Infusions**: three recognized tonics, ten upgrades per gauge. Inventory,
  attribute gains, money, and character identity are checked. The initial
  shield must already have been obtained normally.
- **Crosshair**: native visibility of the two GFx widgets is used.
  IsShown=1, IsInCombat=2, and IsCenterpointVisible=4 masks were compared
  with actual objects. The player validated hiding on 0.15.
- **HUD**: native sizing through MaxResolutionUpScale, a maximum fractional
  increase rather than an inverse multiplier. HUDScale 1/1.25/1.5/2 maps
  to 0/0.25/0.5/1 in XUI.ini. Changes apply on the next launch. The game
  handles aspect ratio; the mod does not resize Vigor effects separately.
- **Aiming**: XStartZoom and XStopZoom on the game thread. The selected aim
  key must match the game's binding. Releasing the key, opening the menu,
  or losing foreground focus ends hold-to-aim.
- **Speed**: adapts the local player's walking/running and sprint attribute
  results. x1 restores the native result; no multiplier is persistently
  written into the character.
- **0.15 FOV**: world setting 80–130 at the local player's camera calculation.
  Aiming transitions keep their native calculation. First-person model FOV
  and zoom at high FOV are known limitations.

## 0.18 — configurable shortcuts

One keybinds::Engine instance owns the configuration and key-edge history.
Each action is identified independently of its key. Modifiers match exactly:
Ctrl+F1 does not also trigger F1. Holding a key does not repeat the operation;
regaining foreground focus with a key still held does not launch a command.

The 23 actions include both menu-opening shortcuts, all grants, and existing
settings. The Ctrl+F8 menu displays eight entries per page; Up/Down selects,
Enter begins capture, and Escape cancels/goes back. The main menu uses Insert
by default. Both menu-opening shortcuts can be reassigned.

Capture begins after key release and suspends mod commands. An already-used
shortcut is rejected. Keyboard and keypad +/- aliases are included in conflict
checks while the default FOV setting is retained. A custom reassignment does
not add a hidden alias. Writing [Keybinds] must succeed before the active
configuration is replaced; on failure, the old shortcut remains usable.

The shortcut menu can open without opening the main menu. Grant and setting
commands keep their conditions: main menu open and game in the foreground.
Mod shortcuts do not change the game's internal bindings.

## 0.18 — refill current gauge values

F1 fills health, shield, and salts up to their current capacities. It does
not replace the Ctrl+F5 infusion command, which increases capacities. It does
not revive a dead character or unlock the initial shield.

Native GOG entry points checked by signature and stack cleanup:

| Operation | RVA | ABI and expected result |
| --- | --- | --- |
| Health | 3554D0 | thiscall(pawn, float), ret4; native setter with cap and HUD notifications |
| Shield | 5EDDF0 | thiscall(pawn, float, int), ret8; final argument zero as in the native refill |
| Salts | Reflected ClientAdjustCurrency function | selector 4, delta toward current cap; VigorEnergySizeAttrib capacity |

Health/XHealth/XHealthMax/XShield/XShieldMax properties and the character
definition are checked before calls. Capacity is not written. A zero shield
cap is retained; a value already above its capacity is not reduced.
After calls, the mod reads all three values, their capacities, and character
identity again. A partial or unconfirmed effect keeps a warning instead of
a success message.

F1 current-value refill is **USER-VALIDATED** on 0.18. The English 0.18.1 binary
preserves that implementation but still awaits its own in-game result.

## Why the FOV experiments are not retained

Builds 0.16–0.17.1 passed mathematics, ABI, or loading checks without achieving
a satisfactory visual result. Weapons, reload parts, and firing effects were
not all aligned. Player feedback and videos take precedence over “hook ready”
messages or synthetic tests.

Earlier reads identified a DPG2 foreground route and a
TranslatedViewProjection matrix supplied in passes 87C020 and C11DC0.
Callbacks for the five inspected flash materials did not demonstrate an
independent projection. Floating-section matrices are world transforms.
These observations do not establish the projection consumed by every primary
weapon/hand draw, nor do they prove a separate first-person camera.

The goal remains **an intact world view and a separate complete first-person
view**, with consistent coordinates for hands, weapons, accessories, and FX.
DPG2 also contains HUD Vigor effects; selecting an entire render group is
insufficient to establish that complete first-person boundary.

Before another visual trial, a bounded draw-time capture must connect DPG,
material, mesh, anchors, and the projection actually consumed, at 80 then 120.
It must distinguish a projection difference from an effect origin that is
already incorrect. An end-of-frame read is insufficient. If origins differ,
the fix must target their generic calculation; per-weapon offsets must not
hide the problem.

Boundary C5F880 is cdecl with seven arguments, view in argument1 and mesh in
argument4. Coverage of all opaque meshes is not yet established.
FSceneView copying involves owned containers: a raw copy must not be treated
as an owning engine object.

The 0.19 diagnostics did not deliver a functional FOV correction. The 0.20
experiment changed foreground view fields around preparation and drawing,
but the player reported HUD Vigor-effect flicker and no weapon correction.
It is **REJECTED**, including its unconfirmed aiming changes. The accepted
0.18 ASI was restored; the player confirmed that the other features were
correct again. Synthetic matrix and ABI checks did not establish visual
success and do not justify retaining those hooks.

A subsequent code-path review distinguished the main skinned draw from the
decal path that earlier vertex-factory reads followed. On the inspected
player components, proxy virtual slot 0x08 enters `C28A10` for the main draw;
slot 0x1C enters `7D8F90` for decals. The primary route reaches mesh-object
virtual slot 0x24 (`A5D130`) and selects the LOD+0x2C factory before submission
through `7C25B0`, with caller `B18B22`. The decal route's factory wrappers
are not evidence for the main weapon/hand shader. This refines the next
investigation target; it is not a new FOV feature or proof of a working fix.

Future criterion: at world FOV 80/110/120/130, every weapon, hand, part, and
effect remains proportionate and aligned during firing/reloading/aiming/weapon
switching; aspect ratio and world objects remain correct.
Expected proportional world zoom:
tan(worldADS/2)=tan(worldHip/2)*tan(nativeADS/2)/tan(nativeHip/2).
The formula does not replace visual validation.

## Publication and reversibility

The stable branch receives only changes retained after validation.
Test builds remain experimental. Before replacement: close the game and
copy the existing ASI, INI, and files needed for rollback. Preserve FOV,
HUD, speed, aiming, and intro preferences. The new mapping is an expressly
requested change.

A delivered ZIP does not contain game files, saves, or captures. It is
extracted to the location specified by its README, with ASI/INI/README and
the required loader, without an unnecessary enclosing folder. Internal
names, hashes, branches, commits, and link availability must be checked
before publishing a release. The shortcut build includes none of the
projection hooks from the rejected experiments.
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
(translated: "Vitals refill and shortcuts OK"). These two features
are USER-VALIDATED. A restart-persistence test and every remapped action were not separately reported;
those remain distinguished from the integrated routing/INI-reload checks. Weapon/hand FOV remains unresolved.
The accepted controls build can serve as the next retained base without importing rejected FOV hooks.

## English localization checkpoint — October 12, 2026

0.18.1 uses the accepted 0.18 controls source. Runtime changes are limited
to text, the version label, and matching fixture expectations. Native wide
identifiers, format specifiers, INI keys, all 23 action identities, default
bindings, formulas, and guards remain unchanged. Existing custom bindings
and the user's complete INI must be preserved by installation.

The checked x86 ASI SHA256 is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
The full retained suite passed, including command safety checks, rebinding,
INI persistence, and the module's own D3D11 menu fixture at 720p/4K/ultrawide.
The 720p main and shortcut menu images were inspected. Status is **BUILT**,
**CHECKED**, **EXPERIMENTAL** pending installation, delivery verification,
and the player's result. These checks supersede the earlier preparation-only
state, without changing earlier game-session evidence.

Six isolated installer/rollback scenarios also passed using the English
rollback entry. They cover custom INI preservation, a rollback copy of later
INI edits, unknown-loader and altered-package rejection, first-installation
cleanup, shared-loader preservation, and refusal to replace a newer ASI.
These fixture checks do not claim an installation in the player's game.

The runtime, project documentation, and delivery text are now intended to be
English. `Rollback.ps1` is the English rollback entry; `Retour.ps1` remains
available for compatibility. English localization does not change native
game text or supply an independent weapon/hand FOV fix. The next optical
experiment stays separate and must cover complete first-person rendering
and proportional aiming rather than per-weapon offsets.
