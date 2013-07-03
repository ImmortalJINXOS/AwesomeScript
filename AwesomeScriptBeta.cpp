// AwesomeScriptBeta.cpp : Defines the exported functions for the DLL application.
// © 2013 JINX OS

#include "stdafx.h"
#include "AwesomeScriptBeta.h"
#include <D3dx9core.h>
#include "FunctionWriter.h"
#include "HackLoader.h"

LPD3DXFONT Font; // Font to draw menu
Hack Hacks[256]; // All registered hacks
int iMaxHacks = 0, iSelected = 0; // Amount of Hacks & Currently Selected
BOOL b1 = FALSE, b2 = FALSE, b3 = FALSE, b4 = FALSE, bSkipDraw = FALSE;
// Booleans to see if a button is currently down or not | if it should skip the draw of the window
CHAR *CurrentVersion; // The current version of awesomenauts

void ChangeHack(Hack *hack) // A function to toggle hacks
{
	if (hack->enabled)
	{
		DisableHack(hack);
		hack->enabled = FALSE;
	}
	else
	{
		EnableHack(hack);
		hack->enabled = TRUE;
	}
}

void ChangeVersion(DWORD ptr) // This changes the version at the bottom from "Version 1.XX.X" to "AwesomeScript"
{
	LPWSTR *trg = (WCHAR**)(ptr + BaseAddress); // This is where the version is stored in Awesomenauts
	int strlen = lstrlenW(*trg); // Length of the Version string
	WCHAR *repl = L"AwesomeScript"; // The replacement
	CurrentVersion = (CHAR*)malloc(strlen + 1); // Allocating memory to store the old version
	wcstombs(CurrentVersion, *(WCHAR**)(ptr + BaseAddress), strlen + 1); // Storing the old version
	*(WCHAR**)(ptr + BaseAddress) = repl; // Setting the old version to AwesomeScript
}

void DrawCenteredString(LPCSTR text, int x, int y, D3DCOLOR color) // Function to draw a centered string
{ // i have no idea what i did here ignore it :D
	RECT rct = {x, y, x + 1, y + 1};
	Font->DrawTextA(NULL, text, -1, &rct, DT_CALCRECT, NULL);

	RECT rect2 = {x - (rct.right - x) / 2, y - (rct.bottom - y) / 2, x + (rct.right - x) / 2, y + (rct.bottom - y) / 2};

	Font->DrawTextA(NULL, text, -1, &rect2, DT_NOCLIP, color);
}

