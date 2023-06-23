#pragma once

class ProcessInfo {
	DWORD pid;
	DWORD ppid;
	bool valid;

	public:
		ProcessInfo(DWORD pid);
		virtual ~ProcessInfo();

		DWORD GetPid();
		DWORD GetPpid();

		bool ParentValid();

		HWND GetParentProcessHwnd();

		ProcessInfo( const ProcessInfo& other);
		ProcessInfo& operator=( const ProcessInfo& other);

		ProcessInfo GetParent();


};

;