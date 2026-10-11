# InfiniteMod 0.15.0 — October 8, 2026

Built and installed ASI: 0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF.
Previous backup: before-v015-20261008-195945-113. The ASI, INI, log,
manifest, XUI.ini, XEngine.ini, and 11 save files (5,019,175 bytes)
were copied before replacement. INI preferences and executable remained
unchanged, checked by hash. The game was relaunched and the player confirmed
in-game tests.

## Changes

Crosshair rejection was corrected using properties observed on actual objects:
IsShown=1, IsInCombat=2, IsCenterpointVisible=4. The previous version expected
2 for centerpoint and rejected both HUD widgets. Hiding still uses native
Flash visibility; animation flags remain native.
The menu distinguishes pending request, hiding applied, and unavailable.
Evidence: progression-research/RETICLE-REPAIR-v0141.md.

Ctrl+F8 adds missing health/salts/shield infusions up to the cap of ten each.
The three stackable campaign tonics are IDs 276/235/329, Number=2.
The initial shield, 346, must already be acquired. Old cap1 variants are
excluded. Native zero-cost transactions use the missing quantity.
All definitions, values, filters, and capacities are checked before grants.
After each transaction, the counter, character, money, and attribute gain
must match. An unconfirmed effect retains its warning on another keypress,
without granting the item again.
Evidence: progression-research/INFUSIONS-20261008.md.

## Automated verification

test.ps1 and build.ps1 pass; logs: validation-v015-tests.log and
validation-v015-build.log. The fixture uses synthetic objects/reflection,
the actual x86 ProcessEvent ABI, and DX11 rendering outside the game.

- Crosshair: captured masks, managed DisplayObject0x48, simulated native
  visibility read/write, restoration, other widgets, HUD lifecycle, and guards.
- Infusions: identities and schemas, cap10, exact gains/operation/filter,
  quantity delta, unchanged money, missing initial shield, unexpected cost,
  effect or inventory alone, partial effect, retry after failure, character
  change, cinematic, and wrong thread.
- Weapon, progression, aiming, speed, FOV, INI, logo, and rendering regressions.
  Ctrl+F8 does not trigger F8. Holding a key does not repeat the command.
- Menu rendered at 720p, 4K, and ultrawide; the 720p image was inspected and
  all entries were visible.

These simulated tests remain distinct from the actual tests below.
Viewmodel behavior and configurable shortcuts are unchanged in this version.

## In-game verification

Launch confirms 0.15.0 and 3840x2160 rendering. The log confirms hiding
two widgets, then restoring 2/2, then hiding them again.
The player confirmed: “Crosshair OK and all three gauges at 10/10”
(translated from the original reply).

Ctrl+F8 performed three native transactions, all verified=1:

| Gauge | Before | Added | After | Attribute before/after |
| --- | ---: | ---: | ---: | --- |
| Health | 0 | 10 | 10 | modifier 1.0 -> 2.5 |
| Salts | 7 | 3 | 10 | capacity 205 -> 250 |
| Shield | 7 | 3 | 10 | modifier 3.1 -> 4.0 |

An independent QUERY+READ process read confirms the three counters,
final attributes, XHealthMax=4000, and initial shield 346 still at quantity1.
Money remains 1,000,000. Evidence retained in
progression-research/infusion-state-v015-after.json; this external tool
did not grant any items. The session log was copied to validation-v015-live.log.