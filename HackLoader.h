#include <Windows.h>
#include <stdio.h>

// HackData loaded from asupdate.ini
struct HackData
{
	CHAR Name[512];
	DWORD Addr;
	DWORD Size;
	UCHAR *Repl;
	CHAR Desc[512];
};

// Get the amount of hacks in asupdate.ini
UINT GetHackCount(LPSTR file)
{
	UINT out = 0;
	char buf2[512];
	char buffer[512];
	while (buffer[0] != 0) // If our buffer with the data from asupdate is empty return
	{
		sprintf_s(buf2, "Hack%d", out); // In C++ you have to use sprintf to concat strings (%d stands for an integer). in java "Hack" + out.
		GetPrivateProfileSectionA(buf2, buffer, 512, file); // Test if this section exists in the asupdata.ini
		out++;
	}
	return out-1; // return the amount of GetSection("Hack"+i) that worked
}

// Load a hack from asupdate.ini. pos is here the index of the hack
HackData LoadHack(LPSTR file, DWORD pos)
{
	char buffer[512];
	sprintf_s(buffer, "Hack%d", pos); // Again print the section of the hack into a string

	HackData data; // Create new HackData
	GetPrivateProfileStringA(buffer, "name", "broken", data.Name, 512, file); // Read the name
	GetPrivateProfileStringA(buffer, "desc", "", data.Desc, 512, file); // Read the description
	data.Addr = GetPrivateProfileIntA(buffer, "ptr", 0, file); // Read the address
	data.Size = GetPrivateProfileIntA(buffer, "size", 0, file); // Read the size
	data.Repl = (UCHAR*)malloc(data.Size); // Allocate memory for the replacement code
	GetPrivateProfileStructA(buffer, "repl", data.Repl, data.Size, file); // Read the replacement code

	return data; // Return the hack data
}