/*
Re-Metrics (C) 2012-2016 Tatsuhiko Shoji
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
#include "ReMetrics.h"
#include "NCFileDialog.h"

#define MAX_LOADSTRING 100
/** ダイアログテンプレートのダイアログの高さ */
const int Height_TemplateUnit = 185;

//
// ダイアログベースアプリケーションフレームワークと
// ユーザー実装のブリッジルーチン
//

// アプリケーションオブジェクト
ReMetrics *appObj;

/**
 * アプリケーションオブジェクトを作成します。
 */
DialogAppliBase *createAppli()
{
	// ここでユーザーのアプリケーションオブジェクトを作成します。
	appObj = new ReMetrics();
	return appObj;
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

	GetNonclientMetrics(&metrics);

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

	// 各スピンボタンの範囲を設定する。
	setItemRange();

	// NONCLIENTMETRICSの設定を画面に反映する。
	applyWindowSetting(metrics);

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
				_tcscpy(settingFile, param);
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
void ReMetrics::applyWindowSetting(NONCLIENTMETRICS &newMetrics)
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

	if (settingFile[0] != _T('\0')) {
		BOOL loadResult;
		loadResult = startLoadWindowItem(settingFile);
		if (!loadResult) {
			// NONCLIENTMETRICSの設定を画面に反映する。
			GetNonclientMetrics(&metrics);
			applyWindowSetting(metrics);
		} else {
			if (setOnStart) {
				setMetrics(&metrics);
				EndDialog(hWnd, 0);

				return (INT_PTR)FALSE;
			}
		}
	}

	UpdateData(false);

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
				_T("Re-Metrics Version 1.11\n\nBy Tatsuhiko Syoji(Tatsu) 2012-2017"),
				_T("Re-Metricsについて"),
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
				_stprintf(buf,
					_T("Windows 10 (%d.%d)"),
					major, minor);
			} else {
				_stprintf(buf,
					_T("Windows Server 2016 (%d.%d)"),
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
		_T("Windowsのバージョン"),
		MB_OK | MB_ICONINFORMATION);
}

/**
* ウインドウ設定を保存するを選択した時の動作
*/
void ReMetrics::OnLoad()
{
	NCFileDialog *dlg = new NCFileDialog(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("設定ファイル(*.ini)\0*.ini\0すべてのファイル(*.*)\0*.*\0\0"),
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
			_T("ウインドウ設定の読み込みに失敗しました。"),
			_T("エラー"),
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

	TCHAR buf[32];
	DWORD result;

	GetNonclientMetrics(&newMetrics);

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

	metrics = newMetrics;

	// NONCLIENTMETRICSの設定を画面に反映する。
	applyWindowSetting(metrics);

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
	NCFileDialog *dlg = new NCFileDialog(
		FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("設定ファイル(*.ini)\0*.ini\0すべてのファイル(*.*)\0*.*\0\0"),
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
			_T("ウインドウ設定の保存に失敗しました。"),
			_T("エラー"),
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

	return TRUE;
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

	if (borderWidth.length() == 0) {
		MessageBox(hWnd, _T("ウインドウの境界が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleWidth.length() == 0) {
		MessageBox(hWnd, _T("タイトルバーの幅が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (titleHeight.length() == 0) {
		MessageBox(hWnd, _T("タイトルバーの高さが入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollWidth.length() == 0) {
		MessageBox(hWnd, _T("スクロールバーの幅が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (scrollHeight.length() == 0) {
		MessageBox(hWnd, _T("スクロールバーの高さが入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteWidth.length() == 0) {
		MessageBox(hWnd, _T("パレットの幅が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (paletteHeight.length() == 0) {
		MessageBox(hWnd, _T("パレットの高さが入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuWidth.length() == 0) {
		MessageBox(hWnd, _T("メニューの幅が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (menuHeight.length() == 0) {
		MessageBox(hWnd, _T("メニューの高さが入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (padding.length() == 0) {
		MessageBox(hWnd, _T("ウインドウ枠内部の幅が入力されてません。"), 
			_T("エラー"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	GetNonclientMetrics(&metrics);

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

	setMetrics(&metrics);

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

NONCLIENTMETRICS *s_fontMetrics;

/**
 * スレッドで幅を設定する。
 *
 * @return 0
 */
unsigned _stdcall setOnThread(void *p)
{

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
	NONCLIENTMETRICS *fontMetrics
) {

	DWORD_PTR ptr;
	LRESULT messageResult;

	// 幅設定
	if (appMenu->isChecked(IDM_ANOTHER)) {
		// UIと別スレッドでSystemParametersInfo(SPI_SETNONCLIENTMETRICSを
		// 実行する。
		s_fontMetrics = fontMetrics;

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
	adjustWindowSize(&metrics, major);
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
	requiredClientHeight = 300;
	RECT rect;
	BOOL result;

	rect.left = 4;
	rect.top = 8;
	if (result = MapDialogRect(hWnd, &rect)) {
		requiredClientHeight = MulDiv(Height_TemplateUnit, rect.top, 8);
	}

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
	// 実行ファイルの情報を得るためのバッファ群
	TCHAR path[_MAX_PATH+1],drive[_MAX_DRIVE+1],dir[_MAX_DIR+1],helpFile[_MAX_PATH+1];

	// 実行ファイルのあるところのBShelp.htmlのパス名を生成する。
	::GetModuleFileName(NULL,path,_MAX_PATH);
	::_tsplitpath(path,drive,dir,NULL,NULL);
	::_stprintf(helpFile,_T("%s%s%s"),drive,dir,_T("ReMetrics.html"));
	
	// 関連付けられたアプリでドキュメントファイルを表示する。
	ShellExecute(hWnd,_T("open"),helpFile,NULL,NULL,SW_SHOW);
}

