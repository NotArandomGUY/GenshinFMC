#include <stdio.h>
#include <windows.h>
#include <psapi.h>

HWND hPrevFgWnd = NULL;
bool bWasPlayingGenshin = false;

bool isPlayingGenshin() {
  // Get foreground window handle
  HWND hFgWnd = GetForegroundWindow();

  // Check if foreground window changed, if not, use cached result
  if (hFgWnd == hPrevFgWnd) return bWasPlayingGenshin;

  // Cache foreground window handle
  hPrevFgWnd = hFgWnd;

  if (hFgWnd == NULL) {
    // Failed to get foreground window handle, probably not playing
    printf("Failed to get window handle, error: %d\n", GetLastError());
    bWasPlayingGenshin = false;
    return false;
  }

  // Get foreground window process id
  DWORD dwProcId = 0;

  if (GetWindowThreadProcessId(hFgWnd, &dwProcId) == 0) {
    // Failed to get process id of foreground window, probably not playing
    printf("Failed to get process id, error: %d\n", GetLastError());
    bWasPlayingGenshin = false;
    return false;
  }

  // Get process handle of foreground window
  HANDLE hFgProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcId);

  if (hFgProc == NULL) {
    // Failed to get process handle of foreground window, probably not playing
    printf("Failed to get process handle, error: %d\n", GetLastError());
    bWasPlayingGenshin = false;
    return false;
  }

  // Get foreground window process name
  CHAR lpFilename[MAX_PATH] = { 0 };

  GetModuleFileNameExA(hFgProc, NULL, lpFilename, MAX_PATH);

  // Close foreground window process handle
  CloseHandle(hFgProc);

  // Check if filename match genshin
  if (strstr(lpFilename, "GenshinImpact.exe") || strstr(lpFilename, "YuanShen.exe")) {
    if (!bWasPlayingGenshin) printf("Focused on game window, freeing cursor...\n");
    bWasPlayingGenshin = true;
    return true;
  }
  else {
    if (bWasPlayingGenshin) printf("No longer focused on game window, stop freeing cursor.\n");
    bWasPlayingGenshin = false;
    return false;
  }
}

void loop() {
  // Check if current window is genshin
  if (!isPlayingGenshin()) return;

  // Force reset cursor state
  ClipCursor(NULL);
}

int main() {
  printf("Genshin, Free My Cursor!!!\n");
  printf("Ready!\n");

  while (true) {
    loop();
    Sleep(500);
  }

  return 0;
}
