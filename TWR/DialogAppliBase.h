#pragma once

#include <windows.h>
#include "BaseDialog.h"
#include "../resource.h"

extern HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X

void addModelessDialog(HWND dialog);
void delModelessDialog(HWND dialog);

int doEvents();

class DialogAppliBase : public BaseDialog {
protected:
	virtual INT_PTR OnInitDialog();
public:
	virtual ~DialogAppliBase();
	virtual BaseDialog *createBaseDialog();
	virtual int OnAppliStart(TCHAR *lpCmdLine);
	virtual int OnWindowShow();
	virtual int OnAppliEnd();

};

extern DialogAppliBase *createAppli();
extern INT_PTR CALLBACK MainDialogProc(
  HWND hwndDlg,  // �_�C�A���O�{�b�N�X�̃n���h��
  UINT uMsg,     // ���b�Z�[�W
  WPARAM wParam, // �ŏ��̃��b�Z�[�W�p�����[�^
  LPARAM lParam  // 2 �Ԗڂ̃��b�Z�[�W�p�����[�^
);
