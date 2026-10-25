# Native first-person position candidate - October 25, 2026

Scope: offline discovery only, requested by the player. No runtime change, build, launch or external process reading.

PROVEN from saved reflection metadata: XHuman.FirstPersonAttachment ObjectProperty +0x1D38. XFirstPersonAttachment.PlayerViewOffset StructProperty +0x200, size12 (three-component vector). These are metadata identities, not reusable heap addresses.

PROVEN in the current on-disk executable: native placement function VA 0x00B7C850 / RVA 0x0077C850 (image base0x00400000), thiscall with two output pointers, ret8. At VA0xB7CA4E/68/8E it reads this+0x208/0x200/0x204. It transforms the vector by a rotation matrix and adds the result to the output position at VA0xB7CAB4..0xB7CB0B. It does not modify projection or scale in this tail. A saved vtable comparison associates this function with the first-person attachment placement slot0x2C8; the saved caller at0xBE1706 invokes it before further placement logic.

INFERRED: changing this offset is a native camera-relative positional route, preferable to per-mesh projection compensation. Local X is the likely forward component; confirm basis sign before choosing a backward displacement.

UNKNOWN: whether all arms, current weapon, reload props and transient muzzle/particle effects inherit this root in every animation, and whether later placement logic overrides it in ADS. A property name and static arithmetic do not establish complete runtime coverage. Do not promise alignment before player validation.

Next experiment: isolate this field on the owned first-person root, small bounded displacement, default0, restore original on disable/ownership change, no cumulative writes, no archetype/global default changes. First prove owner and update path; preserve ADS alignment by disabling offset in ADS initially if required. Test a single revolver with normal/reload/fire/aim before extending. Do not add per-weapon exceptions or renderer hooks.

Installed0.29 sensitivity candidate remains untouched. No FOV correction reintroduced.
Current EXE SHA256: CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A

