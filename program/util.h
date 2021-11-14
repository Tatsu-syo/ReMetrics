/*
Re-Metrics (C) 2012-2017,2020,2021 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <vector>
#include "TWR/tstring.h"

#define PRESET_SECTION "PRESET"
#define APP_FONTSIZE 11

extern bool WIN8_SIZE;
extern std::vector<tstring> langResource;
/** CodePage */
extern int codePage;

void readResourceFile(TCHAR *file);

#endif
