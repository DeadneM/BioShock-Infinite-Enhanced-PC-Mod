# Cumulative TODO

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

## Build 0.18 — shortcuts and gauge refill

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

## Build 0.18.1 — English localization

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

## Next global FOV build — separate experiment

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
