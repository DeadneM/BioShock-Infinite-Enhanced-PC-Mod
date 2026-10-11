#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Resolves only the verified native methods. This installs no detour.
bool installAim(BYTE *image);
bool aimReady();
// Call each camera update on the game thread. playerReady must validate the
// current controller/pawn pair; never pass a cached controller from a worker.
void updateAim(void *currentPC,HWND window,bool playerReady);
