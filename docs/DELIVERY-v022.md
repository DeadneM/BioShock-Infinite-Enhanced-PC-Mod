# 0.22.0 local experimental delivery

October 13 delivery; October 14 verdict. **BUILT, CHECKED, then REJECTED and ROLLED BACK.**

The player declined this lens effect as the FOV solution. Accepted English
0.18.1 was restored and independently verified after closure on resumption.
See [the verdict and recovery receipt](REJECTED-v022.md). The installation
record below is retained as history, not the currently installed state.

Final source: `1c28bb1d5a92d91ac5627c01776db353c1a97b85` on
`experiment/panini-world-v0.22`, from accepted English 0.18.1.
The final integrated suite and real graphics/resize checks passed. No public
branch/tag/release is announced. See [BUILD-v022.md](BUILD-v022.md).

Final local ZIP: `InfiniteMod-v0.22.0-Panini-final-experimental.zip`, 4,630,466
bytes, 21 verified root
entries, no enclosing directory or game/private assets.
SHA256 `6F4792F2899E7449EF596C2E27302687B67E9D148ED468F9BB6E10C5EC409273`.
The archive was independently read, its manifest and three payloads verified,
and extracted to the checked installation staging directory.

| Payload | SHA256 |
| --- | --- |
| InfiniteMod.asi | 69A860A631BBA67C438EAD9A8111C2499CBD2155C7A9C612E5E2D670662082B9 |
| InfiniteMod.ini | DF48E6EFC273D03E2EC6CFD6B00439D0A43F36A658A80548DEDABE4ADDDFCF72 |
| dinput8.dll | D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6 |

The earlier `InfiniteMod-v0.22.0-Panini-experimental.zip`/ASI1E30... was an
intermediate package before the native array-slack guard refinement. It was
not installed and is superseded by the final archive above. Archives are
retained unchanged as audit records; use only the final package.

## Verified installation

The player confirmed normal game closure. All six isolated installer/rollback
scenarios passed using the final ASI: custom INI preservation and recovery,
unknown-loader rejection, tampered-package rejection, first-install rollback,
shared-loader retention, and changed-ASI rollback rejection. The fixture only
modified private copies, not the real game directory.

The final archive's checked staging directory `releases/install-v022-checked`
was then used for the actual installation. Its transaction is **INSTALLED**.
Independent checks confirm:

- Only InfiniteMod.asi was replaced; installed hash matches the final payload above.
- Accepted English 0.18.1 ASI was backed up, hash
  `B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
- The entire existing INI was retained, hash
  `EBE9526EF616D22D0B77A44973C9F28C3781FDB777FE03136B948718EEE8B362`.
- The existing compatible loader was retained, hash matching the table above.

Backup: `C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261013-164852-884-36a295a3`.
Private evidence: `InfiniteMod/tools/v022-distribution-tests.log` and
`InfiniteMod/tools/v022-install.log`, plus that backup's `transaction.json`.

The game was relaunched. Fresh startup logs confirm 0.22, the scene adapter,
the native 3840x2160 game swapchain, and keybind parsing with zero invalid
entries/conflicts. Existing Backtick/Home menu keys, FOV100, HUD1.25 and
speed1.25 were retained. The INI has no WorldProjection key, so Panini starts OFF.

During the subsequent player comparison, the game log reported composite
submission at native horizontal100 and120 (16:55-16:56). This establishes that
the qualified scene boundary reaches the effect in the real session. It does
not establish acceptable framing, HUD ordering, performance or visual alignment.

Compare Ctrl+F9 at FOV100 in active gameplay. The required world/HUD/menus/aiming
and window-transition visual result is pending. A submitted-composite log
does not replace player confirmation. No public 0.22 release was created.

After normal closure, this transaction can be rolled back with:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File 'C:\Games\BioShock Infinite\InfiniteMod\publish\releases\install-v022-checked\Rollback.ps1' -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261013-164852-884-36a295a3'
```

The final ZIP is retained unchanged. Its pre-installation documentation is
historical; this verified receipt supersedes those pending-install statements.

This delivery does not claim independent weapon/hand placement is corrected.
