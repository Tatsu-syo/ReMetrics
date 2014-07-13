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