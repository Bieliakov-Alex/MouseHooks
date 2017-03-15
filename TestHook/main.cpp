#include <windows.h>
#pragma hdrstop
//////////////////////////////////////////////////////////////////////////
//									Глобальные переменные								//
//////////////////////////////////////////////////////////////////////////
#define MYWM_NOTIFY (WM_APP+100)
#define IDC_MYICON  1006

HINSTANCE hinst;					  // Описатель экземпляра приложения
NOTIFYICONDATA NotIconD;        // Для значка в Tray
HWND MainWnd;						  // Окно

HINSTANCE hLib;					  // Библиотека с ловушками

HHOOK hKeybHook;					  // Ловушки
HHOOK hMouseHook;
//TODO: ???
LRESULT CALLBACK *pKeybHook(int, WPARAM, LPARAM);
LRESULT CALLBACK *pMouseHook(int, WPARAM, LPARAM);
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
int InitApplication(HINSTANCE, int); 							// Прототипы
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
void vfSetTrayIcon(HINSTANCE);
void vfResetTrayIcon();
//////////////////////////////////////////////////////////////////////////
//												Функции										//
//////////////////////////////////////////////////////////////////////////
#pragma argsused

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	//----------------
	hLib = LoadLibrary(L"SHook.dll");
	if (hLib)
	{
		(void*)pKeybHook = GetProcAddress(hLib, "KeyboardHook");
		hKeybHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)(pKeybHook),
			hLib, 0);			// Ставим ловушки

		(void*)pMouseHook = GetProcAddress(hLib, "MouseHook");
		hMouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)(pMouseHook),
			hLib, 0);
		//----------------
		if (InitApplication(hInstance, nCmdShow))
		{
			vfSetTrayIcon(hInstance);					// Создаём окна
			while (GetMessage(&msg, (HWND)(NULL), 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			//----------------
			UnhookWindowsHookEx(hKeybHook);
			UnhookWindowsHookEx(hMouseHook);  // Завершаем
			FreeLibrary(hLib);
			vfResetTrayIcon();
			return 0;
		}
	}
	return 1;
}
//---------------------------------------------------------------------------
BOOL InitApplication(HINSTANCE hinstance, int nCmdShow)
{																 // Создание главного окна
	WNDCLASS wcx;                             // Класс окна

	wcx.style = NULL;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hinstance;
	wcx.hIcon = LoadIcon(hinstance, L"MAINICON");
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = L"HookWndClass";

	if (RegisterClass(&wcx))                // Создаём окно
	{
		MainWnd = CreateWindow(L"HookWndClass", L"SSHook",
			WS_OVERLAPPEDWINDOW/*|WS_VISIBLE*/,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, NULL, NULL, hinstance, NULL);

		if (!MainWnd)
			return FALSE;

		return TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam)
{																		// Оконная процедура
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	//------------
	case MYWM_NOTIFY:
	{
		if (lParam == WM_RBUTTONUP) // Правый щелчок на значке - завершаем
			PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
//---------------------------------------------------------------------------
void vfSetTrayIcon(HINSTANCE hInst)
{																			 // Значок в Tray
	char* pszTip = "Хранитель экрана и раскладка";

	NotIconD.cbSize = sizeof(NOTIFYICONDATA);
	NotIconD.hWnd = MainWnd;
	NotIconD.uID = IDC_MYICON;
	NotIconD.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NotIconD.uCallbackMessage = MYWM_NOTIFY;
	NotIconD.hIcon = LoadIcon(hInst, L"MAINICON");
	lstrcpyn(NotIconD.szTip, pszTip, sizeof(NotIconD.szTip));

	Shell_NotifyIcon(NIM_ADD, &NotIconD);
}
//---------------------------------------------------------------------------
void vfResetTrayIcon()
{																			// Удаляем значок
	Shell_NotifyIcon(NIM_DELETE, &NotIconD);
}
