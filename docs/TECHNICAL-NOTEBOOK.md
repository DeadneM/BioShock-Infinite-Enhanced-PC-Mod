## October 25 - 0.31 signed native distance

Player accepted closer offset10 in0.30.1. New requested range -50..+50; Ctrl+Minus decreases, Ctrl+Plus increases by2 with endpoint clamping. Old closer10 loads as-10 without INI rewriting. Zero/native ADS and existing features retained. Extended range remains experimental. See [scope and controls](BUILD-v031.md).

## October 25 - 0.30.1 direction correction

Player reports all sensitivity parameters appear to work and confirms arms and weapon move together in0.30. Requested change: bring the native first-person root closer to the camera. Keep the same0..10 control, zero default, ADS bypass, keybinds and INI. Only local-X sign changes; temporary effects still require visual confirmation. See [build scope](BUILD-v0301.md).

## October 25 - 0.30 native position experiment

Requested: adjustable backward movement of the whole first-person root. Default0; Ctrl+0 cycles bounded offsets; ADS bypassed. Exact on-disk native offset consumer identified. Visual coverage remains unverified; no camera/projection fix is claimed. Keep 0.29 rollback and existing personal settings. See [scope and player test](BUILD-v030.md).

## October 17 - 0.29 local input profiles candidate

Six independent mouse/controller normal, ADS and sniper multipliers are implemented on experiment/input-comfort. All default to x1. Local tests pass; actual game consumption is unverified. Bobbing/sway remains pending. No installation, publication or game launch. See [build record](BUILD-v029.md).

# Cumulative technical notebook

## October16 -0.28 requested camera/aim restoration

Use0.27 accepted gameplay as base; explicitly restore63f87fe player_options camera
and target-return policy with its schema/caller guards. Remove0.27 slider control
from the runtime menu; preserve its saved preference inertly. No native config
write or helper/launcher requirement. Proportional effect previously visible,
exact current-game target/transition behavior not asserted from that history.

XInput polling loads system DLLs dynamically; no new bundled dependencies.
ControllerAimButton choices LT..Y share existing hold state with physical mouse
input. Trigger threshold30, up to four slots, releases on disconnect. Button
identity contributes to the release gate. F10 enables both inputs; Ctrl+F11
cycles controller choice with persistence; native bindings are not intercepted.

## October 16 - global commands / native settings0.27

Preserve action identities/custom keys; append GlobalHotkeys Ctrl+F10. Overlay
edge detection treats the main menu as eligible when global commands are ON,
but retains editor/capture/focus/release gates. Persist Controls.GlobalHotkeys
only after a successful profile write; default ON for missing keys.

Native FOV shortcuts now save Display.NativeFOVSlider0..100, leaving legacy
Display.FOV untouched. Native helper apply-slider changes only FieldOfView0..1
before launch, using the same locked atomic transaction with a separate immutable
slider backup. Runtime reads native config for initial feedback only. No native
cap/projection/ADS mutation. HUD uses retained next-launch UI ceiling handling.
Green source remains historical but is not included, installed or hooked by0.27.

## October 16 - player green evidence and limited lens feasibility gate

Player screenshots show revolver23, hands/arms24 and sleeve25, plus broad67 and
likely HUD-related68. These are session-local identities;192/192 entries means
coverage is incomplete. The player-visible mask works for these shown parts.
Offline source check confirms no isolated native RGB layer or clean underlying
world image is captured. Green replay is not lens-ready segmentation. Exclude
a masked final-image warp and unqualified original-PS single-target replay.
No lens build/install/launch or new observation. See LENS-PROTOTYPE-GATE.md for
the exact blocker and success criterion, without claiming engine impossibility.

## October 16 - separate green mask prototype

User now authorizes opt-in visual selection. Four immediate draw variants replay
selected VS/PS pairs into a private full-resolution RGBA mask with depth disabled.
Original draws execute first. The mask is composited through the existing ImGui
backend. PS, native targets/depth, blend factors/mask and stencil reference are
restored. Shader pairs are grouped by blend/depth-write state; ownership is UNKNOWN.
No native FOV, bone data or shader constants change. No file logging of draws.

An initial fixture exposed runtime rewriting of context table entries; that
approach was discarded. The candidate uses bounded MinHook function detours,
retaining separate typed trampolines for up to eight implementations per draw
variant. Present checks changed method addresses while diagnostic is active.
The private offscreen fixture verifies pixels and state; it cannot prove actual
Infinite layer coverage. GS particles and scaled/MSAA/deferred targets are excluded.
See GREEN-DIAGNOSTIC.md for X-ray/alpha/query/performance limitations.

## October 15 - X/base proxy factory comparison

