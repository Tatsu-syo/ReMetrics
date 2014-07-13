/*
Re-Metrics (C) 2012-2014 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef TWRMENU_H
#define TWRMENU_H

class TwrMenu {
private:
	HMENU menuHandle;
public:
	TwrMenu(HWND wnd);
	void CheckMenuItem(int id,bool checked);
	bool isChecked(int item);
};


#endif
