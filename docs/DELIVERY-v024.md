# 0.24 local delivery receipt

October14,2026. Base: user-accepted English0.18.1. Candidate: BUILT, CHECKED,
EXPERIMENTAL; not installed or user-validated. No GitHub release or push is claimed.

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

Installed runtime still accepted0.18.1 ASI B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9.
Complete current INI FDB4CBE91CABDE593F9005386B77D23EE4E846B2154EE4D345F0ECA97A5DD131
and compatible loader remain unchanged. The installer preserves existing INI
and loader byte-for-byte and backs up the accepted ASI before replacement.

Normal closure has been requested because the ASI is still loaded. Before
installation, run all six private install/rollback fixtures with the final
payload, verify closure, then install only from the checked extracted stage.
Record the actual transaction and independent hashes here after installation.

Player test: world120, main menu, Ctrl+F9 selects PROPORTIONAL/NATIVE. Compare
entry, held aim and release, then change weapons. The chosen hip endpoint
should recover to120 and zoom should be visibly gentler on PROPORTIONAL.
If rejected, close normally and use the checked transaction rollback. No
whole-first-person size, placement or lens correction is claimed.
