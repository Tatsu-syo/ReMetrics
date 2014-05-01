/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
// ReMetrics.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "ReMetrics.h"

#define MAX_LOADSTRING 100

//
// �_�C�A���O�x�[�X�A�v���P�[�V�����t���[�����[�N��
// ���[�U�[�����̃u���b�W���[�`��
//

// �A�v���P�[�V�����I�u�W�F�N�g
ReMetrics *appObj;

/**
 * �A�v���P�[�V�����I�u�W�F�N�g���쐬���܂��B
 */
DialogAppliBase *createAppli()
{
	// �����Ń��[�U�[�̃A�v���P�[�V�����I�u�W�F�N�g���쐬���܂��B
	appObj = new ReMetrics();
	return appObj;
}

/**
 * �_�C�A���O�v���[�V�W��
 *
 */
INT_PTR CALLBACK MainDialogProc(
  HWND hwndDlg,  // �_�C�A���O�{�b�N�X�̃n���h��
  UINT uMsg,     // ���b�Z�[�W
  WPARAM wParam, // �ŏ��̃��b�Z�[�W�p�����[�^
  LPARAM lParam  // 2 �Ԗڂ̃��b�Z�[�W�p�����[�^
  )
{
	// ���[�U�[�A�v���P�[�V�����̃_�C�A���O�v���[�V�W���Ɋۓ������܂��B
	return appObj->dialogProc(hwndDlg, uMsg,wParam,lParam);
}

//
// �_�C�A���O�x�[�X�A�v���P�[�V�����t���[�����[�N�̎�����
//

/**
 * �A�v���P�[�V�����E�C���h�E�N���X�̃C���X�^���X�𐶐����܂��B
 *
 * @return �A�v���P�[�V�����E�C���h�E�N���X�̃C���X�^���X
 */
BaseDialog *ReMetrics::createBaseDialog()
{
	borderWidthUpDown = NULL;
	titleWidthUpDown = NULL;
	titleHeightUpDown = NULL;
	scrollWidthUpDown = NULL;
	scrollHeightUpDown = NULL;
	paletteWidthUpDown = NULL;
	paletteHeightUpDown = NULL;
	menuWidthUpDown = NULL;
	menuHeightUpDown = NULL;
	paddingUpDown = NULL;

	return appObj;
}

/**
 * �A�v���P�[�V�����̊J�n���������s���܂��B
 *
 * @param lpCmdLine �R�}���h���C��
 * @return �\��
 */
int ReMetrics::OnAppliStart(TCHAR *lpCmdLine)
{
	// ���̊֐����I�[�o�[���C�h���ăA�v���ŗL�̏��������s���܂��B
	// �{�A�v���P�[�V�����ł͓��ɏ����Ȃ��B
	return 0;
}

/**
 * �E�C���h�E���\�����ꂽ�Ƃ��̏��������s���܂��B<br>
 * �E�C���h�E�x�[�X�A�v���ƃC���^�t�F�[�X�����킹�邽�߂ɗp�ӂ��Ă��܂��B
 *
 * @return �\��
 */
