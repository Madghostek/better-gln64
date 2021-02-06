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

extern char			pluginName[];

extern void (*CheckInterrupts)( void );
extern char *screenDirectory;

#endif