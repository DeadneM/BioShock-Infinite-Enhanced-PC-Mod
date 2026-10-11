# BioShock Infinite Enhanced PC Mod

v0.15.0 checkpoint user-validated on October 8, 2026: crosshair and all three infusions at 10/10.
The validated binary is retained outside the repository: SHA256 `0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
This source was recovered from reads and changes in this work session, then checked locally.
The camera policy is the v0.15 policy; experimental viewmodel corrections are excluded.
High FOV may still distort first-person rendering and retains native aiming zoom: known defect.

Compatible only with the inspected GOG x86 1.1.25.5165 binary. No game EXE, assets, or saves are included in this repository.
Build with Zig 0.13.0; place Zig in `tools/zig-windows-x86_64-0.13.0`, then run `build.ps1` / `test.ps1`.
ImGui and MinHook dependency licenses accompany their sources.