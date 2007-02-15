#ifndef __VU0_REGISTER_PRESERVER__
#define __VU0_REGISTER_PRESERVER__


class VU0RegisterPreserver
{
	#define NUM_32_BIT_REGISTERS 24
	#define NUM_128_BIT_REGISTERS 32
	u128 VU0Register128StorageSpace[NUM_128_BIT_REGISTERS];
	u32 VU0Register32StorageSpace[NUM_32_BIT_REGISTERS];
	bool isSaved;
public:
	VU0RegisterPreserver()
	{
		isSaved = false;
	}
	void Save()
	{
		ASSERT(!isSaved);
		isSaved = true;

		u128 registerValue128;
		u32 registerValue32;
		int i = 0;

		asm ("cfc2 %0, vi0" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi1" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi2" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi3" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi4" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi5" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi6" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi7" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi8" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi9" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi10" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi11" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi12" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi13" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi14" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi15" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;


		asm ("cfc2 %0, vi16" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi17" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi18" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi20" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi21" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi22" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi27" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;
		asm ("cfc2 %0, vi28" : "=r" (registerValue32) );
		VU0Register32StorageSpace[i++] = registerValue32;

		ASSERT(i == NUM_32_BIT_REGISTERS );

		i = 0;
		asm ("qmfc2 %0, vf1" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf2" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf3" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf4" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf5" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf6" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf7" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf8" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf9" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf10" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf11" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf12" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf13" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf14" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf15" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf16" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf17" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf18" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf19" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf20" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf21" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf22" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf23" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf24" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf25" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf26" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf27" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf28" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf29" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf30" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;
		asm ("qmfc2 %0, vf31" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;

		asm ("VMADDx.xyzw vf1, vf0, vf0\r\n"
			  "qmfc2 %0, vf1" : "=r" (registerValue128) );
		VU0Register128StorageSpace[i++] = registerValue128;

		ASSERT(i == NUM_128_BIT_REGISTERS );
	}
	void Restore()
	{
		ASSERT(isSaved);
		isSaved = false;

		u128 registerValue128;
		u32 registerValue32;
		int i = NUM_128_BIT_REGISTERS;

		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf1\r\n"
			  "VMADDAx.xyzw ACC, vf1, vf0" : "=r" (registerValue128) );

		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf31" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf30" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf29" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf28" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf27" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf26" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf25" : "=r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf24" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf23" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf22" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf21" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf20" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf19" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf18" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf17" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf16" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf15" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf14" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf13" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf12" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf11" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf10" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf9" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf8" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf7" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf6" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf5" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf4" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf3" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf2" : : "r" (registerValue128) );
		registerValue128 = VU0Register128StorageSpace[--i];
		asm ("qmtc2 %0, vf1" : : "r" (registerValue128) );

		ASSERT( i == 0 );

		i = NUM_32_BIT_REGISTERS;

		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi28" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi27" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi22" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi21" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi20" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi18" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi17" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi16" : : "r" (registerValue32) );

		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi15" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi14" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi13" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi12" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi11" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi10" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi9" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi8" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi7" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi6" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi5" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi4" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi3" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi2" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi1" : : "r" (registerValue32) );
		registerValue32 = VU0Register32StorageSpace[--i];
		asm ("ctc2 %0, vi0" : : "r" (registerValue32) );

		ASSERT( i == 0 );
	}
};

// defined in PS2App.cpp
extern VU0RegisterPreserver g_VU0RegisterPreserver;

#endif // __VU0_REGISTER_PRESERVER__

