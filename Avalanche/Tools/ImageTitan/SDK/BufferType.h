#pragma once

enum BufferDataType {
   BDT_BYTE  = 0x1401, // unsigned byte 0-255 
   BDT_INT   = 0x1405, // unsigned integer number, used for ID Channels 
   BDT_FLOAT = 0x1406, // 32 bit IEEE floating point
   BDT_HALF  = 0x140B, // 16 bit IEEE floating point   
};

enum BufferChannels {
   BC_BGR              = 0x80E0, // 3 channels
   BC_BGRA             = 0x80E1, // 4 channels
   BC_LUMINANCE        = 0x1909, // 1 channel
   BC_LUMINANCE_ALPHA  = 0x190A, // 2 channels
};
