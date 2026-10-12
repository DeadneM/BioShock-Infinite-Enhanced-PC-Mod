# Build 0.18.1 — English localization

Build and check checkpoint: October 12, 2026.

## Single objective and criterion

Translate the mod's menus, action and weapon labels, requests, warnings, and
results into English while preserving the accepted 0.18 controls behavior.
Project documentation and delivery text must also be in English.

Success requires readable English menus and messages, passing retained
regression checks, an installable package whose contents and download are
verified, and a player result without loss of accepted controls or preferences.
Compilation and fixture checks do not replace that player result.

## Base and status

- Accepted base: 0.18 controls, source checkpoint
  `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
- Accepted base ASI SHA256:
  `61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
- Working branch: `experiment/v0.18.1-english`; stable controls remain separate.
- New ASI SHA256:
  `B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
- Status: **BUILT**, **CHECKED**, **EXPERIMENTAL**.
- Not yet claimed: installation, ZIP hash, verified remote branch/commit,
  new release/direct-download link, or player validation of this binary.

The player confirmed F1 current-value refill and shortcut reassignment in
0.18, then confirmed the restored controls base was correct except for
weapon/hand FOV. Those results apply to the accepted base. Restart persistence
and every remapped action were not separately player-confirmed.

## Changes and preserved behavior

Runtime edits change text and the version label. Fixture text expectations
match their English equivalents. The pass includes the main menu, all three
shortcut pages, all 23 actions, weapon display labels, progression results,
status messages, and errors.

Native asset/function identifiers, format specifiers, INI keys, command
identities, formulas, signatures, and gameplay guards remain unchanged.
Default shortcuts and existing custom bindings are preserved, including
reassigned menu-opening keys. This build does not reset the user's INI.

The accepted overlay, vitals refill, weapons and ammunition in both slots,
progression, resources, infusions, crosshair hiding, native HUD sizing,
hold-to-aim, player speed, world FOV, and automatic logo skipping remain.
Commands keep their foreground, playable-character, and menu conditions.
No grants occur automatically at launch.

The English rollback entry is `Rollback.ps1`; the previous `Retour.ps1`
entry remains available for compatibility. Installation still preserves an
existing INI and compatible loader. The installer and backup checks must
identify the actual files before any replacement.

## Performed checks

| Check | Recorded result |
| --- | --- |
| Runtime string/identifier review | English text replacements; native names, format specifiers, and behavior preserved |
| x86 compilation | Passed; ASI hash recorded above |
| Full retained regression suite | Passed |
| Native command guards | Passed in the retained fixtures |
| Shortcut capture, exact modifiers, conflicts, rebinding, and INI persistence | Passed in the retained fixtures |
| Menu rendering at 720p, 4K, and ultrawide | Passed in the module's own D3D11 fixture |
| 720p main and shortcut menu image inspection | Performed |
| Six isolated installation/rollback scenarios through the English rollback entry | Passed |
| Installation and backup transaction | Pending |
| ZIP structure/payload/hashes | Pending |
| Published branch/commit and downloaded release payload | Pending |
| Player test of the new binary | Pending |

The D3D11 fixture is a controlled menu-rendering check. It is distinct from
the game's graphics device and is not proof that the new ASI works in a live
session. Likewise, fixture persistence checks are distinct from the player's
restart test.

The distribution fixture covers preservation of a custom INI, keeping later
INI edits in a rollback copy, rejection of an unknown loader or altered
package, returning a first installation to absent mod files, preservation of
a shared loader, and refusal to overwrite a newer ASI during rollback. Its
six scenarios passed. This is separate from installing in the user's game.

## Player test

Use the currently configured menu-opening keys; defaults are Insert and
Ctrl+F8, but the existing INI may assign different keys.

1. Open the main menu, then the shortcut menu. Confirm readable English text,
   all three shortcut pages, and labels matching the saved bindings.
2. With at least one gauge below full, run vitals refill. Confirm current
   health/shield/salts fill without increasing their capacities.
3. Check ammunition and giving a selected weapon in each slot. Check at
   least one progression result or warning and its English text.
4. Reassign a shortcut and test it. Restart normally, then verify that the
   custom menu-opening keys and other preferences remain saved.
5. Confirm that accepted aiming, crosshair, HUD, speed, logos/loading, and
   inventory behavior remains intact.

Record exactly which checks the player performed. A defect prevents promotion
of the affected change; do not infer every action works from one successful
command.

## Delivery and rollback

The verified fallback remains the
[0.18 installable ZIP](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.0-experimental/InfiniteMod-v0.18.0.zip).
Its source, hashes, release, and downloaded payload are recorded in
[DELIVERY-v018.md](DELIVERY-v018.md). This document does not announce a new
download before its actual verification.

For the new package, record the actual source commit/branch, ZIP hash,
entry list, installed ASI/INI hashes, backup transaction, release URL, direct
download, and re-downloaded payload after those checks. The ZIP must contain
its README, INI, scripts, licenses, and required `Binaries/Win32` tree at its
root, without an enclosing folder or private game data.

Close the game normally before replacing the loaded ASI. Back up first and
preserve the complete existing INI. If the new build fails, restore the
accepted 0.18 module identified by its hash or use the verified installation
transaction's rollback command. Restoring an ASI does not undo progression
already stored by a normal game save.

## Remaining issue and next experiment

World FOV works, but arms/weapons remain too distant or distorted at high FOV,
and native aiming applies a coarse zoom. Localization does not fix this.
The 0.19 work was diagnostic-only. The 0.20 render experiment was **REJECTED**
after HUD Vigor-effect flicker and no weapon correction; none of its projection
or unconfirmed aiming hooks is retained here.

Continue the FOV work in a separate experiment from the accepted controls
base. Establish a shared boundary for hands, weapons, reload props, and
effects, then implement an independent complete first-person FOV and
proportional aiming. No per-weapon profiles or offsets should replace that
global objective. The technical notebook records the current evidence and
visual success criterion.
