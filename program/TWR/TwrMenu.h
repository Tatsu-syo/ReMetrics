/*
Re-Metrics (C) 2012-2014 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef TWRMENU_H
#define TWRMENU_H

#include <tchar.h>

class TwrMenu {
private:
	HMENU menuHandle;
public:
	TwrMenu(HWND wnd);
	TwrMenu(HMENU menu);

	void CheckMenuItem(int id,bool checked);
	bool isChecked(int item);
	void setEnabled(int id, bool enabled);
	void setText(int id, const TCHAR *message, BOOL byPosition = TRUE);

	/**
	 * �T�u���j���[���擾����B
	 *
	 * @param pos ���j���[��̈ʒu(0�`)
	 * @return �T�u���j���[�̃I�u�W�F�N�g
	 */
	TwrMenu *getSubMenu(int pos) {
		HMENU hMenu = GetSubMenu(menuHandle, pos);
		return new TwrMenu(hMenu);
	}
};


#endif
