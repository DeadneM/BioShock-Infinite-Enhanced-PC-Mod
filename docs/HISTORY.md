## October 25 - 0.31 signed native distance

Player accepted closer offset10 in0.30.1. New requested range -50..+50; Ctrl+Minus decreases, Ctrl+Plus increases by2 with endpoint clamping. Old closer10 loads as-10 without INI rewriting. Zero/native ADS and existing features retained. Extended range remains experimental. See [scope and controls](BUILD-v031.md).

## October 25 - 0.30.1 direction correction

Player reports all sensitivity parameters appear to work and confirms arms and weapon move together in0.30. Requested change: bring the native first-person root closer to the camera. Keep the same0..10 control, zero default, ADS bypass, keybinds and INI. Only local-X sign changes; temporary effects still require visual confirmation. See [build scope](BUILD-v0301.md).

## October 25 - 0.30 native position experiment

Requested: adjustable backward movement of the whole first-person root. Default0; Ctrl+0 cycles bounded offsets; ADS bypassed. Exact on-disk native offset consumer identified. Visual coverage remains unverified; no camera/projection fix is claimed. Keep 0.29 rollback and existing personal settings. See [scope and player test](BUILD-v030.md).

## October 17 - 0.29 local input profiles candidate

Six independent mouse/controller normal, ADS and sniper multipliers are implemented on experiment/input-comfort. All default to x1. Local tests pass; actual game consumption is unverified. Bobbing/sway remains pending. No installation, publication or game launch. See [build record](BUILD-v029.md).

# Cumulative history

## October16 -0.28 live FOV and XInput hold aim

The player accepts remaining0.27 behavior, requests proportional ADS restored,
FOV without restart, configurable controller hold aim/LT, and GitHub publication.
Retain global shortcuts; restore only the retained camera/target policy and add
XInput input to the hold state machine. No optical/viewmodel experiment returns.
New behavior remains EXPERIMENTAL until player testing; see BUILD-v028.md.

0.28 package installed with unchanged user INI/loader/native options and verified
0.27 rollback. Experimental GitHub release414838404 and tag0113895 are public;
downloaded ZIP/checksum match checked local artifacts. No game launch or renewed
player validation. See DELIVERY-v028.md for immutable hashes and exact source.

## October 16 - abandon optical correction; add global shortcuts

At the player's request abandon the FOV/lens correction project. Preserve gameplay
and retire the0.26 green runtime. New0.27 candidate adds default-ON outside-menu
commands, a rebindable persistent toggle, and native FOV-slider/HUD preferences.
FOV is a closed-game next-launch native token edit, not a live camera modification.
No launch or player validation inferred. See BUILD-v027.md and delivery record.

## October 16 - green screenshots and lens gate

Player provides five screenshots: candidate23 covers revolver,24 hands/arms,
25 sleeve,67 broad surfaces and68 a lower-left HUD-area rectangle. Registry
is full. Record narrow visual proof, not stable ownership or complete rig/FX.
Source feasibility check finds only silhouettes, not isolated color layers.
Do not install a masked warp with known old-image/occlusion errors. Lens prototype
remains unimplemented pending a qualified composition/projection boundary.

## October 16 - opt-in green visual selector

User authorizes a temporary F5-F8 diagnostic to visually identify the rig.
Prepare independent branch experiment/green-layer-selector from the documented
0.25 source, retaining accepted English gameplay and authorized native-camera
restart. Candidate shader pairs are silhouettes, not engine ownership layers.
No FOV change, external process capture or game launch. See GREEN-DIAGNOSTIC.md.

## October 15 - offline native proxy comparison

Check the supported original executable and installed0.25 ASI without opening
a process. Native constructors and125-entry tables establish that Infinite's
X/base skeletal components share their proxy factory. No direct UDK-style
specialization is identified there. Correct the implementation target to the
known primary skin route and retain separate particle handling as necessary.
No new build, capture, installation, launch or visual success is claimed.

## Native slider result and UE3 research

The player confirms0.25 slider operation while proportions remain bad. No
visual correction is validated. The user requires asking before every game
launch/relaunch. UE3/UDK public source research identifies coordinated custom
mesh FOV, arm/overlay propagation and explicit particle-FOV support. This is
a concrete design reference, not an Infinite implementation. One camera lens
does not disprove per-component compensation. No build/install/launch occurs
for this source comparison; see UE3-FIRSTPERSON-FOV.md.

