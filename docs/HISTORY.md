# Cumulative history

This notebook separates behavior confirmed in a game session from compilation
and simulation checks. Validating one feature does not automatically validate
the entire build containing it.

## Statuses

- **PREPARED**: source/documentation changes are ready; no new build, test,
  installation, or delivery is claimed.
- **BUILT**: compilation produced an ASI.
- **CHECKED**: the stated checks were actually performed.
- **EXPERIMENTAL**: a change still awaits an in-game result.
- **USER-VALIDATED**: the player confirmed the relevant behavior.
- **REJECTED**: the player observed a defect; this experiment does not advance
  to the stable branch.

## Retained milestones

| Version | Change and result | Retained status |
| --- | --- | --- |
| 0.9 / 0.9.1 / 0.9.2, September 2026 | Overlay on the game's graphics device. The player confirmed the main menu and overlay. Manual logo skipping was not confirmed; automatic skipping of the four logos, with loading preserved, was then confirmed. Selective logo restoration was checked. | Overlay and automatic skipping **USER-VALIDATED**; manual skipping abandoned as the delivered solution |
| 0.11, October 1, 2026 | Weapons, ammunition in both slots, and hold-to-aim. The player replied “yes, perfect” (translated); in-game reads confirmed magazine/reserve counts and the secondary weapon. | Features retained in the validated base; this version's initial HUD setting is not a visually validated result |
| 0.12 / 0.12.1, October 1–2, 2026 | Speed x1 to x2 and world FOV 80–130. HUD revised to the native MaxResolutionUpScale ceiling; the incorrect ceiling causing excessive interface enlargement at 4K was removed. | World FOV and features retained; first-person rendering **known issue**, HUD effects require a separate check |
| 0.13 / 0.13.1, October 4, 2026 | Grants for Vigors, gear, upgrades, money, and lockpicks. Money/lockpicks confirmed; catalog checks were too strict for some other items and were corrected after reading actual classes and properties. | Resources **USER-VALIDATED**; other grants initially rejected, corrections continued |
| 0.14, October 5, 2026 | Carbine catalog corrected. Native reads: 48 weapon upgrades, 16 Vigor upgrades, 8 Vigors, and 49 gear items. The player subsequently confirmed the other grants but reported the crosshair. | Progression retained; this version's crosshair hiding **defective** |
| 0.15, October 8, 2026 | Hiding of both crosshair widgets corrected; health/salts/shield infusions up to their native cap of ten each. The player confirmed “Crosshair OK and all three gauges at 10/10” (translated). Tests and independent reads agreed. | **USER-VALIDATED** predecessor; accepted features retained in 0.18, with a known first-person FOV limitation |
| 0.16 / 0.16.1, October 9, 2026 | First-person adaptation experiments. The player reported misaligned reload parts, cartridges, and firing effects. | **REJECTED** as visual fixes; validated progression features preserved separately |
| 0.17 / 0.17.1, October 9–10, 2026 | Shared DPG2 projection experiment, followed by preservation of native camera calculations. Builds and mathematical checks passed, but videos and feedback still showed defects. The flash was aligned at world FOV 80 and offset at 120. | FOV fixes **REJECTED**; visual success is not inferred from tests alone |
| 0.18.0, October 10–11, 2026 | Return to the 0.15 camera base. Requested default shortcuts, health/shield/salts refill, and a menu to edit all 23 shortcuts, including Insert. x86 compilation, integrated checks, installation, and public delivery were recorded. The player confirmed F1 refill and shortcut reassignment. | **BUILT**, **CHECKED**; F1/refill and remapping **USER-VALIDATED**. Current accepted controls base; restart persistence and every remapped action not separately confirmed |
| 0.19, October 11, 2026 | FOV diagnostic experiment. Its observations did not produce a functional weapon/hand FOV correction. | **DIAGNOSTIC**; no functional FOV fix retained |
| 0.20, October 11, 2026 | Renderer experiment. The player reported HUD flicker and no weapon/hand FOV correction. | **REJECTED**; excluded from the accepted controls base |
| 0.18.1, October 11–12, 2026 | English localization from the accepted 0.18 controls checkpoint. Runtime edits change text, the version label, and equivalent fixture expectations while preserving behavior. The retained suite and x86 compilation passed; menu fixtures covered 720p/4K/ultrawide. | **BUILT**, **CHECKED**, **EXPERIMENTAL**; installation, verified package/release, and player result pending |

## Resume from the validated base

