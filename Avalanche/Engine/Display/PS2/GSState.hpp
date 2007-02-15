/*************************************/
/*                                   */
/*   d_GSState.hpp                   */
/*   new engine  04/6/01             */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   gs chip state management        */
/*                                   */
/*************************************/

#ifdef PS2	//ps2 only


inline void GSState__Set(u64 u64Address, u64 u64Data)
{
	/* if address is same as last address in list, just replace data (no sense in sending data twice to same register) */

// TJC - dont do this.. its an extra check EVERYWHERE... lots of ICache problems... and its not really saving us anything.
// most resends dont happen sequentially...
// 	if ((u32GSStateCount != 0) && (pADPairs[u32GSStateCount - 1].u64Address == u64Address))
// 	{
// 		pADPairs[u32GSStateCount - 1].u64Data = u64Data;
// 		return;
// 	}

	/* need to add state. if current buffer full, send it */

	if (u32GSStateCount == MAX_GSSTATES_IN_ONE_PACKET)
		GSState__Send();

	/* add state */

	pADPairs[u32GSStateCount].u64Address = u64Address;
	pADPairs[u32GSStateCount].u64Data = u64Data;
	u32GSStateCount++;
}


inline void GSState__Set2(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1)
{
	if(u32GSStateCount >= MAX_GSSTATES_IN_ONE_PACKET-1)
		GSState__Send();

	pADPairs[u32GSStateCount].u64Address = u64Address0;
	pADPairs[u32GSStateCount].u64Data = u64Data0;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address1;
	pADPairs[u32GSStateCount].u64Data = u64Data1;
	u32GSStateCount++;
}

inline void GSState__Set3(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1,
								  u64 u64Address2, u64 u64Data2)
{
	if(u32GSStateCount >= MAX_GSSTATES_IN_ONE_PACKET-2)
		GSState__Send();

	pADPairs[u32GSStateCount].u64Address = u64Address0;
	pADPairs[u32GSStateCount].u64Data = u64Data0;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address1;
	pADPairs[u32GSStateCount].u64Data = u64Data1;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address2;
	pADPairs[u32GSStateCount].u64Data = u64Data2;
	u32GSStateCount++;
}

inline void GSState__Set4(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1,
								  u64 u64Address2, u64 u64Data2,
								  u64 u64Address3, u64 u64Data3)
{
	if(u32GSStateCount >= MAX_GSSTATES_IN_ONE_PACKET-3)
		GSState__Send();

	pADPairs[u32GSStateCount].u64Address = u64Address0;
	pADPairs[u32GSStateCount].u64Data = u64Data0;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address1;
	pADPairs[u32GSStateCount].u64Data = u64Data1;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address2;
	pADPairs[u32GSStateCount].u64Data = u64Data2;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address3;
	pADPairs[u32GSStateCount].u64Data = u64Data3;
	u32GSStateCount++;
}

inline void GSState__Set5(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1,
								  u64 u64Address2, u64 u64Data2,
								  u64 u64Address3, u64 u64Data3,
								  u64 u64Address4, u64 u64Data4)
{
	if(u32GSStateCount >= MAX_GSSTATES_IN_ONE_PACKET-4)
		GSState__Send();

	pADPairs[u32GSStateCount].u64Address = u64Address0;
	pADPairs[u32GSStateCount].u64Data = u64Data0;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address1;
	pADPairs[u32GSStateCount].u64Data = u64Data1;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address2;
	pADPairs[u32GSStateCount].u64Data = u64Data2;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address3;
	pADPairs[u32GSStateCount].u64Data = u64Data3;
	u32GSStateCount++;
	pADPairs[u32GSStateCount].u64Address = u64Address4;
	pADPairs[u32GSStateCount].u64Data = u64Data4;
	u32GSStateCount++;
}
#endif //PS2
