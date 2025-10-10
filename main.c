#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define HOTKEY_ID 1
#define CONFIG_FILE "config.ini"

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
    return 0;
}

UINT LoadHotkey(UINT* vk, char* keyBuffer, int bufferSize) {
    char configPath[MAX_PATH];
    GetModuleFileName(NULL, configPath, MAX_PATH);

    char* lastSlash = strrchr(configPath, '\\');
    if (lastSlash != NULL) {
        strcpy(lastSlash + 1, CONFIG_FILE);
    } else {
        strcpy(configPath, CONFIG_FILE);
    }

    GetPrivateProfileString("Hotkey", "key", "F12", keyBuffer, bufferSize, configPath);
    *vk = ParseKeyString(keyBuffer);
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
    char keyString[16] = {0};

    if (!LoadHotkey(&vk, keyString, sizeof(keyString))) {
        printf("Failed to load or parse hotkey '%s' from config.ini\n", keyString);
        return 1;
    }

    if (!RegisterHotKey(NULL, HOTKEY_ID, 0, vk)) {
        printf("Failed to register hotkey '%s'. It may be in use by another program.\n", keyString);
        return 1;
    }

    printf("Hotkey '%s' registered successfully.\nPress it to launch Task Manager. Close this window to exit.\n", keyString);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            LaunchOrActivateTaskmgr();
        }
    }

    UnregisterHotKey(NULL, HOTKEY_ID);
    return 0;
}