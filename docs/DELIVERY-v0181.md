# v0.18.1 English delivery checkpoint - October 12, 2026

The localization is BUILT, CHECKED and INSTALLED. The player confirmed both
English menus and preserved custom shortcuts on October 12, 2026.
The public prerelease, tag, branch, ZIP and checksum downloads are verified.

- Retained base: accepted v0.18 controls, commit
  `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
- Published English source commit on `main`:
  `cd0873936a92f0a0e2c00dca88557c7724b7b1a6`.
- Source tree: `cca6c50943f84d2c88b1f8eeebcfd2b3e92b0c35`, identical to
  the checked local build snapshot `eab12be926efbfb286051087e95749138d0fb067`.
- Local installable ZIP: `InfiniteMod-v0.18.1-English.zip`, 4,613,586 bytes,
  18 entries at the root/required tree, with no enclosing folder.
- ZIP SHA256:
  `1853FEF39BAFE9CCC6739B1E66A30A959E44999E86A956A4C82BF831B9EAA545`.
- Packaged and installed ASI SHA256:
  `B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.

The ZIP's entry list and each payload hash were checked. It includes the
English `Rollback.ps1` entry, retaining `Retour.ps1` for compatibility.
Six isolated install/rollback scenarios passed. The runtime regression
suite and x86 compilation passed; menus were checked at 720p, 4K and
ultrawide. These results remain separate from the player's result.

Installation occurred after verified game closure at 12:18 CEST. Its backup
contains the accepted v0.18 ASI. The actual transaction is INSTALLED;
the existing INI, compatible loader and game executable retained their
exact hashes. Customized menu keys were preserved.

The game was relaunched. Its new log confirms version 0.18.1, ready native
command/aim/camera hooks, a 3840x2160 swap chain, and zero invalid or
conflicting keybind entries. This is loading evidence, not visual validation.

The player then reported: "English menus OK, Booker in-game" (translated).
English main/shortcut menu readability and existing menu keys are
USER-VALIDATED. This does not add an unreported test of every gameplay
command or restart persistence. The user requested a pause afterward, then
authorized publishing this completed step. FOV research remains paused.

The verified public v0.18 release remains the fallback. No v0.19 diagnostic
or rejected v0.20 renderer/aiming hooks are included. Weapon/hand FOV and
proportional aiming remain the next separate experiment.

The public `experiment/v0.18.1-english` branch was then created from the
checked source commit and verified through GitHub's ref API. The earlier
temporary automatic approval-review HTTP 429 failure did not report a
safety rejection; the later authorized attempt succeeded.

The immutable ZIP records the pre-install documentation checkpoint. These
current notes and the public release notes supersede its pending statuses.
The checked binary and archive have not been rebuilt or overwritten.

## Verified public delivery

- Release ID: `410669351`; published prerelease, not a draft.
- [Release](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/tag/v0.18.1-experimental).
- [Direct installable ZIP](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.1-experimental/InfiniteMod-v0.18.1-English.zip).
- [ZIP checksum](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod/releases/download/v0.18.1-experimental/InfiniteMod-v0.18.1-English.zip.sha256).
- Tag `v0.18.1-experimental` and the English source branch were checked at
  `cd0873936a92f0a0e2c00dca88557c7724b7b1a6`.
- The ZIP and checksum were downloaded from these direct URLs. ZIP SHA256,
  all 18 entry names/bytes, all three manifest payload sizes/hashes, and the
  inner ASI match the exact locally installed package.
- GitHub's uploaded asset digest also matches the independently downloaded ZIP.
- The retained `stable/v0.18.0-controls` branch remains
  `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`; rejected FOV experiments remain separate.

Publication changes only delivery documentation after the checked runtime
snapshot. The source tag continues to identify the build snapshot; subsequent
documentation commits do not change the packaged ASI or immutable ZIP.
