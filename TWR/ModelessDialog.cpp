#include "ModelessDialog.h"

HWND modelessDialogHandle[10];
int modelessDialogHandles;

/**
 * ���[�h���X�_�C�A���O�֘A�̏��������s���B
 */
void initModelessDialog(void)
{
	for (int i = 0; i < 10; i++) {
		modelessDialogHandle[i] = NULL;
	}
	modelessDialogHandles = 0;
}

/**
 * ���[�h���X�_�C�A���O��ǉ�����B
 *
 * @param dialog �_�C�A���O�̃E�C���h�E�n���h��
 */
void addModelessDialog(HWND dialog)
{
	bool found = false;
	int empty = -1;
	for (int i = 0; i < 10; i++) {
		if (modelessDialogHandle[i] == dialog) {
			return;
		}
		if ((modelessDialogHandle[i] == NULL) && (empty == -1)){
			empty = i;
		}
	}
	if (empty > -1) {
		modelessDialogHandle[empty] = dialog;
		modelessDialogHandles++;
	} else {
		modelessDialogHandle[modelessDialogHandles] = dialog;
		modelessDialogHandles++;
	}
}

/**
 * ���[�h���X�_�C�A���O���A�v���P�[�V�����ɂ�鐧�䂩��O���B
 *
 * @param dialog �_�C�A���O�̃E�C���h�E�n���h��
 */
void delModelessDialog(HWND dialog)
{
	bool found = false;
	int empty = -1;
	for (int i = 0;i < 10; i++) {
		if (modelessDialogHandle[i] == dialog) {
			modelessDialogHandle[i] = NULL;
			modelessDialogHandles--;
			return;
		}
	}
}

/**
 * ���[�h���X�_�C�A���O�̃_�C�A���O���b�Z�[�W�n���h���[
 *
 * @param lpMsg �_�C�A���O���b�Z�[�W
 * @return true:���b�Z�[�W���������� false:���b�Z�[�W���������Ȃ�
 */
bool handleDialogMessage(LPMSG lpMsg)
{
	HWND hDlg;

	for (int i = 0; i < 10; i++) {
		hDlg = modelessDialogHandle[i];
		if (hDlg == NULL) {
			continue;
		}
		if (IsDialogMessage(hDlg, lpMsg)) {
			return true;
		}
	}
	return false;
}

