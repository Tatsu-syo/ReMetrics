/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef TWRBUTTON_H
#define TWRBUTTON_H

#include "commctrl.h"
#include "TwrWnd.h"

class TwrUpDown : public TwrWnd {
public:
	TwrUpDown() : TwrWnd() {
	}
	TwrUpDown(HWND hwnd) : TwrWnd(hwnd) {
	}
	HWND create(HWND parent, HMENU id);
	void setBuddy(HWND buddy) {
		SendMessage(hWnd, UDM_SETBUDDY, (WPARAM)buddy, 0);
	}
	HWND getBuddy(void) {
		return (HWND)SendMessage(hWnd, UDM_GETBUDDY, 0, 0);
	}
	void setRange(int min, int max) {
		SendMessage(hWnd, UDM_SETRANGE32, (WPARAM)min, (LPARAM)max);
	}
	void setPos(int pos) {
		SendMessage(hWnd, UDM_SETPOS32, 0, (LPARAM)pos);
	}

};

#endif
