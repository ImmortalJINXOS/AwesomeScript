#include "stdafx.h"
#include <stdio.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include "Methods.h"

// Prevent this from being redefined
#ifndef FUNCTION_WRITER
#define FUNCTION_WRITER

// The structure that holds the hack information
struct Hack
{
	DWORD ptr; // the address of the hack
	DWORD size; // the size of the hack
	BOOL enabled; // is the hack enabled?
	CHAR name[256]; // display name of the hack
	CHAR original[256]; // holds the original data to disable the hack
	UCHAR *replace; // data that replaces the original
	CHAR desc[256]; // description of the hack (unused)
};

DWORD BaseAddress = (DWORD)GetModuleHandle(NULL); // The base address of Awesomenauts (mostly 0x00001000)

// Some memory is protected. To read/write it we unprotect it
DWORD UnprotectFunction(PVOID ptr, DWORD size)
{
	DWORD dwOld;

	VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &dwOld);

	return dwOld;
}

// Enable a hack
VOID EnableHack(Hack *hack)
{
	// This is the position of the code to change in the memory
	char* Target = (char*)(BaseAddress + hack->ptr);

	// We unprotect that memory
	UnprotectFunction(Target, hack->size);

	// We loop through each byte, copy it into the hack.original and then replace it with our code
	for (DWORD i = 0; i < hack->size; i++)
	{
		hack->original[i] = Target[i];
		Target[i] = hack->replace[i];
	}
}

// Disable/Restore a hack
void DisableHack(Hack *hack)
{
	// This is the position of the code to change in the memory
	char *Target = (char*)(BaseAddress + hack->ptr);

	// If the memory got protected again, we unprotect it
	UnprotectFunction(Target, hack->size);

	// We loop through each byte and restore it
	for (DWORD i = 0; i < hack->size; i++)
	{
		Target[i] = hack->original[i];
	}
}

// Don't worry about this pretty much just bs :D
PVOID ReadModulePtr(PVOID targ, CHAR offset)
{
	return (PVOID)*(DWORD*)(*(DWORD *)((DWORD)targ + BaseAddress) + offset);
}

// There are variables, that point to other variables
// In order to set the base variables by just having the pointer we use this
// (We don't need it)
void SetModulePtr(PVOID targ, size_t size, PVOID source, DWORD offset)
{
	DWORD *ptr = (DWORD*)(BaseAddress + (DWORD)targ);
	char* trg = (char*)(*ptr + offset);
	char* src = (char*)source;

	DWORD reset = UnprotectFunction(trg, size);

	for (DWORD i = 0; i < size; i++)
	{
		trg[i] = src[i];
	}
}
#endif