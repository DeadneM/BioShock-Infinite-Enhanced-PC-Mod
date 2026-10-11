# Cumulative TODO

## Validated features to preserve

- [x] Overlay on the game's swapchain and access to the main menu.
- [x] Automatic skipping of the four logos with loading preserved.
- [x] Weapons and ammunition in both slots, hold-to-aim, speed, and world FOV.
- [x] Vigors, gear, weapon and Vigor upgrades, money, and lockpicks.
- [x] Crosshair hiding and all three infusion gauges at 10/10, confirmed on 0.15.
- [x] Recover the camera base predating the rejected experiments and check its suite.
- [x] Separate stable/v0.15 from the experimental 0.17.1 FOV archive.

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
- [ ] Obtain the player's F1 test with at least one partially empty gauge.
- [ ] Obtain the player's test of new shortcuts, particularly F2/F5/F6.
- [ ] Obtain reassignment of both menu-opening shortcuts and persistence after restart.
- [ ] Promote only changes retained after player validation.

## GitHub and delivery

- [x] Create the public BioShock-Infinite-Enhanced-PC-Mod repository on DeadneM.
- [ ] Complete source upload and check remote references.
- [x] Publish an installable ZIP and verify the direct link and downloaded contents.
- [ ] Record the commit, hashes, status, and actual result in BUILD-v018.md.
- [ ] Keep README, history, technical notebook, and this TODO consistent after each build.

## Next global FOV build — separate experiment

- [x] Reject the 0.16/0.16.1/0.17/0.17.1 visual fixes; retain their evidence.
- [x] Retain the world + complete first-person goal, without per-weapon profiles.
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
Completed checkpoint: controlled ZIP structure/payload, verified backup and installation, and player confirmation of F1/refill and shortcut reassignment. GitHub source branches are verified; final release/link verification follows.
