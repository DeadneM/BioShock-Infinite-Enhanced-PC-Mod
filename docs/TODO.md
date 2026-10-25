## October 25 - 0.31 signed native distance

Player accepted closer offset10 in0.30.1. New requested range -50..+50; Ctrl+Minus decreases, Ctrl+Plus increases by2 with endpoint clamping. Old closer10 loads as-10 without INI rewriting. Zero/native ADS and existing features retained. Extended range remains experimental. See [scope and controls](BUILD-v031.md).

## October 25 - 0.30.1 direction correction

Player reports all sensitivity parameters appear to work and confirms arms and weapon move together in0.30. Requested change: bring the native first-person root closer to the camera. Keep the same0..10 control, zero default, ADS bypass, keybinds and INI. Only local-X sign changes; temporary effects still require visual confirmation. See [build scope](BUILD-v0301.md).

## October 25 - 0.30 native position experiment

Requested: adjustable backward movement of the whole first-person root. Default0; Ctrl+0 cycles bounded offsets; ADS bypassed. Exact on-disk native offset consumer identified. Visual coverage remains unverified; no camera/projection fix is claimed. Keep 0.29 rollback and existing personal settings. See [scope and player test](BUILD-v030.md).

## October 17 - 0.29 local input profiles candidate

Six independent mouse/controller normal, ADS and sniper multipliers are implemented on experiment/input-comfort. All default to x1. Local tests pass; actual game consumption is unverified. Bobbing/sway remains pending. No installation, publication or game launch. See [build record](BUILD-v029.md).

# Cumulative TODO

## 0.28 live FOV / XInput hold aim

- [x] Retain0.27 accepted behavior and separate experimental branch.
- [x] Restore live camera/aim policy only; no viewmodel or rendering patch.
- [x] Implement LT-default XInput hold aim, named choices and remapping entry.
- [x] Final build, restored ABI/target, input/persistence and UI checks.
- [x] Verify package and installer/rollback with exact final payload.
- [x] Install after normal closure, preserving all current settings.
- [x] Publish branch/tag/prerelease and verify downloaded ZIP/hash/source.
- [ ] Obtain player confirmation of live FOV, proportional ADS and controller.

## 0.27 global shortcuts / native settings

- [x] Retire green runtime; preserve accepted gameplay and custom assignments.
- [x] Implement default-ON persisted global toggle and remapping entry.
- [x] Add native slider preference controls and closed-game preparation helper.
- [x] Final build, retained/new checks and menu layout verification.
- [x] Root ZIP and installer/rollback verification.
- [x] Install after closure, preserving current INI and native options.
- [ ] Player verifies global commands and next-launch native settings.

Optical lens/viewmodel correction is abandoned at the player's request. Earlier
pending research below is historical, not an active task.

## Limited lens prototype gate - October 16

- [x] Record player-visible candidates23/24/25 and saturated registry.
- [x] Check whether the green implementation provides isolated color/coverage.
- [x] Exclude masked final-image warping: missing clean underlying scene/layer.
- [ ] Qualify complete first-person composition or native projection boundary.
- [ ] Only then implement the common lens prototype; no new build justified yet.

See LENS-PROTOTYPE-GATE.md. No lens correction is built or validated.

## 0.26 temporary green visual selector

- [x] Read current delivery, source and failed selection evidence.
- [x] Isolate experiment; preserve camera policy and saved bindings.
- [x] Implement opt-in shader-pair silhouettes and explicit unknown ownership.
- [x] Complete final build, offscreen state/pixel and retained checks.
- [x] Check root ZIP and transactional installer/rollback fixtures.
- [x] Install only after confirming normal closure, preserving current INIs.
- [x] Player supplies screenshots identifying green candidate groups after manual launch.
- [ ] Do not mistake successful green geometry for a lens-ready segmentation.

## 0.25 native slider experiment