Fresh disk identity matches CE469D44... and installed0.25 ASI48B3CF39.... No
process access/launch or rendering change. Native constructors establish
X component table01619590 over base0159A298. Their125 executable entries differ
only at00 andA8; proxy factory188 is shared00476F10. Its0xE0 allocation calls
007201F0 and produces saved proxy table01599FB8. The A8 override is a small
counter gate forwarding to00B4DA60, not a demonstrated lens facility.

No UDK component class literals appear in ASCII/UTF-16 on disk. Scoped saved
reflection has no mesh FOV member; this is not proof of binary-wide absence.
The direct UDK extension is not a usable INI/SetFOV shortcut on this route.
Retain primary00C28A10 ->00B188D0 for further offline work, not the old decal
7D8F90/44B240 path. Mesh/effect coherence is still unqualified; no functional
build follows from these code checks. See UE3-OFFLINE-APPLICABILITY.md.

## UE3 source comparison and new launch constraint

The player confirms0.25 native slider operation but reports unsatisfactory
proportions. Record the method working, not visual success. No game launch or
relaunch is permitted without asking the user first; the repo AGENTS records
this explicit instruction. Further draw observations are stopped.

Public UDK declarations show a dedicated UDKSkeletalMeshComponent FOV/SetFOV
and specialized scene proxy. UT propagates that FOV to both arms, overlays and
muzzle particles. UDKParticleSystemComponent also overrides its transform for
custom-FOV handling. This makes one inference obsolete: one camera frustum
does not exclude an effective component-specific FOV. Infinite's saved rig
uses XSkeletalMeshComponent; UDK facilities are not automatically available.
See UE3-FIRSTPERSON-FOV.md for primary links, limits and the next offline decision.

## October 14 - native option route instead of observation

0.24.2 game capture:204160 common calls, rows0, commits0, invalid0, started0.
Publication metadata was current/coherent in bounded read-only checks; these
do not isolate the failed owned start gate. Retained TLS metadata contained
an unknown/HUD foreground record, not a qualified owned-rig draw. No corrected
capture or rendering functionality is inferred. The player ends this approach.

0.25 starts from63f87fe. CalcFOV and weapon aim-target hooks are removed; only
the retained speed hook remains in player_options. The native option comment
defines slider0..1 and a percentage of the current base. Historical75 ->82.5
does not prove the complete live law. DefaultEngine's70 is in the editor section.

The helper edits one existing MaxUserFOVOffsetPercent token, with immutable
original backup and atomic checked replacement. FieldOfView remains untouched.
The runtime saves the mod FOV preference and reads native configuration for
feedback, but never writes camera/aim/render state or the native option. The
installer/launcher applies the cap before the game starts, avoiding cached
option overwrite/timing ambiguity. The75 reference only converts the saved
angle preference into a range; actual framing remains native.

## October 14 - native API callsite observer qualification

The separate DrawAudit build now observes common submission RVA3C25B0,
native commit RVA47F740 and seven checked engine API callsites. Mid-function
stubs preserve native argument layout and original CALL/epilogue; no Direct3D
function entrypoint hooks or caller return-slot rewriting are used. Full CPU
state and opaque native RET0C behavior pass own-process comparisons.

The abandoned context-vtable prototype missed real calls in an own-process WARP
fixture because entrypoint pointers changed during state operations. This is
a fixture finding, not attribution for game FOV defects. Native callsites instead
produce actual draw/vertex-shader/cb1 observations and the last full queued
80-byte upload source. The source is not GPU readback. COM getter references
are balanced; upload resource references are released after the bounded capture.
An 8x8 constant-buffer-dependent render produces byte-identical pixels with and
without observation. Ownership, stale-view, nesting, world/HUD exclusion and
bounded marker/timeout checks pass; report schema has 123 columns.

Final diagnostic ASI is EE0A2AAB880DDDFDF115BB925B3439DB08D17BAE0406DA0A519A4D0902B50F54.
No game installation or runtime capture is claimed at packaging. Default build
runtime sections match installed 0.24.1; whole-file hashes differ in metadata.
Installed ASI61523562 remains untouched. Latest configuration checked is
C37882D3ACF974517A0F7E0022BCA8A690FB8945B263683E95B3F68F728C53ED:
world FOV is now110, HoldAim1, NativeAimZoom1; preserve this newer user setting.
Earlier snapshots are historical, not restoration targets.

## October 14 - final unchanged-runtime check

PID23884 is still running. Installed ASI61523562... and loaderD5A059AA...
remain unchanged. Current INI SHA256 is
F34CCAC51492ECC06D66220D5EAEE3E13A42FB7F2403C1D7A83E332BB4F15072.
Compared with the saved installation INI94C231D..., its only line change is
HoldAim0 ->1. Preserve this current preference. NativeAimZoom remains ON.
The log at13:24:00 records a new Hand Cannon entry-target conversion60 ->97.5;
this confirms another adapter invocation, not the ten-second held lens.
Research changes documentation only; runtime source matches checkpoint63f87fe.