## October 14 - 0.25 native option experiment

The player stops observations after0.24.2 records204160 common calls but zero
usable owned-rig rows. That observer is not a functional result. The new branch
starts at retained63f87fe and intentionally retires its camera-result/aim-target
adapters, as authorized. Gameplay/speed/controls remain; only the native
MaxUserFOVOffsetPercent range is managed before launch. Native slider position
and unrelated native/mod settings are preserved. No independent weapon FOV or
improved proportions are claimed. See BUILD-v025.md.

The27-entry candidate ZIP and final ASI/helper were checked. Installation is
verified after normal closure, with exact mod INI preservation and a one-token
native cap change backed by the original file. The mod launcher starts the
game; logs confirm native range and removal of the retired hooks. The single
native-slider/revolver comparison is pending player feedback. No public release.

## October 14 - 0.24.2 bounded draw observer built and checked

Retain the player-confirmed 0.24.1 control and visible aiming effect, with English
0.18.1 as the broader accepted fallback. Add an opt-in compile-time observer at
checked native submission, commit and seven API callsites. One finite marker
request records ownership, native upload source and actual draw bindings without
rendering changes. Opaque CPU/ABI and real offscreen Direct3D pixel comparisons
pass, as does the retained suite. Diagnostic output is not a FOV functionality.
Installation and real game capture remain pending at packaging. No public release
is claimed. See BUILD-v0242.md; no keys or default settings were changed.

## October 14 - isolated first-person restart, no new build

Checkpoint63f87fe preserves the player-confirmed0.24.1 controls and visible aim
effect. A separate first-person branch initially keeps identical runtime code.
The requested held-aim test completed, but the bounded trigger capture contains
zero samples: exact held zoom is unmeasured. Immutable primary Infinite Steam
source corroborates a shared gameplay lens and reference-aspect law; it does
not provide a GOG independent viewmodel fix. Its bone/effect intervention is
not adopted. Record world/rig defects as unresolved; preserve ASI/INI/loader and
the accepted18.1 rollback. No new package, installation or release is claimed.

## October 14 - 0.24.1 menu and visible aiming change confirmed

The player confirms that the ON/OFF row alternates and the aiming effect works
with a radical visible difference. Retain the control/feedback improvement.
Exact held zoom remains to be qualified because ON appears to keep the same
FOV. Arms/weapon and world proportions remain unresolved. Fresh native viewport
inputs and CPU shadow agree with horizontal120/vertical88.507/aspect16:9;
no final-draw correction is claimed. Future first-person work must stay isolated
and preserve this confirmed behavior.

## October 14 - 0.24.1 control-feedback preparation

The player clarifies that the aim row stays NATIVE. An explicit colored ON/OFF
row, unambiguous generic player status and request/save/display logs are prepared
without changing the native lens policy. Production persistence now also runs
in the menu fixture, with real INI readback and failed-save state preservation.
This is not a proven explanation or a new camera fix. All retained tests and
six final-payload installer/rollback scenarios pass. After confirmed closure,
the candidate is installed with the exact current INI preserved and a direct
verified accepted18.1 ASI backup. Menu feedback still needs player confirmation.
See BUILD-v0241.md and DELIVERY-v0241.md.

## October 14 - 0.24 comparison reports native-looking aiming

The player reports that aiming appears to stay native in every mode. 0.24
is not validated or retained. The real INI and log establish option activation
and converted target returns, but not the eventual rendered camera result.
The next discriminating check is held-camera behavior in the current session,
with per-sample weapon identity. No new runtime build or visible fix is claimed.

## October 14 - 0.24 target producer preparation

Revisiting the existing 0.12.1 evidence located the native ordinary/TBar target
getter and its three camera consumers. Fresh executable and live schema checks
confirm the earlier target-before-blend route. 0.24 is prepared from accepted
English 0.18.1 with one optional target-return adapter, preserving the accepted
world policy and all prior actions. This differs from rejected 0.23's unused
CalcFOV zoom path. All native/retained tests and six final-payload installation
fixtures pass. The checked ZIP is installed after confirmed normal closure,
with the accepted ASI backed up and whole INI/loader preserved. No visual
result is claimed yet; see DELIVERY-v024.md for actual transaction details.
See [the bounded objective](BUILD-v024.md).

