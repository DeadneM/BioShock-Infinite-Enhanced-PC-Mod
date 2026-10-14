# 0.24 native authored aim target

Base: user-accepted English 0.18.1, source `99a75aa9e1a47543c418b2dc37b43a8883898c72`.
Branch: `experiment/native-zoom-target`. Status: BUILT, CHECKED, EXPERIMENTAL;
INSTALLED; not user-validated. See [installation receipt](DELIVERY-v024.md).

Player test: reports no apparent effect, aiming stays native. Functional result
is failed/unconfirmed and this candidate is not retained. Runtime option and
target conversion are observed; final held-camera/rendering behavior still
needs qualification. See the October14 notebook follow-up.

## One objective and success criterion

Keep authored zoom magnification relative to the selected world FOV by adapting
the native weapon target before the engine's entry/release blend. At world 120,
ON must visibly soften normal aiming, OFF must restore native aiming, and
release and weapon changes must recover correctly. This does not fix the
independent arms/weapon proportions or placement problem.

The accepted hip-fire camera policy, native aspect handling, HUD and power effect,
crosshair, intros, vitals, ammo, grants and all 23 existing actions are retained.
One appended, editable menu action uses Ctrl+F9. The new INI key is
`Display.NativeAimZoom=0`; rejected `ProportionalAim` preferences cannot enable it.
Full existing settings and custom bindings must be preserved on installation.

## Qualified route

Supported executable SHA256 CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A.
Native getter VA 508380 is a no-argument thiscall returning an x87 float.
Its ordinary/TBar branch reads weapon fields 848/84C. Native camera consumers
at 51DB17, 58296C and 5829FE lock or interpolate that target into LockedFOV.
The latter two use the current camera POV as the other endpoint. Converting
that endpoint again would compound the world correction; this candidate only
converts the authored target returned by the getter.
While enabled, the hip endpoint in CameraCache remains the requested world
FOV throughout entry/release; the engine's separate LockedFOV still owns the
rendered aiming lens. Otherwise accepted CalcFOV zoom pass-through would reset
that endpoint to native hip during the blend. OFF retains accepted behavior.

Live QUERY_READ verification on accepted 0.18.1 confirms OwningPawn at D88,
ordinary/TBar FloatProperty offsets and the active Hand Cannon's 60/60 target.
The policy obtains native hip from the original, unlocked accepted CalcFOV
result, not from the camera cache. It checks caller, current active weapon,
ownership, local alive player, game thread, cinematic state, schema and camera.
No saved weapon/camera property is written by the adapter. Missing provenance
returns the original target. OFF keeps the original target at every caller.

For native target A, native hip H and requested hip D, the new target is
`2 atan(tan(A/2) tan(D/2) / tan(H/2))`. With H=82.5, A=60 and D=120,
the target is 97.5 degrees. The engine still owns animation timing and blends.

## Checks completed and evidence still required

- The original ordinary/TBar getter bytes execute through the real hook and
  trampoline. 2000 native x87 entry/release blends pass, including the requested
  world endpoint, missing baseline, wrong caller/owner/active weapon/thread,
  cinematic state, OFF return and hook removal. Weapon target fields stay intact.
- Optical target ratios pass for world80/100/120/130 and several authored targets.
- The retained gameplay/configuration/ABI suite passes. Main and shortcut menus
  fit720p/4K/ultrawide and preserve DX11 state. The720p main image was inspected.
- All24 actions remain reachable; Ctrl+F9 toggles only aiming, plainF9 only HUD.
- Final x86 ASI compilation and test compilation finish without warnings/errors.
- ASI SHA256 `A70E724FDB14E7FC3A4ED755A191B7655B944E3E0D9BB94250ACD6260F178E19`.
  Private receipts: tools/v024-tests-build.log, v024-tests.log, v024-build.log.
- The24-entry root-layout ZIP and extracted payloads are checked. All six
  installer/rollback scenarios pass with the final candidate. Actual installation
  after confirmed closure preserves the complete INI/keys/loader and has a
  verified accepted0.18.1 ASI backup. See DELIVERY-v024 for exact hashes/paths.
- Obtain the player's visual ON/OFF, recovery and weapon-change verdict.

No in-game correction or public release is claimed. The installed candidate
needs the player's aiming ON/OFF comparison before any retention decision.
