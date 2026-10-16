# 0.26.0 experimental green visual selector

Objective: let the player identify candidate rendering groups by green silhouettes.
Success: identify a numbered shader pair covering the hands/gun or reload/effects,
and explicitly report any world, NPC or HUD contamination. This is NOT a FOV fix.

Base: accepted English 0.18.1 gameplay via retained controls checkpoint63f87fe,
with the subsequently authorized removal of camera/aim overrides and 0.25 native
range preference behavior. Existing INIs, bindings and native options are preserved.
No rejected camera, Panini, packet transform or draw observer is restored.

## Controls

- Ctrl+Alt+F5: enter/leave the temporary diagnostic. It starts OFF on each launch.
- F5: next opaque shader pair with depth writing enabled.
- F6: next opaque shader pair without depth writing.
- F7: next blended shader pair.
- F8: clear the green selection; stay in diagnostic mode.

Close the mod menus while identifying the model. Press each key again to advance
through that group. The status panel gives a session-local candidate number and
matched draws. Record the group/key and number, and what turns green. Numbers
are not guaranteed stable between launches. On focus loss selection is cleared.
Ctrl+Alt+F5 exits and restores normal mod controls. While diagnostic is active,
F5-F8 mod bindings (including modified chords) are suppressed without changing
the saved binding file. No keys are sent to the game. Game-native F-key bindings
are not intercepted. Do not edit bindings during this temporary test.

## What green means

This is a silhouette replay into a private transparent texture, composited over
the image. The game renders its original draw first; native color/depth targets,
projection, bone data and FOV are not rewritten. Green is an X-ray geometry
envelope: occlusion and alpha cutouts are intentionally not reproduced. The
mask may cover geometry normally hidden by the world, or the transparent borders
of particle cards. It is not a reliable visible-pixel segmentation for a lens.

Groups are pipeline heuristics, NOT confirmed arms/reload/effect layers. A shader
pair may be shared with world/NPC/HUD objects. Reload accessories may share the
same group as the main weapon; there is no promised one-to-one key mapping.

Eligible draws: immediate-context triangle lists, a native depth target, full-size
zero-origin viewport, single-sample color target, no GS/HS/DS, no stream output,
no OM UAVs or pixel-shader class instances. Four direct draw variants are supported.
Deferred contexts, indirect drawing, MSAA, geometry-shader particles, scaled
targets and other routes are excluded. Empty green does not prove object absence.
The registry is capped at192 shader pairs; replay at2048 draws per frame.
Replaying draws can lower frame rate and influence active GPU query statistics;
this build is for a short visual test only. Native render targets remain untouched
by replay, but visual/driver compatibility is still EXPERIMENTAL.

## Delivery and rollback

The root ZIP contains the checked ASI, INI, loader and existing transactional
installer/rollback. Existing INIs and loader are preserved; installer saves the
actual previous ASI. This diagnostic installer does not invoke the native-option
helper or change the native FOV range. No game launch is part of installation.
After closing the game, use Rollback.ps1 with the install backup directory printed
by Installer.ps1 to restore the prior module. Current native options are unchanged.

Status: EXPERIMENTAL. Offscreen pixel/state checks and retained checks passed.
Final payload hashes, package/install checks and actual player result are recorded
separately in DELIVERY-v026.md. No GitHub upload implied.
