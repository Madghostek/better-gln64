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
#include <cstdint>
extern "C" {
#endif

	/* Plugin types */
#define PLUGIN_TYPE_GFX				2

#define EXPORT						__declspec(dllexport)
#define CALL						_cdecl

	enum class PluginType
	{
		Video = 2,
		Audio = 3,
		Input = 4,
		RSP = 1,
	};

	enum class ControllerExtension
	{
		None = 1,
		Mempak = 2,
		Rumblepak = 3,
		Transferpak = 4,
		Raw = 5
	};

	enum class SystemType
	{
		NTSC,
		PAL,
		MPAL
	};

	typedef struct
	{
		uint16_t Version;
		uint16_t Type;
		char Name[100];

		/* If DLL supports memory these memory options then set them to TRUE or FALSE
		   if it does not support it */
		int32_t NormalMemory; /* a normal uint8_t array */
		int32_t MemoryBswaped; /* a normal uint8_t array where the memory has been pre
								 bswap on a dword (32 bits) boundry */
	} PLUGIN_INFO;

	typedef struct
	{
		void* hInst;
		// Whether the memory has been pre-byteswapped on a uint32_t boundary
		int32_t MemoryBswaped;
		uint8_t* RDRAM;
		uint8_t* DMEM;
		uint8_t* IMEM;

		uint32_t* MI_INTR_REG;

		uint32_t* SP_MEM_ADDR_REG;
		uint32_t* SP_DRAM_ADDR_REG;
		uint32_t* SP_RD_LEN_REG;
		uint32_t* SP_WR_LEN_REG;
		uint32_t* SP_STATUS_REG;
		uint32_t* SP_DMA_FULL_REG;
		uint32_t* SP_DMA_BUSY_REG;
		uint32_t* SP_PC_REG;
		uint32_t* SP_SEMAPHORE_REG;

		uint32_t* DPC_START_REG;
		uint32_t* DPC_END_REG;
		uint32_t* DPC_CURRENT_REG;
		uint32_t* DPC_STATUS_REG;
		uint32_t* DPC_CLOCK_REG;
		uint32_t* DPC_BUFBUSY_REG;
		uint32_t* DPC_PIPEBUSY_REG;
		uint32_t* DPC_TMEM_REG;

		void(__cdecl* CheckInterrupts)(void);
		void(__cdecl* ProcessDlistList)(void);
		void(__cdecl* ProcessAlistList)(void);
		void(__cdecl* ProcessRdpList)(void);
		void(__cdecl* ShowCFB)(void);
	} RSP_INFO;

	typedef struct
	{
		void* hWnd; /* Render window */
		void* hStatusBar;
		/* if render window does not have a status bar then this is NULL */

		int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
		//   bswap on a dword (32 bits) boundry
		//	eg. the first 8 bytes are stored like this:
		//        4 3 2 1   8 7 6 5

		uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom
		// This will be in the same memory format as the rest of the memory.
		uint8_t* RDRAM;
		uint8_t* DMEM;
		uint8_t* IMEM;

		uint32_t* MI_INTR_REG;

		uint32_t* DPC_START_REG;
		uint32_t* DPC_END_REG;
		uint32_t* DPC_CURRENT_REG;
		uint32_t* DPC_STATUS_REG;
		uint32_t* DPC_CLOCK_REG;
		uint32_t* DPC_BUFBUSY_REG;
		uint32_t* DPC_PIPEBUSY_REG;
		uint32_t* DPC_TMEM_REG;

		uint32_t* VI_STATUS_REG;
		uint32_t* VI_ORIGIN_REG;
		uint32_t* VI_WIDTH_REG;
		uint32_t* VI_INTR_REG;
		uint32_t* VI_V_CURRENT_LINE_REG;
		uint32_t* VI_TIMING_REG;
		uint32_t* VI_V_SYNC_REG;
		uint32_t* VI_H_SYNC_REG;
		uint32_t* VI_LEAP_REG;
		uint32_t* VI_H_START_REG;
		uint32_t* VI_V_START_REG;
		uint32_t* VI_V_BURST_REG;
		uint32_t* VI_X_SCALE_REG;
		uint32_t* VI_Y_SCALE_REG;

		void(__cdecl* CheckInterrupts)(void);
	} GFX_INFO;

	typedef struct
	{
		void* hwnd;
		void* hinst;

		int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
		//   bswap on a dword (32 bits) boundry
		//	eg. the first 8 bytes are stored like this:
		//        4 3 2 1   8 7 6 5
		uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom
		// This will be in the same memory format as the rest of the memory.
		uint8_t* RDRAM;
		uint8_t* DMEM;
		uint8_t* IMEM;

		uint32_t* MI_INTR_REG;

		uint32_t* AI_DRAM_ADDR_REG;
		uint32_t* AI_LEN_REG;
		uint32_t* AI_CONTROL_REG;
		uint32_t* AI_STATUS_REG;
		uint32_t* AI_DACRATE_REG;
		uint32_t* AI_BITRATE_REG;

		void(__cdecl* CheckInterrupts)(void);
	} AUDIO_INFO;

	typedef struct
	{
		int32_t Present;
		int32_t RawData;
		int32_t Plugin;
	} CONTROL;

	typedef union
	{
		uint32_t Value;

		struct
		{
			unsigned R_DPAD : 1;
			unsigned L_DPAD : 1;
			unsigned D_DPAD : 1;
			unsigned U_DPAD : 1;
			unsigned START_BUTTON : 1;
			unsigned Z_TRIG : 1;
			unsigned B_BUTTON : 1;
			unsigned A_BUTTON : 1;

			unsigned R_CBUTTON : 1;
			unsigned L_CBUTTON : 1;
			unsigned D_CBUTTON : 1;
			unsigned U_CBUTTON : 1;
			unsigned R_TRIG : 1;
			unsigned L_TRIG : 1;
			unsigned Reserved1 : 1;
			unsigned Reserved2 : 1;

			signed Y_AXIS : 8;

			signed X_AXIS : 8;
		};
	} BUTTONS;

	typedef struct
	{
		void* hMainWindow;
		void* hinst;

		int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
		//   bswap on a dword (32 bits) boundry, only effects header.
		//	eg. the first 8 bytes are stored like this:
		//        4 3 2 1   8 7 6 5
		uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom)
		CONTROL* Controls; // A pointer to an array of 4 controllers .. eg:
		// CONTROL Controls[4];
	} CONTROL_INFO;

	typedef struct s_rom_header
	{
		uint8_t init_PI_BSB_DOM1_LAT_REG;
		uint8_t init_PI_BSB_DOM1_PGS_REG;
		uint8_t init_PI_BSB_DOM1_PWD_REG;
		uint8_t init_PI_BSB_DOM1_PGS_REG2;
		uint32_t ClockRate;
		uint32_t PC;
		uint32_t Release;
		uint32_t CRC1;
		uint32_t CRC2;
		uint32_t Unknown[2];
		uint8_t nom[20];
		uint32_t unknown;
		uint32_t Manufacturer_ID;
		uint16_t Cartridge_ID;
		uint16_t Country_code;
		uint32_t Boot_Code[1008];
	} t_rom_header;

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
