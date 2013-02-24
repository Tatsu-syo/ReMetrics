/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#include "stdafx.h"
#include "AppliBase.h"
#include "TwrUpDown.h"

/**
 * ボタンを作成します。
 *
 * @param parent 親ウインドウハンドル
 * @param id メニューID
 */
HWND TwrUpDown::create(HWND parent, HMENU id)
{
	hWnd = CreateWindow(
		_T("UPDOWN_CLASS"),
		title.c_str(),
		WS_CHILD|WS_VISIBLE|BS_MULTILINE|BS_CENTER|WS_TABSTOP,
		left,	// X
		top,	// Y
		width,	// Width
		height,	// Height
		parent,	// Parent
		id,
		hInst,
		NULL);

	return hWnd;
}

