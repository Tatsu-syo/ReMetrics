/*
Re-Metrics (C) 2012-2017,2020,2021,2022 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef INIREADER_H
#define INIREADER_H

#include <tchar.h>

DWORD GetPrivateProfileStringExT(
	const char *sectionName,
	const TCHAR *keyName,
	const TCHAR *defaultValue,
	tstring &returnString,
	TCHAR *iniFileName);

#endif