int ReMetrics::OnWindowShow()
{

	// ���̊֐����I�[�o�[���C�h���āA����̕\�����̏������s���܂��B
	// ���̃^�C�~���O�Ń_�C�A���O�����݂���̂ŁA�����ɏ��������邱�Ƃ�
	// �_�C�A���O�������ԂŋN�����̏������������s�����Ƃ��ł��܂��B
	DWORD dwver;
	int winVerMajor;

	dwver = GetVersion();
	winVerMajor = LOBYTE(dwver);

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	// �A�C�R���ȊO�̃t�H���g�����擾����B
	if (winVerMajor > 5) {
		/* Windows Vista/7/8 */
		metrics.cbSize = sizeof(NONCLIENTMETRICS);
	} else {
		/* Windows 2000/XP */
		metrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
	}

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		metrics.cbSize,
		&metrics,
		0);

	TCHAR buf[32];

	_stprintf(buf,_T("%d"), metrics.iBorderWidth);
	borderWidth = buf;

	_stprintf(buf,_T("%d"), metrics.iCaptionWidth);
	titleWidth = buf;

	_stprintf(buf,_T("%d"), metrics.iCaptionHeight);
	titleHeight = buf;

	_stprintf(buf,_T("%d"), metrics.iScrollWidth);
	scrollWidth = buf;

	_stprintf(buf,_T("%d"), metrics.iScrollHeight);
	scrollHeight = buf;

	_stprintf(buf,_T("%d"), metrics.iSmCaptionWidth);
	paletteWidth = buf;

	_stprintf(buf,_T("%d"), metrics.iSmCaptionHeight);
	paletteHeight = buf;

	_stprintf(buf,_T("%d"), metrics.iMenuWidth);
	menuWidth = buf;

	_stprintf(buf,_T("%d"), metrics.iMenuHeight);
	menuHeight = buf;

	_stprintf(buf,_T("%d"), metrics.iPaddedBorderWidth);
	padding = buf;

	UpdateData(false);

	borderWidthUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_BORDER));
	borderWidthUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_BORDER));
	titleWidthUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_TITLE_WIDTH));
	titleWidthUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_TITLE_WIDTH));
	titleHeightUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_TITLE_HEIGHT));
	titleHeightUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_TITLE_HEIGHT));
	scrollWidthUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_SCROLL_WIDTH));
	scrollWidthUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_SCROLL_WIDTH));
	scrollHeightUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_SCROLL_HEIGHT));
	scrollHeightUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_SCROLL_HEIGHT));
	paletteWidthUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_PALETTE_WIDTH));
	paletteWidthUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_PALETTE_WIDTH));
	paletteHeightUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_PALETTE_HEIGHT));
	paletteHeightUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_PALETTE_HEIGHT));
	menuWidthUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_MENU_WIDTH));
	menuWidthUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_MENU_WIDTH));
	menuHeightUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_MENU_HEIGHT));
	menuHeightUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_MENU_HEIGHT));
	paddingUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_PADDING));
	paddingUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_PADDING));

	// �e�X�s���{�^���͈̔͂�ݒ肷��B
	setItemRange();

	borderWidthUpDown->setPos(metrics.iBorderWidth);
	titleWidthUpDown->setPos(metrics.iCaptionWidth);
	titleHeightUpDown->setPos(metrics.iCaptionHeight);
	scrollWidthUpDown->setPos(metrics.iScrollWidth);
	scrollHeightUpDown->setPos(metrics.iScrollHeight);
	paletteWidthUpDown->setPos(metrics.iSmCaptionWidth);
	paletteHeightUpDown->setPos(metrics.iSmCaptionHeight);
	menuWidthUpDown->setPos(metrics.iMenuWidth);
	menuHeightUpDown->setPos(metrics.iMenuHeight);
	paddingUpDown->setPos(metrics.iPaddedBorderWidth);

	adjustWindowSize(&metrics, winVerMajor);

	return 0;
}

void ReMetrics::setItemRange()
{
	borderWidthUpDown->setRange(1, 256);
	titleWidthUpDown->setRange(1, 256);

	minTitleHeight = getMinHeight(&(metrics.lfCaptionFont));
	titleHeightUpDown->setRange(minTitleHeight, 256);

	scrollWidthUpDown->setRange(1, 256);

	scrollHeightUpDown->setRange(1, 256);

	paletteWidthUpDown->setRange(1, 256);

	minPaletteHeight = getMinHeight(&(metrics.lfSmCaptionFont));
	paletteHeightUpDown->setRange(minPaletteHeight, 256);

	menuWidthUpDown->setRange(0, 256);

	minMenuHeight = getMinHeight(&(metrics.lfMenuFont));
	menuHeightUpDown->setRange(minMenuHeight, 256);

	paddingUpDown->setRange(0, 256);
}
/**
 * �t�H���g�ɑΉ������ʗv�f�̍Œ�̍��������߂�B
 *
 * @param font ��ʗv�f�ɐݒ肳�ꂽ�t�H���g
 * @return �t�H���g�ɑΉ������ʗv�f�̍Œ�̍���
 */
