#include "stdafx.h"

#include "TwrCore.h"
#include "BaseDialog.h"
#include "ModelessDialog.h"

BaseDialog *BaseDialog::modalCallback;
static BaseDialog *modelessCallback[10];
static int modelessCallbacks = 0;

/**
 * ���[�h���X�_�C�A���O�̃R�[���o�b�N������������B
 *
 */
void initModelessCallback(void)
{
	for (int i = 0; i < 10; i++) {
		modelessCallback[i] = NULL;
	}
}

/**
 * �R���X�g���N�^
 */
BaseDialog::BaseDialog() : TwrWnd()
{
	parentWindow = NULL;
	// dialogResource = resource;
	isModeless = false;
}


/**
 * �R���X�g���N�^
 *
 * @param parent �e�E�C���h�E�n���h��
 * @param resource �_�C�A���O���\�[�X�ԍ�
 */
BaseDialog::BaseDialog(HWND parent, int resource) : TwrWnd()
{
	parentWindow = parent;
	dialogResource = resource;
	isModeless = false;
}

/**
 * �f�X�g���N�^
 */
BaseDialog::~BaseDialog()
{
	if (isModeless) {
		delModelessDialog(hWnd);

		for (int i = 0; i < 10; i++) {
			if (modelessCallback[i] == this) {
				modelessCallback[i] = NULL;
				modelessCallbacks--;
				break;
			}
		}
		if (hWnd != NULL) {
			if (IsWindow(hWnd)) {
				DestroyWindow(hWnd);
			}
		}
	}
}

/**
 * ���[�_���_�C�A���O��\������B
 *
 * @return �_�C�A���O�̖߂�l
 */
INT_PTR BaseDialog::showModal()
{
	modalCallback = this;
	return ::DialogBox(
		hInst,
		MAKEINTRESOURCE(dialogResource),
		parentWindow,
		&BaseDialog::modalDialogProc);
}

/**
 * ���[�h���X�_�C�A���O��\������B
 *
 * @return �_�C�A���O�̖߂�l
 */
HWND BaseDialog::showModeless()
{
	isModeless = true;

	hWnd = CreateDialog(
		hInst,
		MAKEINTRESOURCE(dialogResource),
		parentWindow,
		&BaseDialog::modelessDialogProc);

	if (hWnd != NULL) {
		bool found = false;
		int empty = -1;
		for (int i = 0; i < 10; i++) {
			if (modelessCallback[i] == this) {
				found = true;
				break;
			}
			if ((modelessCallback[i] == NULL) && (empty == -1)){
				empty = i;
			}
		}
		if (!found) {
			if (empty > -1) {
				modelessCallback[empty] = this;
				modelessCallbacks++;
			}
		}

		addModelessDialog(hWnd);
	}
	this->OnInitDialog();

	return hWnd;
}

/**
 * ���[�_���_�C�A���O�̃_�C�A���O�v���[�V�W��
 *
 * @param hDlg �_�C�A���O�̃E�C���h�E�n���h��
 * @param message ���b�Z�[�W
 * @param wParam WPARAM
 * @param lParam LPARAM
 * @return ��������
 */
INT_PTR CALLBACK BaseDialog::modalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return modalCallback->dialogProc(hDlg, message, wParam, lParam);
}

/**
 * ���[�h���X�_�C�A���O�̃_�C�A���O�v���[�V�W��
 *
 * @param hDlg �_�C�A���O�̃E�C���h�E�n���h��
 * @param message ���b�Z�[�W
 * @param wParam WPARAM
 * @param lParam LPARAM
 * @return ��������
 */
INT_PTR CALLBACK BaseDialog::modelessDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0;i < 10; i++) {
		if (modelessCallback[i] == NULL) {
			continue;
		}
		if (hDlg == modelessCallback[i]->getHwnd()) {
			return modelessCallback[i]->dialogProc(hDlg, message, wParam, lParam);
		}
	}
	return 0;
}

/**
 * �_�C�A���O�̃_�C�A���O�v���[�V�W��
 *
 * @param hDlg �_�C�A���O�̃E�C���h�E�n���h��
 * @param message ���b�Z�[�W
 * @param wParam WPARAM
 * @param lParam LPARAM
 * @return �������� TRUE:���b�Z�[�W�����������B FALSE:���b�Z�[�W���������Ȃ�
 */
INT_PTR CALLBACK BaseDialog::dialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT_PTR result;

	hWnd = hDlg;
	switch (message)
	{
		case WM_INITDIALOG:
            result = OnInitDialog();
			return result;

		case WM_COMMAND:
			result = OnCommand(wParam);
			if (result == 0) {
				return (INT_PTR)TRUE;
			} else {
				return (INT_PTR)FALSE;
			}
	}

	return (INT_PTR)FALSE;
}

/**
 * �_�C�A���O�ƃ_�C�A���O�I�u�W�F�N�g�̃f�[�^�̓������Ƃ�܂��B
 *
 * @param toObj true:�_�C�A���O�̓��e���I�u�W�F�N�g�Ɋi�[����B
 *              false:�I�u�W�F�N�g�̓��e���_�C�A���O�Ɋi�[����B
 */
void BaseDialog::UpdateData(bool toObj)
{
}


/**
 * �_�C�A���O�̏��������ɌĂяo����܂��B
 *
 * @return ��������
 */
INT_PTR BaseDialog::OnInitDialog()
{
	if (title.length() > 0) {
		::SetWindowText(hWnd, title.c_str());
	}
	return (INT_PTR)FALSE;
}


/**
 * �_�C�A���O���삪�s��ꂽ���ɌĂяo����܂��B
 *
 * @return �������� 0:�������s���� ��0:�������s��Ȃ�
 */
INT_PTR BaseDialog::OnCommand(WPARAM wParam)
{
	if (!isModeless) {
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)0;
		}
	}
	return (INT_PTR)TRUE;
}

/**
 * �_�C�A���O���\�[�X��̃E�C���h�E���擾���܂��B
 *
 * @param item ���\�[�XID
 * @return �_�C�A���O��̃I�u�W�F�N�g
 */
TwrWnd *BaseDialog::GetDlgItem(int item)
{
	HWND itemHwnd = ::GetDlgItem(hWnd, item);
	if (itemHwnd == NULL) {
		return NULL;
	}

	TwrWnd *newWnd;
	try {
		newWnd = new TwrWnd(itemHwnd);
		if (newWnd == NULL) {
			return NULL;
		}
		
	}catch(...) {
		return NULL;
	}
	return newWnd;
}

/**
 * �_�C�A���O���\�[�X��̃E�C���h�E�n���h�����擾���܂��B
 *
 * @param ���\�[�XID
 * @return �_�C�A���O��̃E�C���h�E�̃n���h��
 */
HWND BaseDialog::GetDlgItemRaw(int item)
{
	return ::GetDlgItem(hWnd, item);
}

