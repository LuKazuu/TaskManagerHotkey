#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define HOTKEY_ID 1
#define CONFIG_FILE "config.ini"

// Parse hotkey from string (F1–F12 or A–Z)
UINT ParseKeyString(const char* key) {
    if (strlen(key) >= 2 && toupper(key[0]) == 'F') {
        int num = atoi(key + 1);
        if (num >= 1 && num <= 12)
            return VK_F1 + (num - 1);
    } else if (strlen(key) == 1 && isalpha(key[0])) {
        return toupper(key[0]);
    }
    return 0;
}

UINT LoadHotkey(UINT* vk) {
    char keystr[16] = {0};
    GetPrivateProfileString("Hotkey", "key", "F12", keystr, sizeof(keystr), CONFIG_FILE);
    *vk = ParseKeyString(keystr);
    return (*vk != 0);
}

void LaunchOrActivateTaskmgr() {
    HWND hwnd = FindWindow("TaskManagerWindow", NULL);
    if (!hwnd) {
        hwnd = FindWindow(NULL, "Task Manager"); // Fallback by window title
    }

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
    } else {
        WinExec("taskmgr.exe", SW_SHOWNORMAL);
    }
}

int main() {
    UINT vk;
    if (!LoadHotkey(&vk)) {
        printf("Invalid hotkey in config.ini. Use F1–F12 or A–Z.\n");
        return 1;
    }

    if (!RegisterHotKey(NULL, HOTKEY_ID, 0, vk)) {
        printf("Failed to register hotkey.\n");
        return 1;
    }

    printf("Hotkey registered. Press it to launch Task Manager. Ctrl+C to exit.\n");

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            LaunchOrActivateTaskmgr();
        }
    }

    UnregisterHotKey(NULL, HOTKEY_ID);
    return 0;
}
