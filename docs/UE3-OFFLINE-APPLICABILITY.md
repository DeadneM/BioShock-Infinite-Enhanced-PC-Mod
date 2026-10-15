# Infinite component/proxy applicability, October 15, 2026

**OFFLINE CODE CHECKED. NO IMPLEMENTATION, BUILD, INSTALLATION OR GAME LAUNCH.**
This follows UE3-FIRSTPERSON-FOV.md under the user's no-launch/no-further-draw-
observation constraint. It is a specific comparison of the UDK extension with
Infinite's component path, not a claim that first-person FOV is impossible.

## Identity and method

The on-disk GOG executable again matches
`CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A`.
Installed ASI still matches the0.25 candidate
`48B3CF39629183DCA56ACF1E1920CAFF0F8976DC2180CA9AD1A5A773232FCF09`.
No process was opened. Historical heap addresses are used only to identify
saved types/static tables, never as current process pointers.

Private analysis re-reads the original executable, validates executable table
targets, compares contiguous component tables and verifies the proxy allocation
and constructor call at exact known instruction entries. Private raw output,
disassembly and native bytes are not tracked or distributed. Padding-based
function-start guesses are only navigation aids; critical factory assertions
use its exact table entry and decoded call instruction.

## Concrete findings

1. Saved component records consistently associate XSkeletalMeshComponent with
   static vtable VA01619590. Its constructor calls006C9520, which installs
   the base table0159A298, then installs the X table. This establishes the
   parent/derived table relationship from native construction, rather than
   inferring inheritance from adjacent addresses.
2. Both contiguous executable-pointer tables contain125 entries. They differ
   only at slots00 andA8. In particular, the proxy factory slot188 is identical,
   VA00476F10. The125-entry count is a bounded table comparison, not a claim
   that arbitrarily following data after the table yields more virtuals.
3. Factory00476F10 checks the component's skeletal asset/LOD/mesh-object state,
   allocates0xE0 bytes and calls007201F0. That constructor installs proxy table
   01599FB8, matching the saved arms/weapon scene proxies. Both X/base component
   tables therefore select the same proxy factory on this inspected route.
4. The X override atA8 is a49-byte gate around base function00B4DA60, checking
   component+5B4 against a global counter and forwarding a float stack argument.
   It contains no custom projection computation. Its counter's semantic name
   is not asserted without symbols; this is not treated as a mesh SetFOV.
5. Neither UDKSkeletalMeshComponent nor UDKParticleSystemComponent appears as
   an ASCII/UTF-16 literal in this executable. The saved668-record skeletal
   reflection file has no FOV/SetFOV member; only13 records belong directly to
   the recorded X class. The flat file also includes unrelated/inherited
   metadata, so its record count is not an exhaustive correctly typed class API.

**Decision:** the inspected X component has no UDK-style specialized proxy
factory to enable by adding an INI property or calling the UDK SetFOV method.
The UDK declarations cannot be transplanted as binary types. This does not
exclude a differently implemented facility in shared native base code, shaders
or another engine-specific path; no engine-wide absence is proven.

## Correct the earlier candidate boundary

The former7D8F90 proxy virtual1C /44B240 mesh-object virtual28 route is the
decal route. It is not evidence of the primary color hands/weapon transform.
The old603790 matrix callback was investigated in that context and cannot be
promoted as a universal main-view adapter from those observations.

Existing primary-route evidence identifies proxy virtual08 at00C28A10,
submission builder00B188D0, mesh-object virtual24 and the main skin factory.
The original body was cross-checked for its direct00B188D0 calls at00C28B50
and00C28C26. This is the primary mesh preparation direction to retain for
offline work. Shared projection-buffer mutation and the rejected per-packet
partial rig adapters remain excluded.

UDK's particle extension overrides its own transform handling; assigning a
custom skeletal lens is not sufficient for muzzle sprites, ejection effects
or other temporary rendering. Saved Infinite effects use XParticleSystemComponent,
not the UDK class. The particle counterpart and authoritative whole-rig
population are not yet qualified for a functional correction. DPG2 is not a
sufficient selector because it also contains HUD/Vigor display effects.

## Before any functional candidate

Qualify a common per-view first-person correction at the primary mesh
preparation/render path and its particle counterpart, preserving the native
world camera. It must cover hands, gun, overlays, reload accessories and
appropriately attached effects with the same view/frame policy. Native aim
entry/held/release behavior must be specified explicitly. Do not install a
mesh-only adapter or use per-weapon profiles to conceal missing coverage.

No0.26 build is justified by this comparison alone. Next work remains offline
on the shared primary transform consumer and effect-transform counterpart;
no new firing/reload trace is requested. The existing0.25 candidate and backups
are untouched. Any later visual test requires asking before launching the game.
