/*
Re-Metrics (C) 2012-2017 Tatsuhiko Shoji
The sources for Re-Metrics are distributed under the MIT open source license
*/
#ifndef MYAPPLI_H
#define MYAPPLI_H

#include "DialogAppliBase.h"
#include "TwrUpDown.h"
#include "TwrMenu.h"

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
	tstring iconHMergin;
	tstring iconVMergin;

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
	TwrUpDown *iconHMerginUpDown;
	TwrUpDown *iconVMerginUpDown;

	TwrMenu *appMenu;

	int minTitleHeight;
	int minPaletteHeight;
	int minMenuHeight;

	NONCLIENTMETRICS metrics;
	ICONMETRICS iconMetrics;

	TCHAR settingFile[MAX_PATH];
	bool setOnStart;

	bool OnBnClickedOk();
	void OnBnClickedWinVer();
	INT_PTR OnSettingChange(WPARAM wParam, LPARAM lParam);
	void OnLoad();
	void OnSave();

	void applyWindowSetting(
		NONCLIENTMETRICS &newMetrics,
		ICONMETRICS &newIconMetrics);
	void adjustWindowSize(NONCLIENTMETRICS *metrics, int winVerMajor);
	int getMinHeight(LOGFONT *font);
	void setItemRange();
	void setMetrics(NONCLIENTMETRICS *fontMetrics, ICONMETRICS *newIconMetrics);
	void showHelp(void);
	void GetNonclientMetrics(NONCLIENTMETRICS *target);
	void GetIconMetrics(ICONMETRICS *target);
	BOOL startLoadWindowItem(TCHAR *filename);
	BOOL startSaveWindowItem(TCHAR *filename);
	bool isNumStr(TCHAR *buf);
	void getOption(TCHAR *lpCmdLine);
	void parseOption(TCHAR *param, int argCount);
	void getWin10Ver(TCHAR *buf, DWORD major, DWORD minor);
	bool isValidInput(void);
	void screenToMetrics(void);

protected:
	INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
	INT_PTR OnLostFocus(WPARAM wParam, LPARAM lParam);

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

BOOL CALLBACK MonitorNearMouseCallback(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
);
void adjustCenter(RECT parentRect, HWND parentHWnd, HWND myHWnd);

#define ICON_HOL_SPACING_KEY "IconHolizontalSpacing"
#define ICON_VER_SPACING_KEY "IconVerticalSpacing"

#endif