## October 14 - 0.23 functional failure and accepted-base return

The player reported no difference with proportional aiming ON/OFF. Logs show
ordinary aiming uses LockedFOV, which the candidate explicitly bypasses.
0.23 is REJECTED as a functional fix. A bounded camera/target read establishes
the scripted SetFOV descriptor and distinct locked/target values; it does not
qualify a replacement or justify blindly scaling an interpolated field.
The game was later found closed; the planned24-second capture did not run.
Checked rollback restored accepted English0.18.1, preserving the complete
current INI and custom keys. No crash causality or new functional build is
claimed. See [REJECTED-v023.md](REJECTED-v023.md).

## October 14 - native aiming candidate 0.23

The new branch starts from accepted English 0.18.1, retaining cumulative
experiment history but excluding rejected runtime layers. One optional
CalcFOV policy preserves native optical zoom magnification relative to the
selected hip-fire FOV. Ctrl+F9 compares modes and is rebindable; default OFF.
Native Hor+, hip-fire FOV and all prior commands are retained. Locked-camera
routes are bypassed. Whole first-person placement remains unfinished; the
player must confirm the aiming result. See [BUILD-v023.md](BUILD-v023.md).

The final ASI and integrated/native/optical/menu checks pass, as do six private
install/rollback scenarios. The root-layout23-entry ZIP was verified and
installed while closed, with accepted-ASI backup and complete INI/loader
preservation independently checked. The game was relaunched for comparison.
Status: BUILT, CHECKED, INSTALLED, EXPERIMENTAL; not USER-VALIDATED.
See [DELIVERY-v023.md](DELIVERY-v023.md).

## October 14 - Panini rejected, accepted English base restored

The player declined the 0.22 lens effect as the FOV solution, requested native
FOV work, paused, then explicitly resumed. Process inventory confirmed normal
closure. The checked rollback restored accepted English 0.18.1; independent
ASI/complete INI/loader hashes agree. Removed 0.22/preferences remain saved
for recovery. No public 0.22 release exists. Native Hor+ aspect handling is
already established; proportional aiming and independent whole first-person
placement remain unfinished. See [REJECTED-v022.md](REJECTED-v022.md).

## October13 - selected Panini experiment0.22

The player's video at FOV100 was examined; its distant NPC/large nearby grip
contrast did not establish an axis-ratio error. The player selected the stronger
Panini numerical preview. A separate experiment starts from accepted English
0.18.1, without rejected0.21 layers. Its optional scene composite, configurable
Ctrl+F9 and full Direct3D-state restoration are built and checked in the
retained suite, including real WARP output and swapchain resizing. Status:
**INSTALLED, EXPERIMENTAL**, not player-validated. The final package passed six
isolated installation/rollback scenarios and was installed after normal game
closure, with accepted-ASI backup and complete INI/loader preservation verified.
Fresh startup confirms the scene adapter and native 4K swapchain. The visual
ON/OFF result remains pending. First-person placement remains
unfinished. See [BUILD-v022.md](BUILD-v022.md) and [video review](VIDEO-REVIEW-20261013.md).

## October 13, 2026 - rejected 0.21 and verified restoration

The shared first-person projection/ADS experiment 0.21 was built, checked and
installed, then **REJECTED** by the player for severe world/character distortion.
After normal game closure, accepted English 0.18.1 was restored; the ASI,
complete existing INI and loader hashes match the checked accepted backup.
No 0.21 public release was created. The rejected work remains on
`archive/rejected-fov-v0.21` at `bbfe04c67e9f929ab2419e8354a0f3d22a7cd46a`.
See [the retained verdict](REJECTED-v021.md).

Fresh `experiment/physical-fov` starts from accepted checkpoint
`99a75aa9e1a47543c418b2dc37b43a8883898c72`. Its first objective is to establish
the actual horizontal/vertical world lens and viewport contract, before a new
first-person change. No rejected GPU/ADS layers are carried forward. This is
**PREPARED** work; no fresh binary or player validation is claimed.

