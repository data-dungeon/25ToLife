
#if defined(PS2) && defined(SN)

static const char* mNames[17][2] = 
{
	{ "(DISABLED) ", "(DISABLED) " }, // -1
	{ "(reserved) ", "Low Branch " }, // 0
	{ "CPU Cycles ", "CPU Cycles " }, // 1
	{ "Singles    ", "Doubles    " }, // 2
	{ "Branch     ", "BRA PreMiss" }, // 3
	{ "BTAC Miss  ", "TLB Miss   " }, // 4
	{ "TLB Miss   ", "DTLB Miss  " }, // 5
	{ "I$ Miss    ", "D$ Miss    " }, // 6
	{ "DTLB Access", "WBB !single" }, // 7
	{ "!block ld  ", "WBB !burst " }, // 8
	{ "WBB single ", "WBB <full  " }, // 9
	{ "WBB burst  ", "WBB full   " }, // 10
	{ "Addr busy  ", "Data busy  " }, // 11
	{ "Total Instr", "Total Instr" }, // 12
	{ "!BDS Instr ", "!BDS Instr " }, // 13
	{ "COP2 Instr ", "COP1 Instr " }, // 14
	{ "Loads      ", "Stores     " }  // 15
	 										
};

class ProfileTimer
{
	u32 mStart;
public:
	void Start(){
		asm("mfc0 %0, $9" : "=r" (mStart));
	}
	void Stop(){
		u32 end;
		asm("mfc0 %0, $9" : "=r" (end));
		printf("\033[31mCycles: %d\033[0m\n", end - mStart);
	}
};

class ProfileTime : public ProfileTimer
{
public:
	ProfileTime() { Start(); }
	~ProfileTime() { Stop(); }
};

class SinglePerformanceCounter
{
	u32 mCount;
	u32 mSum[2];
	u32 mLast[2];
	u32 mStartVals[2];
	u32 mOvfl[2];
	
public:
	SinglePerformanceCounter()
	{
		mSum[0] = mSum[1] = 0;
		mCount = 0;
		mOvfl[0] = mOvfl[1] = 0;
	}
	void Start(int setNumber, int setNumber2 = -1)
	{
		// build flags to write to pccs
		setNumber &= 0xF;
		u32 flags;
		// set 1
		flags = (setNumber << 5) | 0x001E;
		if(setNumber2 != -1)
		{
			// set 2
			flags |= ((setNumber2 << 5) | 0x001E) << 10;
		}
		// enable
		flags |= 0x80000000;


		// read counter
		asm __volatile__ ("mfpc %0, 0" : "=r" (mStartVals[0]));
		asm __volatile__ ("mfpc %0, 1" : "=r" (mStartVals[1]));

		// start counting...
		asm __volatile__ ("mtps %0, 0" : : "r" (flags));
		asm __volatile__ ("sync.p");
	}

	void Stop()
	{
		// read counters...
		asm __volatile__ ("mfpc %0, 0" : "=r" (mLast[0]));
		asm __volatile__ ("mfpc %0, 1" : "=r" (mLast[1]));

		// build difference & update sum..
		mLast[0] -= mStartVals[0];
		mLast[1] -= mStartVals[1];

		if(mSum[0] + mLast[0] < mSum[0])
			mOvfl[0]++;
		if(mSum[1] + mLast[1] < mSum[1])
			mOvfl[1]++;

		mSum[0] += mLast[0];
		mSum[1] += mLast[1];
		mCount++;
	}

	void Print(int setNumber, int setNumber2 = -1, bool header = false)
	{
		if(header)
			printf("\033[31mname        last  avg   of |name        last  avg   of\033[0m\n");

		printf("\033[31m%s %5.5d %5.5d %s  |%s %5.5d %5.5d %s\033[0m\n",
				 mNames[setNumber+1][0], mLast[0], mSum[0]/mCount, mOvfl[0] ? "Y" : "N",
				 mNames[setNumber2+1][1], mLast[1], mSum[1]/mCount, mOvfl[1] ? "Y" : "N");
	}
};

class PerformanceCounter
{
	u32 mCurrElement;
	
	SinglePerformanceCounter mSingleCounter[16];

	friend class CountPerformance;

public:
	PerformanceCounter()
	{
		mCurrElement = 0;
	}

	void Start()
	{
		mSingleCounter[mCurrElement].Start(mCurrElement, mCurrElement);
	}

	void Stop()
	{
		mSingleCounter[mCurrElement++].Stop();
		if(mCurrElement >= 16)
			mCurrElement = 0;
	}

	void Print(int frequency = 1)
	{
		static int frame = 0;
		if(++frame >= frequency)
		{
			for(int i = 0; i < 16; i++)
				mSingleCounter[i].Print(i, i, i == 0);
			frame = 0;
		}
	}
};

class CountPerformance
{
public:
	CountPerformance(PerformanceCounter& counter)
		: mCounter(counter)
	{
		mCounter.Start();
	}

	~CountPerformance()
	{
		mCounter.Stop();
	}

	PerformanceCounter& mCounter;
};

#endif // defined(PS2) && defined(SN)
