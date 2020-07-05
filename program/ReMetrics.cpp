/*
Re-Metrics (C) 2012-2017,2020 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
// ReMetrics.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <process.h>
#include <shellapi.h>
#include <locale.h>
#include <mbctype.h>
#include "ReMetrics.h"
#include "NCFileDialog.h"
#include "util.h"
#include "langdef.h"

#define MAX_LOADSTRING 100
/** �_�C�A���O�e���v���[�g�̃_�C�A���O�̍��� */
const int Height_TemplateUnit = 217;

//
// �_�C�A���O�x�[�X�A�v���P�[�V�����t���[�����[�N��
// ���[�U�[�����̃u���b�W���[�`��
//

// �A�v���P�[�V�����I�u�W�F�N�g
ReMetrics *appObj;
RECT myMonitorLect;
bool firstMonitor = false;
bool useResource = false;
TCHAR langFileName[MAX_PATH];
TCHAR helpFileName[MAX_PATH];

/**
 * �A�v���P�[�V�����I�u�W�F�N�g���쐬���܂��B
 */
DialogAppliBase *createAppli()
{
	// ���P�[���ɉ������������̎��{
	initializeLocale();

	// �����Ń��[�U�[�̃A�v���P�[�V�����I�u�W�F�N�g���쐬���܂��B
	appObj = new ReMetrics();
	return appObj;
}

/**
 * �e����̔���Ɗe����ɍ��킹�����������s���܂��B
 */
void initializeLocale(void)
{
	TCHAR iniPath[MAX_PATH];
	TCHAR *p;
	TCHAR langWork[64];
	TCHAR findPath[MAX_PATH];

	::GetModuleFileName(NULL, iniPath, _MAX_PATH);
	p = _tcsrchr(iniPath, '\\');
	if (p != NULL) {
		*(p + 1) = '\0';
	}

	// ���P�[���̏�����
	char *localeName = setlocale(LC_ALL, "");
	// �R�[�h�y�[�W�̏�����
	char *codePageDelim = strchr(localeName, '.');
	if (codePageDelim != NULL) {
		_setmbcp(atoi(codePageDelim + 1));
		codePage = atoi(codePageDelim + 1);
	} else {
		_setmbcp(_MB_CP_LOCALE);
		codePageDelim = 0;
	}
	mbstowcs(langWork, localeName, 64);

	//localeName = "aaa";
	int readResult;

	// Language detection
	useResource = true;

	_tcscpy(findPath, iniPath);
	p = _tcsrchr(langWork, _T('.'));
	if (p != NULL) {
		*p = _T('\0');
	}
	_tcscat(findPath, langWork);
	_tcscat(findPath, _T(".lng"));
	WIN32_FIND_DATA fileInfo;
		
	HANDLE found = FindFirstFile(findPath, &fileInfo);
	if (found != INVALID_HANDLE_VALUE) {
		// ����_�n��`���̃t�@�C��������ꍇ
		_tcscpy(langFileName, findPath);
		_tcscpy(helpFileName, iniPath);
		_tcscat(helpFileName, langWork);
		_tcscat(helpFileName, _T(".chm"));
	}
	else {
		// ���ꖼ�̐؂�o��
		_tcscpy(findPath, iniPath);
		p = _tcsrchr(langWork, _T('_'));
		if (p != NULL) {
			*p = _T('\0');
		}
		_tcscat(findPath, langWork);
		_tcscat(findPath, _T(".lng"));
		found = FindFirstFile(findPath, &fileInfo);
		if (found != INVALID_HANDLE_VALUE) {
			// ����̃t�@�C��������ꍇ
			_tcscpy(langFileName, findPath);
			_tcscpy(helpFileName, iniPath);
			_tcscat(helpFileName, langWork);
			_tcscat(helpFileName, _T(".chm"));
		} else {
			// ����t�@�C�������݂��Ȃ��ꍇ
			_tcscpy(langFileName, _T(""));
			_tcscpy(helpFileName, iniPath);
			_tcscat(helpFileName, _T("English.chm"));
		}
	}
	// Language support routine ends here.
	readResourceFile(langFileName);
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
	iconHMerginUpDown = NULL;
	iconVMerginUpDown = NULL;

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
	appMenu = NULL;
	setOnStart = false;
	_tcscpy(settingFile, _T(""));

	// �I�v�V�������擾����B
	getOption(lpCmdLine);

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
	
	dpiY = 96;
	getDpi();
	GetNonclientMetrics(&metrics);
	GetIconMetrics(&iconMetrics);

	// �X�s���{�^���ƃe�L�X�g�{�b�N�X��Ή��Â���B
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
	iconHMerginUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_ICON_HOLI));
	iconHMerginUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_ICON_HOLI));
	iconVMerginUpDown = new TwrUpDown(::GetDlgItem(hWnd, IDC_SPIN_ICON_VERT));
	iconVMerginUpDown->setBuddy(::GetDlgItem(hWnd, IDC_EDIT_ICON_VERT));;

	// �e�X�s���{�^���͈̔͂�ݒ肷��B
	setItemRange();

	// NONCLIENTMETRICS�̐ݒ����ʂɔ��f����B
	applyWindowSetting(metrics, iconMetrics);

	adjustWindowSize(&metrics, winVerMajor);

	return 0;
}

