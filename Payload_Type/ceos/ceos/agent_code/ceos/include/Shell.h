#pragma once

#ifndef SHELL_H
#define SHELL_H
#include <windows.h>
#include "Parser.h"

typedef struct Package* PPackage;  // Forward declare PPackage
																	 //
PPackage executeShell(PParser arguments);
#endif
