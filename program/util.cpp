/*
Re-Metrics (C) 2012-2017,2021 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/

#include "util.h"
#include "iniReader.h"

/** 言語リソース */
std::vector<tstring> langResource;

int codePage = 0;

/**
 * リソースの読み込みを行う。
 *
 * @param file リソースファイル名
 * @param key キー名
 * @param 読み込めなかった時の文字列
 */
void readResourceItem(TCHAR *file, const TCHAR *key, const TCHAR *fallback)
{
	TCHAR buf[255];
	int len = 0;

	if (_tcslen(file) != 0) {
		// INIファイルを読み込む。Unicode版のAPIでもファイルが非Unicodeの場合は
		// 各言語の文字コードのファイルとして読んでくれる。
		if (codePage == 0) {
			len = GetPrivateProfileString(_T("RESOURCE"), key, _T(""), buf, 255, file);
		}
		else {
			len = GetPrivateProfileStringExT("RESOURCE", key, _T(""), buf, 255, file, codePage);
		}
	}

	if (len > 0) {
		langResource.push_back(buf);
	} else {
		langResource.push_back(fallback);
	}

}

/**
 * リソースの読み込みを開始する。
 *
 * @param file リソースファイル名
 */
void readResourceFile(TCHAR *file)
{
	readResourceItem(file, _T("FONT_FACE"),
		_T("Segoe UI")
	);
	readResourceItem(file, _T("FONT_CHARSET"),
		_T("1")
	);
	readResourceItem(file, _T("TITLE"),
		_T("Re-Metrics")
	);
	readResourceItem(file, _T("MENU_FILE"),
		_T("&File")
	);
	readResourceItem(file, _T("MENU_FILE_LOAD"),
		_T("L&oad window settings...")
	);
	readResourceItem(file, _T("MENU_FILE_SAVE"),
		_T("S&ave window settings...")
	);
	readResourceItem(file, _T("MENU_FILE_SET_QUIT"),
		_T("&Set window settings and quit")
	);
	readResourceItem(file, _T("MENU_FILE_QUIT"),
		_T("E&xit")
	);
	readResourceItem(file, _T("MENU_PRESET"),
		_T("&Preset")
	);
	readResourceItem(file, _T("MENU_PRESET_2000"),
		_T("Windows 2000")
	);
	readResourceItem(file, _T("MENU_PRESET_XP_Classic"),
		_T("Windows XP Classic")
	);
	readResourceItem(file, _T("MENU_PRESET_XP"),
		_T("Windows XP")
	);
	readResourceItem(file, _T("MENU_PRESET_VISTA"),
		_T("Windows Vista")
	);
	readResourceItem(file, _T("MENU_PRESET_7_BASIC"),
		_T("Windows 7 Basic")
	);
	readResourceItem(file, _T("MENU_PRESET_7"),
		_T("Windows 7")
	);
	readResourceItem(file, _T("MENU_PRESET_8"),
		_T("Windows 8/8.1")
	);
	readResourceItem(file, _T("MENU_PRESET_10"),
		_T("Windows 10")
	);
	readResourceItem(file, _T("MENU_TOOLS"),
		_T("&Tools")
	);
	readResourceItem(file, _T("MENU_TOOLS_THREAD"),
		_T("Set window settings in separated thread")
	);
	readResourceItem(file, _T("MENU_HELP"),
		_T("&Help")
	);
	readResourceItem(file, _T("MENU_HELP_HELP"),
		_T("&Help")
	);
	readResourceItem(file, _T("MENU_HELP_WIN_VER"),
		_T("&Windows Version")
	);
	readResourceItem(file, _T("MENU_HELP_ABOUT"),
		_T("&About Re-Metrics")
	);
	readResourceItem(file, _T("DLG_BORDER_WIDTH"),
		_T("Border width")
	);
	readResourceItem(file, _T("DLG_TITLE_BAR_WIDTH"),
		_T("Title bar width")
	);
	readResourceItem(file, _T("DLG_TITLE_BAR_HEIGHT"),
		_T("Title bar height")
	);
	readResourceItem(file, _T("DLG_SCROLL_BAR_WIDTH"),
		_T("Scroll bar width")
	);
	readResourceItem(file, _T("DLG_SCROLL_BAR_HEIGHT"),
		_T("Scroll bar height")
	);
	readResourceItem(file, _T("DLG_PALETTE_WIDTH"),
		_T("Palette width")
	);
	readResourceItem(file, _T("DLG_PALETTE_HEIGHT"),
		_T("Palette height")
	);
	readResourceItem(file, _T("DLG_MENU_WIDTH"),
		_T("Menu width")
	);
	readResourceItem(file, _T("DLG_MENU_HEIGHT"),
		_T("Menu height")
	);
	readResourceItem(file, _T("DLG_WINDOW_PADDING"),
		_T("Padded border width")
	);
	readResourceItem(file, _T("DLG_ICON_HORIZONTAL_MARGIN"),
		_T("Icon horizontal spacing")
	);
	readResourceItem(file, _T("DLG_ICON_VERTICAL_MARGIN"),
		_T("Icon vertical spacing")
	);
	readResourceItem(file, _T("DLG_OK"),
		_T("OK")
	);
	readResourceItem(file, _T("DLG_CANCEL"),
		_T("Cancel")
	);
	readResourceItem(file, _T("DLG_WINVER_TITLE"),
		_T("Windows version")
	);
	readResourceItem(file, _T("DLG_ABOUT_TITLE"),
		_T("About Re-Metrics")
	);
	readResourceItem(file, _T("DLG_ABOUT_VERSION"),
		_T("Re-Metrics Version 1.32")
	);
	readResourceItem(file, _T("DLG_ABOUT_CREDIT"),
		_T("By Tatsuhiko Syoji(Tatsu) 2012-2017,2020")
	);
	readResourceItem(file, _T("DLG_ABOUT_CREDIT2"),
		_T("Translate:Tatsu")
	);

	readResourceItem(file, _T("MSG_SETTING_FILE"),
		_T("Window settings file(*.ini)")
	);
	readResourceItem(file, _T("MSG_ALL_FILE"),
		_T("All file(*.*)")
	);
	readResourceItem(file, _T("MSG_LOAD_FAIL"),
		_T("Window settings file load failed.")
	);
	readResourceItem(file, _T("MSG_SAVE_FAIL"),
		_T("Window settings file save failed.")
	);
	readResourceItem(file, _T("MSG_ERROR"),
		_T("Error")
	);
	readResourceItem(file, _T("MSG_ABOUT"),
		_T("About Re-Metrics")
	);

	readResourceItem(file, _T("MSG_NO_WINDOW_BORDER"),
		_T("Please input window border.")
	);
	readResourceItem(file, _T("MSG_NO_TITLEBAR_WIDTH"),
		_T("Please input title bar width.")
	);
	readResourceItem(file, _T("MSG_NO_TITLEBAR_HEIGHT"),
		_T("Please input title bar height.")
	);
	readResourceItem(file, _T("MSG_NO_SCROLL_WIDTH"),
		_T("Please input scroll bar width.")
	);
	readResourceItem(file, _T("MSG_NO_SCROLL_HEIGHT"),
		_T("Please input scroll bar height.")
	);
	readResourceItem(file, _T("MSG_NO_PALETTE_WIDTH"),
		_T("Please input palette width.")
	);
	readResourceItem(file, _T("MSG_NO_PALETTE_HEIGHT"),
		_T("Please input palette height.")
	);
	readResourceItem(file, _T("MSG_NO_MENU_WIDTH"),
		_T("Please input menu bar width.")
	);
	readResourceItem(file, _T("MSG_NO_MENU_HEIGHT"),
		_T("Please input menu bar height.")
	);
	readResourceItem(file, _T("MSG_NO_WINDOW_PADDING"),
		_T("Please input window padding.")
	);
	readResourceItem(file, _T("MSG_NO_ICON_HORIZONTAL_MARGIN"),
		_T("Please input horizontal icon margin.")
	);
	readResourceItem(file, _T("MSG_NO_ICON_VERTICAL_MARGIN"),
		_T("No vertical icon margin.")
	);
	readResourceItem(file, _T("MENU_PRESET_11"),
		_T("Windows 11")
	);
}

