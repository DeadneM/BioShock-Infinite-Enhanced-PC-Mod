# v0.18 release verification - October 11, 2026

The public GitHub prerelease and direct ZIP were fetched and verified.

- Release: https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/tag/v0.18.0-experimental
- Direct installable ZIP: https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.0-experimental/InfiniteMod-v0.18.0.zip
- Release source/tag commit: `7026943a5273c707c7e6763f8b36104ad08b5863`.
- Source tree: `be5f871122740e08a88a289f447b06cd2dd8d3d7`, matching the local curated snapshot.
- ZIP SHA256: `19EB0A28136633554D38B91D749D37DAEA32760EF1F6DDA8658F33460B5BEE0D`.
- ASI inside the downloaded ZIP: `61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
- Size: 4,605,778 bytes;16 verified root/tree entries, without an enclosing folder.
- Repository confirmed public, description/docs/release in English; overlay UI remains French.
- F1 vitals refill and shortcut reassignment are user-confirmed. The release remains a preview of an unfinished mod; weapon/hand FOV is still unresolved.
- `main` and `stable/v0.18.0-controls` retain the accepted controls step. `stable/v0.15` preserves the previous validated baseline; rejected FOV code stays isolated.

Installer/rollback tests passed in a separate fixture. The actual install preserved the existing INI, loader and game EXE. No game assets, saves, profiles, captures, private transcripts or compiler caches are in the published source/ZIP.
