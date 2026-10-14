# UE3 / UDK first-person FOV research

October 14, 2026. **SOURCE RESEARCH / NOT IMPLEMENTED / NOT GAME-VALIDATED.**
The user requires permission before every game launch/relaunch and ends further
draw observations. This investigation reads public source and saved local
evidence only. No game launch, process inspection, build or installation occurs.

## Current player result

The player confirms the0.25 native slider changes framing but proportions remain
unsatisfactory. Retain that narrow finding; the visual objective is not validated.
The candidate remains installed, with gameplay/settings preserved. Do not silently
restore camera/aim adapters or describe a configured native percentage as a fix.

## Primary sources inspected

1. [Epic UE3 Camera Technical Guide](https://docs.unrealengine.com/udk/Three/CameraTechnicalGuide.html)
   documents the Camera/Pawn/PlayerController relationship, TPOV.FOV,
   DefaultFOV, bLockedFOV/LockedFOV and per-frame UpdateViewTarget. These are
   camera-wide controls, not independent weapon-lens controls.
2. Epic's distributed UDK UnrealScript declaration, mirrored in
   [UDKSkeletalMeshComponent.uc](https://raw.githubusercontent.com/ChristopherLeslie/StockholmGame/master/UDK-2013-02/Development/Src/UDKBase/classes/UDKSkeletalMeshComponent.uc),
   declares a native subclass of SkeletalMeshComponent, a per-component FOV,
   native SetFOV and a specialized CreateSceneProxy (FUDKSkeletalMeshSceneProxy).
   Zero means the default lens. The declaration does not include the native
   C++ implementation or establish an equivalent Infinite address/layout.
3. [UTPawn.uc](https://raw.githubusercontent.com/ChristopherLeslie/StockholmGame/master/UDK-2013-02/Development/Src/UTGame/Classes/UTPawn.uc):
   WeaponChanged copies the weapon mesh FOV to both ArmsMesh components.
   FirstPersonArms defaults specify FOV55 and foreground depth priority. The
   number55 belongs to this UT sample, not an Infinite recommendation.
4. [UTWeapon.uc](https://raw.githubusercontent.com/ChristopherLeslie/StockholmGame/master/UDK-2013-02/Development/Src/UTGame/Classes/UTWeapon.uc):
   AttachMuzzleFlash explicitly sets the particle component's FOV from the weapon
   mesh before socket attachment. Overlay mesh setup also propagates FOV.
5. [UDKParticleSystemComponent.uc](https://raw.githubusercontent.com/ChristopherLeslie/StockholmGame/master/UDK-2013-02/Development/Src/UDKBase/classes/UDKParticleSystemComponent.uc)
   declares native SetFOV and overrides SetTransformedToWorld for custom-FOV
   LocalToWorld changes, plus InitParticles and saved scaling state. This is
   engine-assisted particle handling, not automatic inheritance from a socket.

These are public UDK sample/declaration sources, not the full licensed UE3 C++
engine implementation. Generic UE4 GetRenderMatrix tutorials are not evidence
of the same ABI or override in UE3/Infinite. Links were inspected on the date
above; mirrored master URLs can change. No external code is copied into runtime.

## What this changes in our reasoning

A single decoded camera projection does not rule out a distinct *effective*
first-person FOV: component-specific rendering/transformation can compensate
against a shared view. Do not treat the previous Steam VR one-frustum result
as proof that this UE3-style technique is impossible in Infinite.

The concrete UDK model is three coordinated responsibilities: camera/world
FOV, first-person mesh FOV and matching attached-particle handling. Foreground
depth priority and custom mesh FOV are separate settings; group membership
alone does not identify the correct population or provide the lens policy.
The explicit propagation in UT explains why changing only the visible gun
mesh is insufficient as a design. It does not prove the cause of our former
reload/flash defects or guarantee a complete fix for every temporary prop.

## Applicability to Infinite, from saved evidence

The saved current-native-firstperson.json reports XSkeletalMeshComponent for
the inspected XFirstPersonAttachment/XWeaponModelFirstPerson meshes. Its
FOV-filtered component properties show no mesh FOV property. The saved name
pool contains SetFOV, but that name alone does not identify a mesh method;
camera SetFOV is already known. This is scoped historical evidence, not a
fresh exhaustive class/function census or proof of binary-wide absence.

UDKSkeletalMeshComponent is a UDKBase native extension; it is not a guaranteed
feature of every UE3 SkeletalMeshComponent or of Irrational's XSkeletalMeshComponent.
Do not reinterpret an Infinite component as a UDK type, borrow UT offsets,
replace its class pointer or invoke a camera SetFOV as if it were mesh SetFOV.

## Next implementation decision, before another build

First inspect saved reflection/native ownership and the supported on-disk
XSkeletalMeshComponent proxy/transform path for an equivalent component-lens
facility. Reuse a verified equivalent if one exists. If not, a native adapter
must reproduce the coordinated mesh/effect behavior in Infinite's own rendering
and attachment lifecycle; an INI key or UnrealScript name cannot create the
missing native renderer support.

Use one shared first-person policy for hands, weapons, overlays and temporary
reload pieces, with the corresponding muzzle/ejection effects. Keep world,
NPCs, HUD/Vigor display effects and full-screen menus excluded. Do not make
per-weapon profiles or blindly select all foreground particles. Define aiming
entry/held/release behavior explicitly so sights, native camera zoom and effects
remain coherent; a fixed mesh lens alone is not a qualified ADS solution.

No implementation is selected yet. The immediate deliverable is this source
comparison, not another diagnostic or installable experiment. Any later build
needs a defined native seam, safety/rollback qualification and explicit user
permission before launching the game for its visual test.
