/*
 *  ThreadLib.h
 *  commsLib
 *
 *  Created by SIEPRAWSKI Audrey on 18/03/2015.
 *
 */

#ifndef THREADLIB_H
#define THREADLIB_H

#include <windows.h>

HANDLE detachThread(LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
					LPVOID lpParameter,					   // creation flags
					LPDWORD idThread);					   // pointer to receive thread ID)

BOOL endOfThread(HANDLE hThread, DWORD dwExitCode);

#endif