/**
* �R�}���h���C���I�v�V�������擾����B
*
* @param lpCmdLine �R�}���h���C��
*/
void ReMetrics::getOption(TCHAR *lpCmdLine)
{
	TCHAR *p;
	TCHAR *paramStart;
	bool firstCommand = false;
	bool capturing = false;
	TCHAR delimiter;
	int argCount = 0;

	p = lpCmdLine;
	while (*p != _T('\0')) {
		if (*p == _T('\"')) {
			if (!capturing) {
				capturing = true;
				delimiter = _T('\"');
				// ���̕�������p�����[�^�J�n
				paramStart = p + 1;
			} else {
				if (delimiter == _T('\"')) {
					// ��͒��ŋ�؂蕶�����_�u���N�H�[�e�[�V�����̏ꍇ
					// �p�����[�^�I���Ƃ���B
					capturing = false;
					*p = _T('\0');
					argCount++;
					// �����Ńp�����[�^�̌��ɉ������������s���B
					parseOption(paramStart, argCount);
				}
			}
		} else if (_istspace(*p)) {
			// �󔒂̏ꍇ
			if (capturing) {
				if (delimiter != _T('\"')) {
					// ��͒��ŋ�؂蕶�����_�u���N�H�[�e�[�V�����łȂ��ꍇ
					// �p�����[�^�I���Ƃ���B
					capturing = false;
					*p = _T('\0');
					argCount++;
					// �����Ńp�����[�^�̌��ɉ������������s���B
					parseOption(paramStart, argCount);
				}
			}
		} else {
			if (!capturing) {
				// �p�����[�^�J�n
				capturing = true;
				paramStart = p;
				delimiter = _T(' ');
			}
		}
		p++;
	}
	if (capturing) {
		// �܂��R�}���h���C����͂������Ă����炱���܂ł��R�}���h���C���Ƃ���B
		argCount++;
		// �����Ńp�����[�^�̌��ɉ������������s���B
		parseOption(paramStart, argCount);
	}

}

/**
 * �I�v�V�����̈ʒu�ɉ�������͂��s���B
 *
 * @param param �p�����[�^
 * @param argCount �I�v�V�����̌�
 */
void ReMetrics::parseOption(TCHAR *param, int argCount)
{
	switch (argCount) {
		case 1:
			// �ݒ�t�@�C����
			if (_tcscmp(_T("--"), param)) {
				if ((_tcschr(param, '\\') == NULL) && (_tcschr(param, '/') == NULL)) {
					GetCurrentDirectory(MAX_PATH, settingFile);
					_tcscat(settingFile, _T("\\"));

				} else {
					settingFile[0] = _T('\0');
				}
				_tcscat(settingFile, param);
			}
			break;
		default:
			if (!_tcscmp(param, _T("-set"))) {
				setOnStart = true;
			}

			break;
	}
}

/**
 * �E�C���h�E�ݒ����ʂɔ��f����B
 *
 * @param newMetrics �E�C���h�E�ݒ�
 */
void ReMetrics::applyWindowSetting(
	NONCLIENTMETRICS &newMetrics,
	ICONMETRICS &newIconMetrics)
{
	TCHAR buf[32];

	// ��ʂɊe���ڂ̃T�C�Y�𔽉f����B
	_stprintf(buf, _T("%d"), newMetrics.iBorderWidth);
	borderWidth = buf;

	_stprintf(buf, _T("%d"), newMetrics.iCaptionWidth);
	titleWidth = buf;

	_stprintf(buf, _T("%d"), newMetrics.iCaptionHeight);
	titleHeight = buf;

	_stprintf(buf, _T("%d"), newMetrics.iScrollWidth);
	scrollWidth = buf;

	_stprintf(buf, _T("%d"), newMetrics.iScrollHeight);
	scrollHeight = buf;

	_stprintf(buf, _T("%d"), newMetrics.iSmCaptionWidth);
	paletteWidth = buf;

	_stprintf(buf, _T("%d"), newMetrics.iSmCaptionHeight);
	paletteHeight = buf;

	_stprintf(buf, _T("%d"), newMetrics.iMenuWidth);
	menuWidth = buf;

	_stprintf(buf, _T("%d"), newMetrics.iMenuHeight);
	menuHeight = buf;

	_stprintf(buf, _T("%d"), newMetrics.iPaddedBorderWidth);
	padding = buf;

	_stprintf(buf, _T("%d"), newIconMetrics.iHorzSpacing - 32);
	iconHMergin = buf;

	_stprintf(buf, _T("%d"), newIconMetrics.iVertSpacing - 32);
	iconVMergin = buf;

	// �X�s���{�^���̈ʒu��ݒ肷��B
	borderWidthUpDown->setPos(newMetrics.iBorderWidth);
	titleWidthUpDown->setPos(newMetrics.iCaptionWidth);
	titleHeightUpDown->setPos(newMetrics.iCaptionHeight);
	scrollWidthUpDown->setPos(newMetrics.iScrollWidth);
	scrollHeightUpDown->setPos(newMetrics.iScrollHeight);
	paletteWidthUpDown->setPos(newMetrics.iSmCaptionWidth);
	paletteHeightUpDown->setPos(newMetrics.iSmCaptionHeight);
	menuWidthUpDown->setPos(newMetrics.iMenuWidth);
	menuHeightUpDown->setPos(newMetrics.iMenuHeight);
	paddingUpDown->setPos(newMetrics.iPaddedBorderWidth);
	iconHMerginUpDown->setPos(newIconMetrics.iHorzSpacing - 32);
	iconVMerginUpDown->setPos(newIconMetrics.iVertSpacing - 32);

	UpdateData(false);

}