int ReMetrics::getMinHeight(LOGFONT *font)
{
	// �t�H���g���쐬����B
	HFONT hFont = CreateFontIndirect(font);
	// ���g�̃E�C���h�E�n���h������쐬�����f�o�C�X�R���e�L�X�g��
	// �t�H���g��ݒ肷��B
	HDC dc = GetDC(this->getHwnd());
	SelectFont(dc, hFont);

	// �f�o�C�X�R���e�L�X�g����TEXTMETRIC���擾����B
	TEXTMETRIC metric;
	GetTextMetrics(dc, &metric);

	ReleaseDC(this->getHwnd(), dc);
	DeleteObject(hFont);

	int height;
	if (font->lfHeight < 0) {
		// ���̏ꍇ��lfHeight�̓t�H���g���̂̍�����Internal Leading��
		// �܂܂Ȃ��̂�Internal Leading�����Z����B
		height = 0 - font->lfHeight + metric.tmInternalLeading;
	} else if (font->lfHeight > 0) {
		// ���̏ꍇ�͂��ł�Internal Leading���܂�ł���B
		height = font->lfHeight;
	} else {
		// 0�̏ꍇ�̓f�t�H���g�̑傫�����쐬�����t�H���g����擾����B
		height = metric.tmAscent + metric.tmDescent;
	}

	// �]��2px�𑫂����l���t�H���g�ɑΉ������ʗv�f�̍Œ�̍���
	return height + 2;
}

/**
 * �A�v���P�[�V�����̏I�����������s���܂��B
 *
 * @return �\��
 */
int ReMetrics::OnAppliEnd()
{
	// ���̊֐����I�[�o�[���C�h���ăA�v���ŗL�̌㏈�����s���܂��B
	// �{�A�v���P�[�V�����ł͓��ɏ����Ȃ��B
	return 0;
}

//
// �_�C�A���O�x�[�X�A�v���P�[�V�����Ƃ��Ă�
// �A�v���P�[�V�����ŗL�����̎�������
//

/**
 * �_�C�A���O����������
 *
 * @return TRUE: FALSE:�t�H�[�J�X��ݒ肵��
 */
INT_PTR ReMetrics::OnInitDialog()
{
	// �e�N���X�̃_�C�A���O�������������ĂԁB
	DialogAppliBase::OnInitDialog();

	HICON hIcon;

    hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDC_MYICON), IMAGE_ICON, 16, 16, 0);
    SendMessage(this->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	UpdateData(false);

	return (INT_PTR)FALSE;
}

/**
 * �_�C�A���O�R���g���[���ƃI�u�W�F�N�g�̓��e�̓��������B
 *
 * @param toObj true:�R���g���[�����I�u�W�F�N�g false:�I�u�W�F�N�g���R���g���[��
 */
void ReMetrics::UpdateData(bool toObj)
{
	// �����Ƀ_�C�A���O�̃R���g���[���Ɠ�������郁���o�ϐ����L�q���܂��B
	DDX_Text(toObj,IDC_EDIT_BORDER, borderWidth);
	DDX_Text(toObj,IDC_EDIT_TITLE_WIDTH, titleWidth);
	DDX_Text(toObj,IDC_EDIT_TITLE_HEIGHT, titleHeight);
	DDX_Text(toObj,IDC_EDIT_SCROLL_WIDTH, scrollWidth);
	DDX_Text(toObj,IDC_EDIT_SCROLL_HEIGHT, scrollHeight);
	DDX_Text(toObj,IDC_EDIT_PALETTE_WIDTH, paletteWidth);
	DDX_Text(toObj,IDC_EDIT_PALETTE_HEIGHT, paletteHeight);
	DDX_Text(toObj,IDC_EDIT_MENU_WIDTH, menuWidth);
	DDX_Text(toObj,IDC_EDIT_MENU_HEIGHT, menuHeight);
	DDX_Text(toObj,IDC_EDIT_PADDING, padding);
}

