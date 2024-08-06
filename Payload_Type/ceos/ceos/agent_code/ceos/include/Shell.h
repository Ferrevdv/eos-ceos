#pragma once

#ifndef SHELL_H
#define SHELL_H
#include <windows.h>
#include "Package.h"
#include "Parser.h"
#include "Command.h"

PPackage executeShell(PParser arguments);
#endif
