#include <windows.h>
#include <tlhelp32.h>
#include <userenv.h>
#include <shellapi.h>
#include <urlmon.h>
#include <shlwapi.h>

#pragma comment(lib, "urlmon.lib")

void EnableShutdownPrivilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tokenPrivileges;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        MessageBoxW(NULL, L"Failed to open process token.", L"Error", MB_ICONERROR | MB_OK);
        return;
    }

    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid))
    {
        CloseHandle(hToken);
        MessageBoxW(NULL, L"Failed to lookup shutdown privilege.", L"Error", MB_ICONERROR | MB_OK);
        return;
    }

    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        CloseHandle(hToken);
        MessageBoxW(NULL, L"Failed to adjust token privileges.", L"Error", MB_ICONERROR | MB_OK);
        return;
    }

    CloseHandle(hToken);
}

void ShutdownSystem()
{
    EnableShutdownPrivilege();

    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
    {
        DWORD error = GetLastError();

        LPWSTR errorMessage = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);

        MessageBoxW(NULL, errorMessage, L"Error", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
    }
}

void KillWindows()
{
    HMODULE ntdll = LoadLibraryA("ntdll");
    FARPROC RtlAdjustPrivilege = GetProcAddress(ntdll, "RtlAdjustPrivilege");
    FARPROC NtRaiseHardError = GetProcAddress(ntdll, "NtRaiseHardError");

    if (RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL)
    {
        BOOLEAN tmp1;
        DWORD tmp2;
        ((void (*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjustPrivilege)(19, 1, 0, &tmp1);
        ((void (*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))NtRaiseHardError)(0xc0000022, 0, 0, 0, 6, &tmp2);
    }

    HANDLE token;
    TOKEN_PRIVILEGES privileges;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);
    privileges.PrivilegeCount = 1;
    privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_HARDWARE | SHTDN_REASON_MINOR_DISK);
}

bool IsRegistryEditorEnabled()
{
    LSTATUS status;
    HKEY hKey;

    // Open the key for reading system policies
    status = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
    {
        // Error opening the key
        return false;
    }

    // Check if the value exists and its data
    DWORD value = 0;
    DWORD dataSize = sizeof(DWORD);
    status = RegQueryValueEx(hKey, "DisableRegistryTools", NULL, NULL, (LPBYTE)&value, &dataSize);
    if (status != ERROR_SUCCESS)
    {
        // The value doesn't exist or couldn't be read
        RegCloseKey(hKey);
        return false;
    }

    // Close the registry key
    RegCloseKey(hKey);

    // If the value is 0, the Registry Editor is enabled; if it's 1, the Registry Editor is disabled
    return (value == 0);
}

bool ToggleRegistryEditor()
{
    LSTATUS status;
    HKEY hKey;

    // Open the key for modifying system policies
    status = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
    {
        // Error opening the key
        return false;
    }

    // Check the current state
    bool enabled = IsRegistryEditorEnabled();

    // Set the opposite value
    DWORD value = (enabled) ? 1 : 0;
    status = RegSetValueEx(hKey, "DisableRegistryTools", 0, REG_DWORD, (const BYTE *)&value, sizeof(DWORD));
    if (status != ERROR_SUCCESS)
    {
        // Error setting the value
        RegCloseKey(hKey);
        return false;
    }

    // Notify the system about the policy change
    DWORD_PTR result;
    if (!SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Policy", SMTO_NORMAL, 5000, &result))
    {
        // Error notifying the system
        RegCloseKey(hKey);
        return false;
    }

    // Close the registry key
    RegCloseKey(hKey);

    return true;
}

void TurnRegistryEditor()
{
    if (ToggleRegistryEditor())
    {
        if (IsRegistryEditorEnabled())
        {
            MessageBoxW(NULL, L"Registry Editor is enabled.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBoxW(NULL, L"Registry Editor is disabled.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
    }
    else
    {
        MessageBoxW(NULL, L"Failed to toggle the status of the Registry Editor.", L"Error", MB_OK | MB_ICONERROR);
    }
}

void InstallProcHacker()
{
    const wchar_t *url = L"https://github.com/0xMcraxker/Archive-Mirror/raw/main/ProcessHacker.exe";
    const wchar_t *filePath = L"proccesshacker.exe";

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);

    MessageBoxW(NULL, (LPCWSTR)tempPath, L"PATH", MB_OK | MB_ICONWARNING);

    wchar_t finalPath[MAX_PATH];
    PathCombineW(finalPath, tempPath, filePath);

    MessageBoxW(NULL, L"Installing Process Hacker", L"Windows Tools Installer", MB_OK | MB_ICONWARNING);
    MessageBoxW(NULL, (LPCWSTR)finalPath, L"PATH", MB_OK | MB_ICONWARNING);
    HRESULT hr = URLDownloadToFileW(NULL, url, finalPath, 0, NULL);
    if (hr != S_OK)
    {
        DWORD error = GetLastError();

        LPWSTR errorMessage = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);

        MessageBoxW(NULL, errorMessage, L"Error", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
        return;
    }

    ShellExecuteW(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);
}

void InstallNSudo()
{
    const wchar_t *url = L"https://github.com/0xMcraxker/Archive-Mirror/raw/main/ProcessHacker.exe";
    const wchar_t *filePath = L"NSudoGL.exe";

    wchar_t tempPath[MAX_PATH];
    DWORD pathLen = GetTempPathW(MAX_PATH, tempPath);

    wchar_t finalPath[MAX_PATH];

    MessageBoxW(NULL, L"Installing NSudo", L"Windows Tools Installer", MB_OK | MB_ICONWARNING);

    HRESULT hr = URLDownloadToFileW(NULL, url, finalPath, 0, NULL);
    if (hr != S_OK)
    {
        DWORD error = GetLastError();

        LPWSTR errorMessage = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);

        MessageBoxW(NULL, errorMessage, L"Error", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
        return;
    }

    ShellExecuteW(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);
}

void InstallQEMU()
{
    const wchar_t *url = L"https://qemu.weilnetz.de/w64/qemu-w64-setup-20230531.exe";
    const wchar_t *filePath = L"\\QEMUSetup.exe";

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);

    wchar_t finalPath[MAX_PATH];
    PathCombineW(finalPath, tempPath, filePath);

    MessageBoxW(NULL, L"Installing QEMU", L"Windows Tools Installer", MB_OK | MB_ICONWARNING);

    HRESULT hr = URLDownloadToFileW(NULL, url, finalPath, 0, NULL);
    if (hr != S_OK)
    {
        DWORD error = GetLastError();

        LPWSTR errorMessage = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);

        MessageBoxW(NULL, errorMessage, L"Error", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
        return;
    }

    ShellExecuteW(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);
}