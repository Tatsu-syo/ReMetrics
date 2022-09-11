/*
Re-Metrics (C) 2012-2017,2021,2022 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/

#include <assert.h>
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
	tstring buf;
	int len;

	// INIファイルを読み込む。Unicode版のAPIでもファイルが非Unicodeの場合は
	// 各言語の文字コードのファイルとして読んでくれる。
	len = GetPrivateProfileStringExT("RESOURCE", key, _T(""), buf, file);

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
		_T("Re-Metrics Version 1.34")
	);
	readResourceItem(file, _T("DLG_ABOUT_CREDIT"),
		_T("By Tatsuhiko Syoji(Tatsu) 2012-2017,2020,2021,2022")
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

/**
 * charをunsignedとみなしてintに変換する
 *
 * @param c 変換するchar
 * @return cをunsignedとみなして変換した値
 */
inline int charToInt(char c)
{
	if (c < 0) {
		return 256 + c;
	}
	else {
		return c;
	}

}

/**
 * UTF-8文字列をUTF-16文字列に変換する
 *
 * @param src 変換元UTF-8文字列
 * @param dst 変換先領域(NULL可能)
 * @return 必要となるwchar_tの個数
 */
size_t utf8toUtf16(tstring& dst, const char* src)
{
	wchar_t wc;
	int shift_count;
	int i;

	assert(src != NULL);

	while (*src) {
		// Extruct to Unicode
		int c = charToInt(*src);

		wc = 0;
		if (c < 0x80) {
			// 1byte(7bit)
			wc = c;
			shift_count = 0;
		}
		else if (c >= 0xfc) {
			// 6bytes(27-32bit:no use)
			wc = c & 0x01;
			shift_count = 5;
		}
		else if (c >= 0xf8) {
			// 5bytes(22-26bit:no use)
			wc = c & 0x03;
			shift_count = 4;
		}
		else if (c >= 0xf0) {
			// 4bytes(17-21bit)
			wc = c & 0x07;
			shift_count = 3;
		}
		else if (c >= 0xe0) {
			// 3bytes(12-16bit)
			wc = c & 0x0f;
			shift_count = 2;
		}
		else if (c >= 0xc0) {
			// 2bytes(8-11bit)
			wc = c & 0x1f;
			shift_count = 1;
		}
		else {
			break;
		}

		src++;
		for (i = 0; i < shift_count; i++) {
			if (*src == '\0') {
				wc = L'\0';
				break;
			}
			c = charToInt(*src);

			wc = wc << 6;
			wc = wc | (c & 0x3f);
			src++;
		}

		if (c >= 0x10000) {
			// Windows uses UTF-16 LE
			int c1 = (c & 0x3ff) | 0xdc00;
			int c2_1 = (c >> 10) | 0x3f;
			int c2_2 = ((c >> 16) & 0x1f) - 1;
			int c2 = 0xd800 | c2_2 | c2_1;

			dst += c1;
			dst += c2;
		}
		else {
			dst += wc;
		}
	}

	return dst.length();
}

