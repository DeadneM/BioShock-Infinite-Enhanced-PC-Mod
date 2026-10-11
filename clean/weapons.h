#pragma once
#include <windows.h>
struct WeaponChoice {const char *label;const wchar_t *asset;};
extern const WeaponChoice weaponChoices[];
extern const int weaponChoiceCount;
bool installWeapons(BYTE *image);
void weaponWindow(HWND window);
bool requestWeapon(int index,int destination=0);
bool requestAmmo();
bool requestVitals();
enum class ProgressionAction { Powers, Gear, WeaponUpgrades, Money, Lockpicks, VigorUpgrades, Infusions, Count };
bool requestProgression(ProgressionAction action);
void weaponStatus(char *text,size_t capacity);
void playerOptionsStatus(char *text,size_t capacity);
LONG crosshairStatus();
