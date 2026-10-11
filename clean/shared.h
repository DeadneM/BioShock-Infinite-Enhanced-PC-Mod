#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern wchar_t modIni[MAX_PATH], gameIni[MAX_PATH];
extern volatile LONG configWanted, configRequest, configResult;
void modLog(const char *format,...);
bool installGameGraphicsImport(BYTE *image);
bool queueIntroSetting(bool enabled);
extern volatile LONG hudWanted,hudApplied,hudResult,aimWanted,aimKey,modMenuOpen;
extern volatile LONG speedPercent,fovDegrees;
extern volatile LONG crosshairHidden;
bool saveCrosshairSetting(bool hidden);
bool saveHUDSetting(int percent);
bool saveAimSetting(bool enabled);
bool saveAimKey(int key);
bool saveSpeedSetting(int percent);
bool saveFOVSetting(int degrees);
