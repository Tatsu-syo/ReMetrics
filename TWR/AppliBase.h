/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#pragma once

#include <windows.h>
#include "../resource.h"

extern HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X

void addModelessDialog(HWND dialog);
void delModelessDialog(HWND dialog);

int doEvents();