This notebook separates behavior confirmed in a game session from compilation
and simulation checks. Validating one feature does not automatically validate
the entire build containing it.

## Statuses

- **PREPARED**: source/documentation changes are ready; no new build, test,
  installation, or delivery is claimed.
- **BUILT**: compilation produced an ASI.
- **CHECKED**: the stated checks were actually performed.
- **EXPERIMENTAL**: a change still awaits an in-game result.
- **USER-VALIDATED**: the player confirmed the relevant behavior.
- **REJECTED**: the player observed a defect; this experiment does not advance
  to the stable branch.

## Retained milestones

| Version | Change and result | Retained status |
| --- | --- | --- |
| 0.9 / 0.9.1 / 0.9.2, September 2026 | Overlay on the game's graphics device. The player confirmed the main menu and overlay. Manual logo skipping was not confirmed; automatic skipping of the four logos, with loading preserved, was then confirmed. Selective logo restoration was checked. | Overlay and automatic skipping **USER-VALIDATED**; manual skipping abandoned as the delivered solution |
| 0.11, October 1, 2026 | Weapons, ammunition in both slots, and hold-to-aim. The player replied â€œyes, perfectâ€ (translated); in-game reads confirmed magazine/reserve counts and the secondary weapon. | Features retained in the validated base; this version's initial HUD setting is not a visually validated result |
| 0.12 / 0.12.1, October 1â€“2, 2026 | Speed x1 to x2 and world FOV 80â€“130. HUD revised to the native MaxResolutionUpScale ceiling; the incorrect ceiling causing excessive interface enlargement at 4K was removed. | World FOV and features retained; first-person rendering **known issue**, HUD effects require a separate check |
| 0.13 / 0.13.1, October 4, 2026 | Grants for Vigors, gear, upgrades, money, and lockpicks. Money/lockpicks confirmed; catalog checks were too strict for some other items and were corrected after reading actual classes and properties. | Resources **USER-VALIDATED**; other grants initially rejected, corrections continued |
| 0.14, October 5, 2026 | Carbine catalog corrected. Native reads: 48 weapon upgrades, 16 Vigor upgrades, 8 Vigors, and 49 gear items. The player subsequently confirmed the other grants but reported the crosshair. | Progression retained; this version's crosshair hiding **defective** |
| 0.15, October 8, 2026 | Hiding of both crosshair widgets corrected; health/salts/shield infusions up to their native cap of ten each. The player confirmed â€œCrosshair OK and all three gauges at 10/10â€ (translated). Tests and independent reads agreed. | **USER-VALIDATED** predecessor; accepted features retained in 0.18, with a known first-person FOV limitation |
| 0.16 / 0.16.1, October 9, 2026 | First-person adaptation experiments. The player reported misaligned reload parts, cartridges, and firing effects. | **REJECTED** as visual fixes; validated progression features preserved separately |
| 0.17 / 0.17.1, October 9â€“10, 2026 | Shared DPG2 projection experiment, followed by preservation of native camera calculations. Builds and mathematical checks passed, but videos and feedback still showed defects. The flash was aligned at world FOV 80 and offset at 120. | FOV fixes **REJECTED**; visual success is not inferred from tests alone |
| 0.18.0, October 10â€“11, 2026 | Return to the 0.15 camera base. Requested default shortcuts, health/shield/salts refill, and a menu to edit all 23 shortcuts, including Insert. x86 compilation, integrated checks, installation, and public delivery were recorded. The player confirmed F1 refill and shortcut reassignment. | **BUILT**, **CHECKED**; F1/refill and remapping **USER-VALIDATED**. Current accepted controls base; restart persistence and every remapped action not separately confirmed |
| 0.19, October 11, 2026 | FOV diagnostic experiment. Its observations did not produce a functional weapon/hand FOV correction. | **DIAGNOSTIC**; no functional FOV fix retained |
| 0.20, October 11, 2026 | Renderer experiment. The player reported HUD flicker and no weapon/hand FOV correction. | **REJECTED**; excluded from the accepted controls base |
| 0.18.1, October 11â€“12, 2026 | English localization from the accepted 0.18 controls checkpoint. Runtime edits change text, the version label, and equivalent fixture expectations while preserving behavior. The retained suite and x86 compilation passed; menu fixtures covered 720p/4K/ultrawide. Local ZIP, backup and installation were verified. The player confirmed English menus and preserved custom menu keys. | **BUILT**, **CHECKED**, **INSTALLED**; English menus/custom menu keys **USER-VALIDATED**. Public release/download and further unreported gameplay/restart checks pending |

