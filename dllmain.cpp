// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "d3d9.h"
#include "detours.h"

// Include the library detours
#pragma comment( lib, "detours.lib" )

// Load DirectX
HMODULE hMod = LoadLibrary("d3d9.dll");

// These are Functions that hold the real D3DCreate9 and our D3DCreate9
IDirect3D9* (__stdcall *rDirect3DCreate9)(UINT SDKVersion) = (IDirect3D9* (__stdcall *)(UINT))GetProcAddress(hMod, "Direct3DCreate9");
IDirect3D9* (__stdcall *tDirect3DCreate9)(UINT SDKVersion);

// This is the function that will be called by Awesomenauts instead of the one from d3d9.dll
IDirect3D9* __stdcall hkDirect3DCreate9(UINT SDKVersion)
{
	IDirect3D9 *d3dint = tDirect3DCreate9(SDKVersion);

	if( d3dint != NULL )
	{
		hkIDirect3D9 *ret = new hkIDirect3D9(&d3dint);
	}
	else {}

	// Return our IDirect3D9
	return d3dint;
}

// Called by the system when we inject the dll
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// When the dll is attached to a process
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		// attach our function to the Direct3DCreate9 from d3d9.dll
		if (DetourAttachEx((PVOID *)&rDirect3DCreate9, hkDirect3DCreate9, (PDETOUR_TRAMPOLINE *)&tDirect3DCreate9, NULL, NULL))
		{
			MessageBox(0, "Failed!", "", 0);
		}
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

