# Native zoom target restart

October14,2026. **0.24 candidate prepared, not user-validated.**
Current qualification and build status: [BUILD-v024.md](BUILD-v024.md).

Base: accepted English0.18.1 source99a75aa9e1a47543c418b2dc37b43a8883898c72
on separate branch `experiment/native-zoom-target`. Cumulative records are
retained. The new candidate starts from the accepted base. Panini0.22,
CalcFOV zoom0.23 and GPU/first-person0.21 adapters are excluded.

## One next objective

Qualify one native zoom target before camera interpolation. Adapt that target
relative to the chosen world FOV with one policy for every weapon, retaining
native transitions, viewport/aspect handling and all accepted commands/keys.
This is not an independent first-person rendering correction.

The player saw no difference with0.23. Native logs prove ordinary aiming uses
the locked-camera route bypassed by that adapter. A bounded current-camera
read verifies scripted SetFOV(NewFOV), one float input, executed by generic
interpreter9D9FA0. It also finds native weapon target60, retained locked99.848518,
delayed120 and native hip82.5 as distinct values. These facts do not prove
the exact interpolation formula or a usable injection point.

Do not apply a native hip multiplier to an arbitrary locked/cached value:
that value may already include the mod's higher world FOV. Identify the
target and transition source first; avoid the double-correction mechanism.
Do not implement per-weapon profiles or reshape a completed frame.

## Required evidence and success criterion

- The first capture did not run after process closure. A later accepted-runtime
  capture collected476 samples, only10 locked, with a weapon change. It does
  not establish a ten-second hold or one fixed target throughout the capture.
- Qualify the producer of the native target and the consumer that performs
  interpolation. A SetFOV descriptor or interpreter layout alone is not proof
  of a universal usable hook.
- Derive and test any new adapter's actual ABI, ownership, baseline and OFF
  return before another installable functional candidate.
- A functional build must visibly soften aiming at world100/120, keep correct
  hip-fire recovery and work after weapon changes. The player must confirm.
- Preserve the full INI,23 accepted actions, HUD/FX/menus, logos, grants and
  automatic native aspect handling. Whole first-person size/distortion remains
  a separate open goal and must not be claimed fixed by the zoom build.

Current installed accepted ASI B13DBE..., retained current INI FDB4CB..., loader
D5A059... are independently checked in [REJECTED-v023.md](REJECTED-v023.md).
No public release or in-game success is claimed. See BUILD-v024 for candidate
qualification, tests and delivery state rather than the historical capture plan.
