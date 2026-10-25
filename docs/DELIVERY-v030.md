# 0.30 native position delivery - October 25, 2026

Status: BUILT / LOCAL TESTS CHECKED / EXPERIMENTAL. Gameplay UNVALIDATED.
Base: retained 0.28 gameplay with the existing 0.29 input-profile candidate; no claim of new player validation for either candidate. Branch experiment/firstperson-position.
Objective: increase the native first-person root camera distance; no projection or per-weapon adjustments. Default0, Ctrl+0 cycles0/2/4/6/8/10. Positive local X moves forward, away from the camera. ADS/zoom recovery uses original placement.

Final ASI SHA256: D6081027BA56B6BD08FA568E48F6A2AF8C5A991CE95FD277834E3FF5B680CB02.
Final fixture executable SHA256: D8D637E5B44187B4274B7899C4E24A439BC5A91D9F103D9289EA61B64CC2598C.
Final tests exited0. Tests cover actual MinHook trampoline with native aligned frame/ret8 ABI, signature drift refusal, reflected root/Vector identity, bounded positive distance, restoration/reentrancy, ADS/cinematic/death/thread/ownership guards, persistence and new chord cycling, plus retained gameplay/UI fixtures. These are synthetic own-process tests, not a gameplay test or hardware validation.
On-disk supported EXE SHA256, placement entry/XYZ reads/ret8 and saved first-person-root identity checked. Menu reviewed at720p, automated viewport checks also pass4K and ultrawide. Initial build rebuilt moved compiler dependencies and emitted third-party MinHook/libunwind warnings; final source checks do not treat them as gameplay results.
No game launch or external process memory observation. No publication authorized for this local trial. Package/install results are recorded separately after actual completion.

Retained files include player_options.inl, aim.cpp, reticle.inl and progression logic; no FOV, HUD, recoil or bob/sway behavior change intended. Existing personal settings/bindings must remain byte-identical at installation. Installer must create an exact0.29 backup; reject if current payload has changed unexpectedly.

Player test: keep the same FOV; revolver in hand; Ctrl+0 twice reaches4. Compare against0, then fire/reload/aim and change weapon. Check hands, speedloader, cartridges and muzzle flash. If only part moves, no movement occurs or visual detachment appears, disable with0 and report the menu status. Do not compensate per weapon.

Distribution fixture: PASSED with the exact final ASI. Verified update keeps custom INI, rollback restores prior ASI, altered payload/unknown loader rejected, shared loader retained. Private fixture only; no game launch. Personal INI currently has no Ctrl+0 binding conflict.
