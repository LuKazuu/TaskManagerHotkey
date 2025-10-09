#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define HOTKEY_ID 1
#define CONFIG_FILE "config.ini"

// Parse key string like "F12", "A", "z", "f1"
UINT ParseKeyString(const char* key) {
    char upper[4] = {0};
    strncpy(upper, key, sizeof(upper) - 1);
    for (int i = 0; upper[i]; ++i) upper[i] = toupper((unsigned char)upper[i]);

    if (upper[0] == 'F') {
        int num = atoi(upper + 1);
        if (num >= 1 && num <= 12)
            return VK_F1 + (num - 1);
    } else if (strlen(upper) == 1 && isalpha(upper[0])) {
        return upper[0];
    }

    printf("Invalid key in config: %s\n", key);
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
        hwnd = FindWindow(NULL, "Task Manager");
    }

    if (hwnd) {
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        } else {
            ShowWindow(hwnd, SW_SHOW);
        }
        SetForegroundWindow(hwnd);
    } else {
        WinExec("taskmgr.exe", SW_SHOWNORMAL);
    }
}

int main() {
    UINT vk;
    if (!LoadHotkey(&vk)) {
        return 1;
    }

    if (!RegisterHotKey(NULL, HOTKEY_ID, 0, vk)) {
        return 1;
    }

    printf("Hotkey registered. Press it to launch Task Manager. Close this window to exit.\n");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            LaunchOrActivateTaskmgr();
        }
    }

    UnregisterHotKey(NULL, HOTKEY_ID);
    return 0;
}
