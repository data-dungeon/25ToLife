//------------------------------------------------------------------------------
// patcher_win.cpp
//
// Windows-specific source for game patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include <string>

#include "Patcher.h"

bool LaunchPatcherApp(const char* host, const char* hostPath, const char* dirToPatch, const char* updateApp)
{
	char fname[MAX_PATH + 1];	// A buffer useful for all our needs
	std::string cmd;

	if (updateApp)
	{
		cmd = updateApp;
	}
	else
	{
		// Find the WinUpdater exe in the working directory
		char currentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDir);
		cmd = std::string(currentDir) + "\\WinUpdater.exe";
	}

	// Generate our current process ID for synchronization
	DWORD myPID = GetCurrentProcessId();
	_itoa(myPID, fname, 10);

	// Generate the full command line for the updater
	std::string parms = std::string("WinUpdater ") + fname + " \"" + host + "\" \"/" +
		hostPath + "\" \"" + dirToPatch + "\"";

	// Get the name of this executable
	if (GetModuleFileName(NULL, fname, MAX_PATH))
		parms = parms + " \"" + fname + "\"";

	// Launch the updater process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	if (!::CreateProcess((LPTSTR)cmd.c_str(), (LPTSTR)parms.c_str(),
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		// Let the caller know we failed
		return false;
	}

	// Wait for the child process to start up before returning, so it can wait
	// for this process to exit
	WaitForInputIdle(pi.hProcess, INFINITE);

	// All done...
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}
