/*
Re-Metrics (C) 2012,2013 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef MYAPPLI_H
#define MYAPPLI_H

#include "DialogAppliBase.h"
#include "TwrUpDown.h"

class ReMetrics : public DialogAppliBase {
private:

	tstring borderWidth;
	tstring titleWidth;
	tstring titleHeight;
	tstring scrollWidth;
	tstring scrollHeight;
	tstring paletteWidth;
	tstring paletteHeight;
	tstring menuWidth;
	tstring menuHeight;
	tstring padding;

	TwrUpDown *borderWidthUpDown;
	TwrUpDown *titleWidthUpDown;
	TwrUpDown *titleHeightUpDown;
	TwrUpDown *scrollWidthUpDown;
	TwrUpDown *scrollHeightUpDown;
	TwrUpDown *paletteWidthUpDown;
	TwrUpDown *paletteHeightUpDown;
	TwrUpDown *menuWidthUpDown;
	TwrUpDown *menuHeightUpDown;
	TwrUpDown *paddingUpDown;

	NONCLIENTMETRICS metrics;
	LOGFONT iconFont;

	NONCLIENTMETRICS metricsAll;
	LOGFONT iconFontAll;

	bool OnBnClickedOk();
	void OnBnClickedWinVer();
	void adjustWindowSize(NONCLIENTMETRICS *metrics, int winVerMajor);

protected:
	INT_PTR OnCommand(WPARAM wParam);

public:
	BaseDialog *createBaseDialog();
	int OnAppliStart(TCHAR *lpCmdLine);
	int OnWindowShow();
	int OnAppliEnd();
	INT_PTR OnInitDialog();
	void UpdateData(bool toObj);

};

#include	<windows.h>
#include	<sddl.h>
#include	<tchar.h>

TCHAR *getSID(void);

#endif
