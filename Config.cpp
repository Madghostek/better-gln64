#include <windows.h>
#include <stdio.h>
#include "Config.h"
#include "glN64.h"
#include "Resource.h"
#include "RSP.h"
#include "Textures.h"
#include "OpenGL.h"
#include <commctrl.h>

HWND hConfigDlg;

struct
{
	struct
	{
		DWORD width, height, bitDepth, refreshRate;
	} selected;

	DWORD bitDepth[4];

	struct
	{
		DWORD	width, height;
	} resolution[32];

	DWORD refreshRate[32];
	
	DWORD	numBitDepths;
	DWORD	numResolutions;
	DWORD	numRefreshRates;
} fullscreen;

#define numWindowedModes 12

struct
{
	WORD width, height;
	char *description;
} windowedModes[numWindowedModes] = {
	{ 320, 240, "320 x 240" },
	{ 400, 300, "400 x 300" },
	{ 480, 360, "480 x 360" },
	{ 640, 480, "640 x 480" },
	{ 800, 600, "800 x 600" },
	{ 960, 720, "960 x 720" },
	{ 1024, 768, "1024 x 768" },
	{ 1152, 864, "1152 x 864" },
	{ 1280, 960, "1280 x 960" },
	{ 1280, 1024, "1280 x 1024" },
	{ 1440, 1080, "1440 x 1080" },
	{ 1600, 1200, "1600 x 1200" }
};

void EnableCustom(HWND hWndDlg, BOOL enable)
{
	EnableWindow(GetDlgItem(hWndDlg, IDC_WINDOWED_X), enable);
	EnableWindow(GetDlgItem(hWndDlg, IDC_WINDOWED_Y), enable);
}

void LockTextureBits(HWND hWndDlg, BOOL lock)
{
	if (lock)
	{
		SendMessage(GetDlgItem(hWndDlg, IDC_TEXTUREBPP), CB_SETCURSEL, 2, 0);
		EnableWindow(GetDlgItem(hWndDlg, IDC_TEXTUREBPP), FALSE);
		EnableWindow(GetDlgItem(hWndDlg, IDC_FSCALE), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hWndDlg, IDC_TEXTUREBPP), TRUE);
		SendMessage(GetDlgItem(hWndDlg, IDC_TEXTUREBPP), TBM_SETPOS, TRUE, 2);
		EnableWindow(GetDlgItem(hWndDlg, IDC_FSCALE), FALSE);
	}
}

