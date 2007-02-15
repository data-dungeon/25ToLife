#pragma once

enum DXDChunkTypes
{
	TYPE_XPR,
	TYPE_XVD,
};

struct DXDChunkHeader
{
	DWORD						dwType;
	DWORD						dwSystemMemorySize;
	DWORD						dwSystemMemoryOffset;
	DWORD						dwVideoMemorySize;
	DWORD						dwVideoMemoryOffset;
	char						chPad[12];
};