/**
 * �X�s���{�^���̓���͈͂�ݒ肷��B
 */
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

	iconHMerginUpDown->setRange(0, 150);
	iconVMerginUpDown->setRange(0, 150);
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
	if (appMenu != NULL) {
		delete appMenu;
	}
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

	appMenu = new TwrMenu(hWnd);
	// �����ݒ�ł͕ʃX���b�h�ŉ�ʂ̊e���ڂ̕��E������ݒ肷��悤�ɂ���B
	appMenu->CheckMenuItem(IDM_ANOTHER, true);

	applyResource();

	if (settingFile[0] != _T('\0')) {
		BOOL loadResult;
		loadResult = startLoadWindowItem(settingFile);
		if (!loadResult) {
			// NONCLIENTMETRICS�̐ݒ����ʂɔ��f����B
			GetNonclientMetrics(&metrics);
			GetIconMetrics(&iconMetrics);
			applyWindowSetting(metrics, iconMetrics);
		} else {
			if (setOnStart) {
				setMetrics(&metrics, &iconMetrics);
				EndDialog(hWnd, 0);

				return (INT_PTR)FALSE;
			}
		}
	}

	UpdateData(false);

	EnumDisplayMonitors(NULL, NULL, MonitorNearMouseCallback, 0);

	adjustCenter(myMonitorLect, HWND_TOP, this->hWnd);


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
	DDX_Text(toObj, IDC_EDIT_ICON_HOLI, iconHMergin);
	DDX_Text(toObj, IDC_EDIT_ICON_VERT, iconVMergin);
}

/**
 * WM_COMMAND���b�Z�[�W�̏�������B<br>
 * ���͓��e�̌������s������A��ʊe���̕��E�����̐ݒ���s���B
 *
 * @param wParam WPARAM�̒l
 * @return �������� 0:�������s���� ��0:�������s��Ȃ�
 */
INT_PTR ReMetrics::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD hiWord = HIWORD(wParam);

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
			iconHMergin = _T("46");
			iconVMergin = _T("43");
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
			iconHMergin = _T("46");
			iconVMergin = _T("43");
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
			iconHMergin = _T("46");
			iconVMergin = _T("43");
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
			iconHMergin = _T("46");
			iconVMergin = _T("43");
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
			iconHMergin = _T("68");
			iconVMergin = _T("43");
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
			iconHMergin = _T("68");
			iconVMergin = _T("43");
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
			iconHMergin = _T("68");
			iconVMergin = _T("43");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_SET_10:
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
			iconHMergin = _T("68");
			iconVMergin = _T("43");
			UpdateData(false);
			return (INT_PTR)0;
		case IDM_OPEN:
			OnLoad();
			return (INT_PTR)0;
		case IDM_SAVE:
			OnSave();
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
		case IDM_ANOTHER:
			if (appMenu->isChecked(IDM_ANOTHER)) {
				appMenu->CheckMenuItem(IDM_ANOTHER, false);
			} else {
				appMenu->CheckMenuItem(IDM_ANOTHER, true);
			}
			return (INT_PTR)0;
		case IDM_HELPTOPIC:
			showHelp();
			return (INT_PTR)0;
		case IDM_WINVER:
			OnBnClickedWinVer();
			return (INT_PTR)0;
		case IDM_ABOUT:
			MessageBox(hWnd, 
				(langResource[DLG_ABOUT_CREDIT] + _T("\n") + langResource[DLG_ABOUT_CREDIT2]).c_str(),
				langResource[DLG_ABOUT_TITLE].c_str(),
				MB_OK | MB_ICONINFORMATION);
			return (INT_PTR)0;
		default:
			if (hiWord == EN_KILLFOCUS) {
				return OnLostFocus(wParam, lParam);
			}
			break;
	}
	return BaseDialog::OnCommand(wParam, lParam);

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
		case 10:
			if (infoEx.wProductType == VER_NT_WORKSTATION) {
				getWin10Ver(buf, major, minor);
			} else {
				_stprintf(buf,
					_T("Windows Server 2016/2019 (%d.%d)"),
					major, minor);
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
		langResource[DLG_WINVER_TITLE].c_str(),
		MB_OK | MB_ICONINFORMATION);
}

/**
 * Windows 10�̃o�[�W�������擾����B
 *
 * @param buf �o�[�W�����ԍ��i�[��
 * @param major ���W���[�o�[�W����
 * @param minor �}�C�i�[�o�[�W����
 */
void ReMetrics::getWin10Ver(TCHAR *buf, DWORD major, DWORD minor)
{
	TCHAR release[8];
	TCHAR build[8];
	DWORD ubr = 0;

	HKEY key;
	LONG result;
	DWORD size;

	_tcscpy(release, _T("????"));
	_tcscpy(build, _T("?"));

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		0, KEY_READ, &key);
	if (result == ERROR_SUCCESS) {
		size = sizeof(TCHAR) * 8;
		RegQueryValueEx(key, _T("ReleaseId"), NULL, NULL, (LPBYTE)release, (LPDWORD)&size);
		size = sizeof(TCHAR) * 8;
		RegQueryValueEx(key, _T("CurrentBuild"), NULL, NULL, (LPBYTE)build, (LPDWORD)&size);
		size = sizeof(DWORD);
		RegQueryValueEx(key, _T("UBR"), NULL, NULL, (LPBYTE)&ubr, (LPDWORD)&size);
		RegCloseKey(key);
	}

	_stprintf(buf,
		_T("Windows 10 (%d.%d) Version %s Build %s.%d"),
		major, minor, release, build, ubr);

}

/**
 * �t�@�C���_�C�A���O�p�̃t�@�C���}�X�N������쐬
 *
 * @param �t�@�C���}�X�N������o�b�t�@
 * @param �t�@�C���̎�ޖ�
 * @param �t�@�C���̃}�X�N
 * @param ���ׂẴt�@�C���̎�ޖ�
 * @param ���ׂẴt�@�C���̃}�X�N
 */
