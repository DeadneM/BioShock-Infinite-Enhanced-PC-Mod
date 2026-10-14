# 0.24.1 control revision delivery

Base: accepted English0.18.1, separate experiment/native-zoom-target branch.
Status: BUILT, CHECKED, INSTALLED, EXPERIMENTAL; partial user validation below.
Scope update: the player validates ON/OFF menu switching and confirms a visible
aiming difference. This is partial USER-VALIDATION, not validation of independent
first-person projection, world perspective or the exact held zoom ratio.
The0.24 lens experiment is unchanged and remains unvalidated. Objective and
visual criterion: [BUILD-v0241.md](BUILD-v0241.md).

Final ASI:61523562EF1018A9F271EE259C287248CE21A966952A53C19E19D5F99F245F8A.
Default INI:1DF5F344F596FE60117E35D9DA804E405A77265B4774AC346C658BC34AD692AD.
Compilation is warning/error-free. Native, command, control/persistence/failure
and real DX11 layout/state tests pass. Final720p ON/OFF previews are inspected
after ImGui's automatic size stabilizes. Private receipts: tools/v0241-build.log,
v0241-tests-build.log, v0241-tests.log and v0241-package.log.

Checked ZIP: `releases/InfiniteMod-v0.24.1-aim-control-experimental.zip`.
25root entries,4641746bytes. SHA256
883439D02E1C08C26453B5226393BF542A68D05F2DB4F55FD57558989B72158E.
Package verification checks every entry against staged content; independent
count/extracted ASI/INI checks agree. Stage:`releases/install-v0241-checked`.
The package contains the pre-installation objective/status, not this later receipt.
No public push/release is claimed.

The first fixture attempt stopped at Assert-GameClosed before modifications.
The player then confirmed normal closure; explicit authoritative inventory
confirms zero game processes. All six final-payload installer/rollback scenarios
now pass: tools/v0241-distribution-tests.log. Technical success still does not
establish the player's real menu or rendered-camera result.

For a direct accepted18.1 rollback, the entire current INI was copied and checked:
`tools/v0241-current-settings-33e28c9b0a9b418bb063c6283dfa60e7.ini`, SHA256
94C231D77A3DDDE30D223DBCA13DFF36BAA4B92356747DA52C204F52B901DE6F.
The checked0.24 rollback restored accepted B13DBE..., then that exact current
INI was reapplied. Removed0.24 state is preserved in
`C:\Games\BioShock Infinite\InfiniteMod-Backups\before-rollback-20261014-125422-085-97632fa6`.
0.24.1 was installed only from its checked stage. Receipt: tools/v0241-install.log.
The actual transaction is INSTALLED; candidate615235..., full current INI94C...
and loaderD5A059... hashes match independent post-installation checks.
Only the ASI is replaced in the new installation; the complete current INI,
keys and loader are preserved. Native configuration files and saves are not replaced.

New accepted-ASI rollback backup, independently B13DBE...:
`C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-125423-265-d8a64c09`.
After normal closure, from the checked extracted stage:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-125423-265-d8a64c09'
```

Player test: Backtick, Ctrl+F9 with the main menu open; fully release between
presses. The aim row must alternate green ON/grey OFF, with matching save/UI
logs and restart read. Only then assess the actual held-aim camera behavior.
