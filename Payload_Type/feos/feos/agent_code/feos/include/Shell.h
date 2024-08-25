#pragma once

#ifndef SHELL_H
#define SHELL_H
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "Package.h"
#include "Parser.h"
#include "Command.h"

PPackage executeShell(PParser arguments);
PPackage executeDir(PParser arguments);
#endif
