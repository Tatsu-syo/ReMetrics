#pragma once

#include <windows.h>
#include "../resource.h"

extern HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X

void addModelessDialog(HWND dialog);
void delModelessDialog(HWND dialog);

int doEvents();