void Config_LoadConfig()
{
	DWORD value, size;

	HKEY hKey;

	RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\N64 Emulation\\DLL\\glN64", 0, KEY_READ, &hKey );

	if (hKey)
	{
        RegQueryValueEx( hKey, "Fullscreen Bit Depth", 0, NULL, (BYTE*)&OGL.fullscreenBits, &size );
		RegQueryValueEx( hKey, "Fullscreen Width", 0, NULL, (BYTE*)&OGL.fullscreenWidth, &size );
		RegQueryValueEx( hKey, "Fullscreen Height", 0, NULL, (BYTE*)&OGL.fullscreenHeight, &size );
		RegQueryValueEx( hKey, "Fullscreen Refresh", 0, NULL, (BYTE*)&OGL.fullscreenRefresh, &size );
		RegQueryValueEx( hKey, "Windowed Width", 0, NULL, (BYTE*)&OGL.windowedWidth, &size );
		RegQueryValueEx( hKey, "Windowed Height", 0, NULL, (BYTE*)&OGL.windowedHeight, &size );
		RegQueryValueEx( hKey, "Windowed Width", 0, NULL, (BYTE*)&OGL.windowedWidth, &size );
		RegQueryValueEx( hKey, "Force Bilinear", 0, NULL, (BYTE*)&value, &size );
		OGL.forceBilinear = value ? TRUE : FALSE;

		RegQueryValueEx( hKey, "Texture Filter", 0, NULL, (BYTE*)&value, &size );
		OGL.textureFilter = value;

		RegQueryValueEx(hKey, "Filter Scale", 0, NULL, (BYTE*)&value, &size);
		OGL.filterScale = value;
		if (OGL.textureFilter == SaI) OGL.filterScale = 2; //in case something happens

		RegQueryValueEx( hKey, "Enable Fog", 0, NULL, (BYTE*)&value, &size );
		OGL.fog = value ? TRUE : FALSE;

		RegQueryValueEx( hKey, "Texture Cache Size", 0, NULL, (BYTE*)&value, &size );
		cache.maxBytes = value * 1048576;

		RegQueryValueEx( hKey, "Hardware Frame Buffer Textures", 0, NULL, (BYTE*)&value, &size );
		OGL.frameBufferTextures = value ? TRUE : FALSE;

		RegQueryValueEx( hKey, "Dithered Alpha Testing", 0, NULL, (BYTE*)&value, &size );
		OGL.usePolygonStipple = value ? TRUE : FALSE;

		RegQueryValueEx( hKey, "Texture Bit Depth", 0, NULL, (BYTE*)&value, &size );
		OGL.textureBitDepth = value;

		RegQueryValueEx(hKey, "Ignore Scissor", 0, NULL, (BYTE*)&value, &size);
		OGL.ignoreScissor = value ? TRUE : FALSE;

		RegCloseKey( hKey );
	}
	else
	{
		OGL.fog = TRUE;
		OGL.windowedWidth = 640;
		OGL.windowedHeight = 480;
		OGL.fullscreenWidth = 640;
		OGL.fullscreenHeight = 480;
		OGL.fullscreenBits = 16;
		OGL.fullscreenRefresh = 60;
		OGL.forceBilinear = FALSE;
		cache.maxBytes = 32 * 1048576;
		OGL.frameBufferTextures = FALSE;
		OGL.textureFilter = 0;
		OGL.textureBitDepth = 1;
		OGL.usePolygonStipple = FALSE;
	}
}

