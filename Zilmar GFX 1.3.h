/**********************************************************************************
Common gfx plugin spec, version #1.3 maintained by zilmar (zilmar@emulation64.com)

All questions or suggestions should go through the mailing list.
http://www.egroups.com/group/Plugin64-Dev
***********************************************************************************

Notes:
------

Setting the approprate bits in the MI_INTR_REG and calling CheckInterrupts which 
are both passed to the DLL in InitiateGFX will generate an Interrupt from with in 
the plugin.

The Setting of the RSP flags and generating an SP interrupt  should not be done in
the plugin

**********************************************************************************/
#ifndef _GFX_H_INCLUDED__
#define _GFX_H_INCLUDED__

#if defined(__cplusplus)
extern "C" {
#endif

/* Plugin types */
#define PLUGIN_TYPE_GFX				2

#define EXPORT						__declspec(dllexport)
#define CALL						_cdecl

/***** Structures *****/
using PLUGIN_INFO = struct
{
    WORD Version; /* Set to 0x0103 */
    WORD Type; /* Set to PLUGIN_TYPE_GFX */
    char Name[100]; /* Name of the DLL */

    /* If DLL supports memory these memory options then set them to TRUE or FALSE
       if it does not support it */
    BOOL NormalMemory; /* a normal BYTE array */
    BOOL MemoryBswaped; /* a normal BYTE array where the memory has been pre
	                          bswap on a dword (32 bits) boundry */
};

using GFX_INFO = struct
{
    HWND hWnd; /* Render window */
    HWND hStatusBar; /* if render window does not have a status bar then this is NULL */

    BOOL MemoryBswaped; // If this is set to TRUE, then the memory has been pre
    //   bswap on a dword (32 bits) boundry 
    //	eg. the first 8 bytes are stored like this:
    //        4 3 2 1   8 7 6 5

    BYTE* HEADER; // This is the rom header (first 40h bytes of the rom
    // This will be in the same memory format as the rest of the memory.
    BYTE* RDRAM;
    BYTE* DMEM;
    BYTE* IMEM;

    DWORD* MI_INTR_REG;

    DWORD* DPC_START_REG;
    DWORD* DPC_END_REG;
    DWORD* DPC_CURRENT_REG;
    DWORD* DPC_STATUS_REG;
    DWORD* DPC_CLOCK_REG;
    DWORD* DPC_BUFBUSY_REG;
    DWORD* DPC_PIPEBUSY_REG;
    DWORD* DPC_TMEM_REG;

    DWORD* VI_STATUS_REG;
    DWORD* VI_ORIGIN_REG;
    DWORD* VI_WIDTH_REG;
    DWORD* VI_INTR_REG;
    DWORD* VI_V_CURRENT_LINE_REG;
    DWORD* VI_TIMING_REG;
    DWORD* VI_V_SYNC_REG;
    DWORD* VI_H_SYNC_REG;
    DWORD* VI_LEAP_REG;
    DWORD* VI_H_START_REG;
    DWORD* VI_V_START_REG;
    DWORD* VI_V_BURST_REG;
    DWORD* VI_X_SCALE_REG;
    DWORD* VI_Y_SCALE_REG;

    void (*CheckInterrupts)(void);
};

/******************************************************************
  Function: CaptureScreen
  Purpose:  This function dumps the current frame to a file
  input:    pointer to the directory to save the file to
  output:   none
*******************************************************************/
EXPORT void CALL CaptureScreen(char* Directory);

/******************************************************************
  Function: ChangeWindow
  Purpose:  to change the window between fullscreen and window 
            mode. If the window was in fullscreen this should 
			change the screen to window mode and vice vesa.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ChangeWindow(void);

/******************************************************************
  Function: CloseDLL
  Purpose:  This function is called when the emulator is closing
            down allowing the dll to de-initialise.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL CloseDLL(void);

/******************************************************************
  Function: DllAbout
  Purpose:  This function is optional function that is provided
            to give further information about the DLL.
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
EXPORT void CALL DllAbout(HWND hParent);

/******************************************************************
  Function: DllConfig
  Purpose:  This function is optional function that is provided
            to allow the user to configure the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
EXPORT void CALL DllConfig(HWND hParent);

/******************************************************************
  Function: DllTest
  Purpose:  This function is optional function that is provided
            to allow the user to test the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
EXPORT void CALL DllTest(HWND hParent);

/******************************************************************
  Function: DrawScreen
  Purpose:  This function is called when the emulator receives a
            WM_PAINT message. This allows the gfx to fit in when
			it is being used in the desktop.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL DrawScreen(void);

/******************************************************************
  Function: GetDllInfo
  Purpose:  This function allows the emulator to gather information
            about the dll by filling in the PluginInfo structure.
  input:    a pointer to a PLUGIN_INFO stucture that needs to be
            filled by the function. (see def above)
  output:   none
*******************************************************************/
EXPORT void CALL GetDllInfo(PLUGIN_INFO* PluginInfo);

/******************************************************************
  Function: InitiateGFX
  Purpose:  This function is called when the DLL is started to give
            information from the emulator that the n64 graphics
			uses. This is not called from the emulation thread.
  Input:    Gfx_Info is passed to this function which is defined
            above.
  Output:   TRUE on success
            FALSE on failure to initialise
             
  ** note on interrupts **:
  To generate an interrupt set the appropriate bit in MI_INTR_REG
  and then call the function CheckInterrupts to tell the emulator
  that there is a waiting interrupt.
*******************************************************************/
EXPORT BOOL CALL InitiateGFX(GFX_INFO Gfx_Info);

/******************************************************************
  Function: MoveScreen
  Purpose:  This function is called in response to the emulator
            receiving a WM_MOVE passing the xpos and ypos passed
			from that message.
  input:    xpos - the x-coordinate of the upper-left corner of the
            client area of the window.
			ypos - y-coordinate of the upper-left corner of the
			client area of the window. 
  output:   none
*******************************************************************/
EXPORT void CALL MoveScreen(int xpos, int ypos);

/******************************************************************
  Function: ProcessDList
  Purpose:  This function is called when there is a Dlist to be
            processed. (High level GFX list)
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ProcessDList(void);

/******************************************************************
  Function: ProcessRDPList
  Purpose:  This function is called when there is a Dlist to be
            processed. (Low level GFX list)
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ProcessRDPList(void);

/******************************************************************
  Function: RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL RomClosed(void);

/******************************************************************
  Function: RomOpen
  Purpose:  This function is called when a rom is open. (from the 
            emulation thread)
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL RomOpen(void);

/******************************************************************
  Function: ShowCFB
  Purpose:  Useally once Dlists are started being displayed, cfb is
            ignored. This function tells the dll to start displaying
			them again.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ShowCFB(void);

/******************************************************************
  Function: UpdateScreen
  Purpose:  This function is called in response to a vsync of the
            screen were the VI bit in MI_INTR_REG has already been
			set
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL UpdateScreen(void);

/******************************************************************
  Function: ViStatusChanged
  Purpose:  This function is called to notify the dll that the
            ViStatus registers value has been changed.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ViStatusChanged(void);

/******************************************************************
  Function: ViWidthChanged
  Purpose:  This function is called to notify the dll that the
            ViWidth registers value has been changed.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL ViWidthChanged(void);

EXPORT void CALL ReadScreen2(void** dest, long* width, long* height);

/******************************************************************
  Function: mge_get_video_size
  Purpose:  This function retrieves the current video size.
  input:    width - Pointer to video width.
            height - Pointer to video width.
  output:   none
*******************************************************************/
EXPORT void CALL mge_get_video_size(long* width, long* height);

/******************************************************************
  Function: mge_read_video
  Purpose:  This function returns a pointer to a buffer containing the framebuffer data.
  input:    buffer - The buffer containing the framebuffer data. 
            The framebuffer is of the dimensions mge_get_video_size and bit depth of 24.
  output:   none
*******************************************************************/
EXPORT void CALL mge_read_video(void** buffer);

EXPORT void CALL DllCrtFree(void* block)
{
    free(block);
}

#if defined(__cplusplus)
}

#endif
#endif