## October 14 - user-confirmed controls and visible aiming effect

The player confirms green ON/grey OFF switching and a radical visible aiming
difference. Preserve this command/feedback behavior and the selected optional
policy. They report that ON appears to keep the same FOV and that arms/weapon
and world proportions still look deformed. Do not equate a visible difference
with a measured proportional held lens or a full first-person correction.

Real log13:09:35..13:09:52 records each shortcut request, saved state and UI
label consistently. At13:09:53 the Hand Cannon target is60 ->97.5 for native
hip82.5/world120; sniper14 ->27.262 is also recorded. The fresh23884 session
has PC025CC400/pawn023A0000/camera3126E000. Current read-only snapshot uses54
reads, verifies native viewport3840x2160 and native projection methods, with
unlocked delayed/cache120. Predicted horizontal120/vertical88.507 and the
last cb1 CPU shadow agree, including aspect1.777777775. This is not a proof of
actual GPU consumption/ownership by every world or first-person draw.

No second aspect multiplier or axis stretch is supported by these inputs.
At high physical FOV, rectilinear perspective can itself change apparent
depth/edge proportions; the player's remaining world complaint is not thereby
resolved or dismissed. Independent first-person rendering remains open.
A triggered read-only held-aim capture completed without triggering: 5,986
reads, zero captured samples. The player completed the requested aiming test,
but this file cannot establish its held lens. Timing or the combined
camera/controller trigger gate may explain the missing capture; neither cause
is proven. Do not repeat the same capture or present the requested ten seconds
as measured. A subsequent unlocked snapshot has delayed FOV120 and a retained
LockedFOV113.3414; that retained field is not evidence of the held camera.
No new runtime build is introduced.

## October 14 - primary source comparison and first-person restart

The separate `experiment/firstperson-native` branch starts from retained
checkpoint63f87fe. The ON/OFF control and player-reported visible aiming effect
remain intact; their confirmation is narrower than exact proportional zoom.
The functional objective and required ownership/preparation/binding evidence
are recorded in [FIRSTPERSON-NATIVE-RESTART](FIRSTPERSON-NATIVE-RESTART.md).

The public Infinite VR source was inspected at immutable commit
41f004bc6a1e4fa8be0d0e0749f9ac5a0453e75e. Its Infinite engine notes report one
gameplay lens in their Steam test (5,645 valid samples), unlike the separate
foreground lens described for BioShock1. Its fixed16:9 horizontal-reference /
Hor+ law agrees with our GOG native-builder evidence. This is corroboration,
not proof of every draw in our GOG session or a ready independent-lens patch.
Its hands implementation drives bone atoms; effects have another origin lane,
including documented unsolved frozen-position cases. No source or addresses
are transplanted into InfiniteMod. A pistol-only scale result is not universal
reload/effect validation and does not meet our whole-first-person requirement.
See the immutable references in FIRSTPERSON-NATIVE-RESTART.

A bounded selected-module inventory finds one InfiniteMod ASI and Windows
SysWOW64 DXGI/D3D11 modules; no other ASI or ReShade appears in that inventory
or the matching Binaries file search. This does not exclude every possible
overlay/manual-mapped injector and does not attribute the visual complaint to
a stock module. Raw enumeration count141 includes duplicate EXE entries.

These checks exclude a justified extra aspect multiplier, a BioShock1/2
foreground-field transplant, and copying the VR bone implementation as the
next fix. The unresolved requirement is a coherent owned first-person
preparation/submission path, with world/HUD exclusion and actual bound-state
evidence. Another broad DPG2 or final-buffer substitution is not promoted.

## October 14 - distinguish UI state from camera behavior

The player clarifies that the line remains NATIVE after the shortcut. Current
ASI is independently A70E72..., current INI NativeAimZoom=1 and previous target
hook activity prove activation at those observations, not the contemporaneous
on-screen text. Source reads one shared atomic flag for the aim row and adapter;
the generic status nearby also says Native camera retained. That wording is a
possible ambiguity, not a demonstrated cause. No COFF symbols are available for
the prepared external global reader, so no live scalar value is claimed from it.

0.24.1 separates feedback from lens changes: explicit ON/OFF row and logs,
same native policy, and actual production persistence exercised by the menu
fixture. The earlier always-successful test stub did not cover disk failure;
successful key polling alone could not establish the saved/live/rendered state
contract. Player confirmation remains necessary before proceeding to held FOV.

## October 14 - 0.24 player test does not establish a visual fix

The player reports that aiming appears to stay native regardless of the mode.
This is a failed/unconfirmed functional test; 0.24 is not retained or validated.
The latest complete INI contains NativeAimZoom=1. Runtime log shows the qualified
target hook actively returning97.5 for Hand Cannon native60/hip82.5/world120,
and27.262 for sniper native14. A later Hand Cannon observation has hip90 and
target90. These prove option activation and target conversion in those calls,
not that the eventual rendered camera follows that target. No additional
runtime build is justified solely by the existing getter/ABI fixtures.

