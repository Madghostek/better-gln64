#include <windows.h>
#include <commctrl.h>
#include <GL/glew.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include <process.h>
#include "glN64.h"
#include "Debug.h"
#include "Zilmar GFX 1.3.h"
#include "OpenGL.h"
#include "N64.h"
#include "RSP.h"
#include "RDP.h"
#include "VI.h"
#include "Config.h"
#include "Textures.h"
#include "Combiner.h"

HWND		hWnd;
HWND		hStatusBar;
//HWND		hFullscreen;
HWND		hToolBar;
HINSTANCE	hInstance;
#ifdef _DEBUG
char		pluginName[] = "better glN64 v1.2.1 debug";
#else
char		pluginName[] = "better glN64 v1.2.1";
#endif
char		*screenDirectory;

void (*CheckInterrupts)( void );

LONG		windowedStyle;
LONG		windowedExStyle;
RECT		windowedRect;
HMENU		windowedMenu;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Config_LoadConfig();
		RSP.thread = NULL;
		OGL.hRC = NULL;
		OGL.hDC = NULL;
/*		OGL.hPbufferRC = NULL;
		OGL.hPbufferDC = NULL;
		OGL.hPbuffer = NULL;*/
//		hFullscreen = NULL;
	}
	return TRUE;
}

EXPORT void CALL CaptureScreen ( char * Directory )
{
	screenDirectory = Directory;
#ifdef RSPTHREAD
	if (RSP.thread)
	{
		SetEvent( RSP.threadMsg[RSPMSG_CAPTURESCREEN] );
		WaitForSingleObject( RSP.threadFinished, INFINITE );
	}
#else
	OGL_SaveScreenshot();
#endif
}