void createFileMask(
	TCHAR* buf,
	const TCHAR* fileMsg,
	const TCHAR* fileExt,
	const TCHAR* allMsg,
	const TCHAR* allExt)
{
	TCHAR* pDst = buf;
	int len;

	len = _tcslen(fileMsg);
	for (int i = 0; i < len; i++) {
		*pDst = fileMsg[i];
		pDst++;
	}
	*pDst = _T('\0');
	pDst++;

	len = _tcslen(fileExt);
	for (int i = 0; i < len; i++) {
		*pDst = fileExt[i];
		pDst++;
	}
	*pDst = _T('\0');
	pDst++;

	len = _tcslen(allMsg);
	for (int i = 0; i < len; i++) {
		*pDst = allMsg[i];
		pDst++;
	}
	*pDst = _T('\0');
	pDst++;

	len = _tcslen(allExt);
	for (int i = 0; i < len; i++) {
		*pDst = allExt[i];
		pDst++;
	}
	*pDst = _T('\0');
	pDst++;

	*pDst = _T('\0');
	pDst++;
}



/**
* �E�C���h�E�ݒ��ۑ������I���������̓���
*/
void ReMetrics::OnLoad()
{
	TCHAR buf[256];

	createFileMask(
		buf,
		langResource[MSG_SETTING_FILE].c_str(),
		_T("*.ini"),
		langResource[MSG_ALL_FILE].c_str(),
		_T("*.*")
	);

	NCFileDialog *dlg = new NCFileDialog(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		buf,
		this->getHwnd(),
		0);

	int result = dlg->DoModal();
	if (result != IDOK) {
		delete dlg;
		return;
	}

	BOOL loadResult;
	loadResult = startLoadWindowItem(dlg->GetPathName());
	if (!loadResult) {
		MessageBox(
			this->getHwnd(),
			langResource[MSG_LOAD_FAIL].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
	}

	delete dlg;
}

/**
 * �E�C���h�E�e���̕��E�������ǂݍ��݂��J�n����B
 *
 * @param filename ini�t�@�C����
 * @result TRUE:�ۑ����� FALSE:�ۑ����s
 */
BOOL ReMetrics::startLoadWindowItem(TCHAR *filename)
{
	BOOL loadResult;
	NONCLIENTMETRICS newMetrics;
	ICONMETRICS newIconMetrics;

	TCHAR buf[32];
	DWORD result;

	GetNonclientMetrics(&newMetrics);
	GetIconMetrics(&newIconMetrics);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("BorderWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iBorderWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("CaptionWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iCaptionWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("CaptionHeight"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iCaptionHeight = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("ScrollWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iScrollWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("ScrollHeight"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iScrollHeight = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("SmCaptionWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iSmCaptionWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("SmCaptionHeight"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iSmCaptionHeight = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("MenuWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iMenuWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("MenuHeight"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iMenuHeight = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T("PaddedBorderWidth"),
		_T(""),
		buf,
		31,
		filename);
	if ((!result) || (!isNumStr(buf))) {
		return FALSE;
	}
	newMetrics.iPaddedBorderWidth = _tstoi(buf);

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T(ICON_HOL_SPACING_KEY),
		_T(""),
		buf,
		31,
		filename);
	if (result && isNumStr(buf)) {
		newIconMetrics.iHorzSpacing = _tstoi(buf);
	}

	buf[31] = _T('\0');
	result = GetPrivateProfileString(_T("WindowItemSize"),
		_T(ICON_VER_SPACING_KEY),
		_T(""),
		buf,
		31,
		filename);
	if (result && isNumStr(buf)) {
		newIconMetrics.iVertSpacing = _tstoi(buf);
	}

	metrics = newMetrics;
	iconMetrics = newIconMetrics;

	// NONCLIENTMETRICS�̐ݒ����ʂɔ��f����B
	applyWindowSetting(metrics, newIconMetrics);

	return TRUE;
}

/**
 * �����񂪂��ׂĐ����ō\������Ă��邩�ǂ������肷��B
 *
 * @param buf ���肷�镶����
 * @return true:���ׂĐ��� false:�����ȊO�̕��������݂���B
 */
bool ReMetrics::isNumStr(TCHAR *buf)
{
	TCHAR *p = buf;

	while (*p) {
		if (!_istdigit(*p)) {
			return false;
		}
		p++;
	}
	return true;
}

/**
* �E�C���h�E�ݒ��ۑ������I���������̓���
*/
void ReMetrics::OnSave()
{
	UpdateData(true);

	// ���̓`�F�b�N���s���B
	if (!isValidInput()) {
		return;
	}
	// ��ʂ̍��ڂ𔽉f����B
	screenToMetrics();

	TCHAR buf[256];

	createFileMask(
		buf,
		langResource[MSG_SETTING_FILE].c_str(),
		_T("*.ini"),
		langResource[MSG_ALL_FILE].c_str(),
		_T("*.*")
	);

	NCFileDialog *dlg = new NCFileDialog(
		FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		buf,
		this->getHwnd(),
		0);

	int result = dlg->DoModal();
	if (result != IDOK) {
		delete dlg;
		return;
	}

	BOOL saveResult;
	saveResult = startSaveWindowItem(dlg->GetPathName());
	if (!saveResult) {
		MessageBox(
			this->getHwnd(),
			langResource[MSG_SAVE_FAIL].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
	}

	delete dlg;
}

/**
* �E�C���h�E���ۑ����J�n����B
*
* @param filename ini�t�@�C����
* @result TRUE:�ۑ����� FALSE:�ۑ����s
*/
BOOL ReMetrics::startSaveWindowItem(TCHAR *filename)
{
	BOOL result;
	TCHAR buf[32];

	_stprintf(buf, _T("%d"), metrics.iBorderWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("BorderWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iCaptionWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("CaptionWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iCaptionHeight);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("CaptionHeight"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iScrollWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("ScrollWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iScrollHeight);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("ScrollHeight"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iSmCaptionWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("SmCaptionWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iSmCaptionHeight);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("SmCaptionHeight"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iMenuWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("MenuWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iMenuHeight);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("MenuHeight"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), metrics.iPaddedBorderWidth);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T("PaddedBorderWidth"),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), iconMetrics.iHorzSpacing);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T(ICON_HOL_SPACING_KEY),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	_stprintf(buf, _T("%d"), iconMetrics.iVertSpacing);
	result = WritePrivateProfileString(_T("WindowItemSize"),
		_T(ICON_VER_SPACING_KEY),
		buf,
		filename);
	if (!result) {
		return FALSE;
	}

	return TRUE;
}

/**
 * ���͍��ڂɌ�肪���邩��������
 *
 * @return true:���͂Ȃ� FALSE:���͍��ڂɌ�肪������
 */
bool ReMetrics::isValidInput(void)
{
	UpdateData(true);

	if (borderWidth.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_WINDOW_BORDER].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleWidth.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_TITLEBAR_WIDTH].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleHeight.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_TITLEBAR_HEIGHT].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollWidth.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_SCROLL_WIDTH].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollHeight.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_SCROLL_HEIGHT].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteWidth.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_PALETTE_WIDTH].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteHeight.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_PALETTE_HEIGHT].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuWidth.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_MENU_WIDTH].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuHeight.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_MENU_HEIGHT].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (padding.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_WINDOW_PADDING].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (iconHMergin.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_ICON_HORIZONTAL_MARGIN].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (iconVMergin.length() == 0) {
		MessageBox(hWnd,
			langResource[MSG_NO_ICON_VERTICAL_MARGIN].c_str(),
			langResource[MSG_ERROR].c_str(),
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

/**
 * ��ʂ̓��͒l������̐ݒ�l�ɔ��f����B
 */
void ReMetrics::screenToMetrics(void)
{
	GetNonclientMetrics(&metrics);
	GetIconMetrics(&iconMetrics);

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
	iconMetrics.iHorzSpacing = _tstoi(iconHMergin.c_str()) + 32;
	iconMetrics.iVertSpacing = _tstoi(iconVMergin.c_str()) + 32;

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

	// ���̓`�F�b�N���s���B
	if (!isValidInput()) {
		return false;
	}

	// ��ʂ̍��ڂ𔽉f����B
	screenToMetrics();

	setMetrics(&metrics, &iconMetrics);

	return true;
}

/**
 * NONCLIENTMETRICS�\���̂��擾����B
 *
 * @param target NONCLIENTMETRICS�\���̂ւ̃|�C���^
 */
void ReMetrics::GetNonclientMetrics(NONCLIENTMETRICS *target)
{
	DWORD dwver;

	dwver = GetVersion();

	FillMemory(target, sizeof(NONCLIENTMETRICS), 0x00);

	// �A�C�R���ȊO�̃t�H���g�����擾����B
	if (LOBYTE(dwver) > 5) {
		/* Windows Vista/7/8 */
		target->cbSize = sizeof(NONCLIENTMETRICS);
	}
	else {
		/* Windows 2000/XP */
		target->cbSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
	}

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		target->cbSize,
		target,
		0);
}

/**
 * ICONMETRICS�\���̂��擾����B
 *
 * @param target ICONMETRICS�\���̂ւ̃|�C���^
 */
void ReMetrics::GetIconMetrics(ICONMETRICS *target)
{

	FillMemory(target, sizeof(ICONMETRICS), 0x00);

	target->cbSize = sizeof(ICONMETRICS);
	SystemParametersInfo(SPI_GETICONMETRICS,
		sizeof(ICONMETRICS),
		target,
		0);
}

NONCLIENTMETRICS *s_fontMetrics;
ICONMETRICS *s_iconMetrics;

/**
 * �X���b�h�ŕ���ݒ肷��B
 *
 * @return 0
 */
unsigned _stdcall setOnThread(void *p)
{
	SystemParametersInfo(SPI_SETICONMETRICS,
		sizeof(ICONMETRICS),
		s_iconMetrics,
		SPIF_UPDATEINIFILE); // | SPIF_SENDCHANGE);

	SystemParametersInfo(SPI_SETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS),
		s_fontMetrics,
		SPIF_UPDATEINIFILE); // | SPIF_SENDCHANGE);

	_endthreadex(0);
	return 0;
}

/**
 * ��ʊe���̕���ݒ肷��B
 *
 * @param fontMetrics ���w��pNONCLIENTMETRICS
 */
void ReMetrics::setMetrics(
	NONCLIENTMETRICS *fontMetrics,
	ICONMETRICS *newIconMetrics
) {

	DWORD_PTR ptr;
	LRESULT messageResult;

	// ���ݒ�
	if (appMenu->isChecked(IDM_ANOTHER)) {
		// UI�ƕʃX���b�h��SystemParametersInfo(SPI_SETNONCLIENTMETRICS��
		// ���s����B
		s_fontMetrics = fontMetrics;
		s_iconMetrics = newIconMetrics;

		uintptr_t startResult = _beginthreadex(NULL, 0, setOnThread, NULL, 0, NULL);
		if (startResult != 0) {
			// ����ɃX���b�h���J�n������X���b�h�I����ҋ@����B
			HANDLE handle;
			handle = (HANDLE)startResult;

			// �ꉞ5�b�قǑ҂�
			DWORD waitResult = WaitForSingleObject(handle, 5000);
			if (waitResult == WAIT_TIMEOUT) {
				// �X���b�h���I�����Ȃ��ꍇ�͂ǂ����悤���Ȃ��̂ŃX���b�h���I������B
				TerminateThread(handle, 0);
			}
			CloseHandle(handle);
		}
	} else {
		// UI�Ɠ����X���b�h��SystemParametersInfo(SPI_SETNONCLIENTMETRICS��
		// ���s����B
		SystemParametersInfo(SPI_SETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS),
			fontMetrics,
			SPIF_UPDATEINIFILE); // | SPIF_SENDCHANGE);

		SystemParametersInfo(SPI_SETICONMETRICS,
			sizeof(ICONMETRICS),
			newIconMetrics,
			SPIF_UPDATEINIFILE); // | SPIF_SENDCHANGE);
	}

	messageResult = SendMessageTimeout(
		HWND_BROADCAST,
		WM_SETTINGCHANGE,
		SPI_SETNONCLIENTMETRICS,
		(LPARAM)_T("WindowMetrics"),
		SMTO_ABORTIFHUNG,
		5000,
		&ptr);
	if (messageResult == 0) {
		if (GetLastError() == ERROR_TIMEOUT) {
			MessageBox(this->getHwnd(), 
				_T("SendMessage timeout."),
				_T("Information"),
				MB_OK | MB_ICONEXCLAMATION);
		}
	}

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

	SystemParametersInfo(
		SPI_GETICONMETRICS,
		sizeof(ICONMETRICS),
		&iconMetrics,
		0);

	// adjustWindowSize(&metrics, major);
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
	LONG baseUnits;

	// �E�C���h�E�̎l���̈ʒu���擾����B
	GetWindowRect(hWnd, &windowRect);

	// �l���̈ʒu����E�C���h�E���g�̈ʒu�ƕ����Z�o����B
	x = windowRect.left;
	y = windowRect.top;
	width = windowRect.right - windowRect.left + 1;
	height = windowRect.bottom - windowRect.top + 1;

	// �g�̕�
	borderWidth = metrics->iBorderWidth * 2;
	if (winVerMajor > 5) {
		borderWidth += (metrics->iPaddedBorderWidth * 2);
	}

	// �N���C�A���g�̎l�����擾����B
	GetClientRect(hWnd, &clientRect);

	// �N���C�A���g�̈�̕�
	clientWidth = clientRect.right - clientRect.left + 1;
	clientHeight = clientRect.bottom - clientRect.top + 1;

	// ���ۂ̃��j���[�̈�̍���
	// (�c�[���o�[��X�e�[�^�X�o�[���������ꍇ�͂��̕��������K�v������B)
	menuHeight = height - borderWidth - clientHeight - metrics->iCaptionHeight;

	// �~�����N���C�A���g�̈�̍���
	requiredClientHeight = 480 * dpiY / 96;

/*
	RECT rect;
	BOOL result;

	rect.left = 4;
	rect.top = 8;
	result = MapDialogRect(hWnd, &rect);
	if (result) {
		// �_�C�A���O�x�[�X�̒P�ʂ��X�N���[���P�ʂɕϊ�����B
		requiredClientHeight = MulDiv(Height_TemplateUnit, rect.top, 8);
	}
*/

	// �␳��̍���
	newHeight = 
		borderWidth +
		metrics->iCaptionHeight +
		menuHeight +
		requiredClientHeight;

	MoveWindow(hWnd, x, y, width, newHeight, true);
}

/**
 * �_�C�A���O���삪�s��ꂽ���ɌĂяo����܂��B
 *
 * @return �������� 0
 */
INT_PTR ReMetrics::OnLostFocus(WPARAM wParam, LPARAM lParam)
{
	HWND targetWnd = (HWND)lParam;

	UpdateData(true);

	int height;
	TCHAR buf[5];

	if (::GetDlgItem(hWnd, IDC_EDIT_TITLE_HEIGHT) == targetWnd) {
		if (titleHeight.length() == 0) {
			return 0;
		}
		height = _tstoi(titleHeight.c_str());
		if (height < minTitleHeight) {
			_itot_s(minTitleHeight, buf, 5, 10);
			titleHeight = buf;
			UpdateData(false);
		}
	}
	if (::GetDlgItem(hWnd, IDC_EDIT_PALETTE_HEIGHT) == targetWnd) {
		if (paletteHeight.length() == 0) {
			return 0;
		}
		height = _tstoi(paletteHeight.c_str());
		if (height < minPaletteHeight) {
			_itot_s(minPaletteHeight, buf, 5, 10);
			paletteHeight = buf;
			UpdateData(false);
		}
	}
	if (::GetDlgItem(hWnd, IDC_EDIT_MENU_HEIGHT) == targetWnd) {
		if (menuHeight.length() == 0) {
			return 0;
		}
		height = _tstoi(menuHeight.c_str());
		if (height < minMenuHeight) {
			_itot_s(minMenuHeight, buf, 5, 10);
			menuHeight = buf;
			UpdateData(false);
		}
	}

	return 0;
}

/**
 * �h�L�������g�t�@�C����\������B
 *
 */
void ReMetrics::showHelp(void)
{

	// �֘A�t����ꂽ�A�v���Ńh�L�������g�t�@�C����\������B
	ShellExecute(hWnd,_T("open"), helpFileName,NULL,NULL,SW_SHOW);
}

/**
 * DPI���擾����B
 */
void ReMetrics::getDpi()
{
	UINT dpiX = 96;
	HMONITOR hmonitor;
	DWORD dwver;
	dwver = GetVersion();
	int winVer = LOBYTE(dwver);
	int minorVer = HIBYTE(dwver);
	/** Shcore.dll�̃C���X�^���X�n���h�� */
	HINSTANCE hShcore = NULL;
	/** GetDpiPerMonitor API�̃G���g���|�C���g */
	FGETDPIFORMONITOR GetDpiForMonitor = NULL;

	if ((winVer > 6) || ((winVer == 6) && (minorVer >= 3))) {
		hShcore = LoadLibrary(_T("shcore.dll"));
		GetDpiForMonitor = (FGETDPIFORMONITOR)GetProcAddress(hShcore, "GetDpiForMonitor");
	}

	if (GetDpiForMonitor != NULL) {
		/* �܂��A�A�v���P�[�V�����̃E�C���h�E���\������Ă��郂�j�^���擾����B */
		hmonitor = MonitorFromWindow(this->hWnd, MONITOR_DEFAULTTONEAREST);
		/* DPI���擾����B */
		GetDpiForMonitor(hmonitor, 0, &dpiX, &dpiY);
		// hmonitor�͕��Ă͂����Ȃ��B����ƃf�o�b�K�ŃG���[���o��B
	}

}

/**
 * ���\�[�X���e���ڂɐݒ肷��B
 */
void ReMetrics::applyResource()
{
	HDC hDC = GetDC(this->hWnd);

	HFONT displayFont = CreateFont(
		0 - MulDiv(APP_FONTSIZE, dpiY, 72),
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		_tstoi(langResource[FONT_CHARSET].c_str()),
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, // CLEARTYPE_QUALITY,
		FIXED_PITCH | FF_MODERN,
		langResource[FONT_FACE].c_str());

	ReleaseDC(this->hWnd, hDC);

	// �A�v���^�C�g��
	setText(langResource[TITLE].c_str());

	// ���j���[������ύX
	appMenu->setText(0, langResource[MENU_FILE].c_str(), TRUE);
	appMenu->setText(IDM_OPEN, langResource[MENU_FILE_LOAD].c_str(), FALSE);
	appMenu->setText(IDM_SAVE, langResource[MENU_FILE_SAVE].c_str(), FALSE);
	appMenu->setText(IDOK, langResource[MENU_FILE_SET_QUIT].c_str(), FALSE);
	appMenu->setText(IDM_EXIT, langResource[MENU_FILE_QUIT].c_str(), FALSE);

	appMenu->setText(1, langResource[MENU_PRESET].c_str(), TRUE);
	appMenu->setText(IDM_SET_2K, langResource[MENU_PRESET_2000].c_str(), FALSE);
	appMenu->setText(IDM_SET_XP, langResource[MENU_PRESET_XP_CLASSIC].c_str(), FALSE);
	appMenu->setText(IDM_SET_XP_LUNA, langResource[MENU_PRESET_XP].c_str(), FALSE);
	appMenu->setText(IDM_SET_VISTA, langResource[MENU_PRESET_VISTA].c_str(), FALSE);
	appMenu->setText(IDM_SET_7_STD, langResource[MENU_PRESET_7_BASIC].c_str(), FALSE);
	appMenu->setText(IDM_SET_7, langResource[MENU_PRESET_7].c_str(), FALSE);
	appMenu->setText(IDM_SET_8, langResource[MENU_PRESET_8].c_str(), FALSE);
	appMenu->setText(IDM_SET_10, langResource[MENU_PRESET_10].c_str(), FALSE);

	appMenu->setText(2, langResource[MENU_TOOLS].c_str(), TRUE);
	appMenu->setText(IDM_ANOTHER, langResource[MENU_TOOLS_THREAD].c_str(), FALSE);

	appMenu->setText(3, langResource[MENU_HELP].c_str(), TRUE);
	appMenu->setText(IDM_HELPTOPIC, langResource[MENU_HELP_HELP].c_str(), FALSE);
	appMenu->setText(IDM_WINVER, langResource[MENU_HELP_WIN_VER].c_str(), FALSE);
	appMenu->setText(IDM_ABOUT, langResource[MENU_HELP_ABOUT].c_str(), FALSE);

	setChildText(IDC_BORDER_WIDTH, langResource[DLG_BORDER_WIDTH].c_str());
	setChildFont(IDC_BORDER_WIDTH, displayFont);
	setChildText(IDC_TITLEBAR_WIDTH, langResource[DLG_TITLEBAR_WIDTH].c_str());
	setChildFont(IDC_TITLEBAR_WIDTH, displayFont);
	setChildText(IDC_TITLEBAR_HEIGHT, langResource[DLG_TITLEBAR_HEIGHT].c_str());
	setChildFont(IDC_TITLEBAR_HEIGHT, displayFont);
	setChildText(IDC_SCROLL_WIDTH, langResource[DLG_SCROLL_WIDTH].c_str());
	setChildFont(IDC_SCROLL_WIDTH, displayFont);
	setChildText(IDC_SCROLL_HEIGHT, langResource[DLG_SCROLL_HEIGHT].c_str());
	setChildFont(IDC_SCROLL_HEIGHT, displayFont);
	setChildText(IDC_PALETTE_WIDTH, langResource[DLG_PALETTE_WIDTH].c_str());
	setChildFont(IDC_PALETTE_WIDTH, displayFont);
	setChildText(IDC_PALETTE_HEIGHT, langResource[DLG_PALETTE_HEIGHT].c_str());
	setChildFont(IDC_PALETTE_HEIGHT, displayFont);
	setChildText(IDC_MENU_WIDTH, langResource[DLG_MENU_WIDTH].c_str());
	setChildFont(IDC_MENU_WIDTH, displayFont);
	setChildText(IDC_MENU_HEIGHT, langResource[DLG_MENU_HEIGHT].c_str());
	setChildFont(IDC_MENU_HEIGHT, displayFont);
	setChildText(IDC_WINDOW_PADDING, langResource[DLG_WINDOW_PADDING].c_str());
	setChildFont(IDC_WINDOW_PADDING, displayFont);
	setChildText(IDC_ICON_HORIZONTAL_MARGIN, langResource[DLG_ICON_HORIZONTAL_MARGIN].c_str());
	setChildFont(IDC_ICON_HORIZONTAL_MARGIN, displayFont);
	setChildText(IDC_ICON_VERTICAL_MARGIN, langResource[DLG_ICON_VERTICAL_MARGIN].c_str());
	setChildFont(IDC_ICON_VERTICAL_MARGIN, displayFont);

	setChildText(IDOK, langResource[DLG_OK].c_str());
	setChildFont(IDOK, displayFont);
	setChildText(IDCANCEL, langResource[DLG_CANCEL].c_str());
	setChildFont(IDCANCEL, displayFont);

	setChildFont(IDC_EDIT_BORDER, displayFont);
	setChildFont(IDC_EDIT_TITLE_WIDTH, displayFont);
	setChildFont(IDC_EDIT_TITLE_HEIGHT, displayFont);
	setChildFont(IDC_EDIT_SCROLL_WIDTH, displayFont);
	setChildFont(IDC_EDIT_SCROLL_HEIGHT, displayFont);
	setChildFont(IDC_EDIT_PALETTE_WIDTH, displayFont);
	setChildFont(IDC_EDIT_PALETTE_HEIGHT, displayFont);
	setChildFont(IDC_EDIT_MENU_WIDTH, displayFont);
	setChildFont(IDC_EDIT_MENU_HEIGHT, displayFont);
	setChildFont(IDC_EDIT_PADDING, displayFont);
	setChildFont(IDC_EDIT_ICON_HOLI, displayFont);
	setChildFont(IDC_EDIT_ICON_VERT, displayFont);

}

/**
 * �J�[�\���̂��郂�j�^�[�𔻒肷�邽�߂�EnumDisplayMonitors�̃R�[���o�b�N
 *
 * @param hMonitor ���j�^�[�̃n���h��
 * @param hdcMonitor ���j�^�[�̃f�B�X�v���C�R���e�L�X�g
 * @param lprcMonitor ���j�^�[�̍��W���
 * @param dwData EnumDisplayMonitors
 * @return TRUE:�񋓂𑱂��� FALSE:�񋓂���߁A���j�^�[�̍��W�����m�肳����
 */
BOOL CALLBACK MonitorNearMouseCallback(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
)
{
	if (!firstMonitor) {
		// �f�B�X�v���C�̏�񂪉����Ȃ���Ԃ͔�����B
		myMonitorLect = *lprcMonitor;
		firstMonitor = true;
	}

	CURSORINFO cursofInfo;
	cursofInfo.cbSize = sizeof(CURSORINFO);
	BOOL result = GetCursorInfo(&cursofInfo);
	if (result == 0) {
		// �J�[�\���̏��𗘗p�ł��Ȃ��Ƃ��̓v���C�}�����j�^��
		// �J�[�\���̂��郂�j�^�����Ƃ���B
		if ((myMonitorLect.left == 0) && (myMonitorLect.top == 0)) {
			myMonitorLect = *lprcMonitor;
			return FALSE;
		} else {
			return TRUE;
		}
	}

	// �J�[�\���̂��郂�j�^���ǂ������肷��B
	int x = cursofInfo.ptScreenPos.x;
	int y = cursofInfo.ptScreenPos.y;
	if ((x >= lprcMonitor->left) && (x <= lprcMonitor->right)) {
		if ((y >= lprcMonitor->top) && (y <= lprcMonitor->bottom)) {
			myMonitorLect = *lprcMonitor;

			return FALSE;
		}
	}
	return TRUE;
}

/**
 * �E�C���h�E��e�E�C���h�E�̒����ɔz�u����B
 *
 * @param parentRect ���S�ɓ����Ώۂ̗̈�
 * @param parentHWnd �e�E�C���h�E�n���h��
 * @param myHWnd �����񂹂���v�f�̃E�C���h�E�n���h��
 */
void adjustCenter(RECT parentRect, HWND parentHWnd, HWND myHWnd)
{
	int parentWidth, parentHeight;
	int myWidth, myHeight;
	int newTop, newLeft;
	RECT myRect;

	GetWindowRect(myHWnd, &myRect);

	parentWidth = parentRect.right - parentRect.left + 1;
	parentHeight = parentRect.bottom - parentRect.top + 1;

	myWidth = myRect.right - myRect.left + 1;
	myHeight = myRect.bottom - myRect.top + 1;

	if (myWidth >= parentWidth) {
		newLeft = parentRect.left + 1;
	} else {
		newLeft = parentRect.left + MulDiv((parentWidth - myWidth), 45, 100);
	}
	if (myHeight >= parentHeight) {
		newTop = parentRect.top + 1;
	} else {
		newTop = parentRect.top + MulDiv((parentHeight - myHeight), 45, 100);
	}
	SetWindowPos(myHWnd, parentHWnd, newLeft, newTop, myWidth, myHeight, SWP_SHOWWINDOW);

}

