# 0.24.2 - bounded native draw observation

October 14, 2026. **BUILT / CHECKED / EXPERIMENTAL. Not installed or player-validated at packaging. No FOV correction is delivered by this build.**

## Retained base

Checkpoint `63f87fea42141b0055c3d126dfd853120390837f` retains the player-confirmed 0.24.1 ON/OFF control and visible aiming effect. The accepted English 0.18.1 remains the broader gameplay fallback. Exact proportional held zoom, world perspective and independent arms/weapon projection remain unresolved. Rejected Panini, global projection changes and partial rig transformations are excluded.

ASI SHA256: `EE0A2AAB880DDDFDF115BB925B3439DB08D17BAE0406DA0A519A4D0902B50F54`.

## One objective and success criterion

Correlate owned first-person submission with actual native draw calls, the bound vertex shader and constant buffer, and the last full native upload source. One finite capture must distinguish hands/gun/reload props/effect candidates from world/HUD controls. A report with missing ownership, missing draw stages or unknown upload history is incomplete evidence, not a successful FOV fix.

The later functional objective remains one consistent lens for the entire first-person population, with world/HUD exclusion and no per-weapon profiles. This observer does not change any projection, shader, actor transform, bone, emitter or rendering argument. Existing gameplay actions retain their 0.24.1 behavior.

## Operation

Built separately with `clean/build.ps1 -DrawAudit`; the default build excludes this observer. No new UI, shortcuts or INI settings are added. Preserve the complete current INI, including user changes after earlier snapshots.

The diagnostic build installs checked native hooks but remains passive until a local request file is supplied. A developer creates `Binaries/Win32/InfiniteMod.draw-audit.arm` containing exactly eight ASCII bytes, `DRAW-V1` followed by a newline. A recognized request is consumed only when all observer hooks are ready. It waits at most 180 seconds for an owned rig submission with the game foreground, then captures at most 25 seconds, 500,000 common calls and 2,048 records. The output is `Binaries/Win32/InfiniteMod.draw-audit.csv` (123 columns). Raw reports stay private and are excluded from the ZIP and repository.

The requested in-game test is one short sequence with the revolver: menu closed, shoot, reload normally, aim/release. Keep the current world FOV and settings. The observer's purpose is evidence; no visible improvement is expected in this build.

## Checks completed

- Final x86 ASI compiled without warnings; retained controls, commands, progression, vitals, native aim, persistence and overlay/render-state checks passed.
- Own-process fixture: 4,000 opaque native wrapper calls and 1,000 mid-function API calls preserve arguments, stack, registers/flags, x87, MXCSR and XMM state.
- Checked native wrapper/commit signatures and seven engine API callsites use MinHook trampolines. Native API arguments and caller return slots are not rewritten.
- Owned graph, unknown/HUD exclusion, stale owner/view guards, nesting, marker and timeout checks passed. A mismatched bound buffer is reported without rebinding it.
- Offscreen Direct3D WARP fixture records an actual draw, shader identity and last full constant-buffer upload; rendered pixels are byte-identical with and without observation. This is fixture proof, not game-runtime proof.
- Default build's loaded executable/runtime sections match the installed 0.24.1; complete file hashes differ because build metadata differs. The diagnostic build is a separate output.

The queued upload source is not GPU readback. Particle ownership kinds remain candidates until correlated; no future projection change may rely on them alone. Fixed native callsites avoid a failed prototype that hooked state-dependent Direct3D context entrypoints; changing entrypoint addresses was demonstrated in the own-process fixture only, not as the cause of the game's visual defects.

## Installation and rollback

Use the packaged installer only after normal game closure. It backs up the loaded 0.24.1 ASI and preserves existing INI bytes and custom bindings. The printed rollback command restores that immediate checkpoint. The older accepted English 0.18.1 backup remains available. Do not overwrite the EXE or save files, reset settings, or install while the game is running.

The root ZIP includes README, English cumulative documentation, installer/rollback scripts, licenses, manifest and `Binaries/Win32`. It excludes raw reports, native disassembly, fixture executables and game binaries. Package integrity and installation are reported separately; this build is not a public release or player validation merely because it compiles.
