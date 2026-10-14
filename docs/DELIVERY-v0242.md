# 0.24.2 delivery checkpoint

October 14, 2026. **BUILT / CHECKED / INSTALLED / EXPERIMENTAL. Game capture and player validation pending.**

Retained base: checkpoint `63f87fea42141b0055c3d126dfd853120390837f` (player-confirmed 0.24.1 controls and visible aiming effect); accepted English 0.18.1 remains the broader fallback. The diagnostic adds finite observation only, no FOV fix. See BUILD-v0242.md.

Local ZIP: `releases/InfiniteMod-v0.24.2-native-draw-audit-experimental.zip`.
SHA256: `AC293A601ADD2994DEAB147F8501DC2866F81276D7A94F23B08BC38A4AF7D1A6`.
ASI: `EE0A2AAB880DDDFDF115BB925B3439DB08D17BAE0406DA0A519A4D0902B50F54`.

Package generation verifies every entry against staging. A separate extraction check confirms 27 root-layout entries, manifest payload sizes/hashes and x86 PE32. No raw capture, fixture executable, native disassembly or game binary is included. The embedded documents state the packaging checkpoint; this record tracks later installation separately. No GitHub upload or public release is claimed.

Before replacement, installed ASI remains `61523562EF1018A9F271EE259C287248CE21A966952A53C19E19D5F99F245F8A` and current INI remains `C37882D3ACF974517A0F7E0022BCA8A690FB8945B263683E95B3F68F728C53ED`. Preserve the latest INI bytes at installation rather than using an older snapshot. Normal game closure is required to unload the ASI. Final installer/rollback fixtures and the actual backup path remain pending.

## Verified installation

After normal closure, the final payload passed private filesystem integration checks: custom settings preservation, edited-INI recovery, unknown-loader refusal, tampered-package refusal, first-install rollback, shared-loader preservation and changed-ASI refusal. No real game was modified by those fixtures.

The actual installation transaction is `InfiniteMod-Backups/install-20261014-192721-538-757c6c83`, state INSTALLED, version 0.24.2. Installed ASI matches EE0A2AAB above. Current INI still matches C37882D3 above byte-for-byte; loader remains D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6. The backup ASI matches the retained 0.24.1 hash61523562 above and its INI matches the current settings. The accepted English 0.18.1 backup is also retained.

Immediate rollback after normal closure, from the extracted 0.24.2 package:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-192721-538-757c6c83'
```

No marker was created during installation. The observer has no requested capture until Booker is in gameplay. Installation integrity does not validate game rendering or fix the remaining FOV defects.

## Subsequent outcome

The requested game sequence produced204160 common calls but rows0,commits0,
invalid0,started0. No owned-rig capture was obtained. Read-only metadata checks
confirm current player publication but do not establish the failed filter's
cause. The player ends further observation and authorizes a native-option
experiment. The observer is excluded from0.25; no FOV fix is retained from it.