Current controller/camera changed during the session: PID11968, PC31497000,
camera330BE700. A bounded QUERY_READ snapshot verifies current ownership,
unlocked flags8/LockedFOV0/FrameDelayed120 and active sniper target14. This
unzoomed snapshot cannot establish the held-aim result. Older addresses are
stale. A trigger-based, read-only capture is prepared to record actual held
camera values and per-sample weapon identities, avoiding the earlier fixed
start-window and weapon-switch attribution limitations. Clarification is
pending whether the menu label stays NATIVE or shows PROPORTIONAL with no
visible change. No new fix, camera override or first-person success is claimed.

A fresh exact direct-call scan also finds99C55D in addition to the three camera
consumers. Its bounded caller review uses actor vectors, angle comparison and
boolean return; it is not qualified as a camera writer. Do not broaden the
adapter to that angular predicate as a speculative zoom fix. Runtime source
remains unchanged while the final-camera test is pending.

## October 14 - native authored target requalified for 0.24

Accepted-runtime read-only capture collected 476 samples, including only ten
locked samples. It does not establish a ten-second held aim. Weapon targets
were sampled once; a weapon change means their initial 65 cannot be correlated
with every later camera value. A selected player/camera-mode script-reference
search reviewed 1506 functions with zero hits, a negative limited to that scope.

Rechecking NOTES-v0121 identifies the native getter at 508380. Fresh supported
EXE disassembly confirms no-argument thiscall/x87 return, ordinary/TBar fields,
and three qualified consumers. 51DB17 stores the target directly; 58296C and
5829FE blend it with camera POV into LockedFOV. Fresh QUERY_READ confirms
OwningPawn D88 equals the current pawn and target fields are scalar floats.
This qualifies a target-return experiment without intercepting generic VM
execution, scaling interpolated values, writing authored weapon fields or
building per-weapon profiles. Details and boundaries: [BUILD-v024](BUILD-v024.md).

Horizontal, vertical and diagonal FOV are interchangeable angle conventions
for one rectilinear frustum. Equivalent conversion does not change perspective;
independent H/V values must respect viewport aspect to keep square pixels.
The accepted native Hor+ path must not receive another aspect multiplier.
Viewmodel placement remains a separate, unqualified native-rendering goal.

## October 14 - actual aiming route rejects0.23

Player ON/OFF comparison has no visible difference. Real logs identify native
LockedFOV during normal aiming; the candidate's CalcFOV policy does not control
that source. This is a coverage failure, not evidence that the optical equation
or compiled ABI fixture establishes a game correction. Qualified202-read
snapshot verifies scripted camera SetFOV(NewFOV), one input float and generic
interpreter9D9FA0 at UFunction+8C. An unlocked snapshot retains LockedFOV99.848518
with delayed120, while active Hand Cannon target is60 and native hip is82.5.
The source/interpolation formula remains unqualified; blindly converting that
locked value risks expanding the view. Target the native zoom input before
interpolation. Planned24-second capture did not run after game closure.
Accepted English0.18.1 is restored with the complete current FDB4CB... INI,
custom keys and compatible loader preserved. All earlier game addresses are
stale. See [the actual verdict/rollback](REJECTED-v023.md).

## October 14 - native aiming 0.23 objective

The accepted CalcFOV adapter receives an unmodified native result but passes
it through during aiming. The optional new policy composes native optical
magnification with the requested hip-fire tangent at that same camera callsite.
It never derives its native baseline from the modified cache and discards it
on lost gameplay/camera identity. Missing settings default OFF; Ctrl+F9 is an
appended 24th editable action. Native world/aspect behavior is unchanged.
Offline GOG source review confirms camera interpolation at D38CA0 feeds this
callsite; AdvanceFOV separately selects LockedFOV when camera+20C bit0 is set.
That locked route is explicitly bypassed, not advertised as controlled.
No scene shader, render buffer or first-person transform changes are introduced.
The visible aiming result and the independent first-person problem remain
unconfirmed. See [BUILD-v023.md](BUILD-v023.md).

Final ASI67B8E723113C969C0B55496031B23646DBD2C77F92ACFA79C3DC502244B490F3
is compiled and checked. The retained suite passes, including2000 native x87
trampoline returns under proportional zoom, optical magnification invariance,
baseline recovery/reset, unrelated/locked-camera bypass and OFF return. Controls
and real graphics/menu/state/resize checks pass at720p/4K/ultrawide; the720p
image is inspected. Six private install/rollback scenarios pass. No game
visual result or first-person correction follows from these checks alone.

