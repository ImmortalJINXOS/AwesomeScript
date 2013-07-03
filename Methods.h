#include "stdafx.h"
#include <TlHelp32.h>
#include <stdlib.h>

// Complicated stuff, don't worry. It's not just Process.GetProcessByName() sadly
HANDLE GetProcessByName(LPCSTR name)
{
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
			CHAR file[256];
			_splitpath(process.szExeFile, 0, 0, file, 0);
			if (_stricmp(name, file) == 0)
            {
               pid = process.th32ProcessID;
               break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    if (pid != 0)
    {
         return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }

    return NULL;
}