/**
 * WM_COMMAND���b�Z�[�W�̏�������B<br>
 * ���͓��e�̌������s������A��ʊe���̕��E�����̐ݒ���s���B
 *
 * @param wParam WPARAM�̒l
 * @return �������� 0:�������s���� ��0:�������s��Ȃ�
 */
INT_PTR ReMetrics::OnCommand(WPARAM wParam)
{
	switch (LOWORD(wParam)) {
		case IDM_SET_2K:
			borderWidth = _T("1");
			titleWidth = _T("18");
			titleHeight = _T("18");
			scrollWidth = _T("16");
			scrollHeight = _T("16");
			paletteWidth = _T("18");
			paletteHeight = _T("18");
			menuWidth = _T("18");
			menuHeight = _T("18");
			padding = _T("0");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_XP:
			borderWidth = _T("1");
			titleWidth = _T("18");
			titleHeight = _T("18");
			scrollWidth = _T("16");
			scrollHeight = _T("16");
			paletteWidth = _T("12");
			paletteHeight = _T("15");
			menuWidth = _T("18");
			menuHeight = _T("18");
			padding = _T("0");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_XP_LUNA:
			borderWidth = _T("1");
			titleWidth = _T("25");
			titleHeight = _T("25");
			scrollWidth = _T("17");
			scrollHeight = _T("17");
			paletteWidth = _T("17");
			paletteHeight = _T("17");
			menuWidth = _T("18");
			menuHeight = _T("19");
			padding = _T("0");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_VISTA:
			borderWidth = _T("1");
			titleWidth = _T("33");
			titleHeight = _T("20");
			scrollWidth = _T("17");
			scrollHeight = _T("17");
			paletteWidth = _T("17");
			paletteHeight = _T("20");
			menuWidth = _T("19");
			menuHeight = _T("20");
			padding = _T("4");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_7_STD:
			borderWidth = _T("1");
			titleWidth = _T("35");
			titleHeight = _T("21");
			scrollWidth = _T("17");
			scrollHeight = _T("17");
			paletteWidth = _T("17");
			paletteHeight = _T("20");
			menuWidth = _T("19");
			menuHeight = _T("20");
			padding = _T("4");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_7:
			borderWidth = _T("1");
			titleWidth = _T("35");
			titleHeight = _T("21");
			scrollWidth = _T("17");
			scrollHeight = _T("17");
			paletteWidth = _T("17");
			paletteHeight = _T("20");
			menuWidth = _T("19");
			menuHeight = _T("20");
			padding = _T("4");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_8:
			borderWidth = _T("1");
			titleWidth = _T("36");
			titleHeight = _T("22");
			scrollWidth = _T("17");
			scrollHeight = _T("17");
			paletteWidth = _T("22");
			paletteHeight = _T("22");
			menuWidth = _T("19");
			menuHeight = _T("19");
			padding = _T("4");
			UpdateData(false);
			return (INT_PTR)0;
		case IDOK:
		case IDM_SET:
			if (!OnBnClickedOk()) {
				return (INT_PTR)0;
			}
			break;
		case IDM_EXIT:
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)0;
		case IDM_WINVER:
			OnBnClickedWinVer();
			return (INT_PTR)0;
		case IDM_ABOUT:
			MessageBox(hWnd, 
				_T("Re-Metrics Version 1.04\n\nBy Tatsuhiko Syoji(Tatsu) 2012-2014"),
				_T("Re-Metrics�ɂ���"),
				MB_OK | MB_ICONINFORMATION);
			return (INT_PTR)0;
	}
	return BaseDialog::OnCommand(wParam);

}

/**
 * �u�w���v�v�|�u�o�[�W�������v�I�����̏���
 */
