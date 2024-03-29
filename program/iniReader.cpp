/*
Re-Metrics (C) 2012-2017,2020,2021,2022 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include "util.h"

// cl /D "UNICODE" /D "_UNICODE" iniReader.cpp

static bool inSection = false;
static char currentSection[128];
static char valueBuf[256];

/**
 * マルチバイト文字列からコードページに合致したワイド文字列を取得する。
 *
 * @param returnString 文字列格納先バッファ
 * @param readString 読み取った文字列
 * @param returnSize 文字列格納先バッファのサイズ
 * @param codePage 変換先のコードページ
 * @return バッファに格納された文字数
 */
DWORD getWideString(
	TCHAR *returnString,
	char *readString,
	DWORD returnSize,
	UINT codePage)
{
	int transferCount;
	
	transferCount = MultiByteToWideChar(
		codePage,
		MB_PRECOMPOSED,
		readString,
		-1,
		returnString,
		returnSize);
		
	return transferCount;
}

/**
 * 区切り文字を探して区切る
 *
 * @param start 検索開始位置
 * @param delimiter 区切り文字
 * @return 非NULL:区切り文字のあった場所 NULL:区切り文字が存在しない
 */
char *searchAndDelimit(char *start, char delimiter)
{
	char *currentChar = start;

	while(*currentChar != '\0'){
		if (*currentChar == delimiter) {
			*currentChar = '\0';
			return currentChar;
		}
		currentChar++;
	}
	return NULL;
}


/**
 * 変換先のコードページを考慮したiniファイル読み取り
 *
 * @param sectionName セクション名(マルチバイト名不可)
 * @param keyName キー名
 * @param defaultValue 既定の文字列
 * @param returnString 文字列格納先バッファ
 * @param returnSize 文字列格納先バッファのサイズ
 * @param iniFileName iniファイル名
 * @return バッファに格納された文字数
 */
DWORD GetPrivateProfileStringExT(
	const char *sectionName,
	const TCHAR *keyName,
	const TCHAR *defaultValue,
	tstring &returnString,
	TCHAR *iniFileName)
{
	FILE *fp;
	char key[64];
	
	inSection = false;

#if UNICODE
	fp = _wfopen(iniFileName, L"r");
	wcstombs(key, keyName, 64);
#else
	fp = fopen(iniFileName, "r");
	strcpy(key, keyName);
#endif
	if (fp == NULL) {
		returnString = defaultValue;

		return returnString.length();
	}
	
	while(fgets(valueBuf, 255, fp) != NULL) {
		/* Skip comment */
		char *firstChar;
		bool isComment = false;

		firstChar = valueBuf;
		while(*firstChar != '\0') {
			if (*firstChar == ';') {
				isComment = true;;
				break;
			} else if (!isspace(*firstChar)){
				if (*firstChar == '[') {
					// New section start
					inSection = false;
				}
				break;
			}
			firstChar++;
		}
		if (isComment) {
			continue;
		}
		if ((*firstChar == '\0') || (*firstChar == '\n')){
			continue;
		}

		/* remove return code */
		searchAndDelimit(firstChar, '\n');

		if (inSection) {
			/* セクションが決まっているときの処理 */
			if (strcmp(sectionName, currentSection)) {
				continue;
			} else {
				char *keyDelim = NULL;
				
				keyDelim = searchAndDelimit(firstChar, '=');
				if (keyDelim == NULL) {
					/* no value */
					continue;
				}
				if (strcmp(firstChar, key)) {
					/* Key not matched */
					continue;
				} else {
					fclose(fp);
					DWORD len = utf8toUtf16(returnString, keyDelim + 1);
					return len;
				}
			}
		} else {
			
			/* Get section name */
			if (valueBuf[0] != '['){
				continue;
			} else {
				char *p;
				p = strchr(valueBuf, ']');
				if (p == NULL) {
					continue;
				} else {
					*p = '\0';
					strcpy(currentSection, valueBuf + 1);
					inSection = true;
				}
			}
		}
	}
	fclose(fp);

	/* キーと値が見つからなかった */
	returnString = defaultValue;

	return returnString.length();

}
