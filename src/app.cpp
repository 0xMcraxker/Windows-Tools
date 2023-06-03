#include <windows.h>
#include "tools.hpp"

void AddControls(HWND hWnd)
{
    CreateWindowW(L"Button", L"Shutdown Windows", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 10, 150, 50, hWnd, (HMENU)SHUTDOWN_SYSTEM, NULL, NULL);
    CreateWindowW(L"Button", L"Kill Windows", WS_VISIBLE | WS_CHILD | WS_BORDER, 320, 10, 150, 50, hWnd, (HMENU)KILL_WINDOWS, NULL, NULL);
    CreateWindowW(L"Button", L"Turn Regedit", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 70, 150, 50, hWnd, (HMENU)TURN_REGEDIT, NULL, NULL);
    CreateWindowW(L"Button", L"Install Proccess Hacker", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_MULTILINE | BS_CENTER, 320, 70, 150, 50, hWnd, (HMENU)INSTALL_PROC_HACKER, NULL, NULL);
    CreateWindowW(L"Button", L"Install NSudo", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_MULTILINE | BS_CENTER, 20, 140, 150, 50, hWnd, (HMENU)INSTALL_NSUDO, NULL, NULL);
    CreateWindowW(L"Button", L"Install QEMU", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_MULTILINE | BS_CENTER, 320, 140, 150, 50, hWnd, (HMENU)INSTALL_QEMU, NULL, NULL);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case SHUTDOWN_SYSTEM:
            ShutdownSystem();
            break;

        case KILL_WINDOWS:
            KillWindows();
            break;

        case TURN_REGEDIT:
            TurnRegistryEditor();
            break;

        case INSTALL_PROC_HACKER:
            InstallProcHacker();
            break;

        case INSTALL_NSUDO:
            InstallNSudo();
            break;
            
        case INSTALL_QEMU:
            InstallQEMU();
            break;
        }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        AddControls(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}