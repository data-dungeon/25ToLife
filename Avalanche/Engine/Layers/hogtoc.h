//*************************************************************************
// HOG table of contents
//*************************************************************************
#ifndef HOGTOC_H
#define HOGTOC_H

#define HOGTOC_CURRENTVERSION 0x00020001

#if defined(__cplusplus)
extern "C" {
#endif

#pragma pack(1)
struct HOGTOC_HDR
{
	u32								version;
	u32								hdrSize;
	u32								encrypt;
	u32								crc;
	u32								entries;
	u32								tocSize;
	u32								tocCRC;
};
#pragma pack()

// the data, ready to go
#pragma pack(1)
struct HOGTOC_ENTRY
{
	char 								*name;
	char								*data;
	u32								size;
	u32								crc;
};
#pragma pack()

#if defined(__cplusplus)
}
#endif

#endif
