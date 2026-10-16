# 0.28.0 delivery checkpoint

October16,2026. **BUILT / CHECKED / EXPERIMENTAL**; new game/controller result
not yet player-validated. Base0.27 baa1715, with the user's requested restoration
of63f87fe camera/aim policy. Branch experiment/live-fov-controller-aim.

Final warning-free x86 ASI SHA256:
F7D9F5F4A744DAA8F972F28C7E981F9ADE33DB8D64D64441D2D278E6EA8ED593.
See BUILD-v028.md for exact controls and limits. No independent viewmodel fix.

## Checks

The final retained suite passes. Restored native x87 float-argument/return ABI,
live hip FOV changes, proportional ordinary/TBar targets,2000 native entry/release
blend iterations, active-weapon/caller/thread/cinematic/baseline guards, and
unchanged weapon properties are checked in private fixtures. This is not a
recording of the running game's actual rendered FOV.

XInput fixtures cover all four slots, LT threshold/release, all ten choices,
disconnect, rebind release gate, default/persistence and failed saves. Real
controller hardware has not been exercised. Production/native state-machine
checks retain menu/focus/actor/key release guards and shared mouse/controller OR.
The private Present fixture checks global commands, new action, real INI zoom
saves and rebinds, menu suppression and UI/render-state at720p/4K/ultrawide.
The720p menu image was inspected. No game was launched or externally observed.

Final ASI installer/rollback fixtures pass: custom INI and loader preservation,
later INI recovery, altered/unknown payload rejection, first installation and
shared-loader/changed-module guards. Private logs: tools/v028-build.log,
v028-test-build.log, v028-tests.log and v028-distribution-tests.log.

ZIP/install/source/tag/public-download verification are recorded below after
completion. The public release must be a prerelease and state the validation
limits. The user authorized publication; accepted main/stable need no overwrite.

## Verified package and local installation

ZIP: releases/InfiniteMod-v0.28.0-live-FOV-controller-experimental.zip,19 root entries.
SHA256:C9B991C26192FD41215A4ADD0D65642BD49A0554AD7FD691BF9F1EB9DD2D30AD.
Every entry matches staging; independent extraction verifies module hashes and
root paths. Installer checks x86/supported host. No helper/launcher or private
capture/fixture data is shipped. Final scripts and ASI match tested payloads.

Installed after normal closed-game guard. Transaction:
InfiniteMod-Backups/install-20261016-123724-743-a9565769.
Installed ASI matches F7D9F5F4 above; saved prior0.27 module is
EFA819852F2541E32F611CCC1CF581460B946EDD3A250AB8308C04172361970D.
Complete current mod INI, loader and native user options are byte-identical
before/after. New controller button defaults LT without rewriting the INI.
No game was launched; no new player/hardware validation is claimed.

After normal closure, from releases/install-v028-checked:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261016-123724-743-a9565769'
```

ZIP documents preserve their pre-install checkpoint; this cumulative source
record supersedes pending text without modifying the immutable archive.
