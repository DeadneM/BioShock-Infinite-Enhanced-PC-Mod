# Whole first-person native lens restart

October14,2026. Opt-in observer0.24.2 is BUILT / CHECKED / EXPERIMENTAL;
installation and actual game capture are pending. No FOV correction is claimed.
Branch: `experiment/firstperson-native`.

## Retained base and validation scope

Start from checkpoint/aim-control-v0.24.1, commit
63f87fea42141b0055c3d126dfd853120390837f. Runtime source is initially identical.
Installed ASI61523562...; no runtime files are changed by this research.
At the final check, current INI is F34CCAC5...: only HoldAim changed from0
to1 relative to the saved94C231D... installation settings. Preserve this
current player setting; do not restore the older INI as part of future work.
The player confirms ON/OFF control and a radical visible aiming difference.
Exact held zoom, world appearance and independent first-person projection are
not validated. The direct accepted18.1 ASI backup remains available.
No rejected Panini, GPU/world remap or partial rig transformer is inherited.

## One next functional objective

Give the whole first-person population one consistent native rendering lens,
separate from the selected world lens. Hands, every weapon, animated reload
props and attached muzzle/power effects must remain aligned. No per-weapon
profiles, bone edits or saved actor transforms. Preserve accepted gameplay,
current controls/aim option, HUD including Vigor effect, menus and aspect handling.

The world input contract currently agrees with physical H120/V88.507 at16:9;
the last CPU matrix shadow has the same aspect. This does not prove every
submitted draw. Equivalent H/V/diagonal conversion cannot itself remove
rectilinear wide-angle perspective. Do not silently reinterpret FOV=120,
apply another aspect multiplier or promise that axis conversion fixes NPC size.

## Change the approach after the rejected experiments

The missing proof is a correlated native draw boundary, not another generic
object scan or another broad DPG2 projection mutation. Reuse qualified layouts:
primary GPU-skin route C28A10, main factories from LOD+2C, actual loaded color
shader parameters at+6C; typed particle submission B76D80 and native commit
87F740. The previous floating-section matrices are native drift data, not a
hidden camera lens. Loaded shader-map membership is not draw-time selection.

Before a functional adapter, the same bounded frame record must identify the
current owned component/proxy, material/factory, actual view, final constant
upload/binding and relevant local/bone/emitter positions for hands, weapon,
reload prop and muzzle flash. World/HUD draws must be identifiable as excluded.
Native preparation and last GPU binding may differ; a CPU shadow alone is not
final consumption evidence. Do not treat raw DPG2 as ownership or a private
first-person view. A borrowed view cannot become an owning copied FSceneView.

If the real frame shows one shared projection, an independent native view must
be selected coherently for all owned first-person preparation and submission.
Changing only the final matrix or only some actor packets is not a qualified
whole-first-person solution. Diagnostic output is not a functional fix.

## Success criterion before retention

At world80/100/120/130, changing the first-person lens visibly changes the
whole rig while leaving the world unchanged. Shoot, reload, aim/release and
switch weapons: props stay in the hands, effects at their anchors, no HUD
flicker or scale change. OFF returns to the retained base. Native aspect adapts
to the real viewport; resolution alone does not change framing. The player
must confirm visible behavior before another feature is promoted.

Current work preserves the verified control checkpoint and qualifies boundaries.
See BUILD-v0242.md for the finite draw observer and isolated tests. The installed
0.24.1 runtime is still unchanged at packaging. The newer current INI has world
FOV110; preserve its complete contents rather than restoring an older snapshot.

## Primary source cross-check

Reference: VR-Stereo-Hub/bioshock-trilogy-vr, commit
`41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e`, inspected October14,2026.
These are Infinite Steam observations, not our GOG runtime validation:

- [Infinite engine notes](https://github.com/VR-Stereo-Hub/bioshock-trilogy-vr/blob/41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e/docs/bioshockinfinite/ENGINE_NOTES.md):
  the gameplay lens test reports one lens, no separate viewmodel frustum;
  fixed16:9 reference and aspect cross-check agree with our native math.
- [Camera input](https://github.com/VR-Stereo-Hub/bioshock-trilogy-vr/blob/41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e/src/game/bioshockinf/camera.cpp):
  the FOV lever enforces the shared camera caches, not an independent arm lens.
- [Bone driver](https://github.com/VR-Stereo-Hub/bioshock-trilogy-vr/blob/41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e/src/game/bioshockinf/bones.cpp)
  and [effect origin](https://github.com/VR-Stereo-Hub/bioshock-trilogy-vr/blob/41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e/src/game/bioshockinf/fxorigin.cpp):
  separate pose/effect intervention is not the native global lens requested
  here. The engine notes explicitly leave some effect-position writers open.

No public code is incorporated; downloaded reference text stays private.
BioShock1/2 foreground fields and Steam addresses are not GOG patch targets.
The reference strengthens the shared-projection hypothesis; it cannot replace
draw-correlated GOG proof or justify another unqualified runtime change.

The held-aim capture for the player's completed test contains zero samples.
Exact held zoom remains unmeasured. Retained user-confirmed controls/effect,
unchanged runtime and a direct accepted18.1 rollback are the current result.
