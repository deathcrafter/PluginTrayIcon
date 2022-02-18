/*
Copyright (C) 2021 Shaktijeet Sahoo


This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "PluginTrayIcon.h"


// global variables
HINSTANCE g_instance = NULL;
HHOOK g_hook = NULL;
HHOOK g_mouse = NULL;
std::vector<Measure*> g_Measures;

// =========================================================
// Dll Entry
// =========================================================

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD ul_reason_for_call, LPWORD lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		g_instance = hinst;

		// Disable DLL_THREAD_ATTACH and DLL_THREAD_DETACH notification calls : saves resources
		DisableThreadLibraryCalls((HMODULE)hinst);
		break;
	}

	return TRUE;
}

// =========================================================
// Plugin Functions
// =========================================================

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure(rm);
	*data = measure;
}

// ----------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	bool requiresUpdate = false;

	std::wstring icon = RmReadPath(measure->rm, L"IconName", L"");
	size_t len = wcslen(icon.c_str());
	if (len > 4 && wcscmp(icon.c_str() + len - 4, L".ico")) {
		icon += L".ico";
	}
	if (_wcsicmp(icon.c_str(), measure->iconName.c_str()) != 0) {
		requiresUpdate = true;
		measure->iconName = icon;
		if (measure->icon) {
			DestroyIcon(measure->icon);
			measure->icon = nullptr;
		}
		measure->icon = (HICON)LoadImage(
			g_instance,
			measure->iconName.c_str(),
			1,
			0,
			0,
			LR_LOADFROMFILE | LR_DEFAULTSIZE
		);
	}
	std::wstring tooltip = RmReadString(measure->rm, L"ToolTipText", L"");
	if (_wcsicmp(tooltip.c_str(), measure->tooltip.c_str()) != 0) {
		requiresUpdate = true;
		measure->tooltip = tooltip;
	}

	measure->lmbUpAction = RmReadString(rm, L"LeftMouseUpAction", L"");
	measure->rmbUpAction = RmReadString(rm, L"RightMouseUpAction", L"");
	measure->scrlUpAction = RmReadString(rm, L"MouseScrollUpAction", L"");
	measure->scrlDwnAction = RmReadString(rm, L"MouseScrollDownAction", L"");

	if (!measure->active)
		AddMeasure(measure);
	else if (measure->active && requiresUpdate)
		UpdateNotifIcon(measure);
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;
	return 0.0;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	RemoveMeasure(measure);
	delete measure;
}

// ----------------------------------------------------------------------------------------------------------------

// =========================================================
// Tray Icon Management
// =========================================================

void AddNotifIcon(Measure* measure) {
	NOTIFYICONDATA ndi;

	ndi.cbSize = sizeof(NOTIFYICONDATA);

	ndi.hWnd = measure->skinWnd;
	ndi.uID = 1;

	ndi.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	ndi.uCallbackMessage = WM_TRAY_NOTIFYICON;

	wcsncpy_s(ndi.szTip, measure->tooltip.c_str(), _TRUNCATE);
	ndi.hIcon = measure->icon;

	measure->active = Shell_NotifyIcon(NIM_ADD, &ndi);
}

void UpdateNotifIcon(Measure* measure) {
	NOTIFYICONDATA ndi;

	ndi.cbSize = sizeof(NOTIFYICONDATA);

	ndi.hWnd = measure->skinWnd;
	ndi.uID = 1;

	ndi.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	ndi.uCallbackMessage = WM_TRAY_NOTIFYICON;

	wcsncpy_s(ndi.szTip, measure->tooltip.c_str(), _TRUNCATE);
	ndi.hIcon = measure->icon;

	measure->active = Shell_NotifyIcon(NIM_MODIFY, &ndi);
}

void DeleteNotifIcon(Measure* measure) {
	NOTIFYICONDATA ndi;

	ndi.cbSize = sizeof(NOTIFYICONDATA);

	ndi.hWnd = measure->skinWnd;
	ndi.uID = 1;

	measure->active = !Shell_NotifyIcon(NIM_DELETE, &ndi);
}

// ----------------------------------------------------------------------------------------------------------------

// =========================================================
// Measure List Management
// =========================================================

void AddMeasure(Measure* measure) {
	if (std::find(g_Measures.begin(), g_Measures.end(), measure) != g_Measures.end()) {
		if (!measure->active) {
			AddNotifIcon(measure);
		}
		return;
	}

	for (auto m : g_Measures) {
		if (m->skinWnd == measure->skinWnd) {
			RmLog(measure->rm, LOG_ERROR, L"There can't be two instances of this plugin in a single skin!");
			return;
		}
	}

	if (!g_hook) {
		g_hook = SetWindowsHookEx(
			WH_CALLWNDPROC,
			CallWndProc,
			g_instance,
			GetWindowThreadProcessId(measure->skinWnd, NULL)
		);
		if (g_hook) {
			RmLog(measure->rm, LOG_DEBUG, L"Successfully started message hook!");
		}
		else {
			RmLog(measure->rm, LOG_ERROR, L"Failed to start message hook!");
		}
	}

	if (!g_mouse) {
		g_mouse = SetWindowsHookEx(
			WH_MOUSE_LL,
			LowLevelMouseProc,
			g_instance,
			NULL
		);
		if (g_mouse) {
			RmLog(measure->rm, LOG_DEBUG, L"Successfully started mouse hook!");
		}
		else {
			RmLog(measure->rm, LOG_ERROR, L"Failed to start mouse hook!");
		}
	}

	if (g_hook) {
		g_Measures.push_back(measure);
		AddNotifIcon(measure);
	}

	return;
}

void RemoveMeasure(Measure* measure) {
	std::vector<Measure*>::iterator found = std::find(g_Measures.begin(), g_Measures.end(), measure);
	if (found != g_Measures.end()) {
		if (measure->active) {
			DeleteNotifIcon(measure);

			if (measure->icon) {
				DestroyIcon(measure->icon);
				measure->icon = nullptr;
			}
		}
		g_Measures.erase(found);
	}
	if (g_Measures.empty()) {
		if (g_hook) {
			while (UnhookWindowsHookEx(g_hook) == FALSE) {
				RmLog(measure->rm, LOG_ERROR, L"Cannot unhook message hook!");
			}
			g_hook = NULL;
			while (UnhookWindowsHookEx(g_mouse) == FALSE) {
				RmLog(measure->rm, LOG_ERROR, L"Cannot unhook mouse hook!");
			}
			g_mouse = NULL;
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------

// =========================================================
// Hook Procedure
// =========================================================

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(g_hook, nCode, wParam, lParam);
	}

	CWPSTRUCT* cps = (CWPSTRUCT*)lParam;
	Measure* currentMeasure = nullptr;

	switch (cps->message)
	{
	case WM_TRAY_NOTIFYICON:
		for (auto m : g_Measures) {
			if (m->skinWnd == cps->hwnd) {
				currentMeasure = m;
			}
		}

		if (!currentMeasure) {
			return CallNextHookEx(g_hook, nCode, wParam, lParam);
		}

		switch (cps->lParam) {
		case WM_LBUTTONUP:
			RmExecute(currentMeasure->skin, currentMeasure->lmbUpAction.c_str());
			break;
		case WM_RBUTTONUP:
			RmExecute(currentMeasure->skin, currentMeasure->rmbUpAction.c_str());
			break;
		default:
			break;
		}
	default:
		break;
	}

	return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(g_mouse, nCode, wParam, lParam);
	}

	MSLLHOOKSTRUCT* msw;
	switch (wParam)
	{
	case WM_MOUSEWHEEL:
		msw = (MSLLHOOKSTRUCT*)lParam;
		POINT pt = msw->pt;
		for (auto m : g_Measures) {
			NOTIFYICONIDENTIFIER nii;
			nii.cbSize = sizeof(NOTIFYICONIDENTIFIER);
			nii.hWnd = m->skinWnd;
			nii.uID = 1;
			nii.guidItem = GUID_NULL;
			RECT rc;
			if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &rc))) {
				if (pt.x > rc.left && pt.x < rc.right && pt.y > rc.top && pt.y < rc.bottom) {
					if ((short)HIWORD(msw->mouseData) < 0) {
						RmExecute(m->skin, m->scrlDwnAction.c_str());
					}
					else {
						RmExecute(m->skin, m->scrlUpAction.c_str());
					}
				}
			}
		}
		break;
	default:
		break;
	}

	return CallNextHookEx(g_mouse, nCode, wParam, lParam);
}

// --------------------------------------------------------------------------------------------------------------