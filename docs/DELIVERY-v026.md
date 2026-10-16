# 0.26.0 green diagnostic delivery checkpoint

October 16, 2026. Experimental branch: experiment/green-layer-selector.
Base and exact scope: see GREEN-DIAGNOSTIC.md. This is a visual selector, not a
lens correction and not proof of separate native rendering layers.

The initial private fixture passed mask pixels and native-color/state restoration,
then exposed DirectX rewriting patched context-table slots. That interception was
replaced before delivery. The function-detour implementation passed retained
gameplay/config/key/overlay checks and all four direct draw replay checks.
Final checks and payload hashes are recorded below after packaging.

No external process capture or game launch is authorized by this test request.
Installation must preserve the current complete INI and loader and back up the
actual preceding ASI. Native user options are not edited by this package.
No public GitHub release is claimed.

## Final build and fixture checks

Status: **BUILT / CHECKED / EXPERIMENTAL**, not user-validated in Infinite.
Final warning-free x86 ASI SHA256:
`167DD5947FF5BCD4EB8D93F1B26401030882B9F61B719B7163B708183BA01E72`.
The final offscreen test passes the four direct draw variants, actual green mask
pixels, unchanged blue native target, PS/RT/depth/blend restoration, default OFF,
focus edges, temporary key suppression and mask resizing. Retained gameplay,
configurations, commands, bindings and overlay viewport checks pass as well.
The 720p menu image was inspected. This does not establish real game coverage.

Transactional installer fixtures cover complete INI preservation, rollback with
later INI edits, unknown loader/altered package refusal, first installation,
shared loader retention and refusal to overwrite a changed installed ASI.
Private logs: tools/green-build.log, green-build-test.log, green-tests.log and
green-distribution-tests.log. No private data is included in the package.

## Verified ZIP and local installation

Root ZIP: releases/InfiniteMod-v0.26.0-green-diagnostic-experimental.zip,
19 entries, SHA256
`60EADEF455065AFD6B79C4649C4A5FF3CD11E9D8B9A7C8FE05E9AE2F21F21146`.
Every ZIP entry matches staging; independent extraction checks manifest payload
hashes and x86 format. Scripts and final ASI match the tested installer inputs.

Installed after the installer's normal game-closed guard. Backup transaction:
`InfiniteMod-Backups/install-20261016-105719-709-3b9284f3`.
Installed ASI matches167DD594 above. Actual previous ASI is the0.25 candidate
48B3CF39629183DCA56ACF1E1920CAFF0F8976DC2180CA9AD1A5A773232FCF09.
The current complete mod INI remains
7981A76B712CF1963240E7AFC4956A4F6CA3DF738F40B03F4B25AD1AE705BEB7;
loader and current native user options are also byte-identical before/after.
No game launch, process capture or player validation performed.

After normal game closure, from releases/install-v026-checked:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261016-105719-709-3b9284f3'
```

This restores the actual preceding0.25 module, not an invented validated FOV fix.
ZIP documents preserve their pre-install checkpoint; this source delivery record
supersedes pending installation text without overwriting the immutable archive.
