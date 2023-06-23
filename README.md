# HideMe
This utility will hide or unhide the current top-level window of a task. 
Rather than minimize the process to the task bar, 'hide' will remove the window from the user interface with HIDEWINDOW. 'unhide' will attempt to bring it back with SHOWWINDOW.

This Windows tool acts on the top level window of an ancestor process.

Usage: 
```
    hideme [-v] hide
```
```
    hideme [-v] unhide
```
The program is hazardous. It will hide the current shell when run from a command line. This will make it impossible to close the now-hidden shell. 

However, it is useful to hide/unhide a currently running scheduled task that might be a script or batch file. 
