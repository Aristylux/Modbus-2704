/*
 *  ThreadLib.c
 *  commsLib
 *
 *  Created by Audrey SIEPRAWSKI on 18/03/2015.
 *
 */

#include "Thread.h"

HANDLE detachThread(LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
					LPVOID lpParameter,					   // creation flags
					LPDWORD idThread)					   // pointer to receive thread ID)
{
	SECURITY_ATTRIBUTES secAttrib;
	secAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttrib.lpSecurityDescriptor = NULL;
	secAttrib.bInheritHandle = TRUE;

	HANDLE handleThread = CreateThread(&secAttrib,
									   0,
									   lpStartAddress,
									   lpParameter,
									   0,
									   idThread);

	return handleThread;
}

BOOL endOfThread(HANDLE hThread, DWORD dwExitCode)
{
	TerminateThread(hThread, dwExitCode);
	CloseHandle(hThread);
	hThread = NULL;

	return 1;
}
