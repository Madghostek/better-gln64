#include <windows.h>
#include "N64.h"
#include "Types.h"

u8 *DMEM;
u8 *IMEM;
u64 TMEM[512];
u8 *RDRAM;
u32 RDRAMSize = 0x01f610a0; //0x800000 / 4;

N64Regs REG;
