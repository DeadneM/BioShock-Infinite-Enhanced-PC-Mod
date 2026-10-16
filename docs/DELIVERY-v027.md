# 0.27.0 delivery checkpoint

October 16, 2026. **BUILT / CHECKED / EXPERIMENTAL**. Not yet player-validated.
Branch: experiment/global-hotkeys-native-settings. Base and exact user-facing
behavior: BUILD-v027.md. No optical correction, green runtime or game launch.

Final x86 ASI SHA256:
EFA819852F2541E32F611CCC1CF581460B946EDD3A250AB8308C04172361970D.
Native preparation helper SHA256:
E49D620123B159373BEF45490684A1A692A37B30EF7962431C0093233F925989.

Warning-free builds and the retained suite pass. Real offscreen Present tests
cover commands without menu, default setting, persistent toggle, unfocused
suppression, binding capture/editor suppression, existing custom bindings,
bounded native slider shortcuts and saved preference. Menu/layout/state checks
pass at720p/4K/ultrawide including the new last shortcuts page.720p image inspected.
Native config fixtures check exact slider-only edits, separate immutable backup,
idempotence, UTF-8/UTF-16LE, invalid values, restore and unrelated-byte preservation.
Legacy native-range/configuration tests remain passing.

Final ASI installer fixtures pass preservation of custom INI/keys, later-INI
recovery, unknown loader/altered package refusal, shared loader and changed-ASI
guards. Launcher syntax is checked without execution. Production ASI excludes
green diagnostic labels, and source excludes its include/hooks.

Private logs: tools/v027-build.log, v027-test-build.log, v027-tests.log,
v027-distribution-tests.log. These checks are not a live-game result. Actual
native slider/UI application, gameplay commands and persistent key behavior
need player confirmation. ZIP verification/installation are recorded separately
after completion. No GitHub publication is claimed.

## Verified package and installation

ZIP: releases/InfiniteMod-v0.27.0-global-hotkeys-experimental.zip,21 root entries.
SHA256:415BFA6E3728D11D208494C54B5D1B57BDB6CA0E5650897EC301F389F2E9C2D1.
All entries were verified against staging; independent extraction checks module
and helper hashes and both x86 formats. Extracted final installer inputs match
the previously tested scripts/payloads. No private fixtures/game data are shipped.

Installed with normal closed-game guard. Transaction:
InfiniteMod-Backups/install-20261016-120451-235-ab63bfab.
Installed ASI matches EFA81985 above. Previous module is0.26 diagnostic,
SHA256167DD5947FF5BCD4EB8D93F1B26401030882B9F61B719B7163B708183BA01E72.
Complete current mod INI, existing loader and native user options were checked
byte-identical before/after. Missing GlobalHotkeys defaults ON without rewriting
the user's INI. No native preference was applied and no game was launched.

Status: BUILT / CHECKED / INSTALLED / EXPERIMENTAL; player confirmation pending.
From releases/install-v027-checked, after normal closure:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261016-120451-235-ab63bfab'
```

The native slider changes only on explicit subsequent use of Launch-Mod.ps1.
The archive retains its pre-install checkpoint; this source record supersedes
that pending text without repackaging the verified ZIP.
