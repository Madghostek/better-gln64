#ifndef GLN64_H
#define GLN64_H
#include <windows.h>
#include <malloc.h>
//#include <commctrl.h>
//#define DEBUG
#define RSPTHREAD
extern HWND			hWnd;
//extern HWND			hFullscreen;
extern HWND			hStatusBar;
extern HWND			hToolBar;
extern HINSTANCE	hInstance;

extern void (*CheckInterrupts)( void );
extern char *screenDirectory;

#ifdef _DEBUG
#define DEBUG_SUFFIX " (Debug)"
#else
#define DEBUG_SUFFIX ""
#endif

#define PLUGIN_NAME "better glN64 v1.3" DEBUG_SUFFIX

#endif