EXPORT void CALL ChangeWindow (void)
{
#ifdef RSPTHREAD
	// Textures seem to get corrupted when changing video modes (at least on my Radeon), so destroy them
	SetEvent( RSP.threadMsg[RSPMSG_DESTROYTEXTURES] );
	WaitForSingleObject( RSP.threadFinished, INFINITE );

	if (shared_fbo != UINT_MAX)
	{
		printf("[gln] Deleting FBO...\n");
		glDeleteFramebuffers(1, &shared_fbo);
		glDeleteTextures(1, &tex_color_buf);
		shared_fbo = UINT_MAX;
	}

	if (!OGL.fullscreen)
	{
		DEVMODE fullscreenMode;
		memset( &fullscreenMode, 0, sizeof(DEVMODE) );
		fullscreenMode.dmSize = sizeof(DEVMODE);
		fullscreenMode.dmPelsWidth			= OGL.fullscreenWidth;
		fullscreenMode.dmPelsHeight			= OGL.fullscreenHeight;
		fullscreenMode.dmBitsPerPel			= OGL.fullscreenBits;
		fullscreenMode.dmDisplayFrequency	= OGL.fullscreenRefresh;
		fullscreenMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings( &fullscreenMode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox( NULL, "Failed to change display mode", pluginName, MB_ICONERROR | MB_OK );
			return;
		}

		ShowCursor( FALSE );

		windowedMenu = GetMenu( hWnd );

		if (windowedMenu)
			SetMenu( hWnd, NULL );

		if (hStatusBar)
			ShowWindow( hStatusBar, SW_HIDE );

		windowedExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
		windowedStyle = GetWindowLong( hWnd, GWL_STYLE );

		SetWindowLong( hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST );
		SetWindowLong( hWnd, GWL_STYLE, WS_POPUP );

		GetWindowRect( hWnd, &windowedRect );

		OGL.fullscreen = TRUE;
		OGL_ResizeWindow();
	}
	else
	{
		ChangeDisplaySettings( NULL, 0 );

		ShowCursor( TRUE );

		if (windowedMenu)
			SetMenu( hWnd, windowedMenu );

		if (hStatusBar)
			ShowWindow( hStatusBar, SW_SHOW );

		SetWindowLong( hWnd, GWL_STYLE, windowedStyle );
		SetWindowLong( hWnd, GWL_EXSTYLE, windowedExStyle );
		SetWindowPos( hWnd, NULL, windowedRect.left, windowedRect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

		OGL.fullscreen = FALSE;
		OGL_ResizeWindow();
	}


	printf("Init FBO %d x %d\n", OGL.width, OGL.height);

	glGenFramebuffers(1, &shared_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, shared_fbo);
	glGenTextures(1, &tex_color_buf);
	glBindTexture(GL_TEXTURE_2D, tex_color_buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, OGL.width, OGL.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buf, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		char text[260] = { 0 };
		sprintf(text, "Error while creating shared framebuffer! Error code: %d", status);
		MessageBox(hWnd, text, pluginName, MB_ICONERROR | MB_OK);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SetEvent( RSP.threadMsg[RSPMSG_INITTEXTURES] );
	WaitForSingleObject( RSP.threadFinished, INFINITE );
#endif
}

EXPORT void CALL CloseDLL (void)
{
}

EXPORT void CALL DllAbout ( HWND hParent )
{
	MessageBox( hParent, "Fork of glN64 by Madghostek, originally by Orkin\nRelease v1.1\nGithub: https://github.com/Madghostek/better-gln64", pluginName, MB_OK | MB_ICONINFORMATION );
}

EXPORT void CALL DllConfig ( HWND hParent )
{
	Config_DoConfig();
}

EXPORT void CALL DllTest ( HWND hParent )
{
}

EXPORT void CALL DrawScreen (void)
{
}

EXPORT void CALL GetDllInfo ( PLUGIN_INFO * PluginInfo )
{
	PluginInfo->Version = 0x103;
	PluginInfo->Type = PLUGIN_TYPE_GFX;
	strcpy( PluginInfo->Name, pluginName );
	PluginInfo->NormalMemory = FALSE;
	PluginInfo->MemoryBswaped = TRUE;
}

BOOL CALLBACK FindToolBarProc( HWND hWnd, LPARAM lParam )
{
	if (GetWindowLong( hWnd, GWL_STYLE ) & RBS_VARHEIGHT)
	{
		hToolBar = hWnd;
		return FALSE;
	}
	return TRUE;
}

EXPORT BOOL CALL InitiateGFX (GFX_INFO Gfx_Info)
{
	hWnd = Gfx_Info.hWnd;
	hStatusBar = Gfx_Info.hStatusBar;
	hToolBar = NULL;

	EnumChildWindows( hWnd, FindToolBarProc,0 );

	DMEM = Gfx_Info.DMEM;
	IMEM = Gfx_Info.IMEM;
	RDRAM = Gfx_Info.RDRAM;

	REG.MI_INTR = Gfx_Info.MI_INTR_REG;
	REG.DPC_START = Gfx_Info.DPC_START_REG;
	REG.DPC_END = Gfx_Info.DPC_END_REG;
	REG.DPC_CURRENT = Gfx_Info.DPC_CURRENT_REG;
	REG.DPC_STATUS = Gfx_Info.DPC_STATUS_REG;
	REG.DPC_CLOCK = Gfx_Info.DPC_CLOCK_REG;
	REG.DPC_BUFBUSY = Gfx_Info.DPC_BUFBUSY_REG;
	REG.DPC_PIPEBUSY = Gfx_Info.DPC_PIPEBUSY_REG;
	REG.DPC_TMEM = Gfx_Info.DPC_TMEM_REG;

	REG.VI_STATUS = Gfx_Info.VI_STATUS_REG;
	REG.VI_ORIGIN = Gfx_Info.VI_ORIGIN_REG;
	REG.VI_WIDTH = Gfx_Info.VI_WIDTH_REG;
	REG.VI_INTR = Gfx_Info.VI_INTR_REG;
	REG.VI_V_CURRENT_LINE = Gfx_Info.VI_V_CURRENT_LINE_REG;
	REG.VI_TIMING = Gfx_Info.VI_TIMING_REG;
	REG.VI_V_SYNC = Gfx_Info.VI_V_SYNC_REG;
	REG.VI_H_SYNC = Gfx_Info.VI_H_SYNC_REG;
	REG.VI_LEAP = Gfx_Info.VI_LEAP_REG;
	REG.VI_H_START = Gfx_Info.VI_H_START_REG;
	REG.VI_V_START = Gfx_Info.VI_V_START_REG;
	REG.VI_V_BURST = Gfx_Info.VI_V_BURST_REG;
	REG.VI_X_SCALE = Gfx_Info.VI_X_SCALE_REG;
	REG.VI_Y_SCALE = Gfx_Info.VI_Y_SCALE_REG;

	CheckInterrupts = Gfx_Info.CheckInterrupts;

	return TRUE;
}

EXPORT void CALL MoveScreen (int xpos, int ypos)
{
}

EXPORT void CALL ProcessDList(void)
{
#ifdef RSPTHREAD
	if (RSP.thread)
	{
		SetEvent( RSP.threadMsg[RSPMSG_PROCESSDLIST] );
		WaitForSingleObject( RSP.threadFinished, INFINITE );
	}
#else
	RSP_ProcessDList();
#endif
}

EXPORT void CALL ProcessRDPList(void)
{
	//*REG.DPC_CURRENT = *REG.DPC_START;
/*	RSP.PCi = 0;
	RSP.PC[RSP.PCi] = *REG.DPC_CURRENT;
	
	RSP.halt = FALSE;

	while (RSP.PC[RSP.PCi] < *REG.DPC_END)
	{
		RSP.cmd0 = *(DWORD*)&RDRAM[RSP.PC[RSP.PCi]];
		RSP.cmd1 = *(DWORD*)&RDRAM[RSP.PC[RSP.PCi] + 4];
		RSP.PC[RSP.PCi] += 8;

/*		if ((RSP.cmd0 >> 24) == 0xE9)
		{
			*REG.MI_INTR |= MI_INTR_DP;
			CheckInterrupts();
		}
		if ((RSP.cmd0 >> 24) == 0xCD)
			RSP.cmd0 = RSP.cmd0;

		GFXOp[RSP.cmd0 >> 24]();*/
		//*REG.DPC_CURRENT += 8;
//	}
}

EXPORT void CALL RomClosed (void)
{
#ifdef RSPTHREAD
	int i;

	if (RSP.thread)
	{
//		if (OGL.fullscreen)
//			ChangeWindow();

		if (RSP.busy)
		{
			RSP.halt = TRUE;
			WaitForSingleObject( RSP.threadFinished, INFINITE );
		}

		SetEvent( RSP.threadMsg[RSPMSG_CLOSE] );
		WaitForSingleObject( RSP.threadFinished, INFINITE );
		for (i = 0; i < 4; i++)
			if (RSP.threadMsg[i])
				CloseHandle( RSP.threadMsg[i] );
		CloseHandle( RSP.threadFinished );
		CloseHandle( RSP.thread );
	}

	RSP.thread = NULL;
#else
	OGL_Stop();
#endif

#ifdef DEBUG
	CloseDebugDlg();
#endif
}

EXPORT void CALL RomOpen (void)
{
#ifdef RSPTHREAD
	DWORD threadID;
	int i;

	// Create RSP message events
	for (i = 0; i < 7; i++) 
	{ 
		RSP.threadMsg[i] = CreateEvent( NULL, FALSE, FALSE, NULL );
		if (RSP.threadMsg[i] == NULL)
		{ 
			MessageBox( hWnd, "Error creating video thread message events, closing video thread...", "glN64 Error", MB_OK | MB_ICONERROR );
			return;
		} 
	} 

	// Create RSP finished event
	RSP.threadFinished = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (RSP.threadFinished == NULL)
	{ 
		MessageBox( hWnd, "Error creating video thread finished event, closing video thread...", "glN64 Error", MB_OK | MB_ICONERROR );
		return;
	} 

	RSP.thread = CreateThread( NULL, 4096, RSP_ThreadProc, NULL, NULL, &threadID );
	WaitForSingleObject( RSP.threadFinished, INFINITE );
#else
	RSP_Init();
#endif

	OGL_ResizeWindow();

#ifdef DEBUG
	OpenDebugDlg();
#endif
}

EXPORT void CALL ShowCFB (void)
{	
}

EXPORT void CALL UpdateScreen (void)
{
#ifdef RSPTHREAD
	if (RSP.thread)
	{
		SetEvent( RSP.threadMsg[RSPMSG_UPDATESCREEN] );
		WaitForSingleObject( RSP.threadFinished, INFINITE );
	}
#else
	VI_UpdateScreen();
#endif
}

EXPORT void CALL ViStatusChanged (void)
{
}

EXPORT void CALL ViWidthChanged (void)
{
}

void CALL mge_get_video_size(long* width, long* height)
{
	*width = OGL.width;
	*height = OGL.height;
}

void CALL mge_read_video(void** buffer)
{
	// This can be called before FBO initialization, so we must check here
	if (shared_fbo == UINT_MAX)
	{
		*buffer = nullptr;
		return;
	}

	fbo_mutex.lock();
	glBindFramebuffer(GL_FRAMEBUFFER, shared_fbo);
	glReadPixels(0, OGL.heightOffset, OGL.width, OGL.height, GL_BGR, GL_UNSIGNED_BYTE, *buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	fbo_mutex.unlock();
}
