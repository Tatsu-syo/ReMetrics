/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
// ReMetrics.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "ReMetrics.h"

#define MAX_LOADSTRING 100

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
	// 本アプリケーションでは特に処理なし。
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

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	// アイコン以外のフォント情報を取得する。
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

	// 各スピンボタンの範囲を設定する。
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
	// 本アプリケーションでは特に処理なし。
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
				_T("Re-Metricsについて"),
				MB_OK | MB_ICONINFORMATION);
			return (INT_PTR)0;
	}
	return BaseDialog::OnCommand(wParam);

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

	DWORD dwver;

	dwver = GetVersion();

	FillMemory(&metrics,sizeof(NONCLIENTMETRICS),0x00);

	// アイコン以外のフォント情報を取得する。
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

	return true;
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

	GetClientRect(hWnd, &clientRect);
	GetWindowRect(hWnd, &windowRect);

	// ウインドウ自身の位置と幅
	x = windowRect.left;
	y = windowRect.top;
	width = windowRect.right - windowRect.left + 1;
	height = windowRect.bottom - windowRect.top + 1;

	// 枠の幅
	borderWidth = metrics->iBorderWidth * 2;
	if (winVerMajor > 5) {
		borderWidth += (metrics->iPaddedBorderWidth * 2);
	}

	// クライアント領域の幅
	clientWidth = clientRect.right - clientRect.left + 1;
	clientHeight = clientRect.bottom - clientRect.top + 1;

	// 実際のメニュー領域の高さ
	// (ツールバーやステータスバーがあった場合はその分も引く必要がある。)
	menuHeight = height - borderWidth - clientHeight - metrics->iCaptionHeight;

	// 欲しいクライアント領域の高さ
	requiredClientHeight = 300;

	// 補正後の高さ
	newHeight = 
		borderWidth +
		metrics->iCaptionHeight +
		menuHeight +
		requiredClientHeight;

	MoveWindow(hWnd, x, y, width, newHeight, true);
}