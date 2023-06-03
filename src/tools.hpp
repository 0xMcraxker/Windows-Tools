#define SHUTDOWN_SYSTEM 1
#define KILL_WINDOWS 2
#define TURN_REGEDIT 3
#define INSTALL_PROC_HACKER 4
#define INSTALL_NSUDO 5
#define INSTALL_ANTIFORKIE 6
#define INSTALL_QEMU 7

void EnableShutdownPrivilege();
void ShutdownSystem();
void KillWindows();
bool ToggleRegistryEditor();
bool IsRegistryEditorEnabled();
void TurnRegistryEditor();
void InstallProcHacker();
void InstallNSudo();
void InstallQEMU();