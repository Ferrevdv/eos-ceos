#pragma once

#ifndef DIR_H
#define DIR_H
#include <windows.h>
#include "Package.h"
#include "Parser.h"
#include "Command.h"

PPackage executeDir(PParser arguments);
#endif
