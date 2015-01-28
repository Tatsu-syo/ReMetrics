/*
noMeiryoUI (C) 2005,2012-2015 Tatsuhiko Shoji
The sources for noMeiryoUI are distributed under the MIT open source license
*/
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<tchar.h>
#include	<windows.h>
#include	"NCFileDialog.h"

/**
 * �R���X�g���N�^
 *
 * @param bOpenFileDialog �\�z����_�C�A���O�{�b�N�X�̎��<br>
 * TRUE:[�t�@�C�����J��] �_�C�A���O�{�b�N�X FALSE:[�t�@�C���������ĕۑ�]�_�C�A���O�{�b�N�X
 * @param lpszDefExt ����̊g���q
 * @param lpszFileName �����\�������t�@�C����
 * @param dwFlags �_�C�A���O�{�b�N�X�̐ݒ�p�t���O
 * @param lpszFilter �t�B���^������
 * @param pParentWnd �e�E�C���h�E�̃n���h��
 * @param dwSize OPENFILENAME�\���̂̃T�C�Y
 */
NCFileDialog::NCFileDialog(
	BOOL bOpenFileDialog,
	const char *lpszDefExt,
	const TCHAR *lpszFileName,
	DWORD dwFlags,
	const TCHAR *lpszFilter,
	HWND pParentWnd,
	DWORD dwSize
)
{
	// �R���X�g���N�^����m_OpenFileDialog�ɏ���ݒ肷��B
	// ���
	m_OpenFileDialog = bOpenFileDialog;

	// �\���̂̃T�C�Y
	memset(&m_ofn,0x00,sizeof(OPENFILENAME));
	if (dwSize > 0) {
		m_ofn.lStructSize = dwSize;
	}else{
		m_ofn.lStructSize = sizeof(OPENFILENAME);
	}
	// ����̊g���q(���g�p)
	m_defExt = lpszDefExt;
	// �e�E�C���h�E
	m_ofn.hwndOwner = pParentWnd;
	// �t�B���^
	m_ofn.lpstrFilter = lpszFilter;
	m_ofn.nFilterIndex = 1;
	// �t���O
	m_ofn.Flags = dwFlags;
	// �����t�@�C����
	memset(gotFileName,0x00,sizeof(TCHAR) * _MAX_PATH);
	if (lpszFileName != NULL) {
		_tcscpy(gotFileName,lpszFileName);
	}
	m_ofn.lpstrFile = gotFileName;
	m_ofn.nMaxFile = _MAX_PATH;
	// m_ofn.lpstrTitle = "�ݒ�t�@�C���̎w��";
}

/**
 * �_�C�A���O��\�����܂��B
 *
 * @return IDOK:OK�{�^������ IDCANCEL:�L�����Z���{�^������
 */
int NCFileDialog::DoModal()
{
	int result;

	if (m_OpenFileDialog == FALSE){
		result = ::GetSaveFileName(&m_ofn);
	}else{
		result = ::GetOpenFileName(&m_ofn);
	}
	if (result){
		return IDOK;
	}else{
		return IDCANCEL;
	}
}
