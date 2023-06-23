
#include <Windows.h>

#include <tlhelp32.h>
#include <stdio.h>
#include "processinfo.h"

class Snapshot {
 
    static HANDLE snapshot;
public:
    static HANDLE Handle()
    {
        if (!snapshot) {
            snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        }
        if (snapshot == INVALID_HANDLE_VALUE) {
            return NULL;
        }
        return snapshot;
    }
};

HANDLE Snapshot::snapshot = NULL;

ProcessInfo::ProcessInfo(DWORD pid) : pid(pid), ppid(0), valid(true)
{
    if (!pid) {
        valid = false;
        return;
    }

    FILETIME creationTime;
    FILETIME dummies[3];
    if (pid == GetCurrentProcessId()) {
        if (!GetProcessTimes(GetCurrentProcess(), &creationTime, dummies + 0, dummies + 1, dummies + 2)) {
            valid = false;
            return;
        }
    }
    else {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (!hProcess) {
            valid = false;
            return;
        }
        if (!GetProcessTimes(hProcess, &creationTime, dummies + 0, dummies + 1, dummies + 2)) {
            valid = false;
            CloseHandle(hProcess);
            return;
        }
        CloseHandle(hProcess);
    }
    if (!GetPpid() || !valid) {
        return;
    }
    // get process handle from parent process id
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ppid);
    if (!hProcess) {
        valid = false;
        return;
    }

    // get parent process start time
    FILETIME parentCreationTime;
    if (!GetProcessTimes(hProcess, &parentCreationTime, dummies + 0, dummies + 1, dummies + 2)) {
        CloseHandle(hProcess);
        valid = false;
        return;
    }
    CloseHandle(hProcess);

    // if parent process started after this process, then it is not the parent
    if (CompareFileTime(&creationTime, &parentCreationTime) < 0) {
        valid = false;
    }
    else {
        valid = true;
    }

}

ProcessInfo::~ProcessInfo()
{
}

ProcessInfo::ProcessInfo(const ProcessInfo& other) 
{
	pid = other.pid;
	ppid = other.ppid;
    valid = other.valid;

}

ProcessInfo& ProcessInfo::operator=(const ProcessInfo& other) 
{
	if (this != &other) {
		pid = other.pid;
		ppid = other.ppid;
        valid = other.valid;
	}
	return *this;
}

DWORD ProcessInfo::GetPid() 
{
	return pid;
}

DWORD ProcessInfo::GetPpid() 
{
    if (!valid) {
        return 0;
    }
    if (ppid) {
        return ppid;
    }
  
    HANDLE snapshot = Snapshot::Handle();
    if (!snapshot ) {
        valid = false;
        return 0;
    }

    // See How can a Win32 process get the pid of its parent? "https://stackoverflow.com/a/558251/1867392"
   
    PROCESSENTRY32 pentry;

    ZeroMemory(&pentry, sizeof(pentry));
    pentry.dwSize = sizeof(pentry);

    BOOL success = FALSE;
    for (success = Process32First(snapshot, &pentry);
        success;
        success = Process32Next(snapshot, &pentry)) {
        if (pentry.th32ProcessID == pid) {
            ppid = pentry.th32ParentProcessID;
            break;
        }
        
        ZeroMemory(&pentry, sizeof(pentry));
        pentry.dwSize = sizeof(pentry);
    }

    if (!success) {
        valid = false;
        ppid = 0;
    }
    return ppid;
}

bool ProcessInfo::ParentValid()
{
    if (!valid) {
		return false;
	}
    if (!ppid) {
		GetPpid();
	}
	return valid;
}

HWND ProcessInfo::GetParentProcessHwnd()
{
    int enumerated = 0;
    if( !valid || !GetPpid() ) {
        return NULL;
    }
    HWND hwnd = NULL;
    for (hwnd = FindWindowEx(NULL, hwnd, NULL, NULL);
        hwnd;
        hwnd = FindWindowEx(NULL, hwnd, NULL, NULL)) {
        DWORD winPID = 0;
        ++enumerated;
        if (!GetWindowThreadProcessId(hwnd, &winPID)) {
            continue;
        }

        if (winPID == ppid) {
            return hwnd;
        }
    }
    return NULL;
}


ProcessInfo ProcessInfo::GetParent()
{
    if (!valid || !GetPpid()) {
		return ProcessInfo(0);
	}
	return ProcessInfo(ppid);
}


    
