# Physical FOV restart

October 13, 2026. Status: **PREPARED**, without a new build or visual fix.

Historical checkpoint: superseded on October14 by the rejected
[Panini verdict](REJECTED-v022.md) and the separate native
[aiming experiment0.23](BUILD-v023.md). The optical/native-aspect evidence below
is retained; its pre-build status is not the current installation status.

## Current result

The accepted 0.18.1 binary and complete INI were restored and checked, then
launched. The player reports that the world still looks deformed at requested
120. Thus rollback is verified as a file operation, not as a visual resolution.
It is no longer appropriate to attribute the whole complaint to 0.21 alone.

Offline native code and a bounded QUERY_READ snapshot now establish the input
contract for this gameplay viewport. The native projection builder converts
the camera degrees from a horizontal 16:9 reference:

```text
tan(V / 2) = tan(referenceDegrees / 2) / (16 / 9)
tan(H / 2) = tan(V / 2) * viewportAspect
```

In the current accepted session the viewport is 3840x2160, its verified
virtual methods select that builder, delayed/cache FOV are 120 and the
aspect-constraint bit is clear. The predicted lens is H120/V88.507155.
A native cb1 CPU shadow independently has rotation-invariant column lengths
0.577350300, 1.026400561 and 1.000000015, consistent with H119.999998,
V88.507152 and aspect1.777777826. This shadow is not a proof of the GPU state
or ownership of every world draw. No game memory was written or engine call
made; the targeted snapshot used 48 reads.

The game already performs width expansion in this path. Do not add another
aspect multiplier. The player also reports stretching/flattening at requested
100. The log confirms a real change to100, followed by the player's further
change to80. The subsequent reader rejected stale controller/camera identity;
no matrix measurement at100 or80 is claimed. A visual example is now needed
to identify the actual distortion rather than issuing another angle-only build.
An axis conversion preserves the same lens; it cannot by itself make distant
characters larger at the same physical field of view.

The post-builder callback at878916 is present in this session, but its actual
method C7FA70 copies the input matrix to scratch and does not directly write
that input. Interface presence alone is not proof of a projection override.
The role of its downstream external consumers is not established here.

## Accepted base and rejected experiment

The source starts from accepted English 0.18.1, checkpoint
`99a75aa9e1a47543c418b2dc37b43a8883898c72`, on the separate local branch
`experiment/physical-fov`. The installed accepted ASI is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
The player rejected 0.21 after severe world/character distortion; its sources
and evidence remain archived. See [the verdict and rollback](REJECTED-v021.md).
None of its GPU-buffer or proportional-ADS hooks are retained here.

## Objective

Establish one coherent world-camera lens, with explicitly identified horizontal
and vertical angles and automatic adaptation to the actual render viewport.
World and NPC proportions must remain correct before another shared
first-person experiment is installed. Independent arms/weapons FOV remains an
unfinished second step; a world-angle label is not a first-person fix.

## Optical contract

For a symmetric perspective projection with square pixels, viewport aspect
`A = width / height` links the physical angles:

```text
H = 2 * atan(A * tan(V / 2))
V = 2 * atan(tan(H / 2) / A)
```

Angles in these calculations are radians. The UI may accept either axis, but
the other must be derived. Independently stretching the two axes changes
proportions. At 16:9, horizontal 120 degrees is vertical 88.507 degrees;
vertical 120 degrees is horizontal 144.017 degrees. These examples do not
establish which convention BioShock Infinite currently uses.

Changing pixel resolution while retaining aspect must retain framing. A
future automatic width expansion can preserve the vertical lens from a 16:9
reference and expose additional width on wider displays. An explicit
horizontal input at the current aspect is another valid convention. Neither
convention should silently reinterpret an existing saved `FOV=120`.

Use actual render-viewport dimensions, rather than desktop resolution or
unqualified swapchain dimensions. Native letterboxing, cinematics and
non-player views must retain their existing behavior.

## Evidence required before a camera change

1. Establish the game's native projection convention from the pure projection
   matrix and its construction, on accepted 0.18.1. A rotated/translated
   view-projection matrix cannot supply angles from its diagonal alone.
2. Compare the native result and final world projection at requested 80 and
   120, without aiming. Distinguish a camera cache value from a rendered angle.
3. Check `P11 / P00` against actual viewport aspect. If native aspect is already
   correct, do not apply a second axis correction.
4. Preserve the existing preference, menu bindings and native ADS behavior
   until a separate, source-qualified zoom change is justified.

For the conventional symmetric matrix, `H=2*atan(1/P00)` and
`V=2*atan(1/P11)`. Expected values at 3840x2160, if the requested value is the
physical horizontal angle, are:

| Requested H | Expected V | P00 | P11 |
| --- | --- | --- | --- |
| 80 | 50.534016 | 1.19175359 | 2.11867305 |
| 120 | 88.507155 | 0.57735027 | 1.02640048 |

## First build success criterion

One camera change per experiment, with return to the accepted binary available.
The measured horizontal/vertical angles must match the documented input at
80, 100 and 120; aspect must match the viewport, and the player must confirm
normal world/NPC proportions. The same aspect at 1080p and 4K must retain
framing. Automatic width expansion, when implemented, must preserve vertical
framing on ultrawide. Existing commands, HUD, logos and custom menu keys must
remain intact. No diagnostic or mathematical fixture counts as player validation.

## References and limits

### Alternative projection to assess after the visual example

[Epic's Panini projection documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/panini-projection-in-unreal-engine)
describes a nonlinear projection intended to reduce wide-angle stretching
at the screen edges. This is a distinct rendering change, not a horizontal/
vertical unit conversion. It trades the ordinary perspective image for a
different mapping, with cropping/sharpness considerations.

This is a research candidate only. BioShock Infinite is UE3; the modern Unreal
console settings in that document are not established as available here.
Any original implementation would need a qualified scene boundary, an exact
disabled return and preservation of the HUD/menu. It does not establish an
independent complete first-person lens or explain the current NPC complaint.
No Panini source, build or installation is claimed.

### Native first-person reference

[The Infinite-specific engine notes of bioshock-trilogy-vr](https://github.com/VR-Stereo-Hub/bioshock-trilogy-vr/blob/main/docs/bioshockinfinite/ENGINE_NOTES.md)
independently describe the camera angle as horizontal at a fixed 16:9 reference.
They also report a single measured gameplay frustum, with no second viewmodel
frustum in their Steam build/test. This is useful external evidence, not proof
of every draw on the GOG build. A BioShock 1/2 foreground-lens implementation
must not be assumed to apply to Infinite. No external code or addresses have
been copied into the mod.

[Microsoft's perspective-matrix API](https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectivefovlh)
defines a vertical lens and a separate X:Y aspect. This supports the optical
relationship; it does not establish the game's implementation.
[Modern Unreal's camera API](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UCameraComponent)
documents horizontal and first-person lens settings. It is UE5 documentation,
not evidence that BioShock Infinite's UE3 build exposes those settings.
