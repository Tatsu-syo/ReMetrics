/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include "TwrWnd.h"

class BaseDialog : public TwrWnd {
private:
	HWND parentWindow;
	int dialogResource;
	bool isModeless;
protected:
	static BaseDialog *modalCallback;
	virtual INT_PTR OnInitDialog();
	virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
	/**
	 * �_�C�A���O���삪�s��ꂽ���ɌĂяo����܂��B
	 *
	 * @return �������� 0:�������s���� ��0:�������s��Ȃ�
	 */
	virtual INT_PTR OnSettingChange(WPARAM wParam, LPARAM lParam)
	{
		return (INT_PTR)TRUE;
	}
	/**
	 * �R���g���[���̃A�N�e�B�u��Ԃ��ω��������ɌĂяo����܂��B
	 *
	 * @return �������� 0:�������s����
	 */
	virtual INT_PTR OnLostFocus(WPARAM wParam, LPARAM lParam)
	{
		return (INT_PTR)0;
	}

	TCHAR ddx_buf[512];
public:
	BaseDialog();
	BaseDialog(HWND parent, int resource);
	virtual ~BaseDialog();

	INT_PTR showModal();
	HWND showModeless();
	static INT_PTR CALLBACK modalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK modelessDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK dialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void setParent(HWND parent)
	{
		parentWindow = parent;
	}

	TwrWnd *GetDlgItem(int item);
	HWND GetDlgItemRaw(int item);
	void setChildText(int id, const TCHAR *message);
	void setChildFont(int id, HFONT font);

	virtual void UpdateData(bool toObj);

};

// MFC��DDX���ǂ�Form Data eXchange�}�N��
// UpdateData�֐��̈����A�_�C�A���O�R���g���[��ID�A�ݒ�Ώۂ̕ϐ�

// �R���g���[���̃e�L�X�g
#define DDX_Text(toObj, ctl, str) \
	if (toObj) { \
		GetDlgItemText(hWnd,ctl,ddx_buf,512); \
		str = ddx_buf; \
	} else { \
		SetDlgItemText(hWnd,ctl,str.c_str()); \
	}

// �`�F�b�N�{�b�N�X�̃`�F�b�N���
#define DDX_Check(toObj, ctl, stat) \
	if (toObj) { \
		stat = SendMessage(::GetDlgItem(hWnd,ctl),BM_GETCHECK,0,0); \
	} else { \
		SendMessage(::GetDlgItem(hWnd,ctl),BM_SETCHECK,(WPARAM)stat,0); \
	}

// �R���{�{�b�N�X�̃C���f�b�N�X
#define DDX_CBIndex(toObj, ctl, index) \
	if (toObj) { \
		index = SendMessage(::GetDlgItem(hWnd,ctl),CB_GETCURSEL,0,0); \
	} else { \
		SendMessage(::GetDlgItem(hWnd,ctl),CB_SETCURSEL,(WPARAM)index,0); \
	}


#endif
