#include <Windows.h>
#include <windowsx.h>
#include <iostream>

using namespace std;

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
MSLLHOOKSTRUCT mouseStruct;

// This is the callback function. Consider it the event that is raised when, in this case, 
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_MOUSEMOVE)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			mouseStruct = *((MSLLHOOKSTRUCT*)lParam);
			// a key (non-system) is pressed.
			
				// F1 is pressed!
				//MessageBox(NULL, L"MOUSE!!!", L"MOUSE!!!", MB_ICONINFORMATION);
			cout << mouseStruct.pt.x << " " << mouseStruct.pt.y << endl;
			
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook. If you create a hack you will not need the callback function 
	// in another place then your own code file anyway. Read more about it at MSDN.
	if (!(_hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, NULL, 0)))
	{
		MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}

void main()
{
	// Set the hook
	SetHook();

	// Don't mind this, it is a meaningless loop to keep a console application running.
	// I used this to test the keyboard hook functionality. If you want to test it, keep it in ;)
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{

	}
}