void ReMetrics::OnBnClickedWinVer()
{
	// Windows�̓����o�[�W�����𒲂ׂ�B
	DWORD dwVersion = GetVersion();
	TCHAR buf[128];

	DWORD major = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD minor = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// �T�[�o�[���ǂ����̒ǉ������擾���邽��
	// GetVersionEx��OSVERSIONINFOEX��n���ČĂяo���B
	// Windows 98/Me,NT4�ȑO�͍l�����Ȃ��̂ŌĂѕ����͂Ȃ��B
	OSVERSIONINFOEX infoEx;
	memset(&infoEx, 0, sizeof(OSVERSIONINFOEX));
	infoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&infoEx);

	switch(major) {
		case 5:
			switch (minor) {
				case 0:
					_stprintf(buf,
						_T("Windows 2000 (%d.%d)"),
						major,minor);
					break;
				case 1:
					_stprintf(buf,
						_T("Windows XP (%d.%d)"),
						major,minor);
					break;
				case 2:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Windows XP 64bit (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Windows Server 2003 (%d.%d)"),
							major,minor);
					}
					break;
			}
			break;
		case 6:
			switch (minor) {
				case 0:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Windows Vista (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Windows Server 2008 (%d.%d)"),
							major,minor);
					}
					break;
				case 1:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Windows 7 (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Windows Server 2008 R2 (%d.%d)"),
							major,minor);
					}
					break;
				case 2:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Windows 8 (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Windows Server 2012 (%d.%d)"),
							major,minor);
					}
					break;
				case 3:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Windows 8.1 (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Windows Server 2012 R2 (%d.%d)"),
							major,minor);
					}
					break;
				default:
					if (infoEx.wProductType == VER_NT_WORKSTATION) {
						_stprintf(buf,
							_T("Future Windows Client (%d.%d)"),
							major,minor);
					} else {
						_stprintf(buf,
							_T("Future Windows Server (%d.%d)"),
							major,minor);
					}
					break;
			}
			break;
		default:
			if (infoEx.wProductType == VER_NT_WORKSTATION) {
				_stprintf(buf,
					_T("Future Windows Client (%d.%d)"),
					major,minor);
			} else {
				_stprintf(buf,
					_T("Future Windows Server (%d.%d)"),
					major,minor);
			}
			break;
	}

	MessageBox(hWnd, 
		buf,
		_T("Windows�̃o�[�W����"),
		MB_OK | MB_ICONINFORMATION);
}

/**
 * OK�{�^���������̏����B<br>
 * ���͓��e�̌������s������A��ʊe���̕��E�����̐ݒ���s���B
 *
 * @return true:�ݒ���s�����B false:�ݒ���s��Ȃ�
 */
