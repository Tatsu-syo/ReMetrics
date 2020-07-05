/*
Re-Metrics (C) 2012-2017,2020 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
// ReMetrics.cpp : アプリケーションのエントリ ポイントを定義します。
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
/** ダイアログテンプレートのダイアログの高さ */
const int Height_TemplateUnit = 217;

//
// ダイアログベースアプリケーションフレームワークと
// ユーザー実装のブリッジルーチン
//

// アプリケーションオブジェクト
ReMetrics *appObj;
RECT myMonitorLect;
bool firstMonitor = false;
bool useResource = false;
TCHAR langFileName[MAX_PATH];
TCHAR helpFileName[MAX_PATH];

/**
 * アプリケーションオブジェクトを作成します。
 */
DialogAppliBase *createAppli()
{
	// ロケールに応じた初期化の実施
	initializeLocale();

	// ここでユーザーのアプリケーションオブジェクトを作成します。
	appObj = new ReMetrics();
	return appObj;
}

/**
 * 各国語の判定と各国語に合わせた初期化を行います。
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

	// ロケールの初期化
	char *localeName = setlocale(LC_ALL, "");
	// コードページの初期化
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
		// 言語_地域形式のファイルがある場合
		_tcscpy(langFileName, findPath);
		_tcscpy(helpFileName, iniPath);
		_tcscat(helpFileName, langWork);
		_tcscat(helpFileName, _T(".chm"));
	}
	else {
		// 言語名の切り出し
		_tcscpy(findPath, iniPath);
		p = _tcsrchr(langWork, _T('_'));
		if (p != NULL) {
			*p = _T('\0');
		}
		_tcscat(findPath, langWork);
		_tcscat(findPath, _T(".lng"));
		found = FindFirstFile(findPath, &fileInfo);
		if (found != INVALID_HANDLE_VALUE) {
			// 言語のファイルがある場合
			_tcscpy(langFileName, findPath);
			_tcscpy(helpFileName, iniPath);
			_tcscat(helpFileName, langWork);
			_tcscat(helpFileName, _T(".chm"));
		} else {
			// 言語ファイルが存在しない場合
			_tcscpy(langFileName, _T(""));
			_tcscpy(helpFileName, iniPath);
			_tcscat(helpFileName, _T("English.chm"));
		}
	}
	// Language support routine ends here.
	readResourceFile(langFileName);
}


/**
 * ダイアログプローシジャ
 *
 */
INT_PTR CALLBACK MainDialogProc(
  HWND hwndDlg,  // ダイアログボックスのハンドル
  UINT uMsg,     // メッセージ
  WPARAM wParam, // 最初のメッセージパラメータ
  LPARAM lParam  // 2 番目のメッセージパラメータ
  )
{
	// ユーザーアプリケーションのダイアログプローシジャに丸投げします。
	return appObj->dialogProc(hwndDlg, uMsg,wParam,lParam);
}

//
// ダイアログベースアプリケーションフレームワークの実装部
//

/**
 * アプリケーションウインドウクラスのインスタンスを生成します。
 *
 * @return アプリケーションウインドウクラスのインスタンス
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
 * アプリケーションの開始処理を実行します。
 *
 * @param lpCmdLine コマンドライン
 * @return 予備
 */
int ReMetrics::OnAppliStart(TCHAR *lpCmdLine)
{
	// この関数をオーバーライドしてアプリ固有の初期化を行います。
	appMenu = NULL;
	setOnStart = false;
	_tcscpy(settingFile, _T(""));

	// オプションを取得する。
	getOption(lpCmdLine);

	return 0;
}

/**
 * ウインドウが表示されたときの処理を実行します。<br>
 * ウインドウベースアプリとインタフェースを合わせるために用意しています。
 *
 * @return 予備
 */
