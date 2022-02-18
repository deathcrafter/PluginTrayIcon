#pragma once

#include <Windows.h>
#include <stdlib.h>
#include "../API/RainmeterAPI.h"
#include <string>
#include <vector>
#include <commctrl.h>

// messages

#define WM_TRAY_NOTIFYICON WM_APP + 594

struct Measure
{
	void* rm;
	void* skin;
	HWND skinWnd;

	std::wstring iconName;
	std::wstring lmbUpAction;
	std::wstring rmbUpAction;
	std::wstring scrlUpAction;
	std::wstring scrlDwnAction;

	std::wstring tooltip;
	HICON icon;

	bool active;


	Measure(void* p_rm) :
		rm(p_rm),
		skin(RmGetSkin(p_rm)),
		skinWnd(RmGetSkinWindow(p_rm)),
		iconName(),
		tooltip(),
		icon(),
		active(false)
	{
	}
};

void AddNotifIcon(Measure*);
void UpdateNotifIcon(Measure*);
void RemoveNotifIcon(Measure*);

void AddMeasure(Measure*);
void RemoveMeasure(Measure*);

LRESULT CALLBACK CallWndProc(int, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelMouseProc(int, WPARAM, LPARAM);