bool ReMetrics::OnBnClickedOk()
{
	UpdateData(true);

	if (borderWidth.length() == 0) {
		MessageBox(hWnd, _T("�E�C���h�E�̋��E�����͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleWidth.length() == 0) {
		MessageBox(hWnd, _T("�^�C�g���o�[�̕������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleHeight.length() == 0) {
		MessageBox(hWnd, _T("�^�C�g���o�[�̍��������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollWidth.length() == 0) {
		MessageBox(hWnd, _T("�X�N���[���o�[�̕������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollHeight.length() == 0) {
		MessageBox(hWnd, _T("�X�N���[���o�[�̍��������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteWidth.length() == 0) {
		MessageBox(hWnd, _T("�p���b�g�̕������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteHeight.length() == 0) {
		MessageBox(hWnd, _T("�p���b�g�̍��������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuWidth.length() == 0) {
		MessageBox(hWnd, _T("���j���[�̕������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuHeight.length() == 0) {
		MessageBox(hWnd, _T("���j���[�̍��������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (padding.length() == 0) {
		MessageBox(hWnd, _T("�E�C���h�E�g�����̕������͂���Ă܂���B"), 
			_T("�G���["), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	DWORD dwver;

	dwver = GetVersion();

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	// �A�C�R���ȊO�̃t�H���g�����擾����B
	if (LOBYTE(dwver) > 5) {
		/* Windows Vista/7/8 */
		metrics.cbSize = sizeof(NONCLIENTMETRICS);
	} else {
		/* Windows 2000/XP */
		metrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
	}

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		metrics.cbSize,
		&metrics,
		0);

	metrics.iBorderWidth = _tstoi(borderWidth.c_str());
	metrics.iCaptionWidth = _tstoi(titleWidth.c_str());
	metrics.iCaptionHeight = _tstoi(titleHeight.c_str());
	metrics.iScrollWidth = _tstoi(scrollWidth.c_str());
	metrics.iScrollHeight = _tstoi(scrollHeight.c_str());
	metrics.iSmCaptionWidth = _tstoi(paletteWidth.c_str());
	metrics.iSmCaptionHeight = _tstoi(paletteHeight.c_str());
	metrics.iMenuWidth = _tstoi(menuWidth.c_str());
	metrics.iMenuHeight = _tstoi(menuHeight.c_str());
	metrics.iPaddedBorderWidth = _tstoi(padding.c_str());

	SystemParametersInfo(SPI_SETNONCLIENTMETRICS,
		metrics.cbSize,
		&metrics,
		SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	// �F���Đݒ肷�邱�Ƃŉ�ʂ����t���b�V������B
	// �̂����AIObit StartMenu 8���N�����Ă���Ƃ���SetSysColors��
	// �Ăяo���Ɖ������Ȃ��Ȃ�̂ŌĂяo�����s��Ȃ����Ƃɂ����B
#if 0
	DWORD btnColor;
	btnColor = GetSysColor(COLOR_BTNTEXT);

	INT colorItems[1];
	colorItems[0] = COLOR_BTNTEXT;
	COLORREF colors[1];
	colors[0] = btnColor;
	SetSysColors(1,colorItems,colors);
#endif

	return true;
}

/**
 * �_�C�A���O���삪�s��ꂽ���ɌĂяo����܂��B
 *
 * @return �������� 0:�������s���� ��0:�������s��Ȃ�
 */
INT_PTR ReMetrics::OnSettingChange(WPARAM wParam, LPARAM lParam)
{
	DWORD dwVersion = GetVersion();

	DWORD major = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	UINT structSize;

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	/* �A�C�R���ȊO�̃t�H���g�����擾����B */
	if (major > 5) {
		structSize = sizeof(NONCLIENTMETRICS);
	} else {
		structSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
	}
	metrics.cbSize = structSize;

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		structSize,
		&metrics,
		0);
	adjustWindowSize(&metrics, major);
	setItemRange();

	return (INT_PTR)TRUE;
}

/**
 * �E�C���h�E�̍����𒲐߂���B
 *
 * @param metrics NONCLIENTMETRICS�\����
 * @param winMajorVer Windows�̃��W���[�o�[�W�����ԍ�
 */
void ReMetrics::adjustWindowSize(NONCLIENTMETRICS *metrics, int winVerMajor)
{
	HWND hWnd = this->getHwnd();
	RECT clientRect;
	RECT windowRect;
	int x,y,width,height;
	int clientWidth, clientHeight;
	int borderWidth;
	int menuHeight;
	int newHeight;
	int requiredClientHeight;

	GetClientRect(hWnd, &clientRect);
	GetWindowRect(hWnd, &windowRect);

	// �E�C���h�E���g�̈ʒu�ƕ�
	x = windowRect.left;
	y = windowRect.top;
	width = windowRect.right - windowRect.left + 1;
	height = windowRect.bottom - windowRect.top + 1;

	// �g�̕�
	borderWidth = metrics->iBorderWidth * 2;
	if (winVerMajor > 5) {
		borderWidth += (metrics->iPaddedBorderWidth * 2);
	}

	// �N���C�A���g�̈�̕�
	clientWidth = clientRect.right - clientRect.left + 1;
	clientHeight = clientRect.bottom - clientRect.top + 1;

	// ���ۂ̃��j���[�̈�̍���
	// (�c�[���o�[��X�e�[�^�X�o�[���������ꍇ�͂��̕��������K�v������B)
	menuHeight = height - borderWidth - clientHeight - metrics->iCaptionHeight;

	// �~�����N���C�A���g�̈�̍���
	requiredClientHeight = 300;

	// �␳��̍���
	newHeight = 
		borderWidth +
		metrics->iCaptionHeight +
		menuHeight +
		requiredClientHeight;

	MoveWindow(hWnd, x, y, width, newHeight, true);
}