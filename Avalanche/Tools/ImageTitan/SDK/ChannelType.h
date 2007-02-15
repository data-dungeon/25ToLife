#pragma once

enum ChannelType {
   BYTE_CHANNEL   = 1 << 1, // 0-256 
   HALF_CHANNEL   = 1 << 2, // 16 bit IEEE floating point   
   INT_CHANNEL    = 1 << 3, // Integer number, used for ID Channels 
   FLOAT_CHANNEL  = 1 << 4  // 32 bit IEEE floating point
};