- [x] Branch from retained63f87fe and explicitly retire experimental aim/camera adapters.
- [x] Preserve gameplay, speed, keys and INI; exclude draw observation from the build.
- [x] Implement single-key native range preparation and immutable backup/restore.
- [x] Preserve native slider/other settings, including later edits during rollback.
- [x] Build and run final retained/configuration checks.
- [x] Verify27-entry installable ZIP and extracted payloads.
- [x] Run final installer/rollback checks after normal closure, including native range/helper paths.
- [x] Install with current settings preserved and launch through native preparation.
- [x] Obtain player feedback: native slider changes framing; proportions remain unsatisfactory.
- [x] Record the narrow result without claiming a visual fix or independent weapon lens.
- [x] Compare UE3/UDK primary source for component FOV and particle coordination.
- [x] Compare X/base component construction and native proxy factory offline; no direct UDK-style specialized factory identified.
- [ ] Qualify shared primary mesh transform and particle counterpart before selecting a functional implementation.
- [ ] Ask before every game launch/relaunch; no further draw observations.

## 0.24.2 finite draw observation

- [x] Build opt-in observer without changing retained controls or rendering values.
- [x] Qualify opaque native ABI, mid-function callsites and real offscreen draw/pixel equivalence.
- [x] Run retained gameplay/UI/persistence suite and check default runtime sections.
- [x] Verify 27-entry installable ZIP, extracted payload hashes and x86 format.
- [x] Run final payload installer/rollback fixtures after normal closure.
- [x] Install after normal closure, preserving the complete latest INI and 0.24.1 backup.
- [x] Run one bounded actual game request:204160 calls,zero owned rows; inconclusive and abandoned at the player's request.
- [ ] Use correlated evidence to select a coherent whole-first-person native view approach.
- [ ] Deliver and obtain user confirmation of an actual first-person FOV correction.

## Whole first-person restart

- [x] Preserve the confirmed0.24.1 control/visible-effect checkpoint63f87fe.
- [x] Create a separate first-person branch with initially identical runtime.
- [x] Compare immutable primary Infinite source; distinguish one-lens Steam evidence from GOG proof.
- [x] Record the failed held-aim capture without claiming a measured result.
- [ ] Establish actual shared native draw/preparation boundary for the entire rig and effects.
- [ ] Qualify a single lens policy with world/HUD exclusion before a new functional build.
- [ ] Confirm visible all-weapon/prop/effect behavior before retention.

## Current control candidate0.24.1

- [x] Preserve the current lens experiment; prepare explicit colored ON/OFF feedback.
- [x] Share production persistence with the menu test instead of an always-successful stub.
- [x] Pass real INI/state/failure checks and retained suite; compile final ASI.
- [x] Build and verify the25-entry ZIP and extracted payloads.
- [x] Run final-payload installer fixtures after normal closure.
- [x] Install after closure with complete current INI/key preservation and accepted rollback.
- [x] Confirm visible ON/OFF feedback; player also confirms a visible aiming effect.
- [ ] Qualify the exact held-aim camera result; ON appears nearly unchanged in FOV.

## Current candidate 0.24 - authored native target

- [x] Requalify the original getter, native interpolation consumers and live owner/field schema.
- [x] Prepare one optional target-return policy on the accepted English base.
- [x] Pass native getter/blend/guard and retained tests; compile final x86 ASI.
- [x] Verify installable archive, accepted rollback and complete INI preservation.
- [ ] Obtain visible ON/OFF zoom, release and weapon-change confirmation.
- [ ] Qualify a separate whole first-person rendering/placement solution.

## Current native zoom target restart

- [x] Reject0.23 after the player's no-difference result; identify actual LockedFOV coverage failure.
- [x] Verify closure and restore accepted English0.18.1, preserving the complete current INI/keys.
- [x] Verify camera SetFOV descriptor and native target/locked values in a bounded read.
- [ ] Qualify the native target before interpolation; do not rescale an already adapted/interpolated lens.
- [ ] Capture the stable native aiming target and its transition on the accepted base; previous PID is stale.
- [ ] Build only after a single common native input/target path and rollback are qualified.
- [ ] Keep the independent whole first-person placement/lens goal separate and unclaimed.

