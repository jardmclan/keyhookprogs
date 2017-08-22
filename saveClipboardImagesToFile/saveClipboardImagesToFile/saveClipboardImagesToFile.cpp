#pragma comment( lib, "user32.lib" )

#include <atlimage.h>
#include <windows.h>
#include <ctime>
#include <string>
#include <stdio.h>

using namespace std;

static const int MAX_STORED = 10;
static const string BASE_PATH = "c:/screenshots/";
int pos = 0;
bool full = false;
string *names[MAX_STORED];

LRESULT CALLBACK kbHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (((KBDLLHOOKSTRUCT*)lParam)->vkCode == VK_SNAPSHOT && wParam == WM_KEYDOWN) {
		if (!OpenClipboard(NULL)) {
			return 1;
		}
		HBITMAP bmp = (HBITMAP)GetClipboardData(CF_BITMAP);
		if (bmp != NULL) {
			CImage image;
			image.Attach(bmp);
			time_t t = time(0);
			struct tm now;
			localtime_s(&now, &t);
			char buffer[80];
			strftime(buffer, 80, "%Y-%m-%d_%H;%M;%S", &now);
			string *fname = new string(BASE_PATH + string(buffer));
			int numInSec = 0;
			while (PathFileExists(((*fname) + ".jpg").c_str())) {
				numInSec++;
			}
			if (numInSec != 0) {
				(*fname) += "_" + to_string(numInSec);
			}
			(*fname) += ".jpg";
			image.Save((*fname).c_str(), Gdiplus::ImageFormatJPEG);

			if (!full) {
				names[pos] = fname;
				if (++pos % MAX_STORED == 0) {
					full = true;
				}
			}
			else {
				//remove((*(names[pos])).c_str());
				delete names[pos];
				names[pos] = fname;
				pos = ++pos % MAX_STORED;
			}
		}

		CloseClipboard();
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {

	HHOOK kHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)kbHookCallback, NULL, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {

	}
	UnhookWindowsHookEx(kHook);
}