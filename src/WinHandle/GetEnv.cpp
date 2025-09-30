#include "GetEnv.h"
#include <tlhelp32.h>

std::string g_env = "UNKNOWN";

std::string getParentProcessName() {

    DWORD pid = GetCurrentProcessId();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return "";

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    DWORD ppid = 0;
    if (Process32First(hSnap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }

    std::string parentName;
    if (ppid != 0) {
        Process32First(hSnap, &pe);
        do {
            if (pe.th32ProcessID == ppid) {
                parentName = pe.szExeFile;
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }

    CloseHandle(hSnap);
    return parentName;
}

void getEnv(){

    g_env = getParentProcessName();
}