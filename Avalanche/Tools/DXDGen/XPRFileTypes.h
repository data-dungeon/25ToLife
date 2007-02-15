#pragma once

#define XPR0_MAGIC_VALUE	0x30525058 // 'XPR0'
#define XPR1_MAGIC_VALUE	0x31525058 // 'XPR1'
#define XPR2_MAGIC_VALUE	0x32525058 // 'XPR2'

// Resource struct for tracking
typedef struct _XPRFileHeader
{
	DWORD						dwMagic;
	DWORD						dwTotalSize;
	DWORD						dwHeaderSize;
} XPRFileHeader;