The0.23 build/test/delivery checklist below is historical and superseded by
its rejection. No proportional aiming feature is retained from that build.

## October 14 - native camera restart

- [x] Record the player's rejection of the 0.22 lens effect and resumption.
- [x] Verify closure and restore accepted English 0.18.1; preserve removed state for recovery.
- [x] Prepare one optional proportional native aiming policy from accepted 0.18.1, excluding rejected runtime layers.
- [x] Compile the final x86 ASI and pass native optical/ABI checks plus the retained suite.
- [x] Check all24 controls and main/shortcut menu bounds/state/resize at720p/4K/ultrawide.
- [x] Pass all six isolated installation/rollback scenarios with the final0.23 payload.
- [x] Verify the final23-entry archive and extracted payload hashes.
- [x] Install0.23 after closure; independently check accepted-ASI backup and unchanged complete INI/loader.
- [x] Relaunch the game for one ON/OFF native aiming comparison.
- [ ] Obtain the player's ON/OFF aiming and return-to-hip-fire result before retaining 0.23.
- [ ] Preserve native aspect handling and every accepted command, INI preference and shortcut.
- [ ] Keep the whole first-person placement/lens correction separate and unclaimed until proven.

## Rejected candidate0.22 - stronger Panini (historical)

- [x] Review the supplied NPC/grip video and record its FOV100 provenance and limits.
- [x] Prepare a numerical comparison; player selects stronger base Panini(d1).
- [x] Isolate the candidate from accepted English0.18.1 and rejected0.21 layers.
- [x] Build opt-in scene composite and configurable Ctrl+F9, preserving the23 prior actions.
- [x] Check real shader output, complete context restoration, native buffer bytes, feature level and resize.
- [x] Pass the retained command/controls/configuration regression suite.
- [x] Verify package/archive and all six installation/rollback scenarios on the final payload.
- [x] Install after normal closure; independently verify accepted-ASI backup and complete INI/loader preservation.
- [x] Verify fresh 0.22 startup, scene adapter, native 4K swapchain and keybind parsing without conflicts.
- [ ] Obtain player confirmation of the world change and immediate OFF return.
- [ ] Confirm native HUD,3D Vigor HUD effects, menus, aiming and window transitions remain correct.
- [ ] Correct whole first-person placement/lens independently after the world result is accepted.

The selected offline preview is not validation of this candidate in-game.

## Current restart - October 13, 2026

- [x] Record 0.21 as REJECTED after the player's visual failure.
- [x] Restore accepted English 0.18.1 after normal closure and verify ASI/INI/loader.
- [x] Archive the rejected source and start `experiment/physical-fov` from accepted 0.18.1.
- [x] Define the coherent horizontal/vertical lens objective and first success criterion.
- [x] Establish native input convention: horizontal 16:9 reference, native Hor+ and verified current 4K viewport methods.
- [x] Read native cb1 shadow agreeing with H120/V88.507/aspect16:9; keep GPU/draw-ownership limits explicit.
- [x] Obtain the requested 100 player comparison: stretching/flattening still reported, log confirms actual change.
- [x] Obtain and examine the October 13 NPC/grip video; record the optical and measurement limits.
- [ ] Qualify the rendering boundary before changing it; an external callback is not automatically a matrix override.
- [ ] Implement one backwards-compatible world-lens change supported by that evidence.
- [ ] Check native aspect handling before adding any automatic aspect correction.
- [ ] Obtain the player's confirmation of world/NPC proportions before another first-person build.

This restart checkpoint is historical and superseded by the current 0.22
candidate above. The local build, ZIP and installation are checked; no public
0.22 release or player visual validation is claimed.

## Validated features to preserve