int ReMetrics::OnWindowShow()
{
	// この関数をオーバーライドして、初回の表示時の処理を行います。
	// このタイミングでダイアログが存在するので、ここに処理を入れることで
	// ダイアログがある状態で起動時の初期化処理を行うことができます。
	DWORD dwver;
	int winVerMajor;

	dwver = GetVersion();
	winVerMajor = LOBYTE(dwver);
	
	dpiY = 96;
	getDpi();
	GetNonclientMetrics(&metrics);
	GetIconMetrics(&iconMetrics);

	// スピンボタンとテキストボックスを対応づける。
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

	// 各スピンボタンの範囲を設定する。
	setItemRange();

	// NONCLIENTMETRICSの設定を画面に反映する。
	applyWindowSetting(metrics, iconMetrics);

	adjustWindowSize(&metrics, winVerMajor);

	return 0;
}

/**
* コマンドラインオプションを取得する。
*
* @param lpCmdLine コマンドライン
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
				// 次の文字からパラメータ開始
				paramStart = p + 1;
			} else {
				if (delimiter == _T('\"')) {
					// 解析中で区切り文字がダブルクォーテーションの場合
					// パラメータ終了とする。
					capturing = false;
					*p = _T('\0');
					argCount++;
					// ここでパラメータの個数に応じた処理を行う。
					parseOption(paramStart, argCount);
				}
			}
		} else if (_istspace(*p)) {
			// 空白の場合
			if (capturing) {
				if (delimiter != _T('\"')) {
					// 解析中で区切り文字がダブルクォーテーションでない場合
					// パラメータ終了とする。
					capturing = false;
					*p = _T('\0');
					argCount++;
					// ここでパラメータの個数に応じた処理を行う。
					parseOption(paramStart, argCount);
				}
			}
		} else {
			if (!capturing) {
				// パラメータ開始
				capturing = true;
				paramStart = p;
				delimiter = _T(' ');
			}
		}
		p++;
	}
	if (capturing) {
		// まだコマンドライン解析が続いていたらここまでをコマンドラインとする。
		argCount++;
		// ここでパラメータの個数に応じた処理を行う。
		parseOption(paramStart, argCount);
	}

}

/**
 * オプションの位置に応じた解析を行う。
 *
 * @param param パラメータ
 * @param argCount オプションの個数
 */
