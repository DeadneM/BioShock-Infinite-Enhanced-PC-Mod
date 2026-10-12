# Install an experimental build

This package supports the checksum-verified BioShock Infinite GOG x86 build.
The retained gameplay and controls base is v0.18.0. New features remain **EXPERIMENTAL**
until the user confirms their in-game test. The build objective and success
criterion are recorded in `package.json`.

1. Close the game normally.
2. Extract the ZIP into a directory separate from the game. README, licenses,
   scripts and `Binaries/Win32` are at the ZIP root, without an enclosing folder.
3. From the extracted directory, run the installer in PowerShell:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Installer.ps1 -GameDirectory 'C:\Games\BioShock Infinite'
```

The installer verifies the files, backs up the existing ASI and INI under
`InfiniteMod-Backups/install-*`, then installs the module. An existing INI
is preserved in full; new shortcuts use defaults supplied by the ASI.
Installation stops if a different `dinput8.dll` is already present.
A compatible existing loader is reused and left intact during rollback.
A loader added by this installation is also retained during rollback if
other ASI plugins are present.

These scripts do not replace saves, the game executable or native game INI
files. During gameplay, the module can change progression through the chosen
commands and apply HUD/logo options to native INI files, as described in the
README. Rolling back the module does not undo an autosave already made by
the game.

To restore the previous ASI and INI, close the game and use the backup path
printed by the installer:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Rollback.ps1 -GameDirectory 'C:\Games\BioShock Infinite' -BackupDirectory 'C:\Games\BioShock Infinite\InfiniteMod-Backups\install-YYYYMMDD-HHMMSS-fff-ID'
```

Rollback verifies checksums and preserves the removed state under
`InfiniteMod-Backups/before-rollback-*`. It refuses to replace an ASI or an
owned loader that changed after installation. The INI may have been edited
during gameplay: it is copied before the previous INI is restored.

The earlier `Retour.ps1` entry point is retained for existing rollback commands.
Both entry points run the same checked rollback operation.

To compile the source repository, the bootstrap downloads Zig 0.13.0 from
[the official website](https://ziglang.org/download/index.json) and verifies
its SHA256:

```powershell
.\distribution\bootstrap.ps1
.\test.ps1
.\build.ps1
```

An existing compiler can be selected with `INFINITEMOD_ZIG`, set to the full
path of its `zig.exe` version 0.13.0. ImGui/MinHook source and licenses are
included in the repository. `bootstrap.ps1 -IncludeLoader` also downloads
the [official v9.7.4 loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/tag/v9.7.4)
using pinned archive and DLL checksums. The compiler and its caches are not
included in the installable ZIP.
