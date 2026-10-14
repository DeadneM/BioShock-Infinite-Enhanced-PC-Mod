# 0.25.0 delivery checkpoint

October 14, 2026. **BUILT / CHECKED / INSTALLED / EXPERIMENTAL. Player confirms native slider operation; proportions remain unsatisfactory. Visual fix NOT VALIDATED.**

Branch: `experiment/native-fov-slider`. Base: retained controls63f87fe, accepted English0.18.1 gameplay. User-authorized changes retire the camera result/weapon target adapters and abandon draw observation. The replacement manages only the native FOV slider range before launch. See BUILD-v025.md for the precise scope, native75 migration convention and visual limitations.

Final ASI SHA256: `48B3CF39629183DCA56ACF1E1920CAFF0F8976DC2180CA9AD1A5A773232FCF09`.
Native helper SHA256: `D86F58AC9BD0EFB1E08BEDCD1ED982A9F33CE7D0C832D87D36E71C7E7C66E813`.

The final retained suite passes, including speed, unchanged native CalcFOV/ADS/cache behavior, grants/resources/infusions, held aiming, crosshair, bindings and overlay/render-state at720p/4K/ultrawide. Native configuration tests pass for exact single-token editing, preserved slider/unrelated bytes and comments, duplicate/malformed refusal, UTF-8/UTF-16LE, first-backup immutability, writer exclusion, idempotence and restore that preserves a later slider change.

ZIP generation/extraction checks and final distribution integration checks are recorded below when completed. Installation, actual native option loading, visible slider/aim/reload result and player proportion assessment remain separate and pending. No public upload or new FOV functionality is claimed merely from compile/configuration checks. Existing current mod INI and shortcuts must be preserved byte-for-byte at installation.

## Verified delivery and installation

ZIP: `releases/InfiniteMod-v0.25.0-native-FOV-experimental.zip`,27 root-layout entries.
ZIP SHA256: `4231FFE80BF60A98D6074DD0F6731508120E51557E4EDF6A05FF63F5CD4767A8`.
Generation verifies every ZIP entry; independent extraction verifies paths/count,
manifest module sizes/hashes, helper hash and x86 PE32 ASI/helper. No raw capture,
game binaries or private fixtures are included. Embedded documents retain the
pre-install checkpoint; this record supersedes their pending installation status.

Final distribution checks pass in private copies using the extracted ASI and
byte-matching installer/rollback scripts. A private helper entry point invokes
the same native configuration editor against fixture files only. Version0.25
installer/rollback invocation, preserved slider, original cap restoration and
altered-helper refusal pass, alongside existing settings/loader/package guards.
This fixture does not load the game or write real user native options.

Actual installation after verified normal closure:
`InfiniteMod-Backups/install-20261014-232833-229-539c2fb5`,state INSTALLED,
version0.25.0,nativeFOVApplied=true. Installed ASI matches48B3CF39 above.
The complete mod INI remains
`C37882D3ACF974517A0F7E0022BCA8A690FB8945B263683E95B3F68F728C53ED`.
The saved prior ASI is the actual diagnostic0.24.2,
`EE0A2AAB880DDDFDF115BB925B3439DB08D17BAE0406DA0A519A4D0902B50F54`.
Older retained0.24.1 and accepted18.1 backups remain available separately.

The native configuration changes exactly one token:15.000000 ->46.666667.
Independent byte comparison proves all other bytes, including FieldOfView1.0,
are unchanged before launch. The immutable full native backup matches original
SHA256 `E67FAFB2BC042E0D511D3EF77D2FDF6E6AC02E938E7BB5474979B51B6D86959A`.
Post-edit native SHA256 is
`DC05582FF4F64E9A1472701A6AFDC5F7340FEA8FB194B2497B15FACD4263EDB`.

The checked launcher prepares the option before starting the original EXE.
Startup logs confirm0.25, no FOV/aim-target/render hooks installed, native
range config46.666667, UI cap1.25,3840x2160 Present and no invalid/conflicting
bindings. These logs establish startup/configuration, not visible improvement.
The player was asked for one native-slider/revolver/aim comparison, no capture.

Rollback after normal closure, from the extracted0.25 package:

```powershell
.\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-20261014-232833-229-539c2fb5'
```

The helper restores only the original native range token and keeps any later
native slider/other edits; module rollback follows. No GitHub push/release is
claimed for this candidate.
