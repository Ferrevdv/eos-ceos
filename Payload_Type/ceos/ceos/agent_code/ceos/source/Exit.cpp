
#include "Exit.h"
#include <cstdlib>

BOOL executeExit(PParser arguments)
{
	SIZE_T uuidLength = 36;
	PCHAR taskUuid = getString(arguments, &uuidLength);
	UINT32 nbArg = getInt32(arguments);
	SIZE_T size = 0;

	// Create the response package
	PPackage responseTask = newPackage(POST_RESPONSE, TRUE);
	addString(responseTask, taskUuid, FALSE);

	// Free ceosConfig
	if (ceosConfig->hostName)
		LocalFree(ceosConfig->hostName);
	if (ceosConfig->endPoint)
		LocalFree(ceosConfig->endPoint);
	if (ceosConfig->userAgent)
		LocalFree(ceosConfig->userAgent);
	if (ceosConfig->httpMethod)
		LocalFree(ceosConfig->httpMethod);
	if (ceosConfig->proxyURL)
		LocalFree(ceosConfig->proxyURL);
	LocalFree(ceosConfig);

	exit(0);

	Parser* ResponseParser = sendPackage(responseTask);


	return TRUE;

}
