# Install / restore InfiniteMod

Current instructions apply to the v0.31 experimental line, targeting checksum-verified BioShock Infinite GOG 1.1.25.5165 Win32. Read `package.json` for the build's objective and success criterion. Player approval and local checks are recorded separately in the repository.

1. Close the game normally.
2. Extract the ZIP into a directory separate from the game. It contains scripts and `Binaries/Win32` directly at the root.
3. In PowerShell from that directory, run:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Installer.ps1 -GameDirectory 'P:\Games\BioShock Infinite'
```

Substitute your actual game root. The installer verifies the supported EXE and payload, backs up existing mod files under `InfiniteMod-Backups/install-*`, preserves an existing INI byte-for-byte and reuses a compatible loader. A different `dinput8.dll` is refused. New preferences use runtime defaults without overwriting your custom settings. Saves and the game EXE are not replaced.

Launch normally. Current world FOV updates are live; no special launcher or NativeFOV utility is needed. HUD/menu size and logo options take effect at the next launch. The module can apply its documented logo/HUD preferences to native INI files and change progression on explicit commands; rollback does not reverse game autosaves.

## Restore the previous module

Close the game and use the exact backup directory printed during installation:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Rollback.ps1 -GameDirectory 'P:\Games\BioShock Infinite' -BackupDirectory 'P:\Games\BioShock Infinite\InfiniteMod-Backups\install-<timestamp-id>'
```

Rollback verifies checksums, preserves removed state in `before-rollback-*` and refuses to replace an unexpectedly changed ASI/owned loader. Existing INIs that the installer reused are retained. Any INI restoration is protected by a recovery copy. Shared loaders are retained when other ASIs need them. `Retour.ps1` remains a compatibility entry point.

## Build the source

```powershell
.\distribution\bootstrap.ps1
.\test.ps1
.\build.ps1
```

Zig0.13.0 is pinned. Set `INFINITEMOD_ZIG` to the matching `zig.exe` if already installed. `bootstrap.ps1 -IncludeLoader` obtains the pinned Ultimate ASI Loader. ImGui/MinHook sources and licenses are in the repository. Compiler caches, test fixtures and local installation backups do not belong in release ZIPs.

The historical FOV tool is compiled only with `-BuildLegacyNativeFOV`. Legacy0.25/0.27 launch/range tooling and rollback support remain for historical packages; they are not instructions for the current release. See the archived build records when reproducing those versions.
