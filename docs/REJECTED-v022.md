# Rejected 0.22 Panini scene experiment

Player decision: October 13, 2026. Verified return: October 14, 2026.

**REJECTED as the selected FOV solution.** The player reported that the lens
effect did not feel right and requested a correction of the game's native FOV
behavior instead. The selected offline preview was not in-game validation.
The work was paused and explicitly resumed the next day.

The checked local ASI was
`69A860A631BBA67C438EAD9A8111C2499CBD2155C7A9C612E5E2D670662082B9`;
the final ZIP hash was
`6F4792F2899E7449EF596C2E27302687B67E9D148ED468F9BB6E10C5EC409273`.
Tests, installation checks and real-game composite submission passed. They
did not establish the requested visual result. No public 0.22 release exists.

On resumption, process inventory confirmed the game was closed. The checked
rollback restored accepted English 0.18.1 from
`InfiniteMod-Backups/install-20261013-164852-884-36a295a3`.
Independent installed hashes are:

- ASI: `B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
- Complete retained INI: `EBE9526EF616D22D0B77A44973C9F28C3781FDB777FE03136B948718EEE8B362`.
- Loader: `D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6`.

The removed 0.22 ASI and updated INI, including WorldProjection=Panini, remain
recoverable in
`InfiniteMod-Backups/before-rollback-20261014-095558-590-79c65ef8`.
Private receipt: `InfiniteMod/tools/v022-rollback.log`. Progress saves and
native game configuration were not rolled back. No relaunch is claimed here.

The next experiment must start from accepted English 0.18.1 without a Panini
shader, scene-composite hook or rejected 0.21 GPU/ADS layers. Existing native
Hor+ aspect handling is already established. Correct native camera/aiming
behavior in a separate build; whole first-person placement/lens remains an
independent unfinished problem. Do not describe a lens-axis conversion as a
fix for natural wide-angle perspective.
