// HideMe.cpp : Defines the entry point for the console application.

#include <Windows.h>

#include <tlhelp32.h>
#include <stdio.h>

#include "processinfo.h"


struct Table {
    const char* name;
    UINT code;
};
;
const Table options[] = {
    { "hide", SWP_HIDEWINDOW,  },
    { "unhide", SWP_SHOWWINDOW },
    { NULL, 0 }
};

const int ARG_VERBOSE = 1;

Table flags[] = {
    { "-v", ARG_VERBOSE },
    { NULL, 0 }
};

int main(int argc, char* argv[])
{
    DWORD action = 0;
    DWORD theFlags = 0;
    bool fail = false;

    for( int arg = 1; arg < argc; arg++ ) {
        bool found = false;
        for (int i = 0; options[i].name; i++) {
            if (!strcmp(argv[arg], options[i].name)) {
                found = true;
                if (action) {
                    fprintf(stderr, "Surplus option: %s\n", argv[arg]);
                    fail = true;
                }
                else {
                    action = options[i].code;
                }
                break;
            }
        }
        if (found) {
            continue;
        }
        for (int i = 0; flags[i].name; i++) {
            if (!strcmp(argv[arg], flags[i].name)) {
                found = true;
                if (flags[i].code & theFlags) {
                    fail = true;
                    fprintf(stderr, "Duplicate flag: %s\n", argv[arg]);
                }
                else {
                    theFlags |= flags[i].code;
                }
                break;
            }
        }
        if (!found) {
            fail = true;
            fprintf(stderr, "Unknown argument: %s\n", argv[arg]);
        }
    }
    if (!action || fail) {
        fprintf(stderr, "Usage: hideme [-v] hide|unhide\n");
        exit(1);
    }

    UINT flags = SWP_NOMOVE |
        SWP_ASYNCWINDOWPOS |
        SWP_NOOWNERZORDER |
        SWP_NOACTIVATE |
        SWP_NOSENDCHANGING |
        SWP_NOZORDER |
        SWP_NOSIZE;


    ProcessInfo pi(GetCurrentProcessId());
    while (pi.ParentValid()) {
        HWND parent = pi.GetParentProcessHwnd();
        if (theFlags & ARG_VERBOSE) {
            fprintf(stderr, "hwnd %08llx pid %x parent pid %x\n", (LPARAM)parent, pi.GetPid(), pi.GetPpid());
        }
  
        if (parent) {
            SetWindowPos(parent, NULL, 0, 0, 0, 0, flags | action);
            break;
        }
        pi = pi.GetParent();
  
    }

    if (pi.ParentValid()) {
        return 0;
    }
    fprintf(stderr, "Parent process not found\n");

    return 1;
    
}
