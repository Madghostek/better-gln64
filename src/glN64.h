#ifndef GLN64_H
#define GLN64_H

#define RSPTHREAD

extern HWND hWnd;
extern HWND hStatusBar;
extern HWND hToolBar;
extern HINSTANCE hInstance;

extern void (*CheckInterrupts)(void);
extern char* screenDirectory;

#define PLUGIN_VERSION "1.4"

#ifdef _M_X64
#define PLUGIN_ARCH "-x64"
#else
#define PLUGIN_ARCH "-x86"
#endif

#ifdef _DEBUG
#define PLUGIN_TARGET "-debug"
#else
#define PLUGIN_TARGET "-release"
#endif

#define PLUGIN_NAME "better-gln64 " PLUGIN_VERSION PLUGIN_ARCH PLUGIN_TARGET

#endif
