# October13 video and selected projection

## Evidence

Player video: `BioShock Infinite 2026-10-13 13-39-12.mp4`.
SHA256 `8434C0BE7FC07D564F86936486235B0E571F42A92656B6A03072479DE85E0BA4`.
Duration31.84 seconds, recorded2560x1440, square pixels,16:9.
Raw video, screenshots and native snapshots remain private.

The retained log records FOV100 at13:38:22 before this video. The installed ASI
still matches accepted English0.18.1. A later bounded snapshot of the same
session finds native cache/delayed100 and a3840x2160 native viewport. A read
native cb1 shadow agrees with H100/V67.67275/aspect16:9; that is CPU shadow
evidence, not a capture of the video's bound GPU state.

## Visual observations and limits

The NPC has a small apparent size in the distant shot; the near hand/grip
occupies much of the image. Close NPC shots do not show an obvious uniform
axis flattening. This favors investigating perspective and first-person
placement over independently stretching horizontal/vertical projection axes.
The stated5m distance and NPC height are not measured by the video.

Existing80/110 first-person snapshots show unit actor/component scales and
near-orthonormal matrices. The camera also moved between those captures, so
they do not prove a FOV-only placement difference. A fresh current snapshot
shows hand/weapon components owned by the same first-person attachment, with
unit scales. Its PlayerViewOffset field exists and is zero; that alone does
not establish an operative native adjustment. No such field was written.

## Numerical preview and decision

Original, mild base Panini(d0.25) and stronger Panini(d1) were compared on the
same extracted frame at H100. No vertical straightening was used. Their
central magnifications are1.000,1.111 and1.278 respectively, with unchanged
horizontal coverage and different vertical framing. The stronger option was
selected by the player. This validates the preferred preview, not a mod build.

The preview contains the weapon and native HUD. It intentionally demonstrates
that a depth-blind image remap cannot independently resize the grip relative
to a subject at the same image position, reconstruct hidden background, or
preserve the HUD automatically. The next build must qualify its scene boundary
and keep OFF as a direct return. See [BUILD-v022.md](BUILD-v022.md).

References: [Epic's implementation](https://dev.epicgames.com/documentation/en-us/unreal-engine/panini-projection?application_version=4.27),
[Sharpless's projection description](https://hugin.sourceforge.io/docs/manual/The_General_Panini_Projection.html).
Their modern-engine commands are not presumed available in Infinite UE3.
