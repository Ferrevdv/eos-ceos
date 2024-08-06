#pragma once

#ifndef SHELL_H
#define SHELL_H
#include <windows.h>
#include "Parser.h"
#include "Package.h"

PPackage executeShell(PParser arguments);
#endif