- [x] Overlay on the game's swapchain and access to the main menu.
- [x] Automatic skipping of the four logos with loading preserved.
- [x] Weapons and ammunition in both slots, hold-to-aim, speed, and world FOV.
- [x] Vigors, gear, weapon and Vigor upgrades, money, and lockpicks.
- [x] Crosshair hiding and all three infusion gauges at 10/10, confirmed on 0.15.
- [x] Recover the camera base predating the rejected experiments and check its suite.
- [x] Separate stable/v0.15 from the experimental 0.17.1 FOV archive.
- [x] Retain 0.18 controls after player confirmation of F1 refill and shortcut reassignment.

Completed items above do not mean first-person FOV is fixed. The size/position
of Vigor effects remains a separate visual check from the native HUD setting.

## Build 0.18 Ã¢â‚¬â€ shortcuts and gauge refill

- [x] Implement the exact requested mapping, with F5 for the active slot and F6 for the secondary.
- [x] Implement capture and persistence for all 23 shortcuts, including Insert.
- [x] Implement F1 using native health/shield setters and a native salts delta.
- [x] Complete and record the integrated tests and delivery build, without warnings.
- [x] Check menu rendering and bounds at 720p/4K/ultrawide, including all three pages.
- [x] Check navigation and conflicts after reassigning menu-opening shortcuts.
- [x] Prepare and check the installable ZIP and its hashes.
- [x] Install after backup and normal game closure; retain preferences.
- [x] Obtain the player's F1 refill confirmation.
- [x] Obtain the player's shortcut reassignment confirmation.
- [ ] Obtain the player's test of new shortcuts, particularly F2/F5/F6.
- [ ] Obtain reassignment of both menu-opening shortcuts and persistence after restart.
- [x] Retain the accepted controls step separately from rejected FOV experiments.

## GitHub and delivery

- [x] Create the public BioShock-Infinite-Enhanced-PC-Mod repository on DeadneM.
- [x] Complete the 0.18 source upload and check remote references.
- [x] Publish an installable ZIP and verify the direct link and downloaded contents.
- [x] Record 0.18 commit, hashes, release, and downloaded payload in DELIVERY-v018.md.
- [x] Record verified 0.18.1 local package, installation, source and player menu results in DELIVERY-v0181.md.
- [x] Append verified 0.18.1 public release/download results after publication.
- [x] Keep README, build notes, history, technical notebook, and this TODO consistent for 0.18.1 delivery.

## Build 0.18.1 Ã¢â‚¬â€ English localization