void Config_SaveConfig()
{
	DWORD value;
	HKEY hKey;

	RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\N64 Emulation\\DLL\\glN64", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL );

	RegSetValueEx( hKey, "Fullscreen Bit Depth", 0, REG_DWORD, (BYTE*)&OGL.fullscreenBits, 4 );
	RegSetValueEx( hKey, "Fullscreen Width", 0, REG_DWORD, (BYTE*)&OGL.fullscreenWidth, 4 );
	RegSetValueEx( hKey, "Fullscreen Height", 0, REG_DWORD, (BYTE*)&OGL.fullscreenHeight, 4 );
	RegSetValueEx( hKey, "Fullscreen Refresh", 0, REG_DWORD, (BYTE*)&OGL.fullscreenRefresh, 4 );
	RegSetValueEx( hKey, "Windowed Width", 0, REG_DWORD, (BYTE*)&OGL.windowedWidth, 4 );
	RegSetValueEx( hKey, "Windowed Height", 0, REG_DWORD, (BYTE*)&OGL.windowedHeight, 4 );

	value = OGL.forceBilinear ? 1 : 0;
	RegSetValueEx( hKey, "Force Bilinear", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.textureFilter;
	RegSetValueEx( hKey, "Texture Filter", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.filterScale;
	RegSetValueEx(hKey, "Filter Scale", 0, REG_DWORD, (BYTE*)&value, 4);

	value = OGL.fog ? 1 : 0;
	RegSetValueEx( hKey, "Enable Fog", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = cache.maxBytes / 1048576;
	RegSetValueEx( hKey, "Texture Cache Size", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.frameBufferTextures ? 1 : 0;
	RegSetValueEx( hKey, "Hardware Frame Buffer Textures", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.usePolygonStipple ? 1 : 0;
	RegSetValueEx( hKey, "Dithered Alpha Testing", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.textureBitDepth;
	RegSetValueEx( hKey, "Texture Bit Depth", 0, REG_DWORD, (BYTE*)&value, 4 );

	value = OGL.ignoreScissor ? 1 : 0;
	RegSetValueEx(hKey, "Ignore Scissor", 0, REG_DWORD, (BYTE*)&value, 4);

	RegCloseKey( hKey );
}

void Config_ApplyDlgConfig( HWND hWndDlg )
{
	char text[256];
	int i;

	SendDlgItemMessage( hWndDlg, IDC_CACHEMEGS, WM_GETTEXT, 4, (LPARAM)text );
	cache.maxBytes = atol( text ) * 1048576;

	OGL.forceBilinear = (SendDlgItemMessage( hWndDlg, IDC_FORCEBILINEAR, BM_GETCHECK, NULL, NULL ) == BST_CHECKED);
	int temp = OGL.textureFilter;
	OGL.textureFilter = SendDlgItemMessage( hWndDlg, IDC_TEXTUREFILTER, CB_GETCURSEL, NULL, NULL );
	if (temp != OGL.textureFilter) OGL.filterChanged = TRUE;
	temp = OGL.filterScale;
	OGL.filterScale = SendDlgItemMessage(hWndDlg, IDC_FSCALE,TBM_GETPOS, NULL, NULL);
	if (temp != OGL.filterScale) OGL.filterChanged = TRUE;
	OGL.fog = (SendDlgItemMessage( hWndDlg, IDC_FOG, BM_GETCHECK, NULL, NULL ) == BST_CHECKED);
	OGL.originAdjust = (OGL.textureFilter==1 ? 0.25 : 0.50);
	OGL.ignoreScissor = (SendDlgItemMessage(hWndDlg, IDC_SCISSOR, BM_GETCHECK, NULL, NULL) == BST_CHECKED);

	OGL.fullscreenBits = fullscreen.bitDepth[SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_GETCURSEL, 0, 0 )];
	i = SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_GETCURSEL, 0, 0 );
	OGL.fullscreenWidth = fullscreen.resolution[i].width;
	OGL.fullscreenHeight = fullscreen.resolution[i].height;
	OGL.fullscreenRefresh = fullscreen.refreshRate[SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_GETCURSEL, 0, 0 )];

	i = SendDlgItemMessage( hWndDlg, IDC_TEXTUREBPP, CB_GETCURSEL, 0, 0 );
	OGL.textureBitDepth = i;

	i = SendDlgItemMessage( hWndDlg, IDC_WINDOWEDRES, CB_GETCURSEL, 0, 0 );
	if (i == SendMessage(GetDlgItem(hWndDlg, IDC_WINDOWEDRES), CB_GETCOUNT, 0, 0) - 1)
	{
		char val[32];
		SendMessage(GetDlgItem(hWndDlg,IDC_WINDOWED_X), WM_GETTEXT, 32, (LPARAM)val);
		OGL.windowedWidth = atoi(val);
		SendMessage(GetDlgItem(hWndDlg, IDC_WINDOWED_Y), WM_GETTEXT, 32, (LPARAM)val);
		OGL.windowedHeight = atoi(val);
	}
	else
	{
		OGL.windowedWidth = windowedModes[i].width;
		OGL.windowedHeight = windowedModes[i].height;
	}

	OGL.frameBufferTextures = (SendDlgItemMessage( hWndDlg, IDC_FRAMEBUFFER, BM_GETCHECK, NULL, NULL ) == BST_CHECKED);
	OGL.usePolygonStipple = (SendDlgItemMessage( hWndDlg, IDC_DITHEREDALPHATEST, BM_GETCHECK, NULL, NULL ) == BST_CHECKED);

	if (!OGL.fullscreen)
		OGL_ResizeWindow();

	Config_SaveConfig();
}

void UpdateFullscreenConfig( HWND hWndDlg )
{
	DEVMODE deviceMode;
	int i,j;
	char text[256];

	memset( &fullscreen.bitDepth, 0, sizeof( fullscreen.bitDepth ) );
	memset( &fullscreen.resolution, 0, sizeof( fullscreen.resolution ) );
	memset( &fullscreen.refreshRate, 0, sizeof( fullscreen.refreshRate ) );
	fullscreen.numBitDepths = 0;
	fullscreen.numResolutions = 0;
	fullscreen.numRefreshRates = 0;

	i = 0;
	SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_RESETCONTENT, 0, 0 );
	while (EnumDisplaySettings( NULL, i, &deviceMode ) != 0)
	{
		for (j = 0; j < fullscreen.numBitDepths; j++)
		{
			if (deviceMode.dmBitsPerPel == fullscreen.bitDepth[j])
				break;
		}
		
		if ((deviceMode.dmBitsPerPel != fullscreen.bitDepth[j]) && (deviceMode.dmBitsPerPel > 8))
		{
			fullscreen.bitDepth[fullscreen.numBitDepths] = deviceMode.dmBitsPerPel;
			sprintf( text, "%i bit", deviceMode.dmBitsPerPel );
			SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_ADDSTRING, 0, (LPARAM)text );

			if (fullscreen.selected.bitDepth == deviceMode.dmBitsPerPel)
				SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_SETCURSEL, fullscreen.numBitDepths, 0 );
			fullscreen.numBitDepths++;
		}

		i++;
	}

	if (SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_GETCURSEL, 0, 0 ) == CB_ERR)
		SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_SETCURSEL, SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_GETCOUNT, 0, 0 ) - 1, 0 );


	i = 0;
	SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_RESETCONTENT, 0, 0 );
	while (EnumDisplaySettings( NULL, i, &deviceMode ) != 0)
	{
		for (int j = 0; j < fullscreen.numResolutions; j++)
		{
			if ((deviceMode.dmPelsWidth == fullscreen.resolution[j].width) &&
				(deviceMode.dmPelsHeight == fullscreen.resolution[j].height))
			{
				break;
			}
		}
		if (((deviceMode.dmPelsWidth != fullscreen.resolution[j].width) ||
			(deviceMode.dmPelsHeight != fullscreen.resolution[j].height)) &&
			(deviceMode.dmBitsPerPel != fullscreen.selected.bitDepth))
		{
			fullscreen.resolution[fullscreen.numResolutions].width = deviceMode.dmPelsWidth;
			fullscreen.resolution[fullscreen.numResolutions].height = deviceMode.dmPelsHeight;
			sprintf( text, "%i x %i", deviceMode.dmPelsWidth, deviceMode.dmPelsHeight );
			SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_ADDSTRING, 0, (LPARAM)text );

			if ((fullscreen.selected.width == deviceMode.dmPelsWidth) &&
				(fullscreen.selected.height == deviceMode.dmPelsHeight))
				SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_SETCURSEL, fullscreen.numResolutions, 0 );

			fullscreen.numResolutions++;
		}
		i++;
	}

	if (SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_GETCURSEL, 0, 0 ) == CB_ERR)
		SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_SETCURSEL, SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_GETCOUNT, 0, 0 ) - 1, 0 );

	i = 0;
	SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_RESETCONTENT, 0, 0 );
	while (EnumDisplaySettings( NULL, i, &deviceMode ) != 0)
	{
		for (int j = 0; j < fullscreen.numRefreshRates; j++)
		{
			if ((deviceMode.dmDisplayFrequency == fullscreen.refreshRate[j]))
				break;
		}
		if ((deviceMode.dmDisplayFrequency != fullscreen.refreshRate[j]) && 
				(deviceMode.dmPelsWidth == fullscreen.selected.width) &&
				(deviceMode.dmPelsHeight == fullscreen.selected.height) &&
				(deviceMode.dmBitsPerPel == fullscreen.selected.bitDepth))
		{
			fullscreen.refreshRate[j] = deviceMode.dmDisplayFrequency;
			sprintf( text, "%i Hz", deviceMode.dmDisplayFrequency );
			SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_ADDSTRING, 0, (LPARAM)text );

			if (fullscreen.selected.refreshRate == deviceMode.dmDisplayFrequency)
				SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_SETCURSEL, fullscreen.numRefreshRates, 0 );

			fullscreen.numRefreshRates++;
		}
		i++;
	}
	if (SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_GETCURSEL, 0, 0 ) == CB_ERR)
		SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_SETCURSEL, SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_GETCOUNT, 0, 0 ) - 1, 0 );
}