ZIP80BC7623FA3DC0B67971CEF6EA0629D278C7B787E22AC9DBC128D0A3B997A626 has23
verified root entries. It was installed from the checked extracted stage while
closed; independent hashes and the transaction verify only ASI replacement,
with accepted B13DBE... backup and unchanged complete EBE952... INI/D5A059... loader.
The game was relaunched PID16896; all older session addresses are stale.
The player ON/OFF result remains pending. See [delivery](DELIVERY-v023.md).

## October 14 - return to native camera work

The player rejected Panini as the solution despite real-game submission.
Process inventory confirmed closure; checked rollback and independent hashes
restore accepted English 0.18.1. The updated Panini INI is recoverable.
The accepted CalcFOV adapter sets the requested hip-fire lens at its qualified
camera callsite, but returns the unmodified native lens while aiming. Thus it
does not implement the previously requested proportional aiming zoom. This is
a code fact; it does not identify the cause of all first-person visual defects.
The next native experiment must not inherit scene-composite or GPU/ADS layers
from rejected builds. See [the recorded verdict](REJECTED-v022.md).

## October13 - stronger Panini candidate

The player selected distance1 from a numerical comparison at native H100.
The candidate uses the primary view's actual pure projection and a separate
fullscreen composite after native scene-render6E6F40, with viewport/actor/
single-view guards. This is a new scene boundary experiment; native HUD ordering
and actual game output remain to be confirmed. Native camera/projection/bone/
constant-buffer data are not changed. SwapDeviceContextState restores the
complete compatible D3D11 state; feature level/creation threading flags match
the existing device. ClearState applies only to the temporary state before
restoration so it cannot retain the back buffer. Real WARP/state/byte tests
and swapchain resize checks pass. First-person placement remains separate.
See [the exact objective, limits and test](BUILD-v022.md).

Final source is 1c28bb1d5a92d91ac5627c01776db353c1a97b85; checked ASI
69A860A631BBA67C438EAD9A8111C2499CBD2155C7A9C612E5E2D670662082B9.
The final root-layout ZIP and all six installation/rollback scenarios passed.
Normal closure was confirmed before installation. The transaction and
independent hashes verify only the ASI was replaced, with accepted B13DBE...
backed up and the complete INI/loader retained. Fresh 0.22 startup reports the
scene adapter, native 3840x2160 swapchain, and no keybind conflicts. Panini
initializes OFF because the retained INI has no WorldProjection key. This is
installation/startup evidence, not a submitted composite or visual success.
The player comparison remains pending; see [delivery receipt](DELIVERY-v022.md).

During that comparison, real-game composite submission is logged at native
horizontal100 and120 (16:55-16:56). The eligible native scene boundary is reached
in this session. Player visual/HUD/aiming/window-transition confirmation remains
pending; submission alone does not establish visual success.

## October 13, 2026 - current FOV boundary

The restarted accepted session still looks deformed to the player at 120.
The current native viewport is 3840x2160 with width/height methods F32420 /
F9B3D0, aspect method A3C340 and projection method 46DCA0. Static builder
conversion is unconditional Hor+ from a horizontal 16:9 reference, before
optional view-rect constraint. Current camera constraint bit is clear.
The accepted camera caches 120 and a read native cb1 shadow agrees with
H120/V88.507/aspect16:9 under rigid-view decoding. CPU shadow is not actual
bound GPU state or draw ownership. Therefore do not introduce another aspect
correction; no source/runtime modification has been made. The player also
reports stretching at 100, and the log confirms that change. The player then
selected 80. A subsequent reader rejected stale camera identity; no matrix
at either lower value is claimed. Obtain a visual example before another build.
See [the current contract](FOV-RESTART.md).

0.21 is **REJECTED**. Its fixtures and native adapter checks passed, but the
player observed severe world/character distortion. An attached-effect GPU
transaction was logged; a restored hands/gun transaction was not established.
Possible ADS double correction and GPU-state leakage remain hypotheses,
without demonstrated runtime causality. See [REJECTED-v021.md](REJECTED-v021.md).

Accepted English 0.18.1 is restored, with ASI/INI/loader hashes verified.
`experiment/physical-fov` starts from its accepted source, without the rejected
GPU/ADS layers. Establish the actual pure world projection and viewport aspect
before modifying angles. Preserve existing saved FOV semantics and all
accepted controls. [FOV-RESTART.md](FOV-RESTART.md) defines the objective and
success criterion; it is not an implemented or visually validated fix.

Latest delivery: v0.18.1 English is installed and published. English main and
shortcut menus plus preserved opening keys are player-confirmed. The public
ZIP and checksum were downloaded and verified, including every entry and
inner ASI; see [DELIVERY-v0181.md](DELIVERY-v0181.md). Earlier pending
checkpoints below are retained as history and superseded by that record.

## Scope and base

x86 ASI mod for the GOG executable checked by SHA256:
`CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A`.
Compatibility with other executables is not established. The project does not
distribute the game executable. Native signatures are checked before adapters
are installed; an unknown schema rejects the command.