## Resume from the validated base

The current retained base is **0.18.0 controls**, with F1 vitals refill and
shortcut reassignment confirmed by the player. The accepted source checkpoint
for the localization is `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
The accepted packaged ASI has SHA256:
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
This feature validation does not establish restart persistence or fix FOV.

The original user-validated 0.15 ASI has SHA256:
`0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
It is distinct from new builds of the recovered source.

Camera-policy sources predating the 0.16 experiments were recovered from the
local work history, then checked using the base's full test suite. Recovering
the source does not make a newly built ASI a previously user-validated binary.

Historical recovery checkpoints checked on October 10:

| Branch | Checked commit | Role |
| --- | --- | --- |
| stable/v0.15 | 0d331468ac6f987b1742d8a95fbb60fc34ef4c11 | Recovered base sources and validation provenance |
| experiment/fov-v0.17.1 | d96f9d5aec59d92606350a4727470e1741504f0f | Archive of rejected FOV experiments |
| experiment/v0.18.0-bindings | Delivery source/tag commit `7026943a5273c707c7e6763f8b36104ad08b5863`, recorded in DELIVERY-v018.md | Shortcuts and gauge refill, without the rejected FOV experiments |

Public repository:
[DeadneM/BioShock-Infinite-Enhanced-PC-Mod](https://github.com/DeadneM/BioShock-Infinite-Enhanced-PC-Mod).
The public 0.18 release, tag/source commit, direct ZIP, downloaded payload,
and retained `main` / `stable/v0.18.0-controls` references were verified as
recorded in [DELIVERY-v018.md](DELIVERY-v018.md). The previous stable/v0.15
checkpoint and rejected FOV work remain separate. Private captures, saves,
raw logs, and game files are excluded from published mod sources.

Built 0.18.0 ASI checked by SHA256:
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
The integrated tests and compilation completed without warnings. Checks
include menus at multiple resolutions, all three shortcut pages, navigation
after reassigning menu-opening shortcuts, and the new simulated native refill.
These results are not in-game validation.

## Next step

Publish the already checked English ZIP, then verify its release and downloaded
contents before replacing the public download link. The installation and player
menu result are recorded in [DELIVERY-v0181.md](DELIVERY-v0181.md). Keep restart
persistence and all remapped actions as explicit remaining player checks.
FOV research is paused; publication does not claim an optical correction.

The FOV investigation remains separate. After the unsuccessful 0.19 diagnostic
and rejected 0.20 experiment, reassess the evidence and correction boundary
before another visual build. The goal remains a world view and a shared view
for the character's complete first-person rendering.
## Player confirmation - October 11, 2026

The player confirmed that current-value vitals refill and shortcut reassignment work in-game
(translated: "Vitals refill and shortcuts OK"). These two features
are USER-VALIDATED. A restart-persistence test and every remapped action were not separately reported;
those remain distinguished from the integrated routing/INI-reload checks. Weapon/hand FOV remains unresolved.
The accepted 0.18 controls build is the retained base for 0.18.1, preserving
earlier validated features and excluding rejected FOV hooks.

## 0.18.1 preparation â€” October 11, 2026

Historical preparation checkpoint; superseded by the build and installation
checkpoints below.

Objective: translate the mod and project text into English without changing
accepted behavior. The runtime pass covers both menus, all 23 action labels,
weapon display names, request/progression statuses, and errors. Native names,
config keys, custom shortcuts, defaults, formulas, signatures, and game
parameters are preserved. Source review found string-only edits with identical
wide identifiers and printf format specifiers; equivalent fixture phrases were
updated. This review is not a new compilation or regression-suite result.

The build succeeds after English text and layout, retained behavior, package
preservation, and the downloaded release have been checked, with the player's
result recorded separately. At preparation there is no new ASI/ZIP hash or
verified release link. The README continues to offer the verified 0.18 ZIP.

## 0.18.1 build and regression checkpoint â€” October 12, 2026

Historical pre-installation checkpoint; its pending delivery/player statements
are superseded by the installation and confirmation checkpoint below.

The English localization was compiled for x86 from
`experiment/v0.18.1-english`, based on the accepted controls checkpoint
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
The ASI SHA256 is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.

The full retained suite passed, including native command guards, key capture,
conflicts, reassignment, configuration persistence, and the module's own
D3D11 menu fixture at 720p, 4K, and ultrawide. The 720p main menu and shortcut
menu images were inspected. These are controlled checks, not game-session
validation of the new binary.

All 23 action identities, default shortcuts, custom INI entries, native asset
names, signatures, formulas, and command conditions remain unchanged. English
text covers both menus, weapon labels, requests, warnings, and progression
results. The English `Rollback.ps1` entry retains `Retour.ps1` compatibility.
No rejected FOV hooks were imported, and no weapon/hand or aiming FOV fix is
claimed. Installation, package/release verification, and the player result
are pending at this checkpoint.

## 0.18.1 installation and menu confirmation â€” October 12, 2026

The immutable installable ZIP was checked: 18 entries at the root and required
`Binaries/Win32` tree, without game files or private data. Its SHA256 is
`1853FEF39BAFE9CCC6739B1E66A30A959E44999E86A956A4C82BF831B9EAA545`;
the packaged and installed ASI has SHA256
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
Its bundled documentation is the earlier pre-installation checkpoint and is
retained unchanged. This record and eventual release notes supersede its
pending-status statements.

Installation followed verified normal game closure and a checked backup on
October 12. The complete custom INI, compatible loader, and executable retained
their exact hashes. The player confirmed both English menus and existing custom
menu keys. Those results are **USER-VALIDATED**; no additional gameplay-command
or restart-persistence test is inferred.

English source commit on `main`:
`cd0873936a92f0a0e2c00dca88557c7724b7b1a6`, tree
`cca6c50943f84d2c88b1f8eeebcfd2b3e92b0c35`, matching the checked local build
snapshot `eab12be926efbfb286051087e95749138d0fb067`. The separate stable 0.18
controls checkpoint stays at `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`.
The public release and downloaded payload were subsequently verified below.
See [DELIVERY-v0181.md](DELIVERY-v0181.md) for the actual delivery checkpoint.

The user paused after the English-menu confirmation. FOV research remains
paused and unfinished; no diagnostic 0.19 or rejected 0.20 hooks are retained.

## 0.18.1 public delivery - October 12, 2026

The user authorized publishing the completed English step while FOV research
remained paused. The English branch and source tag were verified at
`cd0873936a92f0a0e2c00dca88557c7724b7b1a6`. Prerelease ID410669351 is
published at `v0.18.1-experimental`; it contains the immutable installable ZIP
and checksum. Both were downloaded from their public direct links. ZIP hash,
all 18 entry bytes, manifest payload sizes/hashes and inner ASI match the
locally installed package. The accepted stable controls branch is unchanged.
README now points to the verified English release. Full delivery evidence:
[DELIVERY-v0181.md](DELIVERY-v0181.md). This is publication of the confirmed
English menus and retained controls, not a new FOV correction.

October25 delivery update: 0.30 native position candidate BUILT / CHECKED / INSTALLED on P: with exact0.29 rollback; personal INI and loader unchanged. Gameplay unvalidated and game not launched. See DELIVERY-v030.md under docs/.

October25: 0.30.1 installed locally after checks; only local-X direction reversed. Existing settings preserved; new closer direction not yet player validated.


## Player approval and publication request - October 25

The player accepted 0.31 ("c'est tres bien") and explicitly requested GitHub publication. Treat the signed distance controls and selected placement as player accepted, not as exhaustive extreme-position/effect coverage. Preserve the exact tested ZIP FCB9A7DF21C9A7BF097A6F48E372C1B4615300490F635E686B39A42782F2657D and final ASI6DD72AB0168205B4DFC6CBF4B435392097D78ECB117553C7D7FC80859E408DFB. Publication must use a separate experimental branch/tag/release; main remains untouched. No game launch.
