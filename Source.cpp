#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "shlwapi")

#include <windows.h>
#include <shlwapi.h>

TCHAR szClassName[] = TEXT("Window");

typedef struct {
	LPWSTR lpwzText;
	DWORD dwValue;
}CheckItem;

CheckItem items[] = {
	{ L"URL_UNESCAPE",                    0x10000000 },
	{ L"URL_ESCAPE_UNSAFE",               0x20000000 },
	{ L"URL_PLUGGABLE_PROTOCOL",          0x40000000 },
	{ L"URL_WININET_COMPATIBILITY",       0x80000000 },
	{ L"URL_DONT_ESCAPE_EXTRA_INFO",      0x02000000 },
	{ L"URL_ESCAPE_SPACES_ONLY",          0x04000000 },
	{ L"URL_DONT_SIMPLIFY",               0x08000000 },
	{ L"URL_UNESCAPE_INPLACE",            0x00100000 },
	{ L"URL_CONVERT_IF_DOSPATH",          0x00200000 },
	{ L"URL_UNESCAPE_HIGH_ANSI_ONLY",     0x00400000 },
	{ L"URL_INTERNAL_PATH",               0x00800000 },
	{ L"URL_FILE_USE_PATHURL",            0x00010000 },
	{ L"URL_DONT_UNESCAPE",               0x00020000 },
	{ L"URL_ESCAPE_AS_UTF8",              0x00040000 },
	{ L"URL_ESCAPE_PERCENT",              0x00001000 },
	{ L"URL_ESCAPE_SEGMENT_ONLY",         0x00002000 },
	{ L"URL_PARTFLAG_KEEPSCHEME",         0x00000001 },
	{ L"URL_APPLY_GUESSSCHEME",           0x00000002 },
	{ L"URL_APPLY_GUESSFILE",             0x00000004 },
	{ L"URL_APPLY_FORCEAPPLY",            0x00000008 }
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1, hEdit2;
	static HWND hCheck[_countof(items)];
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL, 10, 10, 600, 128, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("BUTTON"), TEXT("↓Escape"), WS_CHILD | WS_VISIBLE, 10, 148, 100, 30, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("BUTTON"), TEXT("↑Unescape"), WS_CHILD | WS_VISIBLE, 166, 148, 100, 30, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL, 10, 188, 600, 128, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		{
			int i = 0;
			for (auto item : items)
			{
				hCheck[i] = CreateWindowW(L"BUTTON", item.lpwzText, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10 + (i % 2) * 300, 188 + 128 + 10 + (i / 2) * 32, 300, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
				SetWindowLongPtr(hCheck[i], GWLP_USERDATA, item.dwValue);
				if (item.dwValue == URL_ESCAPE_SEGMENT_ONLY || item.dwValue == URL_ESCAPE_PERCENT) SendMessage(hCheck[i], BM_SETCHECK, 1, 0);
				++i;
			}
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 100)
		{
			DWORD dwSize = GetWindowTextLength(hEdit1);
			if (!dwSize) return 0;
			LPTSTR p = (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)*(dwSize + 1));
			GetWindowText(hEdit1, p, dwSize + 1);
			DWORD dwFlags = 0;
			for (HWND hCheckWnd : hCheck) {
				if (SendMessage(hCheckWnd, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					dwFlags |= GetWindowLongPtr(hCheckWnd, GWLP_USERDATA);
				}
			}
			TCHAR temp;
			dwSize = 1;
			UrlEscape(p, &temp, &dwSize, dwFlags);
			LPTSTR q = (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)*(++dwSize));
			if (UrlEscape(p, q, &dwSize, dwFlags) == S_OK) {
				SetWindowText(hEdit2, q);
			}
			GlobalFree(p);
			GlobalFree(q);
		}
		else if (LOWORD(wParam) == 101)
		{
			DWORD dwSize = GetWindowTextLength(hEdit2);
			if (!dwSize) return 0;
			LPTSTR p = (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)*(dwSize + 1));
			GetWindowText(hEdit2, p, dwSize + 1);
			DWORD dwFlags = 0;
			for (HWND hCheckWnd : hCheck) {
				if (SendMessage(hCheckWnd, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					dwFlags |= GetWindowLongPtr(hCheckWnd, GWLP_USERDATA);
				}
			}
			TCHAR temp;
			dwSize = 1;
			UrlUnescape(p, &temp, &dwSize, dwFlags);
			LPTSTR q = (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)*(++dwSize));
			if (UrlUnescape(p, q, &dwSize, dwFlags) == S_OK) {
				SetWindowText(hEdit1, q);
			}
			GlobalFree(q);
			GlobalFree(p);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("UrlEscape 関数を使た URLエンコード・URLデコード"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
