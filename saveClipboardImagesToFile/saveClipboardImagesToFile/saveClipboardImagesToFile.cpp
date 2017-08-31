#pragma comment( lib, "user32.lib" )

#include <atlimage.h>
#include <windows.h>
#include <ctime>
#include <string>
#include <direct.h>
#include <fstream>
#include <regex>

using namespace std;

static const int MAX_STORED = 10;
static const char const *BASE_PATH = "c:\\screenshots";
static const char const *BASE_FILE = "c:\\screenshots\\*";
static const char const *BASE_BACKUP_PATH = "c:\\screenshots\\temp";
static const char const *BASE_BACKUP_FILE = "c:\\screenshots\\temp\\*";
static const char const *READ_ME_MSG = "Will save up to 10 screenshots at a time (if you would like an increased capacity ask the developer). Oldest screenshots will be overwritten first.\nTo save screenshots, move to another folder.\nOn program restart (e.g. when the computer is restarted), screenshots from the last session will be backed up in the \"temp\" folder.\nScreenshots more than one session old will be deleted.\nWARNING: Any files stored in the \"temp\" folder will be deleted on program start. DO NOT STORE ANY FILES YOU WISH TO SAVE IN THIS FOLDER.";
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

			string *fname;
			if (!full) {
				names[pos] = new string(string(buffer));
				fname = names[pos];
				pos = ++pos % MAX_STORED;
				if (pos == 0) {
					full = true;
				}
			}
			else {
				remove((*(names[pos])).c_str());
				delete names[pos];
				names[pos] = new string(string(buffer));
				fname = names[pos];
				pos = ++pos % MAX_STORED;
			}

			int numInSec = 0;
			if (PathFileExists(((*fname) + ".jpg").c_str())) {
				numInSec++;
				while (PathFileExists(((*fname) + "_" + to_string(numInSec) + ".jpg").c_str())) {
					numInSec++;
				}
			}
			if (numInSec != 0) {
				(*fname) += "_" + to_string(numInSec);
			}
			(*fname) += ".jpg";
			image.Save((*fname).c_str(), Gdiplus::ImageFormatJPEG);
		}

		CloseClipboard();
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void generateReadme() {
	ofstream readme;
	readme.open("READ_ME.txt");
	readme << READ_ME_MSG;
	readme.close();
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {

	CreateDirectory(BASE_PATH, NULL);
	CreateDirectory(BASE_BACKUP_PATH, NULL);
	_chdir(BASE_BACKUP_PATH);

	HANDLE h;
	//LP stands for long pointer
	WIN32_FIND_DATA fdata;

	if ((h = FindFirstFile(BASE_BACKUP_FILE, &fdata)) != INVALID_HANDLE_VALUE) {
		do {
			DeleteFile(fdata.cFileName);
		} while (FindNextFile(h, &fdata));
		FindClose(h);
	}

	_chdir(BASE_PATH);

	if ((h = FindFirstFile(BASE_FILE, &fdata)) != INVALID_HANDLE_VALUE) {
		do {
			if (regex_match(fdata.cFileName, regex(".*\\.jpg$"))) {
				MoveFile(fdata.cFileName, (string(BASE_BACKUP_PATH) + "\\" + fdata.cFileName).c_str());
			}
		} while (FindNextFile(h, &fdata));
		FindClose(h);
	}

	if (!PathFileExists("READ_ME.txt")) {
		generateReadme();
	}

	HHOOK kHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)kbHookCallback, NULL, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {

	}

	UnhookWindowsHookEx(kHook);
	return 0;
}