The current accepted controls base is **0.18.0**, ASI SHA256
`61F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2`.
Its F1 vitals refill and shortcut reassignment were confirmed by the player,
who also confirmed the restored base was correct except for weapon/hand FOV.
The accepted source checkpoint is
`b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`. Restart persistence and every
remapped action have not been separately player-confirmed.

The original user-validated predecessor is 0.15:
`0901F21DBD06CD854EE8A4D3DAABFD03D2E0A1F2039F7BEED483A165A149B1EF`.
The stable/v0.15 checkpoint contains recovered and checked sources, not a
promise of binary identity with that ASI. Rejected FOV experiments remain in
experiment/fov-v0.17.1. The accepted 0.18 controls step keeps the 0.15 camera
policy. English localization starts from 0.18, without rejected FOV hooks.

## Preserved architecture

The overlay attaches to the factory and swapchain actually created by the
game. The D3D11 device and context used to display the menu belong to that
swapchain. Graphics state is restored, and the menu does not pause the game.
Commands require the game to be in the foreground.

Gameplay requests are queued from the overlay, then executed on the game
thread through the existing camera connection. The queue checks the requesting
character, playable context, and a two-second expiry. Operations use native
functions and then verify their effects. Acquisitions may subsequently be
retained by normal game saves; no items are granted automatically at launch.

External diagnostic tools are read-only. Reading an inventory or matrix
does not validate the rendered result.

## Retained features and decisions

- **Intros**: the four logo StartupMovies entries are disabled through
  configuration; LoopingMovieUntilStartupFinishes remains active. The player
  accepted and validated automatic skipping. Manual-skipping experiments
  did not achieve the requested result and are not presented as active.
- **Weapons**: the eight selectable main weapons use native functions.
  Active and secondary slots are read again after execution. Magazine and
  reserve are filled through their two separate operations.
- **Progression**: eight Vigors, 49 recognized fixed gear definitions,
  48 upgrades for the twelve weapons/variants, and 16 Vigor upgrades.
  Complete identity checks and no duplicates. Recognized promotional
  variants replace their standard equivalents; unused variants are not
  additional items to invent.
- **Resources**: money and lockpicks use the caps read from the character;
  observed caps are 1,000,000 and 30. Legitimate values above a cap are not
  reduced.
- **Infusions**: three recognized tonics, ten upgrades per gauge. Inventory,
  attribute gains, money, and character identity are checked. The initial
  shield must already have been obtained normally.
- **Crosshair**: native visibility of the two GFx widgets is used.
  IsShown=1, IsInCombat=2, and IsCenterpointVisible=4 masks were compared
  with actual objects. The player validated hiding on 0.15.
- **HUD**: native sizing through MaxResolutionUpScale, a maximum fractional
  increase rather than an inverse multiplier. HUDScale 1/1.25/1.5/2 maps
  to 0/0.25/0.5/1 in XUI.ini. Changes apply on the next launch. The game
  handles aspect ratio; the mod does not resize Vigor effects separately.
- **Aiming**: XStartZoom and XStopZoom on the game thread. The selected aim
  key must match the game's binding. Releasing the key, opening the menu,
  or losing foreground focus ends hold-to-aim.
- **Speed**: adapts the local player's walking/running and sprint attribute
  results. x1 restores the native result; no multiplier is persistently
  written into the character.
- **0.15 FOV**: world setting 80â€“130 at the local player's camera calculation.
  Aiming transitions keep their native calculation. First-person model FOV
  and zoom at high FOV are known limitations.

## 0.18 â€” configurable shortcuts

One keybinds::Engine instance owns the configuration and key-edge history.
Each action is identified independently of its key. Modifiers match exactly:
Ctrl+F1 does not also trigger F1. Holding a key does not repeat the operation;
regaining foreground focus with a key still held does not launch a command.

The 23 actions include both menu-opening shortcuts, all grants, and existing
settings. The Ctrl+F8 menu displays eight entries per page; Up/Down selects,
Enter begins capture, and Escape cancels/goes back. The main menu uses Insert
by default. Both menu-opening shortcuts can be reassigned.

Capture begins after key release and suspends mod commands. An already-used
shortcut is rejected. Keyboard and keypad +/- aliases are included in conflict
checks while the default FOV setting is retained. A custom reassignment does
not add a hidden alias. Writing [Keybinds] must succeed before the active
configuration is replaced; on failure, the old shortcut remains usable.

The shortcut menu can open without opening the main menu. Grant and setting
commands keep their conditions: main menu open and game in the foreground.
Mod shortcuts do not change the game's internal bindings.

## 0.18 â€” refill current gauge values

F1 fills health, shield, and salts up to their current capacities. It does
not replace the Ctrl+F5 infusion command, which increases capacities. It does
not revive a dead character or unlock the initial shield.

Native GOG entry points checked by signature and stack cleanup:

