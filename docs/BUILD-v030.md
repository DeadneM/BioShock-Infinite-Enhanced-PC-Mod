# 0.30 native first-person position - experimental

Base: retained gameplay from 0.28, plus the existing 0.29 input-profile candidate (not yet player validated). This build adds only the native first-person root position experiment. Branch: experiment/firstperson-position. Previous installed module: 0.29; rollback must restore that exact module and preserve its INI.

Objective: move the owned arms/weapon root farther from the camera through its native placement calculation without changing world FOV, projection, scale, mesh transforms, HUD or individual weapon assets.
Success criterion: Ctrl+0 produces a visible backward movement of both arms and weapon; firing/reloading parts and effects stay aligned, and aiming returns to native position. If only one population moves or nothing changes, reject the experiment rather than add per-weapon exceptions.

## Controls

Ctrl+0 cycles BackOffset 0, 2, 4, 6, 8, 10 native units. Default0 is original placement. The action appears in the main menu and is reconfigurable in the existing shortcut editor. Existing bindings and mod INI are preserved. Settings save under [FirstPerson] BackOffset. Missing/invalid values default0. This uses the main keyboard0; choose another chord in the editor if the keyboard layout interferes.

ADS intentionally uses original position for this trial, including the zoom recovery interval. The world FOV and proportional-aim policy remain exactly as before. The optional sensitivity profiles remain at their prior values; do not adjust them during this position test.

## Mechanism and evidence

Saved reflection identifies XHuman.FirstPersonAttachment (+0x1D38), XFirstPersonAttachment.Instigator (+0xA0), and PlayerViewOffset (Vector, +0x200,12 bytes). The root class and placement vtable slot are checked before activation. The original native placement function (RVA0x77C850) rotates this vector and adds it to output location. Exact entry and offset-consumer signatures qualify the hook. Current EXE hash, XYZ reads, return ABI and root vtable identity are checked offline.

During a qualified hip-fire placement call only, add a bounded amount to local X (forward in engine coordinates, away from the camera), call the native placement, then restore the original X if it still equals our temporary value and ownership remains intact. No accumulation across calls; nested calls do not apply twice. A native change is not overwritten. Thread, living pawn, cinematic, root-ownership, schema and ADS guards bypass the experiment when unqualified. The default0 makes no property writes.

## Limits

Offline arithmetic and synthetic tests cannot prove that every temporary effect inherits this root in the actual game. A later placement override or a separately placed effect can still break visual alignment. Recoil, camera animation, bobbing and sway are not disabled. No viewmodel lens, Panini, green shader selection or external process capture is introduced.

## Player test

1. Launch normally only when ready; load the same save and take the revolver. Keep world FOV and all other settings unchanged.
2. Open the usual mod menu, press Ctrl+0 twice to reach4, close the menu. Compare with0, then6 if the change is subtle.
3. Fire and reload once. Check hands, speedloader, cartridges and muzzle flash. Aim and release, then change weapon.
4. Report whether arms and weapon both move, whether all parts/effects remain aligned, and the position-status line if nothing changes.
5. Cycle back to0 to disable. Stop this trial if there is clipping, detached pieces or no common movement.

Status at preparation: EXPERIMENTAL. See DELIVERY-v030.md for actual build/package/install checks; no gameplay success is implied by this document.
