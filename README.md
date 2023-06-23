# HideMe
Hide or unhide the current top-level window of a process. Useful to hide/unhide a currently running scheduled task. Rather than minimize the process to the task bar, 'hide' will remove the window from the user interface. 'unhide' will attempt to bring it back.

This Windows tool looks for the top level window of its ancestor processes and hides or unhides it.

Usage: 
    hideme [-v] hide
    hideme [-v] unhide

The program is hazardous. It will hide the current shell when run from a command line. This will make it impossible to close the now-hidden shell with the keyboard.