The current retained base is **0.18.0 controls**, with F1 vitals refill and
shortcut reassignment confirmed by the player. The accepted source checkpoint
for the localization is `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
The accepted packaged ASI has SHA256:
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
This feature validation does not establish restart persistence or fix FOV.

The original user-validated 0.15 ASI has SHA256:
`0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
It is distinct from new builds of the recovered source.

Camera-policy sources predating the 0.16 experiments were recovered from the
local work history, then checked using the base's full test suite. Recovering
the source does not make a newly built ASI a previously user-validated binary.

Historical recovery checkpoints checked on October 10:

| Branch | Checked commit | Role |
| --- | --- | --- |
| stable/v0.15 | 0d331468ac6f987b1742d8a95fbb60fc34ef4c11 | Recovered base sources and validation provenance |
| experiment/fov-v0.17.1 | d96f9d5aec59d92606350a4727470e1741504f0f | Archive of rejected FOV experiments |
| experiment/v0.18.0-bindings | Delivery source/tag commit `7026943a5273c707c7e6763f8b36104ad08b5863`, recorded in DELIVERY-v018.md | Shortcuts and gauge refill, without the rejected FOV experiments |

Public repository:
[DeadneM/BioShock-Infinite-Enhanced-PC-Mod](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod).
The public 0.18 release, tag/source commit, direct ZIP, downloaded payload,
and retained `main` / `stable/v0.18.0-controls` references were verified as
recorded in [DELIVERY-v018.md](DELIVERY-v018.md). The previous stable/v0.15
checkpoint and rejected FOV work remain separate. Private captures, saves,
raw logs, and game files are excluded from published mod sources.

Built 0.18.0 ASI checked by SHA256:
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
The integrated tests and compilation completed without warnings. Checks
include menus at multiple resolutions, all three shortcut pages, navigation
after reassigning menu-opening shortcuts, and the new simulated native refill.
These results are not in-game validation.

## Next step

Package the built and checked English localization, preserve the existing INI,
then verify its package and direct download before delivery. Record its actual
results in [BUILD-v0181.md](BUILD-v0181.md). Keep restart persistence and all
remapped actions as explicit remaining player checks.

The FOV investigation remains separate. After the unsuccessful 0.19 diagnostic
and rejected 0.20 experiment, reassess the evidence and correction boundary
before another visual build. The goal remains a world view and a shared view
for the character's complete first-person rendering.
## Player confirmation - October 11, 2026

The player confirmed that current-value vitals refill and shortcut reassignment work in-game
(translated: "Vitals refill and shortcuts OK"). These two features
are USER-VALIDATED. A restart-persistence test and every remapped action were not separately reported;
those remain distinguished from the integrated routing/INI-reload checks. Weapon/hand FOV remains unresolved.
The accepted 0.18 controls build is the retained base for 0.18.1, preserving
earlier validated features and excluding rejected FOV hooks.

## 0.18.1 preparation — October 11, 2026

Objective: translate the mod and project text into English without changing
accepted behavior. The runtime pass covers both menus, all 23 action labels,
weapon display names, request/progression statuses, and errors. Native names,
config keys, custom shortcuts, defaults, formulas, signatures, and game
parameters are preserved. Source review found string-only edits with identical
wide identifiers and printf format specifiers; equivalent fixture phrases were
updated. This review is not a new compilation or regression-suite result.

The build succeeds after English text and layout, retained behavior, package
preservation, and the downloaded release have been checked, with the player's
result recorded separately. At preparation there is no new ASI/ZIP hash or
verified release link. The README continues to offer the verified 0.18 ZIP.

## 0.18.1 build and regression checkpoint — October 12, 2026

The English localization was compiled for x86 from
`experiment/v0.18.1-english`, based on the accepted controls checkpoint
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
The ASI SHA256 is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.

The full retained suite passed, including native command guards, key capture,
conflicts, reassignment, configuration persistence, and the module's own
D3D11 menu fixture at 720p, 4K, and ultrawide. The 720p main menu and shortcut
menu images were inspected. These are controlled checks, not game-session
validation of the new binary.

All 23 action identities, default shortcuts, custom INI entries, native asset
names, signatures, formulas, and command conditions remain unchanged. English
text covers both menus, weapon labels, requests, warnings, and progression
results. The English `Rollback.ps1` entry retains `Retour.ps1` compatibility.
No rejected FOV hooks were imported, and no weapon/hand or aiming FOV fix is
claimed. Installation, package/release verification, and the player result
are pending at this checkpoint.
