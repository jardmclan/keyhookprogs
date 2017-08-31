#pragma comment( lib, "user32.lib" )


#include <windows.h>
#include <stdio.h>


LRESULT CALLBACK kbHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if(((KBDLLHOOKSTRUCT*)lParam) -> vkCode == VK_INSERT) {
		return 1;
	}
	return CallNextHookEx(NULL, nCode,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {

	HHOOK kHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)kbHookCallback, NULL, 0);
	
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) != 0) {
		
	}
	UnhookWindowsHookEx(kHook);
}