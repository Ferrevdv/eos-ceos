#pragma once

#ifndef DIR_H
#define DIR_H
#include <windows.h>
#include "Parser.h"

typedef struct Package* PPackage;  // Forward declare PPackage
																	 //
PPackage executeDir(PParser arguments);
#endif