void DrawFilledRectangle(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9 *dev) // Simple filled rectangle with color
{
	D3DRECT rct = {x, y, x + w, y + h};
	dev->Clear(1, &rct, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorderBox(int x, int y, int w, int h, int thick, D3DCOLOR color, IDirect3DDevice9 *dev) // Simple rectangle without fill
{
	DrawFilledRectangle(x,y,w,thick,color,dev);
	DrawFilledRectangle(x,y,thick,h,color,dev);
	DrawFilledRectangle(x+w-thick,y,thick,h,color,dev);
	DrawFilledRectangle(x,y+h-thick,w,thick,color,dev);
}

void RegisterHack(HackData hack) // Registering a hack
{
	Hacks[iMaxHacks] = Hack(); // Create new Hack
	memcpy(Hacks[iMaxHacks].name, hack.Name, strlen(hack.Name)); // Copy the name from HackData to the Hack
	memcpy(Hacks[iMaxHacks].desc, hack.Desc, strlen(hack.Desc)); // Copy the description from HackData to the hack
	Hacks[iMaxHacks].enabled = FALSE; // not enabled
	Hacks[iMaxHacks].ptr = hack.Addr; // address of the hack
	Hacks[iMaxHacks].size = hack.Size; // size of the hack
	Hacks[iMaxHacks].replace = hack.Repl; // data that replaces the original
	iMaxHacks+=1; // increase the hack count
}

void InitAwesomeScript(IDirect3DDevice9 *dev) // initialize AwesomeScript (called by DirectX)
{
	// Create our font
	D3DXCreateFont(dev, 22, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE, "Arial", &Font);
	char path[MAX_PATH];
	// Get the path to the update file (it's under %appdata%/asupdate.ini)
	sprintf_s(path, "%s/asupdate.ini", getenv("APPDATA"));
	
	// Calls previous function ChangeVersion with the parameter read from the asupdate.ini
	ChangeVersion(GetPrivateProfileIntA("Version","ptr",0,path));
	
	// Calls a function that gets the amount of hacks in the asupdate.ini
	UINT count = GetHackCount(path);
	for (UINT i = 0; i < count; i++)
	{
		// Loops through all hacks, loading them from asupdate.ini and then registering them
		RegisterHack(LoadHack(path, i));
	}
}

D3DCOLOR GetHackColor(Hack *hack)
{
	// When hack is enabled return green, if not return red
	return hack->enabled ? D3DCOLOR_ARGB(255, 0, 255, 0) : D3DCOLOR_ARGB(255, 255, 0, 0);
}

void DrawHack(IDirect3DDevice9 *dev, Hack *hack, int x, int y, int w, int h, BOOL bSel)
{
	// If our hack is selected draw a grey rectangle behind it
	if (bSel) DrawFilledRectangle(x, y, w, h, D3DCOLOR_ARGB(255, 80, 80 ,80), dev);
	// Draw a grey outline
	DrawBorderBox(x, y, w, h, 2, D3DCOLOR_ARGB(255, 155, 155, 155), dev);
	// Draw the name of the hack
	DrawCenteredString(hack->name, 110, y + 12, GetHackColor(hack));
}

void DrawAwesomeScript(IDirect3DDevice9 *dev)
{
	if (bSkipDraw) return; // If it should skip the draw, return
	// Draw the red outline
	DrawBorderBox(10, 10, 200, 70 + iMaxHacks * 30, 2, D3DCOLOR_ARGB(255, 255, 255, 255), dev);
	// Draw the title
	DrawCenteredString("AwesomeScript - Beta", 110, 30, D3DCOLOR_ARGB(255, 255, 255, 255));
	// Draw the awesomenauts version
	DrawCenteredString(CurrentVersion, 110, 50, D3DCOLOR_ARGB(255, 255, 255, 255));
	// Loop through all hacks
	for (int i = 0; i < iMaxHacks; i++) DrawHack(dev, &(Hacks[i]), 13, 80 + i * 30, 194, 27, i == iSelected);
}

// When the button was pressed and it's not pressed anymore return true
// When it was not pressed and it's now pressed set bPressed = TRUE and return false
BOOL UpdateButton(int button, PBOOL pressed)
{
	if (*pressed)
	{
		if (!GetAsyncKeyState(button)) return *pressed = FALSE;
	}
	else
	{
		if (GetAsyncKeyState(button)) return *pressed = TRUE;
	}
	return FALSE;
}

// Update AwesomeScript (called before draw)
void UpdateAwesomeScript()
{
	// if right arrow is pressed toggle the currently selected hack
	if (UpdateButton(VK_RIGHT, &b1)) ChangeHack(&(Hacks[iSelected]));
	// if up arrow is pressed go one up
	if (UpdateButton(VK_UP, &b2)) iSelected--;
	// if down arrow is pressed go one down
	if (UpdateButton(VK_DOWN, &b3)) iSelected++;
	// if insert is pressed toggle bSkipDraw
	if (UpdateButton(VK_INSERT, &b4)) bSkipDraw = !bSkipDraw;

	// if the selection is under 0, set it to the highest element
	if (iSelected < 0) iSelected = iMaxHacks-1;
	// if the selection is over the maximum, set it to the lowest element
	else if (iSelected == iMaxHacks) iSelected = 0;
}
