# Rejected 0.21 first-person projection experiment

Player decision and verified rollback: October 13, 2026.

## Verdict

**REJECTED.** The player reported catastrophic world FOV and unacceptable
character proportions. Passing fixtures and linked adapter checks did not
establish the required visible result. This source does not advance to the
accepted branch and is not published as a successful fix.

## Checked build and local delivery

- Source: `9982aaa1c5c983eb11aa1ce8b437fce9bb55bdc1` on
  `experiment/first-person-v0.21`.
- ASI: `2F110C6E9141F2D7FE2CBF5155C6E0F058DA25F0A96A3EE6D2E60C9281988D8E`.
- Local ZIP: `InfiniteMod-v0.21.0-shared-FOV.zip`, SHA256
  `9BD5BD4EF8AF35F34AFE736EA9A5900E54EBBE86F2E30A3AB51CBF5F257C6AD8`.
- All 19 entries and three payload hashes were verified independently.
- The complete integrated suite, six linked adapter checks and six isolated
  installer/rollback scenarios passed. These are technical checks only.

## Observations and uncertainty

The native first-person scope was armed. A restored attached-effect GPU
transaction was logged. No restored rig transaction was logged; hands/gun
coverage was not established in the real scene. Requested/default/cache
camera values alone did not prove the final projected world angle.

Two interference paths remain hypotheses, not demonstrated causes:

1. The ADS layer could accept an already-adapted locked lens, or an unzoomed
   higher candidate with an older usable baseline. Such inputs can produce a
   double correction. The rejected session did not capture those source values.
2. CPU-shadow restoration does not restore GPU cb1 already uploaded/bound.
   An unrelated draw before another native commit could consume the modified
   buffer. Actual world/NPC draw ordering was not established.

The primary native caller's SceneInfo argument was checked and is correct.
The specific guard that prevented a rig transaction remains unidentified.
These facts do not justify further blanket shader-buffer changes.

## Verified return to accepted 0.18.1

After normal game closure, the checked installer backup was restored:
`InfiniteMod-Backups/install-20261013-000138-846-bb656b09`.
Rollback retained the removed state and updated preferences for recovery in
`InfiniteMod-Backups/before-rollback-20261013-000953-608-579b3313`.

Installed accepted ASI is again
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
INI is `C6B4E75BDA653B31B9BF2A88C09CF2FAE0057D7CF6DBA0EEA3B5EE8C342A586D`;
shared loader remains `D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6`.
The public 0.18.1 release is unchanged. Game progress was not rolled back.

After relaunch, the player reports that the world remains deformed at requested
120 and also at100. File restoration is verified; visual restoration is not.
The [restart record](FOV-RESTART.md) supersedes any assumption that removing
0.21 alone resolves the complete complaint.

## Next approach

Start from accepted English 0.18.1 again. Establish actual horizontal/vertical
projection and viewport aspect before changing camera behavior. Define one
consistent lens and automatic aspect adaptation; preserve world proportions
first. A new branch must not inherit the rejected GPU/ADS layers.