| Operation | RVA | ABI and expected result |
| --- | --- | --- |
| Health | 3554D0 | thiscall(pawn, float), ret4; native setter with cap and HUD notifications |
| Shield | 5EDDF0 | thiscall(pawn, float, int), ret8; final argument zero as in the native refill |
| Salts | Reflected ClientAdjustCurrency function | selector 4, delta toward current cap; VigorEnergySizeAttrib capacity |

Health/XHealth/XHealthMax/XShield/XShieldMax properties and the character
definition are checked before calls. Capacity is not written. A zero shield
cap is retained; a value already above its capacity is not reduced.
After calls, the mod reads all three values, their capacities, and character
identity again. A partial or unconfirmed effect keeps a warning instead of
a success message.

F1 current-value refill is **USER-VALIDATED** on 0.18. The English 0.18.1 binary
preserves that implementation. Its English menus and custom menu keys have
been player-confirmed; a separate F1 test on that binary was not reported.

## Why the FOV experiments are not retained

Builds 0.16â€“0.17.1 passed mathematics, ABI, or loading checks without achieving
a satisfactory visual result. Weapons, reload parts, and firing effects were
not all aligned. Player feedback and videos take precedence over â€œhook readyâ€
messages or synthetic tests.

Earlier reads identified a DPG2 foreground route and a
TranslatedViewProjection matrix supplied in passes 87C020 and C11DC0.
Callbacks for the five inspected flash materials did not demonstrate an
independent projection. Floating-section matrices are world transforms.
These observations do not establish the projection consumed by every primary
weapon/hand draw, nor do they prove a separate first-person camera.

The goal remains **an intact world view and a separate complete first-person
view**, with consistent coordinates for hands, weapons, accessories, and FX.
DPG2 also contains HUD Vigor effects; selecting an entire render group is
insufficient to establish that complete first-person boundary.

Before another visual trial, a bounded draw-time capture must connect DPG,
material, mesh, anchors, and the projection actually consumed, at 80 then 120.
It must distinguish a projection difference from an effect origin that is
already incorrect. An end-of-frame read is insufficient. If origins differ,
the fix must target their generic calculation; per-weapon offsets must not
hide the problem.

Boundary C5F880 is cdecl with seven arguments, view in argument1 and mesh in
argument4. Coverage of all opaque meshes is not yet established.
FSceneView copying involves owned containers: a raw copy must not be treated
as an owning engine object.

The 0.19 diagnostics did not deliver a functional FOV correction. The 0.20
experiment changed foreground view fields around preparation and drawing,
but the player reported HUD Vigor-effect flicker and no weapon correction.
It is **REJECTED**, including its unconfirmed aiming changes. The accepted
0.18 ASI was restored; the player confirmed that the other features were
correct again. Synthetic matrix and ABI checks did not establish visual
success and do not justify retaining those hooks.

A subsequent code-path review distinguished the main skinned draw from the
decal path that earlier vertex-factory reads followed. On the inspected
player components, proxy virtual slot 0x08 enters `C28A10` for the main draw;
slot 0x1C enters `7D8F90` for decals. The primary route reaches mesh-object
virtual slot 0x24 (`A5D130`) and selects the LOD+0x2C factory before submission
through `7C25B0`, with caller `B18B22`. The decal route's factory wrappers
are not evidence for the main weapon/hand shader. This refines the next
investigation target; it is not a new FOV feature or proof of a working fix.

Future criterion: at world FOV 80/110/120/130, every weapon, hand, part, and
effect remains proportionate and aligned during firing/reloading/aiming/weapon
switching; aspect ratio and world objects remain correct.
Expected proportional world zoom:
tan(worldADS/2)=tan(worldHip/2)*tan(nativeADS/2)/tan(nativeHip/2).
The formula does not replace visual validation.

## Publication and reversibility

The stable branch receives only changes retained after validation.
Test builds remain experimental. Before replacement: close the game and
copy the existing ASI, INI, and files needed for rollback. Preserve FOV,
HUD, speed, aiming, and intro preferences. The new mapping is an expressly
requested change.

A delivered ZIP does not contain game files, saves, or captures. It is
extracted to the location specified by its README, with ASI/INI/README and
the required loader, without an unnecessary enclosing folder. Internal
names, hashes, branches, commits, and link availability must be checked
before publishing a release. The shortcut build includes none of the
projection hooks from the rejected experiments.
## Installation checkpoint - October 11, 2026

The controlled v0.18 ASI was installed at08:17 CEST, after a normal game shutdown.
The transaction and backup hashes were verified; the existing mod INI, loader, and game EXE were unchanged.
The initial local installation ZIP was checked for its16 entries and exact payload hashes.
The game was relaunched; its log confirms v0.18, ready command/aim/camera hooks, a3840x2160 swap chain,
and keybind loading with zero invalid entries or conflicts. These observations are not user validation.
F1 on partially empty gauges, actual menu operation, and persistent remapping await the player.
Six isolated installer/rollback scenarios passed, including custom INI preservation and shared-loader handling.
The public release ZIP and final remote commit IDs are recorded separately once they are verified.

