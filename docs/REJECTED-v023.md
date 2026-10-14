# Rejected 0.23 native CalcFOV aiming experiment

October 14, 2026. **REJECTED as a functional aiming fix.**
The player compared ON/OFF with the Hand Cannon at World FOV120 and reported
no visible difference. The native game log explains the coverage failure:
ordinary aiming activates the locked-camera route, which this adapter bypasses.
Passing optical/ABI/control fixtures did not cover that actual gameplay route.

Checked source514714ac6a4f12a91994040657f65efd658ff4da and ASI
67B8E723113C969C0B55496031B23646DBD2C77F92ACFA79C3DC502244B490F3 are retained
as experimental evidence, not a successful feature. The checked local23-entry
ZIP hash80BC7623FA3DC0B67971CEF6EA0629D278C7B787E22AC9DBC128D0A3B997A626 is
retained unchanged; no public0.23 branch/tag/release was announced.

## Actual route evidence and limits

Qualified QUERY_READ camera snapshot used202 reads, with ownership checked
before/after. Camera SetFOV is a scripted function: one input float NewFOV at
offset0,4-byte parameter block, no return, native interpreter entry9D9FA0.
The corrected descriptor offset for that entry is+8C, not+88. The observed
unlocked camera retained LockedFOV99.848518 while delayedFOV was120. The active
Hand Cannon's native zoom target field was60. Native unzoomed CalcFOV was82.5.

This establishes distinct values; it does not establish the full interpolation
formula or prove which caller supplied99.848518. Blindly applying the native
hip ratio to that already elevated value could expand the view instead of
zooming. The next attempt must target the native zoom target before blending,
not rescale an arbitrary cached/locked/interpolated value. Offline interpreter
code reads FFrame.Node+10, Object+14, Code+18 and parameter/locals storage+1C;
these are derivation facts, not an implemented or qualified new hook.

A24-second scalar capture was prepared but NOT performed: OpenProcess rejected
the old PID and process inventory confirmed the game was closed. No crash
cause or runtime failure is inferred from closure; no sample data are claimed.
All PID16896/controller31528C00/camera24CB0000 addresses are now stale.

## Verified return to accepted English 0.18.1

While closed, the checked rollback restored accepted ASI B13DBE... and retained
the removed state in
`InfiniteMod-Backups/before-rollback-20261014-103747-411-80c59ce2`.
The complete current INI was copied and checked before rollback, then preserved
afterward, including updated FOV120 and custom Backtick/Home keys. Independent
final installed hashes:

- ASI: `B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
- Current INI: `FDB4CBE91CABDE593F9005386B77D23EE4E846B2154EE4D345F0ECA97A5DD131`.
- Loader: `D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6`.

Private receipt: `InfiniteMod/tools/v023-rollback.log`.
The extra ProportionalAim key is ignored by accepted0.18.1. Progress saves and
native game configuration were not rolled back. No independent arms/weapon
FOV correction is claimed. Resume from accepted source again; exclude the
rejected CalcFOV conversion and Panini/GPU layers.
