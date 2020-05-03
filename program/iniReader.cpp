/*
noMeiryoUI (C) 2005,2012-2018 Tatsuhiko Shoji
The sources for noMeiryoUI are distributed under the MIT open source license
*/
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>

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
 * @param codePage �ϊ���̃R�[�h�y�[�W
 * @return �o�b�t�@�Ɋi�[���ꂽ������
 */
DWORD GetPrivateProfileStringExT(
	char *sectionName,
	TCHAR *keyName,
	TCHAR *defaultValue,
	TCHAR *returnString,
	DWORD returnSize,
	TCHAR *iniFileName,
	UINT codePage)
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
		_tcsncpy(returnString, defaultValue, returnSize - 1);
		returnString[returnSize - 1] = _T('\0');
		return _tcslen(returnString);
	}
	
	while(fgets(valueBuf, 255, fp) != NULL) {
		/* Skip comment */
		char *firstChar;
		char *currentChar;
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
					return getWideString(returnString, keyDelim + 1, returnSize, codePage);
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

	/* �L�[�ƒl��������Ȃ����� */
	_tcsncpy(returnString, defaultValue, returnSize - 1);
	returnString[returnSize - 1] = _T('\0');
	return _tcslen(returnString);

}
