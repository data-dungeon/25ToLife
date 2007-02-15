//============================================================================
//=
//= Decoy.h -- a class that allocs and frees random sized blockes at random
//=            intervals, looking for someone to stomp it.
//=
//============================================================================

#ifndef _DECOY_H
#define _DECOY_H

#include "Layers/Breakpoint/HardwareBreak.h"

class CDecoy
{
public:
	CDecoy(int minSize, int maxSize, int minInterval, int maxInterval)
	{
		d_minSize = minSize;
		d_maxSize = maxSize;
		d_minInterval = minInterval;
		d_maxInterval = maxInterval;
		d_block = NULL;
		d_blockSize = 0;
		d_byte = 0;
		d_nextTime = d_minInterval;
	}
	~CDecoy()
	{
		Reset();
	}
	void Reset()
	{
		FreeBlock();
	}
	void Update()
	{
		u32 curTime = g_timer.GetOSTime();
		if (curTime >= d_nextTime)
		{
			FreeBlock();
			AllocBlock();
			d_nextTime = curTime + g_random.InRange(d_minInterval, d_maxInterval);
		}
	}
	void FreeBlock()
	{
		if (d_block)
		{
			CheckBlock();
			HardwareBreak::Disable();
			delete d_block;
			d_block = NULL;
			d_blockSize = 0;
			d_byte = 0;
		}
	}
	void AllocBlock()
	{
		d_blockSize = g_random.InRange(d_minSize, d_maxSize);
		d_block = new u8[d_blockSize];
		d_byte = g_random.InRange(1, 255);
		memset(d_block, d_byte, d_blockSize);
		HardwareBreak::Enable(true, false, (u32)d_block, (u32)d_blockSize);
	}
	void CheckBlock()
	{
		for (int i = d_blockSize - 1; i >= 0; i--)
			if (d_block[i] != d_byte)
				*(volatile int*)1 = 0;
	}

private:
	u8*	d_block;
	int	d_blockSize;
	u8		d_byte;
	u32	d_nextTime;
	int	d_minSize;
	int	d_maxSize;
	int	d_minInterval;
	int	d_maxInterval;
};

#endif //_DECOY_H