- [x] Start from accepted 0.18 controls checkpoint `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
- [x] Translate runtime menus, all 23 action labels, weapon labels, results, and warnings into English.
- [x] Preserve native identifiers, format specifiers, default bindings, INI keys, formulas, and guards.
- [x] Compile x86 ASI and record its checked SHA256 in BUILD-v0181.md.
- [x] Pass the retained command, rebinding, persistence, and regression suite.
- [x] Check menu fixtures at 720p/4K/ultrawide and inspect 720p main/shortcut menu images.
- [x] Pass six isolated installer/rollback scenarios using the English rollback entry.
- [x] Update English README, cumulative history, notebook, TODO, and build objective.
- [x] Verify the root-layout installable ZIP, all 18 entries, and exact payload hashes.
- [x] Close the game normally, verify the backup, and install while preserving the complete existing INI and loader.
- [x] Verify the English source commit/tree published on main against the checked local build snapshot.
- [x] Obtain the player's confirmation of English main/shortcut menus and preserved custom menu keys.
- [x] Verify the experimental remote branch, source tag, public release, direct link, and downloaded ZIP payload.
- [ ] Obtain any further gameplay-command or restart-persistence result; do not infer it from menu confirmation.

This build is **BUILT**, **CHECKED**, and **INSTALLED**. English menus and custom
menu keys are **USER-VALIDATED**. It localizes the accepted controls base; it
does not resolve weapon/hand FOV. The v0.18.1 public prerelease and direct
ZIP/checksum downloads are verified. The immutable 0.18.1 ZIP
has SHA256 `1853FEF39BAFE9CCC6739B1E66A30A959E44999E86A956A4C82BF831B9EAA545`;
its earlier bundled pre-installation statuses are superseded by the current
repository delivery record and published release notes.

## Next global FOV build Ã¢â‚¬â€ separate experiment

Research is paused after the player's English-menu confirmation. Publication
of the localization does not resume native experiments or claim a FOV fix.

- [x] Reject the 0.16/0.16.1/0.17/0.17.1 visual fixes; retain their evidence.
- [x] Record 0.19 as diagnostic-only and 0.20 as REJECTED; restore the accepted 0.18 base.
- [x] Retain the world + complete first-person goal, without per-weapon profiles.
- [x] Distinguish the inspected primary skinned draw route from the decal route.
- [ ] Establish the projection and defect origins at the draw, at 80 then 120.
- [ ] Prove a shared boundary covering hands/weapons, props, and DPG2 FX.
- [ ] Select one evidence-supported correction, with an A/B return to the validated base.
- [ ] Check CPU/RHI/bound-buffer consistency, without treating a shadow as GPU proof.
- [ ] Test every weapon through firing/reloading/aiming/switching, at 80/110/120/130.
- [ ] Check 16:9 then ultrawide and preservation of world effects/objects.
- [ ] Have the player confirm proportions, anchors, and proportional zoom.

A probe, log, or correct matrix does not complete this work. If a capture
misses the faulty draws, improve the instrument at the intended boundary;
do not restart a series of general scans or per-weapon profiles.

## Resume afterward

- [ ] Visually reassess Vigor effects and menus with the native HUD ceiling.
- [ ] Define a separate graphics-improvement goal and measurable success criterion.
- [ ] Evaluate other executables only after dedicated signature checks and testing.

Current progression commands add to the save; they are not yet a New Game+
campaign with story progression restarted.
Completed 0.18 checkpoint: controlled ZIP structure/payload, verified backup
and installation, player confirmation of F1 refill and shortcut reassignment,
and verified GitHub sources/release/direct download. The 0.18.1 English ASI
is built, checked and installed; its local package/source were verified and
English menus/custom keys confirmed. The public prerelease, direct ZIP/checksum
and all 18 downloaded entries are verified. Further unreported gameplay/restart
results remain separate. The release source tag identifies
`cd0873936a92f0a0e2c00dca88557c7724b7b1a6`, tree
`cca6c50943f84d2c88b1f8eeebcfd2b3e92b0c35`, matching local build snapshot
`eab12be926efbfb286051087e95749138d0fb067`; stable controls stay at
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.

0.30 follow-up checklist:
- [x] Qualify native position field and exact supported executable consumer offline.
- [x] Build final bounded, default-off distance adapter and pass local tests.
- [x] Complete exact ZIP/installer verification and record actual installation.
- [ ] Player confirms whole-rig movement and firing/reload/ADS alignment.

0.30.1: built, local tests/package checked, installed with exact0.30 rollback. Pending: player confirms closer direction and acceptable clipping/effect alignment. Six sensitivity parameters and joint rig movement were reported working by the player.

0.31: built, tested, packaged and installed with exact0.30.1 rollback; user test of signed range and CtrlMinus/CtrlPlus pending. Personal INI unchanged.


## Player approval and publication request - October 25

The player accepted 0.31 ("c'est tres bien") and explicitly requested GitHub publication. Treat the signed distance controls and selected placement as player accepted, not as exhaustive extreme-position/effect coverage. Preserve the exact tested ZIP FCB9A7DF21C9A7BF097A6F48E372C1B4615300490F635E686B39A42782F2657D and final ASI6DD72AB0168205B4DFC6CBF4B435392097D78ECB117553C7D7FC80859E408DFB. Publication must use a separate experimental branch/tag/release; main remains untouched. No game launch.
