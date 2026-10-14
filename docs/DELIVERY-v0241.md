# 0.24.1 control revision delivery

Base: accepted English0.18.1, separate experiment/native-zoom-target branch.
Status: BUILT, CHECKED, EXPERIMENTAL; not installed/user-validated.
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

Actual installed runtime remains0.24 A70E724F..., PID11968. Complete current
INI94C231D77A3DDDE30D223DBCA13DFF36BAA4B92356747DA52C204F52B901DE6F
contains NativeAimZoom1 and current user prefs. These files are unchanged by
preparation. The final installer fixture attempt stopped at Assert-GameClosed
before modifications. A mixed PowerShell table obscured the process row in the
earlier inventory; explicit JSON reconfirmed the same live session. Normal
closure has been requested. Do not claim successful fixtures or installation.

After closure: run all six final-payload installer fixtures. For a direct
accepted18.1 rollback reference, preserve the complete CURRENT INI, use the
checked0.24 rollback from install-20261014-113404-373-950f43fd, reapply the exact
preserved current INI, verify accepted B13DBE... and loader D5A059..., then install
only the new checked stage. Verify the new transaction/accepted backup and
unchanged complete INI before relaunch. Record actual receipt here.

Player test: Backtick, Ctrl+F9 with the main menu open; fully release between
presses. The aim row must alternate green ON/grey OFF, with matching save/UI
logs and restart read. Only then assess the actual held-aim camera behavior.
