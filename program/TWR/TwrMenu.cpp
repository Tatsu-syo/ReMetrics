/*
Re-Metrics (C) 2012-2014 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#include <string.h>
#include <windows.h>

#include "TwrMenu.h"

/**
 * ���j���[�������N���X
 */

/**
 * �R���X�g���N�^
 *
 * @param �I�u�W�F�N�g�Ɍ��ѕt����E�C���h�E�̃n���h��
 */
TwrMenu::TwrMenu(HWND wnd)
{
	menuHandle = GetMenu(wnd);
}

/**
 * �R���X�g���N�^
 *
 * @param �I�u�W�F�N�g�Ɍ��ѕt����E�C���h�E�̃n���h��
 */
TwrMenu::TwrMenu(HMENU menu)
{
	menuHandle = menu;
}

/**
 * ���j���[�̃`�F�b�N��Ԃ�ݒ肷��B
 *
 * @param id ���j���[ID
 * @param true:�`�F�b�N���� false:�`�F�b�N���Ȃ�
 */
void TwrMenu::CheckMenuItem(int id, bool checked)
{
	 MENUITEMINFO info;

	 memset(&info, 0, sizeof(MENUITEMINFO));
	 info.cbSize = sizeof(MENUITEMINFO);
	 info.fMask = MIIM_STATE;

	 GetMenuItemInfo(menuHandle, id, FALSE, &info);
	 if (checked) {
		 info.fState |= MFS_CHECKED;
	 } else {
		 if (info.fState & MFS_CHECKED) {
			info.fState ^= MFS_CHECKED;
		 }
	 }
	 SetMenuItemInfo(menuHandle, id, FALSE, &info);
}

/**
 * ���j���[�̃`�F�b�N��Ԃ�ݒ肷��B
 *
 * @param item ���j���[ID
 * @return true:�`�F�b�N���� false:�`�F�b�N���Ȃ�
 */
bool TwrMenu::isChecked(int item)
{
	 MENUITEMINFO info;

	 memset(&info, 0, sizeof(MENUITEMINFO));
	 info.cbSize = sizeof(MENUITEMINFO);
	 info.fMask = MIIM_STATE;

	 GetMenuItemInfo(menuHandle, item, FALSE, &info);
	 if (info.fState & MFS_CHECKED) {
		 return true;
	 } else {
		 return false;
	 }
}

/**
 * ���j���[�̗L����Ԃ�ݒ肷��B
 *
 * @param id ���j���[ID
 * @param true:�L���ɂ��� false:�L���ɂ��Ȃ�
 */
void TwrMenu::setEnabled(int id, bool enabled)
{
	MENUITEMINFO info;

	memset(&info, 0, sizeof(MENUITEMINFO));
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;

	GetMenuItemInfo(menuHandle, id, FALSE, &info);
	if (enabled) {
		if (info.fState & MFS_ENABLED) {
			info.fState ^= MFS_ENABLED;
		}
	} else {
		info.fState |= MFS_DISABLED;
	}
	SetMenuItemInfo(menuHandle, id, FALSE, &info);
}

/**
 * ���j���[�̕�����ݒ肷��B
 *
 * @param position ���j���[�̈ʒu/���j���[���ڂ�ID
 * @param message �ݒ肷�郁�b�Z�[�W
 * @param byPosition �w����@(TRUE:���j���[�̈ʒu FALSE:���j���[���ڂ�ID)
 */
void TwrMenu::setText(int position, const TCHAR *message, BOOL byPosition)
{
	MENUITEMINFO info;

	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_FTYPE | MIIM_STRING;
	info.fType = MFT_STRING;
	info.dwTypeData = (LPTSTR)message;
	SetMenuItemInfo(menuHandle, position, byPosition, &info);
}