## Public-source build check

The isolated publication checkout built successfully with `dist/` absent, using official Zig0.13.0.
The build created the ASI and default INI without requiring game assets or changing any source or Git ref.
The freshly linked output was SHA256160B010A9899DEA3B47BE162D3F735A16BEFB506A2A545059F5F9CD6226799FF.
It is a separate reproducibility check, not the packaged/installed binary. The installable package pins
SHA25661F2C81E8E737F378B5BA7825249F517C59AACB8738DF5B7885FA6A583F86BA2 from the fully tested build.
Compiler/dependency warnings appeared while populating the fresh compiler cache; the build exited0.

## Player confirmation - October 11, 2026

The player confirmed that current-value vitals refill and shortcut reassignment work in-game
(translated: "Vitals refill and shortcuts OK"). These two features
are USER-VALIDATED. A restart-persistence test and every remapped action were not separately reported;
those remain distinguished from the integrated routing/INI-reload checks. Weapon/hand FOV remains unresolved.
The accepted controls build can serve as the next retained base without importing rejected FOV hooks.

## English localization checkpoint â€” October 12, 2026

Historical build checkpoint; its pending installation/player statements are
superseded by the installation and confirmation checkpoint below.

0.18.1 uses the accepted 0.18 controls source. Runtime changes are limited
to text, the version label, and matching fixture expectations. Native wide
identifiers, format specifiers, INI keys, all 23 action identities, default
bindings, formulas, and guards remain unchanged. Existing custom bindings
and the user's complete INI must be preserved by installation.

The checked x86 ASI SHA256 is
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
The full retained suite passed, including command safety checks, rebinding,
INI persistence, and the module's own D3D11 menu fixture at 720p/4K/ultrawide.
The 720p main and shortcut menu images were inspected. Status is **BUILT**,
**CHECKED**, **EXPERIMENTAL** pending installation, delivery verification,
and the player's result. These checks supersede the earlier preparation-only
state, without changing earlier game-session evidence.

Six isolated installer/rollback scenarios also passed using the English
rollback entry. They cover custom INI preservation, a rollback copy of later
INI edits, unknown-loader and altered-package rejection, first-installation
cleanup, shared-loader preservation, and refusal to replace a newer ASI.
These fixture checks do not claim an installation in the player's game.

The runtime, project documentation, and delivery text are now intended to be
English. `Rollback.ps1` is the English rollback entry; `Retour.ps1` remains
available for compatibility. English localization does not change native
game text or supply an independent weapon/hand FOV fix. The next optical
experiment stays separate and must cover complete first-person rendering
and proportional aiming rather than per-weapon offsets.

## English installation and source checkpoint â€” October 12, 2026

The local ZIP is immutable at SHA256
`1853FEF39BAFE9CCC6739B1E66A30A959E44999E86A956A4C82BF831B9EAA545`.
All 18 entries and payload hashes were checked; the ASI matches the built
`B13DBE911E2A650A7A9308575214A0C9129DF1AEBD653CA5B11121E3F13EAFD9`.
The root layout includes scripts, docs, licenses, INI, ASI and loader, without
an enclosing folder or private game data. Its documentation preserves the
earlier pre-installation checkpoint. Current delivery records and eventual
release notes supersede its pending-status statements without repackaging.

The installed transaction followed normal game closure and a verified backup
on October 12. The complete existing custom INI, loader and executable kept
their exact hashes. The player confirmed readable English main and shortcut
menus with the custom menu-opening keys preserved. This is **USER-VALIDATED**
for those menus and keys only; every gameplay command and restart persistence
were not separately reported.

The English source published on `main` is
`cd0873936a92f0a0e2c00dca88557c7724b7b1a6`, with tree
`cca6c50943f84d2c88b1f8eeebcfd2b3e92b0c35`, identical to the checked local
build snapshot `eab12be926efbfb286051087e95749138d0fb067`. The stable controls
base remains `b54167ef6a0ed254b4ef9d52319f9be4e886e5fc`. A new public release
and its downloaded ZIP still need verification. Actual installation/source
evidence is recorded in [DELIVERY-v0181.md](DELIVERY-v0181.md).

FOV research is paused at the user's request. Independent complete first-person
FOV and proportional aiming remain unfinished. No 0.19 diagnostic or rejected
0.20 projection/aiming hooks are included in this localization.

October25 delivery update: 0.30 native position candidate BUILT / CHECKED / INSTALLED on P: with exact0.29 rollback; personal INI and loader unchanged. Gameplay unvalidated and game not launched. See DELIVERY-v030.md under docs/.
