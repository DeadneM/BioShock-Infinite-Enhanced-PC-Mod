# 0.22.0 - Panini scene experiment

## Objective and status

**BUILT, CHECKED, then REJECTED and ROLLED BACK.**

The player declined the in-game lens effect and requested native FOV work.
Accepted English 0.18.1 was restored on October 14. See
[REJECTED-v022.md](REJECTED-v022.md). The original objective and checks below
are historical; none establish a retained visual fix.

Try the stronger Panini mapping selected by the player from a numerical
comparison of their October 13 video. One rendering objective: make central
world subjects more prominent while retaining horizontal coverage. This is
not a first-person placement/FOV fix.

Base: accepted English 0.18.1, source `99a75aa9e1a47543c418b2dc37b43a8883898c72`,
with the documented rejection/restart checkpoint `3c4f4f9622ce39b0b727e8a2bd4819b4bc90502a`.
Work is isolated on `experiment/panini-world-v0.22`; rejected 0.21 GPU/ADS
adapters are excluded. Public accepted releases are unchanged.

Checked ASI SHA256:
`69A860A631BBA67C438EAD9A8111C2499CBD2155C7A9C612E5E2D670662082B9`.

## Change

- Optional base Panini, distance parameter1, without vertical straightening.
  At native horizontal100 on16:9, its center magnification is approximately
  1.278 while horizontal coverage remains100. Vertical center coverage is
  approximately55.36 instead of67.67 degrees. Other angles/aspects use the
  actual native projection, including aiming transitions.
- A new configurable **Ctrl+F9** action toggles it with the main menu open.
  All24 actions, including menu-opening shortcuts, remain editable.
- `[Display] WorldProjection=Perspective` is the default, including when the
  key is absent in an older INI. `Panini` opts in. Existing files/preferences
  are preserved; this experiment is not silently enabled during installation.
- An exact GOG native scene-render signature is checked. Application requires
  the current playable pawn, its native viewport, a single full-size view,
  and a coherent finite native projection. Other views pass through.
- The composite runs after the qualified native scene-render call. Its
  placement before the final native HUD is still **EXPERIMENTAL** until the
  player verifies it. It is not applied at Present.
- The Direct3D11 immediate context is switched to a separate temporary state,
  then restored completely. Native constant-buffer contents, projection
  matrices, camera fields, bones and primitive transforms are not modified.
  Temporary bindings are cleared before restoration so no back buffer remains
  retained by the effect state. Unsupported devices/targets reject application.

The scene includes the native arms/weapon; Panini alone cannot independently
correct their size or internal perspective. No additional weapon, reload,
Vigor-HUD or ADS repair is claimed.

## Checks performed

The integrated suite passed. New checks use the actual native renderer
prologue/trampoline and a real WARP Direct3D11 device:

- Primary-view identity/aspect/viewport guards and source immutability.
- Legitimate native array slack (capacity17 for one view), without confusing
  allocation capacity with the number of actual views.
- Native void-thiscall ABI, OFF and unrelated-view passthrough.
- Real shader output and central magnification.
- Target, constant-buffer and compute-stage restoration; native buffer bytes
  and device feature level unchanged.
- Effect application to the test swapchain at720p,4K and ultrawide, followed
  by working resize and release without retained back buffers.
- Retained commands, key editing/persistence, native aiming, HUD/intro
  configuration, crosshair, weapons, ammunition and progression fixtures.

These tests establish the stated technical behavior, not the game's actual
scene/HUD ordering, visual result or performance on the player's GPU.

The final ASI also passed all six isolated installation/rollback scenarios.
After the player confirmed normal game closure, the final package was installed
with the accepted ASI backed up and the entire existing INI and loader retained.
Independent hashes and the installation receipt agree. The fresh game log
confirms 0.22 startup, the scene adapter and the native 4K game swapchain;
keybind parsing reports no invalid entries or conflicts. The player visual
test below is still pending. See [DELIVERY-v022.md](DELIVERY-v022.md).

## Player test and success criterion

1. Load the same scene, keep World FOV100, and open the existing main-menu key.
2. Use Ctrl+F9, or its reassigned shortcut, to compare Perspective and Panini.
   The central NPC should become more prominent, with the same horizontal
   coverage and a result resembling the chosen offline comparison.
3. Confirm native HUD, reticle, Vigor HUD effects and pause/objective menus
   keep their placement and readability. Check aiming and a weapon reload.
4. Turn the projection OFF: the accepted perspective rendering must return
   immediately. Check Alt+Tab and window/resolution changes.

The visual result must be explicitly player-confirmed before retention. A
submitted composite log is not proof of a successful visual correction.
If HUD/effects shift or the scene does not change, use OFF and retain the
observations; do not advance this experiment to the stable branch.

## Rollback and next step

The installer retains the complete INI and compatible loader, and backs up the
accepted ASI. Normal game closure is required before replacement. Its printed
Rollback.ps1 command restores that transaction. Source and the accepted
English release remain separate.

After the world result is accepted, investigate one shared first-person
placement/lens boundary covering arms, weapon, reload props and attached FX.
Do not introduce per-weapon profiles or confuse a global image remap with
that remaining correction.
