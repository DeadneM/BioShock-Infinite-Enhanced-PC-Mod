# 0.23.0 local native aiming delivery

October 14, 2026. Experimental native-camera candidate on
`experiment/native-zoom-v0.23`, from accepted English 0.18.1.
The player rejected Panini and requested native FOV work. Accepted ASI B13DBE...
was independently restored before this candidate was prepared.

Objective and player criterion: [BUILD-v023.md](BUILD-v023.md).
New proportional aiming defaults OFF; the complete existing INI, custom menu
keys, accepted commands and native world/aspect calculations are preserved.
No independent arms/weapon fix, game visual validation or public release is claimed.

## Built and checked payload

**BUILT, CHECKED, then REJECTED and ROLLED BACK.**
The player observed no ON/OFF difference. Accepted English0.18.1 is restored
with the complete current INI preserved; see [the verdict and actual installed
hashes](REJECTED-v023.md). The installation receipt below is historical.

| Payload | SHA256 |
| --- | --- |
| InfiniteMod.asi | 67B8E723113C969C0B55496031B23646DBD2C77F92ACFA79C3DC502244B490F3 |
| Packaged default InfiniteMod.ini | 34B07C7883B7DB7DD9132EC1840FEF2FF8038929758A5DBFB26E656F2B7E9BE7 |
| Compatible dinput8.dll | D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6 |

The complete retained suite and new native camera/optical/shortcut checks passed,
as did real menu/state/resize checks at720p/4K/ultrawide. The720p main menu was
visually inspected. Production compilation completed without warnings/errors.
All six private installation/rollback scenarios passed with the final ASI.
Private receipts: `InfiniteMod/tools/v023-final-tests.log`,
`v023-final-tests-build.log`, `v023-final-build.log` and `v023-distribution-tests.log`.

## Verified local archive and installation

Source checkpoint: `514714ac6a4f12a91994040657f65efd658ff4da` on
`experiment/native-zoom-v0.23`; runtime source is unchanged since its checked build.
Local ZIP: `releases/InfiniteMod-v0.23.0-native-aim-experimental.zip`.
SHA256: `80BC7623FA3DC0B67971CEF6EA0629D278C7B787E22AC9DBC128D0A3B997A626`.
Size:4,632,602bytes. All23 root entries are verified, without an enclosing
folder, game executable, game-derived data or private test artifacts.
The archive was extracted into the new `releases/install-v023-checked` stage;
all three staged payload hashes match its manifest and checked build.

Process inventory and the installer confirmed closure before replacement.
The transaction is INSTALLED. Independent post-installation checks confirm:

- Installed ASI matches67B8E... above, and accepted English0.18.1 B13DBE... was
  backed up and independently verified.
- The complete existing INI is retained byte-for-byte:
  `EBE9526EF616D22D0B77A44973C9F28C3781FDB777FE03136B948718EEE8B362`.
- The existing compatible loader is retained with its hash above.

Backup: `C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-101515-179-05f464de`.
Private install receipt: `InfiniteMod/tools/v023-install.log`; the backup's
`transaction.json` records that only the ASI was replaced. Native game
configuration and progress saves were not replaced or rolled back.

Existing Backtick/Home menu keys, FOV100, HUD1.25, speed1.25, aim/crosshair and
logo preferences are retained. Missing ProportionalAim defaults OFF. The game
was relaunched PID16896 for the requested player comparison. All addresses from
older game sessions are stale. No visible success is inferred from installation.

After normal game closure, restore this transaction with:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File 'C:\Games\BioShock Infinite\InfiniteMod\publish\releases\install-v023-checked\Rollback.ps1' -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-101515-179-05f464de'
```

The ZIP is retained unchanged. Its bundled packaging-checkpoint documentation
is historical; this verified receipt supersedes pre-installation status.
No public0.23 branch/tag/release is announced. Public accepted0.18.1 is unchanged.
