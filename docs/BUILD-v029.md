# 0.29 input profiles - local experimental candidate

Base: retained 0.28 source, commit ebcad37. This is not a newly player-validated baseline; the accepted gameplay inherited by 0.28 is preserved. Branch: experiment/input-comfort.

Objective: independently scale normal, ADS and sniper look input for mouse and controller without persisting changes to native options.
Success: the player can feel each profile independently, defaults x1 retain original input, menus remain usable, and settings persist without changing existing bindings.

Controls: Ctrl+1 mouse normal, Ctrl+2 mouse ADS, Ctrl+3 mouse sniper, Ctrl+4 controller normal, Ctrl+5 controller ADS, Ctrl+6 controller sniper. Each cycles x0.10, x0.25, x0.50, x0.75, x1.00, x1.25, x1.50, x2.00. All six actions can be rebound in the existing key editor. Existing conflicting custom bindings take precedence through the normal conflict loader.

Mechanism: an executable-signature-checked core ProcessEvent hook temporarily scales reflected XPlayerInput MouseSensitivity / LookRightScale / LookUpScale fields only around its qualified PlayerInput event, on the foreground game thread with menus closed and the owned pawn alive. The active weapon archetype distinguishes sniper ADS. Values are restored afterward unless native code changed them. Defaults x1 cause no sensitivity field writes.

CHECKED: local synthetic native ABI/trampoline, reflection, mode selection, restoration, reentrancy, focus/menu/cinematic guards, six values, persistence and existing UI tests. These checks do not prove Infinite consumes these fields or routes the event through ProcessEvent. Actual mouse/controller behavior is UNKNOWN until player testing. No game was launched or externally read for this candidate.

NOT IMPLEMENTED: bobbing/sway reduction. Broad camera-animation disabling would also affect recoil and scripted effects, so it is excluded pending a qualified specific path. No optical, viewmodel, Panini or green diagnostic changes.

Test: observe the sensitivity status in the mod menu. Compare x1 versus x0.25 for each device and mode, with the other profiles at x1. Verify ordinary ADS and sniper separately. If the native event never matches or input is unchanged, reject this adapter rather than repeat external captures. Restore x1 for all six to disable its field adjustments.

Rollback: retain the published 0.28 ZIP. Use the supplied installer and Rollback.ps1; personal INI and bindings must be preserved. This local candidate is not installed or published.
