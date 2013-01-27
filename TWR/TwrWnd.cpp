#include "stdafx.h"
#include "TwrWnd.h"

/**
 * �R���X�g���N�^
 */
TwrWnd::TwrWnd()
{
	hWnd = NULL;
	title = _T("");
	left = 0;
	top = 0;
	width = 0;
	height = 0;
}

/**
 * �R���X�g���N�^
 *
 * @param newHwnd �I�u�W�F�N�g�Ɍ��ѕt����E�C���h�E�n���h��
 */
TwrWnd::TwrWnd(HWND newWnd)
{
	hWnd = newWnd;
	title = _T("");
	left = 0;
	top = 0;
	width = 0;
	height = 0;
}

/**
 * �f�X�g���N�^<br>
 * <br>
 * Windows�̃E�C���h�E���j�����܂��B
 */
TwrWnd::~TwrWnd()
{
	if (hWnd != NULL) {
		::DestroyWindow(hWnd);
	}
}

/**
 * �E�C���h�E���X�V���܂��B
 *
 * @return �X�V����
 */
BOOL TwrWnd::UpdateWindow()
{
	return ::UpdateWindow(hWnd);
}

/**
 * �E�C���h�E�̕\����Ԃ�ύX���܂��B
 *
 * @return �ύX����
 */
BOOL TwrWnd::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(hWnd, nCmdShow);
}

/**
 * �E�C���h�E�̗��p�\��Ԃ�ύX���܂��B
 *
 * @param enabled true:�g�p�\ false:�g�p�s�\
 */
void TwrWnd::EnableWindow(BOOL enabled)
{
	::EnableWindow(hWnd, enabled);
}

/**
 * �E�C���h�E�̃^�C�g���������ύX���܂��B
 *
 * @param text �ݒ肷�镶����
 */
void TwrWnd::setText(const TCHAR *text)
{
	title = text;
	if (hWnd != NULL) {
		SetWindowText(hWnd, text);
	}
}

/**
 * �E�C���h�E�̃^�C�g���������ύX���܂��B
 *
 * @param x x���W
 * @param y y���W
 */
void TwrWnd::setPoint(int x, int y)
{
	left = x;
	top = y;

	if (hWnd != NULL) {
		MoveWindow(hWnd,left,top,width,height,true);
	}
}

/**
 * �E�C���h�E�̃T�C�Y��ύX���܂��B
 *
 * @param w ��
 * @param h ����
 */
void TwrWnd::setSize(int w, int h)
{
	width = w;
	height = h;

	if (hWnd != NULL) {
		MoveWindow(hWnd,left,top,width,height,true);
	}
}

/**
 * �E�C���h�E�̃t�H���g��ύX���܂��B
 *
 * @param font �t�H���g�̃n���h��
 */
void TwrWnd::setFont(HFONT font)
{
	SendMessage(hWnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
}