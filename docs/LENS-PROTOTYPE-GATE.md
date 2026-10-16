# Limited first-person lens prototype: feasibility gate

October 16, 2026. Status: OFFLINE SOURCE CHECKED; no lens build, installation,
launch or new process observation. The installed0.26 diagnostic is unchanged.

## Player evidence

The five player screenshots identify session-local candidates:

| Candidate | Visible green coverage |
| --- | --- |
| 23 | Revolver geometry |
| 24 | Part of the hands/arms |
| 25 | Blue sleeve/cloth |
| 67 | Large surfaces; ownership not established |
| 68 | Rectangle near the lower-left power HUD; likely UI-related |

23-25 show no visible world/NPC contamination in these particular stills.
They do not establish other weapons, reload attachments, particles or stable
identities after restart. The registry is saturated at192/192 in every image.
Candidate67 is not identified as world geometry merely from its screen coverage.
Screenshots remain personal input, not redistributed repository assets.

## Source-level barrier

clean/green_layers.inl stores VS/PS COM references plus a pipeline group. It does
not store shader bytecode identities, engine ownership or an isolated color image.
Each callback executes the original draw first. inspect then substitutes a
constant green pixel shader and disables depth for a replay into our RGBA mask.
clean/overlay.cpp composites that mask at Present over the already composed image.
This establishes geometry access, not a complete first-person color layer.

A screen-space warp restricted by that mask cannot generally recover the world
behind the original hands/gun when the corrected silhouette moves. The original
pixels are already in the image; sampling it can bring background/HUD into the
warped region or leave duplicate old weapon pixels. An X-ray mask without alpha
coverage is not sufficient to repair these missing color/visibility inputs.

Replaying the original pixel shader into a private single target is not yet
qualified either: the diagnostic admits arbitrary native passes, including MRT
draws, without proving their output is the final lit first-person color. Native
effect draws also remain unqualified. No assertion about all Infinite rendering
being deferred, or about lens correction being impossible, follows from this.

## Prototype contract and decision

Objective: one common correction for the hands/revolver, preserving reload props
and muzzle/ejection effects, with no world/HUD change and no per-weapon profiles.
Success requires both normal visible alignment and a clean native OFF state.

Before an installable lens build, qualify either a real isolated color/coverage
composition boundary including a clean underlying world image, or a common native
geometry/projection policy shared by the complete first-person rig and effects.
The green selections alone qualify neither. Hard-coded session numbers23-25,
shared camera/constant-buffer edits and a masked final-image warp are excluded.

Decision: do not build/install a known-incomplete masked lens merely to produce
another test. This feasibility gate is NOT a functional prototype and NOT a fix.
No further user test is requested from these findings alone. Additional engine
integration would be a substantial separate investigation, not a small final
filter adjustment. Preserve the last accepted gameplay and diagnostic evidence.
