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
 * �}���`�o�C�g�����񂩂�R�[�h�y�[�W�ɍ��v�������C�h��������擾����B
 *
 * @param returnString ������i�[��o�b�t�@
 * @param readString �ǂݎ����������
 * @param returnSize ������i�[��o�b�t�@�̃T�C�Y
 * @param codePage �ϊ���̃R�[�h�y�[�W
 * @return �o�b�t�@�Ɋi�[���ꂽ������
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
 * ��؂蕶����T���ċ�؂�
 *
 * @param start �����J�n�ʒu
 * @param delimiter ��؂蕶��
 * @return ��NULL:��؂蕶���̂������ꏊ NULL:��؂蕶�������݂��Ȃ�
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
 * �ϊ���̃R�[�h�y�[�W���l������ini�t�@�C���ǂݎ��
 *
 * @param sectionName �Z�N�V������(�}���`�o�C�g���s��)
 * @param keyName �L�[��
 * @param defaultValue ����̕�����
 * @param returnString ������i�[��o�b�t�@
 * @param returnSize ������i�[��o�b�t�@�̃T�C�Y
 * @param iniFileName ini�t�@�C����
 * @return �o�b�t�@�Ɋi�[���ꂽ������
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
			/* �Z�N�V���������܂��Ă���Ƃ��̏��� */
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

	/* �L�[�ƒl��������Ȃ����� */
	returnString = defaultValue;

	return returnString.length();

}
