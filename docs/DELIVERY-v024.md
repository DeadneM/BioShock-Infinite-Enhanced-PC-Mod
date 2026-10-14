# 0.24 local delivery receipt

October14,2026. Base: user-accepted English0.18.1. Candidate: BUILT, CHECKED,
EXPERIMENTAL, INSTALLED; not user-validated. No GitHub release or push is claimed.

Objective and visual criterion: [BUILD-v024.md](BUILD-v024.md).
Final native/menu/configuration suite: tools/v024-tests.log.
Final warning-free compilation receipts: tools/v024-build.log and
tools/v024-tests-build.log. Test results establish ABI, policy and regression
coverage, not visible correction in the real game.

Local installable archive:
`releases/InfiniteMod-v0.24.0-native-target-experimental.zip`
24 entries,4637623bytes, no enclosing folder.
SHA256 `4DB4B7C0F430A2316D5A58982072310242CDC7831F49C4C8B1C640841835C201`.
The package procedure verifies every entry against its staged source; an
independent inventory confirms the layout. Extracted install stage:
`releases/install-v024-checked`.
ASI `A70E724FDB14E7FC3A4ED755A191B7655B944E3E0D9BB94250ACD6260F178E19`.
Default INI `1DF5F344F596FE60117E35D9DA804E405A77265B4774AC346C658BC34AD692AD`.

Accepted0.18.1 ASI B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9 is backed up and independently verified.
Complete current INI FDB4CBE91CABDE593F9005386B77D23EE4E846B2154EE4D345F0ECA97A5DD131
and compatible loader remain unchanged. The installer preserves existing INI
and loader byte-for-byte and backs up the accepted ASI before replacement.

The player confirmed normal closure and independent process inventory confirms
it. All six private install/rollback scenarios pass with the final ASI payload:
custom-INI update/rollback, unknown loader, altered package, first install,
shared loader and modified-ASI rollback refusal. Receipt:
tools/v024-distribution-tests.log. Actual installation from the checked stage
completed with state INSTALLED. Receipt: tools/v024-install.log. Independent
hashes confirm candidate ASI A70E72..., complete INI FDB4CB... and loader D5A059....

Accepted-runtime backup:
`C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-113404-373-950f43fd`.
Only the ASI was replaced; INI and loader were retained byte-for-byte.
Native configurations and saves were not replaced by the installer.

After normal game closure, checked rollback command from the extracted stage:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-113404-373-950f43fd'
```

The ZIP is the checked pre-installation package; its bundled records describe
the state at packaging time. This current receipt adds actual installation.

Player test: world120, main menu, Ctrl+F9 selects PROPORTIONAL/NATIVE. Compare
entry, held aim and release, then change weapons. The chosen hip endpoint
should recover to120 and zoom should be visibly gentler on PROPORTIONAL.
If rejected, close normally and use the checked transaction rollback. No
whole-first-person size, placement or lens correction is claimed.
