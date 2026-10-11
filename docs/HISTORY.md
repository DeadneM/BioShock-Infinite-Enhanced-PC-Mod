# Cumulative history

This notebook separates behavior confirmed in a game session from compilation
and simulation checks. Validating one feature does not automatically validate
the entire build containing it.

## Statuses

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
| 0.15, October 8, 2026 | Hiding of both crosshair widgets corrected; health/salts/shield infusions up to their native cap of ten each. The player confirmed “Crosshair OK and all three gauges at 10/10” (translated). Tests and independent reads agreed. | Latest expressly **USER-VALIDATED** base, with a known first-person FOV limitation |
| 0.16 / 0.16.1, October 9, 2026 | First-person adaptation experiments. The player reported misaligned reload parts, cartridges, and firing effects. | **REJECTED** as visual fixes; validated progression features preserved separately |
| 0.17 / 0.17.1, October 9–10, 2026 | Shared DPG2 projection experiment, followed by preservation of native camera calculations. Builds and mathematical checks passed, but videos and feedback still showed defects. The flash was aligned at world FOV 80 and offset at 120. | FOV fixes **REJECTED**; visual success is not inferred from tests alone |
| 0.18.0, October 10, 2026 | Return to the 0.15 camera base. Requested default shortcuts, health/shield/salts refill, and a menu to edit all 23 shortcuts, including Insert. x86 compilation and integrated tests passed. | **BUILT**, **CHECKED**, **EXPERIMENTAL**; installation/delivery to be recorded in BUILD-v018.md; player validation pending |

## Resume from the validated base

The original user-validated 0.15 ASI has SHA256:
`0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
It is distinct from new builds of the recovered source.

Camera-policy sources predating the 0.16 experiments were recovered from the
local work history, then checked using the base's full test suite. Recovering
the source does not make a newly built ASI a previously user-validated binary.

Local branches checked on October 10:

| Branch | Checked commit | Role |
| --- | --- | --- |
| stable/v0.15 | 0d331468ac6f987b1742d8a95fbb60fc34ef4c11 | Recovered base sources and validation provenance |
| experiment/fov-v0.17.1 | d96f9d5aec59d92606350a4727470e1741504f0f | Archive of rejected FOV experiments |
| experiment/v0.18.0-bindings | In preparation; delivery commit not yet finalized | Shortcuts and gauge refill, without the 0.16–0.17.1 FOV experiments |

Public repository:
[DeadneM/BioShock-Infinite-Enhanced-PC-Mod](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod).
It has been created; branch and ZIP publication must be checked before
announcing delivery. Private captures, saves, raw logs, and game files are
not publishable mod sources.

Built 0.18.0 ASI checked by SHA256:
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
The integrated tests and compilation completed without warnings. Checks
include menus at multiple resolutions, all three shortcut pages, navigation
after reassigning menu-opening shortcuts, and the new simulated native refill.
These results are not in-game validation.

## Next step

Validate the new shortcuts, F1 refill, and 0.18 shortcut persistence. Then
resume a separate global FOV experiment: a world view and a shared view for
the character's complete first-person rendering. The technical decision is
described in TECHNICAL-NOTEBOOK.md; per-weapon fixes must not replace this goal.