void ReMetrics::parseOption(TCHAR *param, int argCount)
{
	switch (argCount) {
		case 1:
			// 設定ファイル名
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
 * ウインドウ設定を画面に反映する。
 *
 * @param newMetrics ウインドウ設定
 */
void ReMetrics::applyWindowSetting(
	NONCLIENTMETRICS &newMetrics,
	ICONMETRICS &newIconMetrics)
{
	TCHAR buf[32];

	// 画面に各項目のサイズを反映する。
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

	// スピンボタンの位置を設定する。
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
 * スピンボタンの動作範囲を設定する。
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
 * フォントに対応する画面要素の最低の高さを求める。
 *
 * @param font 画面要素に設定されたフォント
 * @return フォントに対応する画面要素の最低の高さ
 */
int ReMetrics::getMinHeight(LOGFONT *font)
{
	// フォントを作成する。
	HFONT hFont = CreateFontIndirect(font);
	// 自身のウインドウハンドルから作成したデバイスコンテキストに
	// フォントを設定する。
	HDC dc = GetDC(this->getHwnd());
	SelectFont(dc, hFont);

	// デバイスコンテキストからTEXTMETRICを取得する。
	TEXTMETRIC metric;
	GetTextMetrics(dc, &metric);

	ReleaseDC(this->getHwnd(), dc);
	DeleteObject(hFont);

	int height;
	if (font->lfHeight < 0) {
		// 負の場合はlfHeightはフォント自体の高さでInternal Leadingを
		// 含まないのでInternal Leadingを加算する。
		height = 0 - font->lfHeight + metric.tmInternalLeading;
	} else if (font->lfHeight > 0) {
		// 正の場合はすでにInternal Leadingを含んでいる。
		height = font->lfHeight;
	} else {
		// 0の場合はデフォルトの大きさを作成したフォントから取得する。
		height = metric.tmAscent + metric.tmDescent;
	}

	// 余白2pxを足した値がフォントに対応する画面要素の最低の高さ
	return height + 2;
}

/**
 * アプリケーションの終了処理を実行します。
 *
 * @return 予備
 */
int ReMetrics::OnAppliEnd()
{
	// この関数をオーバーライドしてアプリ固有の後処理を行います。
	if (appMenu != NULL) {
		delete appMenu;
	}
	return 0;
}

//
// ダイアログベースアプリケーションとしての
// アプリケーション固有処理の実装部分
//

/**
 * ダイアログ初期化処理
 *
 * @return TRUE: FALSE:フォーカスを設定した
 */
INT_PTR ReMetrics::OnInitDialog()
{
	// 親クラスのダイアログ初期化処理を呼ぶ。
	DialogAppliBase::OnInitDialog();

	HICON hIcon;

    hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDC_MYICON), IMAGE_ICON, 16, 16, 0);
    SendMessage(this->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	appMenu = new TwrMenu(hWnd);
	// 初期設定では別スレッドで画面の各項目の幅・高さを設定するようにする。
	appMenu->CheckMenuItem(IDM_ANOTHER, true);

	applyResource();

	if (settingFile[0] != _T('\0')) {
		BOOL loadResult;
		loadResult = startLoadWindowItem(settingFile);
		if (!loadResult) {
			// NONCLIENTMETRICSの設定を画面に反映する。
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
 * ダイアログコントロールとオブジェクトの内容の同期を取る。
 *
 * @param toObj true:コントロール→オブジェクト false:オブジェクト→コントロール
 */
void ReMetrics::UpdateData(bool toObj)
{
	// ここにダイアログのコントロールと同期を取るメンバ変数を記述します。
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
 * WM_COMMANDメッセージの処理分岐。<br>
 * 入力内容の検査を行った後、画面各部の幅・高さの設定を行う。
 *
 * @param wParam WPARAMの値
 * @return 処理結果 0:処理を行った 非0:処理を行わない
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
 * 「ヘルプ」－「バージョン情報」選択時の処理
 */
void ReMetrics::OnBnClickedWinVer()
{
	// Windowsの内部バージョンを調べる。
	DWORD dwVersion = GetVersion();
	TCHAR buf[128];

	DWORD major = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD minor = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// サーバーかどうかの追加情報を取得するため
	// GetVersionExをOSVERSIONINFOEXを渡して呼び出す。
	// Windows 98/Me,NT4以前は考慮しないので呼び分けはなし。
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
 * Windows 10のバージョンを取得する。
 *
 * @param buf バージョン番号格納先
 * @param major メジャーバージョン
 * @param minor マイナーバージョン
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
 * ファイルダイアログ用のファイルマスク文字列作成
 *
 * @param ファイルマスク文字列バッファ
 * @param ファイルの種類名
 * @param ファイルのマスク
 * @param すべてのファイルの種類名
 * @param すべてのファイルのマスク
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
* ウインドウ設定を保存するを選択した時の動作
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
 * ウインドウ各部の幅・高さ情報読み込みを開始する。
 *
 * @param filename iniファイル名
 * @result TRUE:保存成功 FALSE:保存失敗
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

	// NONCLIENTMETRICSの設定を画面に反映する。
	applyWindowSetting(metrics, newIconMetrics);

	return TRUE;
}

/**
 * 文字列がすべて数字で構成されているかどうか判定する。
 *
 * @param buf 判定する文字列
 * @return true:すべて数字 false:数字以外の文字が存在する。
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
* ウインドウ設定を保存するを選択した時の動作
*/
void ReMetrics::OnSave()
{
	UpdateData(true);

	// 入力チェックを行う。
	if (!isValidInput()) {
		return;
	}
	// 画面の項目を反映する。
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
* ウインドウ情報保存を開始する。
*
* @param filename iniファイル名
* @result TRUE:保存成功 FALSE:保存失敗
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
 * 入力項目に誤りがあるか検査する
 *
 * @return true:誤りはない FALSE:入力項目に誤りがあった
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
 * 画面の入力値を内部の設定値に反映する。
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
 * OKボタン押下時の処理。<br>
 * 入力内容の検査を行った後、画面各部の幅・高さの設定を行う。
 *
 * @return true:設定を行った。 false:設定を行わない
 */
bool ReMetrics::OnBnClickedOk()
{
	UpdateData(true);

	// 入力チェックを行う。
	if (!isValidInput()) {
		return false;
	}

	// 画面の項目を反映する。
	screenToMetrics();

	setMetrics(&metrics, &iconMetrics);

	return true;
}

/**
 * NONCLIENTMETRICS構造体を取得する。
 *
 * @param target NONCLIENTMETRICS構造体へのポインタ
 */
void ReMetrics::GetNonclientMetrics(NONCLIENTMETRICS *target)
{
	DWORD dwver;

	dwver = GetVersion();

	FillMemory(target, sizeof(NONCLIENTMETRICS), 0x00);

	// アイコン以外のフォント情報を取得する。
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
 * ICONMETRICS構造体を取得する。
 *
 * @param target ICONMETRICS構造体へのポインタ
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
 * スレッドで幅を設定する。
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
 * 画面各部の幅を設定する。
 *
 * @param fontMetrics 幅指定用NONCLIENTMETRICS
 */
void ReMetrics::setMetrics(
	NONCLIENTMETRICS *fontMetrics,
	ICONMETRICS *newIconMetrics
) {

	DWORD_PTR ptr;
	LRESULT messageResult;

	// 幅設定
	if (appMenu->isChecked(IDM_ANOTHER)) {
		// UIと別スレッドでSystemParametersInfo(SPI_SETNONCLIENTMETRICSを
		// 実行する。
		s_fontMetrics = fontMetrics;
		s_iconMetrics = newIconMetrics;

		uintptr_t startResult = _beginthreadex(NULL, 0, setOnThread, NULL, 0, NULL);
		if (startResult != 0) {
			// 正常にスレッドを開始したらスレッド終了を待機する。
			HANDLE handle;
			handle = (HANDLE)startResult;

			// 一応5秒ほど待つ
			DWORD waitResult = WaitForSingleObject(handle, 5000);
			if (waitResult == WAIT_TIMEOUT) {
				// スレッドが終了しない場合はどうしようもないのでスレッドを終了する。
				TerminateThread(handle, 0);
			}
			CloseHandle(handle);
		}
	} else {
		// UIと同じスレッドでSystemParametersInfo(SPI_SETNONCLIENTMETRICSを
		// 実行する。
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

	// 色を再設定することで画面をリフレッシュする。
	// のだが、IObit StartMenu 8が起動しているときはSetSysColorsを
	// 呼び出すと応答がなくなるので呼び出しを行わないことにした。
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
 * ダイアログ操作が行われた時に呼び出されます。
 *
 * @return 処理結果 0:処理を行った 非0:処理を行わない
 */
INT_PTR ReMetrics::OnSettingChange(WPARAM wParam, LPARAM lParam)
{
	DWORD dwVersion = GetVersion();

	DWORD major = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	UINT structSize;

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	/* アイコン以外のフォント情報を取得する。 */
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
 * ウインドウの高さを調節する。
 *
 * @param metrics NONCLIENTMETRICS構造体
 * @param winMajorVer Windowsのメジャーバージョン番号
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

	// ウインドウの四隅の位置を取得する。
	GetWindowRect(hWnd, &windowRect);

	// 四隅の位置からウインドウ自身の位置と幅を算出する。
	x = windowRect.left;
	y = windowRect.top;
	width = windowRect.right - windowRect.left + 1;
	height = windowRect.bottom - windowRect.top + 1;

	// 枠の幅
	borderWidth = metrics->iBorderWidth * 2;
	if (winVerMajor > 5) {
		borderWidth += (metrics->iPaddedBorderWidth * 2);
	}

	// クライアントの四隅を取得する。
	GetClientRect(hWnd, &clientRect);

	// クライアント領域の幅
	clientWidth = clientRect.right - clientRect.left + 1;
	clientHeight = clientRect.bottom - clientRect.top + 1;

	// 実際のメニュー領域の高さ
	// (ツールバーやステータスバーがあった場合はその分も引く必要がある。)
	menuHeight = height - borderWidth - clientHeight - metrics->iCaptionHeight;

	// 欲しいクライアント領域の高さ
	requiredClientHeight = 480 * dpiY / 96;

/*
	RECT rect;
	BOOL result;

	rect.left = 4;
	rect.top = 8;
	result = MapDialogRect(hWnd, &rect);
	if (result) {
		// ダイアログベースの単位をスクリーン単位に変換する。
		requiredClientHeight = MulDiv(Height_TemplateUnit, rect.top, 8);
	}
*/

	// 補正後の高さ
	newHeight = 
		borderWidth +
		metrics->iCaptionHeight +
		menuHeight +
		requiredClientHeight;

	MoveWindow(hWnd, x, y, width, newHeight, true);
}

/**
 * ダイアログ操作が行われた時に呼び出されます。
 *
 * @return 処理結果 0
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
 * ドキュメントファイルを表示する。
 *
 */
void ReMetrics::showHelp(void)
{

	// 関連付けられたアプリでドキュメントファイルを表示する。
	ShellExecute(hWnd,_T("open"), helpFileName,NULL,NULL,SW_SHOW);
}

/**
 * DPIを取得する。
 */
void ReMetrics::getDpi()
{
	UINT dpiX = 96;
	HMONITOR hmonitor;
	DWORD dwver;
	dwver = GetVersion();
	int winVer = LOBYTE(dwver);
	int minorVer = HIBYTE(dwver);
	/** Shcore.dllのインスタンスハンドル */
	HINSTANCE hShcore = NULL;
	/** GetDpiPerMonitor APIのエントリポイント */
	FGETDPIFORMONITOR GetDpiForMonitor = NULL;

	if ((winVer > 6) || ((winVer == 6) && (minorVer >= 3))) {
		hShcore = LoadLibrary(_T("shcore.dll"));
		GetDpiForMonitor = (FGETDPIFORMONITOR)GetProcAddress(hShcore, "GetDpiForMonitor");
	}

	if (GetDpiForMonitor != NULL) {
		/* まず、アプリケーションのウインドウが表示されているモニタを取得する。 */
		hmonitor = MonitorFromWindow(this->hWnd, MONITOR_DEFAULTTONEAREST);
		/* DPIを取得する。 */
		GetDpiForMonitor(hmonitor, 0, &dpiX, &dpiY);
		// hmonitorは閉じてはいけない。閉じるとデバッガでエラーが出る。
	}

}

/**
 * リソースを各項目に設定する。
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

	// アプリタイトル
	setText(langResource[TITLE].c_str());

	// メニュー文字列変更
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
 * カーソルのいるモニターを判定するためのEnumDisplayMonitorsのコールバック
 *
 * @param hMonitor モニターのハンドル
 * @param hdcMonitor モニターのディスプレイコンテキスト
 * @param lprcMonitor モニターの座標情報
 * @param dwData EnumDisplayMonitors
 * @return TRUE:列挙を続ける FALSE:列挙をやめ、モニターの座標情報を確定させる
 */
BOOL CALLBACK MonitorNearMouseCallback(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
)
{
	if (!firstMonitor) {
		// ディスプレイの情報が何もない状態は避ける。
		myMonitorLect = *lprcMonitor;
		firstMonitor = true;
	}

	CURSORINFO cursofInfo;
	cursofInfo.cbSize = sizeof(CURSORINFO);
	BOOL result = GetCursorInfo(&cursofInfo);
	if (result == 0) {
		// カーソルの情報を利用できないときはプライマリモニタを
		// カーソルのいるモニタ扱いとする。
		if ((myMonitorLect.left == 0) && (myMonitorLect.top == 0)) {
			myMonitorLect = *lprcMonitor;
			return FALSE;
		} else {
			return TRUE;
		}
	}

	// カーソルのいるモニタかどうか判定する。
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
 * ウインドウを親ウインドウの中央に配置する。
 *
 * @param parentRect 中心に入れる対象の領域
 * @param parentHWnd 親ウインドウハンドル
 * @param myHWnd 中央寄せする要素のウインドウハンドル
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