BOOL CALLBACK ConfigDlgProc( HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam ) 
{ 
	char text[256];
	int	 i;
	bool custom = true;
	DEVMODE deviceMode;
	switch (message) 
    { 
		case WM_INITDIALOG:
			hConfigDlg = hWndDlg;

			fullscreen.selected.width = OGL.fullscreenWidth;
			fullscreen.selected.height = OGL.fullscreenHeight;
			fullscreen.selected.bitDepth = OGL.fullscreenBits;
			fullscreen.selected.refreshRate = OGL.fullscreenRefresh;
			UpdateFullscreenConfig( hWndDlg );

			EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &deviceMode );

			// Fill windowed mode resolution
			for (i = 0; i < numWindowedModes; i++)
			{
				if ((deviceMode.dmPelsWidth > windowedModes[i].width) &&
					(deviceMode.dmPelsHeight > windowedModes[i].height))
				{
					SendDlgItemMessage( hWndDlg, IDC_WINDOWEDRES, CB_ADDSTRING, 0, (LPARAM)windowedModes[i].description );
					if ((OGL.windowedWidth == windowedModes[i].width) &&
						(OGL.windowedHeight == windowedModes[i].height))
					{
						SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_SETCURSEL, i, 0);
						custom = false;
					}
				}
			}
			SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_ADDSTRING, 0, (LPARAM)"Custom...");

			char val[32];
			sprintf(val,"%d", OGL.windowedWidth);
			SendDlgItemMessage(hWndDlg, IDC_WINDOWED_X, WM_SETTEXT, 0, (LPARAM)val);
			sprintf(val, "%d", OGL.windowedHeight);
			SendDlgItemMessage(hWndDlg, IDC_WINDOWED_Y, WM_SETTEXT, 0, (LPARAM)val);
			if (custom)
			{
				int num = SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_GETCOUNT, 0, 0)-1;
				EnableCustom(hWndDlg, TRUE);
				SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_SETCURSEL, num, 0);
			}

			SendDlgItemMessage(hWndDlg, IDC_TEXTUREFILTER, CB_ADDSTRING, 0, (LPARAM)"None");
			SendDlgItemMessage(hWndDlg, IDC_TEXTUREFILTER, CB_ADDSTRING, 0, (LPARAM)"2xSaI");
			SendDlgItemMessage(hWndDlg, IDC_TEXTUREFILTER, CB_ADDSTRING, 0, (LPARAM)"xBRZ");
			SendDlgItemMessage(hWndDlg, IDC_TEXTUREFILTER, CB_SETCURSEL, OGL.textureFilter, 0);
			SendMessage(GetDlgItem(hWndDlg, IDC_FSCALE), TBM_SETRANGE, TRUE, MAKELONG(2,6));
			SendMessage(GetDlgItem(hWndDlg, IDC_FSCALE), TBM_SETPOS, TRUE, OGL.filterScale);
			if(OGL.textureFilter == xBRZ) LockTextureBits(hConfigDlg, TRUE);
			//SendDlgItemMessage( hWndDlg, IDC_ENABLE2XSAI, BM_SETCHECK, OGL.enable2xSaI ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL );
			// Set forced bilinear check box
			SendDlgItemMessage( hWndDlg, IDC_FORCEBILINEAR, BM_SETCHECK, OGL.forceBilinear ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL );
			SendDlgItemMessage( hWndDlg, IDC_TEXTUREBPP, CB_ADDSTRING, 0, (LPARAM)"16-bit only (faster)" );
			SendDlgItemMessage( hWndDlg, IDC_TEXTUREBPP, CB_ADDSTRING, 0, (LPARAM)"16-bit and 32-bit (normal)" );
			SendDlgItemMessage( hWndDlg, IDC_TEXTUREBPP, CB_ADDSTRING, 0, (LPARAM)"32-bit only (best for 2xSaI)" );
			SendDlgItemMessage( hWndDlg, IDC_TEXTUREBPP, CB_SETCURSEL, OGL.textureBitDepth, 0 );
			SendDlgItemMessage(hWndDlg, IDC_SCISSOR, BM_SETCHECK, OGL.ignoreScissor ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL);
			// Enable/disable fog
			SendDlgItemMessage( hWndDlg, IDC_FOG, BM_SETCHECK, OGL.fog ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL );
			SendDlgItemMessage( hWndDlg, IDC_FRAMEBUFFER, BM_SETCHECK, OGL.frameBufferTextures ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL );

			SendDlgItemMessage( hWndDlg, IDC_DITHEREDALPHATEST, BM_SETCHECK, OGL.usePolygonStipple ? (LPARAM)BST_CHECKED : (LPARAM)BST_UNCHECKED, NULL );

			_ltoa( cache.maxBytes / 1048576, text, 10 );
			SendDlgItemMessage( hWndDlg, IDC_CACHEMEGS, WM_SETTEXT, NULL, (LPARAM)text );

			return TRUE;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
					Config_ApplyDlgConfig( hWndDlg );
					EndDialog( hWndDlg, wParam );
					hConfigDlg = NULL;
					return TRUE;
 
                case IDCANCEL: 
                    EndDialog( hWndDlg, wParam );
					hConfigDlg = NULL;
                    return TRUE;

				case IDC_FULLSCREENBITDEPTH:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						fullscreen.selected.bitDepth = fullscreen.bitDepth[SendDlgItemMessage( hWndDlg, IDC_FULLSCREENBITDEPTH, CB_GETCURSEL, 0, 0 )];

						UpdateFullscreenConfig( hWndDlg );
					}
					break;
				case IDC_FULLSCREENRES:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						i = SendDlgItemMessage( hWndDlg, IDC_FULLSCREENRES, CB_GETCURSEL, 0, 0 );
						fullscreen.selected.width = fullscreen.resolution[i].width;
						fullscreen.selected.height = fullscreen.resolution[i].height;

						UpdateFullscreenConfig( hWndDlg );
					}
					break;
				case IDC_FULLSCREENREFRESH:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						fullscreen.selected.refreshRate = fullscreen.refreshRate[SendDlgItemMessage( hWndDlg, IDC_FULLSCREENREFRESH, CB_GETCURSEL, 0, 0 )];

						UpdateFullscreenConfig( hWndDlg );
					}
					break;

				case IDC_WINDOWEDRES:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_GETCURSEL, 0, 0) == SendDlgItemMessage(hWndDlg, IDC_WINDOWEDRES, CB_GETCOUNT, 0, 0)-1)
						{
							EnableCustom(hWndDlg, TRUE);
						}
						else
						{
							EnableCustom(hWndDlg, FALSE);
						}
					}
				case IDC_TEXTUREFILTER:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (SendDlgItemMessage(hWndDlg, IDC_TEXTUREFILTER, CB_GETCURSEL, 0, 0) == xBRZ)
						{
							LockTextureBits(hWndDlg, TRUE);
						}
						else
						{
							LockTextureBits(hWndDlg, FALSE);
						}
					}
			} 
    } 
    return FALSE; 
} 

void Config_DoConfig()
{
	if (!hConfigDlg)
		DialogBox( hInstance, MAKEINTRESOURCE(IDD_CONFIGDLG), hWnd, (DLGPROC)ConfigDlgProc );
}
