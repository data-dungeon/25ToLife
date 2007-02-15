/*************************************/
/*                                   */
/*   aura.c                          */
/*   big juju prototype  06/03/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   aura system                     */
/*                                   */
/*************************************/

/* auras are typically a translucent version of an instance moved closer to the camera so that it looks bigger and
surrounds the instance */

/******************* includes ****************************/

#include "Game/GamePCH.h"



#include "Layers/Timing.h"



/******************* defines *****************************/

/* max simultaneous auras */

#define MAX_AURAS 50

/* aura types */

enum
{
	AURA_UNKNOWN = 0,			// temporary until we get some types defined

	MAX_AURA_TYPES		// must be last
};

/* flags (for ts_Aura.dh.u16Flags) */

#define AURA_NEEDS_ADVANCEMENT		0x0001	// for AdvanceAuras use
#define AURA_HAS_SINE_WAVE				0x0002
#define AURA_HAS_AMPLITUDE_DAMPING	0x0004	// amplitude damping is active
#define AURA_HAS_DISTANCE_DAMPING	0x0008	// distance damping is active
#define AURA_DELAY_DISTANCE_DAMPING	0x0010	// use distance damping only after lifetime ends
#define AURA_ZERO_DISTANCE_DEALLOC	0x0020	// use distance damping only after lifetime ends

/******************* macros ******************************/

/******************* structures **************************/

/* general aura stucture */

struct ts_Aura
{
	ts_SubFlagsDataHeader	dh;						// data header-- MUST BE FIRST-- see comments in datatypes.h
	void							*pOwner;
	float							Distance;				// distance once damping, sine wave taken into account
	float							BaseDistance;			// initial (passed) distance from owner, towards camera
	float							BaseDistanceDamping;	// negative damping reduces base distance over time, aura ends when it reaches 0.0
	float							SinAngle;				// for sine-wave strength cycling
	float							SinAngleInc;			// for sine-wave strength cycling
	float							Amplitude;				// for sine-wave strength cycling
	float							AmplitudeDamping;		// negative damping reduces sine wave amplitude over time, sine wave ends when it reaches 0.0
	float							Lifetime;				// remaining lifetime, 0.0 means forever
	ts_Aura						*pNext;					// linked list pointer-- does not have to be first
};

/* aura manager */

struct ts_AuraMgr
{
	ts_SLinkList FreeList;		// linked list of free records
	ts_SLinkList ActiveList;	// linked list of active records
	ts_Aura *pRecordBuffer;		// pointer to malloc'd records

	/* for aura advancing */

	ts_Aura **ppAdvanceList;
	int nAdvanceListSize;
};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* aura system */

static ts_AuraMgr AuraMgr;

/******************* local prototypes ********************/

static ts_Aura *AuraMgr__AllocAura(void);
static void AuraMgr__FreeAura(ts_Aura *pAura);
static void AuraMgr__SetAuraDistance(ts_Aura *pAura);

/******************* functions ***************************/

/* initialize aura system */

int AuraMgr__Init(int nMaxAuras)
{
	int i;
	ts_Aura *pAura;

	/* check some stuff */

	ASSERT(MAX_AURA_TYPES <= 256);	// sub type field is an unsigned char

	/* clear the manager */

	memset(&AuraMgr, 0, sizeof(ts_AuraMgr));

	/* done if no auras */

	if (nMaxAuras == 0)
		return(TRUE);

	/* allocate records, initialize linked lists */

	ASSERT(nMaxAuras < MAX_HANDLES);	// see definition of MAX_HANDLES
	MEM_SET_ALLOC_NAME("AuraMgr list");
	AuraMgr.pRecordBuffer = (ts_Aura *) memAlloc(nMaxAuras * sizeof(ts_Aura));
	SLinkList__Init(&AuraMgr.FreeList, offsetof(ts_Aura, pNext));
	SLinkList__LinkBlock(&AuraMgr.FreeList, AuraMgr.pRecordBuffer, nMaxAuras, sizeof(ts_Aura));
	SLinkList__Init(&AuraMgr.ActiveList, offsetof(ts_Aura, pNext));
	
	/* allocate advance list */

	MEM_SET_ALLOC_NAME("AuraMgr advance");
	AuraMgr.ppAdvanceList = (ts_Aura **) memAlloc(nMaxAuras * sizeof(ts_Aura *));

	/* initialize records */

	pAura = AuraMgr.pRecordBuffer;
	for (i = 0; i < nMaxAuras; i++, pAura++)
	{
		pAura->dh.u8DataType = DATA_AURA;
		pAura->dh.u8SubDataType = AURA_UNKNOWN;
		pAura->dh.u16Flags = 0;
	}

	/* success */

	return(TRUE);
}

static ts_Aura *AuraMgr__AllocAura(void)
{
	ts_Aura *pAura;

	/* grab a aura record from the free list */

	pAura = (ts_Aura *) SLinkList__GetAndRemoveHead(&AuraMgr.FreeList);
	if (pAura == NULL)
		return(NULL);
	ASSERT(pAura->dh.u8DataType == DATA_AURA);

	/* add to head of active list */

	SLinkList__AddLinkToHead(&AuraMgr.ActiveList, pAura);

	/* clear all flags and done */

	pAura->dh.u16Flags = 0;
	return(pAura);
}

static void AuraMgr__FreeAura(ts_Aura *pAura)
{
	/* memory check, list check */

	ASSERT(pAura >= AuraMgr.pRecordBuffer);
	ASSERT(SLinkList__InList(&AuraMgr.ActiveList, pAura));

//yuch-- notify owner

	/* move from active to free list */

	SLinkList__RemoveLink(&AuraMgr.ActiveList, pAura);
	SLinkList__AddLinkToHead(&AuraMgr.FreeList, pAura);
	pAura->dh.u16Flags = 0;	// clear all flags
}

/* add a aura */

t_Handle AuraMgr__AddAura(void *pOwner, float Distance, float DistanceDamping, float SinAngleStart, float SinAngleInc, float Amplitude, float AmplitudeDamping, float Lifetime, int nFlags)
{
	ts_Aura *pAura;

	/* rules */

	ASSERT(Amplitude >= 0.0f);											// no negative amplitudes allowed (SinAngleStart provides all needed flexibility)
	ASSERT(AmplitudeDamping <=0.0f);									// can't handle growing amplitude at this point
	ASSERT(DistanceDamping <= 0.0f);									// can't handle growing distance at this point
	ASSERT((SinAngleInc == 0.0f) || (Amplitude != 0.0f));		// if sine wave active, amplitude must be non-zero
	ASSERT((AmplitudeDamping == 0.0f) || (Amplitude != 0.0f));	// if amplitude damping active, amplitude must be non-zero
	ASSERT((DistanceDamping == 0.0f) || (Distance != 0.0f));	// if distance damping active, distance must be non-zero
	ASSERT(Distance + Amplitude > 0.0f);								// effective distance must be greater than 0.0 at some point for aura to be seen
	ASSERT((nFlags & ~0xff) == 0);										// flags must currently fit in a byte

	/* grab an aura record */

	pAura = AuraMgr__AllocAura();
	if (pAura == NULL)
		return(INVALID_HANDLE);

	/* set up aura */

	pAura->pOwner = pOwner;
	pAura->BaseDistance = Distance;
	pAura->BaseDistanceDamping = DistanceDamping;
	pAura->SinAngle = SinAngleStart;
	pAura->SinAngleInc = SinAngleInc;
	pAura->Amplitude = Amplitude;
	pAura->AmplitudeDamping = AmplitudeDamping;
	pAura->Lifetime = Lifetime;

	/* flags */

	pAura->dh.u16Flags = nFlags;
	if (SinAngleInc != 0.0f)
		pAura->dh.u16Flags |= AURA_HAS_SINE_WAVE;
	if (AmplitudeDamping != 0.0f)
		pAura->dh.u16Flags |= AURA_HAS_AMPLITUDE_DAMPING;
	if ((DistanceDamping != 0.0f) && ((nFlags & AURA_DELAY_DISTANCE_DAMPING) == 0))
		pAura->dh.u16Flags |= AURA_HAS_DISTANCE_DAMPING;

	/* calculate initial distance */

	AuraMgr__SetAuraDistance(pAura);

	/* success */

//yuch-- need to implement handles
	return(0);	
}

/* update aura system */

void AuraMgr__Advance(void)
{
	ts_Aura *pAura;
	int i;
	bool bDealloc;

	/* quick exit */

	if (AuraMgr.ActiveList.pHead == NULL)
		return;

	/* build list of auras to advance. this is necessary because auras may get stolen during advance process */
	/* (mainly due to spawning at life's end), and i don't want to advance new auras. worse things can happen, */
	/* too, like the chain of links pointing back to the beginning of the list and advancing auras twice */

	AuraMgr.nAdvanceListSize = 0;
	pAura = (ts_Aura *) AuraMgr.ActiveList.pHead;
	while (pAura)
	{
		AuraMgr.ppAdvanceList[AuraMgr.nAdvanceListSize++] = pAura;
		pAura->dh.u16Flags |= AURA_NEEDS_ADVANCEMENT;	// will be cleared by alloc/dealloc if aura gets stolen
		pAura = pAura->pNext;
	}

	/* go through auras */

	for (i = 0; i < AuraMgr.nAdvanceListSize; i++)
	{
		pAura = AuraMgr.ppAdvanceList[i];

		/* check advancement flag */

		if ((pAura->dh.u16Flags & AURA_NEEDS_ADVANCEMENT) == 0)
			continue;

		/* still alive */

		bDealloc = FALSE;

		/* sine wave */

		if (pAura->dh.u16Flags & AURA_HAS_SINE_WAVE)
			pAura->SinAngle += pAura->SinAngleInc * SecondsPerFrame;

		/* distance damping */

		if (pAura->dh.u16Flags & AURA_HAS_DISTANCE_DAMPING)
		{
			pAura->BaseDistance += pAura->BaseDistanceDamping * SecondsPerFrame;
			if (pAura->BaseDistance <= 0.0f)
				pAura->dh.u16Flags |= AURA_ZERO_DISTANCE_DEALLOC;	// dealloc next time effective distance <= 0.0 (avoids sudden cut out if sine wave still going)
		}

		/* amplitude damping */

		if (pAura->dh.u16Flags & AURA_HAS_AMPLITUDE_DAMPING)
		{
			pAura->Amplitude += pAura->AmplitudeDamping * SecondsPerFrame;
			if (pAura->Amplitude <= 0.0f)
				pAura->dh.u16Flags &= ~AURA_HAS_AMPLITUDE_DAMPING;
		}

		/* lifetime */

		if (pAura->Lifetime != 0.0f)
		{
			pAura->Lifetime -= SecondsPerFrame;
			if (pAura->Lifetime <= 0.0f)
			{
				pAura->Lifetime = 0.0f;
				if ((pAura->dh.u16Flags & AURA_DELAY_DISTANCE_DAMPING) && (pAura->BaseDistanceDamping != 0.0f))
					pAura->dh.u16Flags |= AURA_HAS_DISTANCE_DAMPING;
				else
					bDealloc = TRUE;
			}
		}

		/* calculate distance. maybe time to kill aura? */

		AuraMgr__SetAuraDistance(pAura);
		if ((pAura->Distance <= 0.0f) && (pAura->dh.u16Flags & AURA_ZERO_DISTANCE_DEALLOC))
			bDealloc = TRUE;

		/* did current aura end? */

		if (bDealloc)
			AuraMgr__FreeAura(pAura);
	}
}

/* set the effective distance of an aura from its owner */

static void AuraMgr__SetAuraDistance(ts_Aura *pAura)
{
	pAura->Distance = pAura->BaseDistance;
	if (pAura->dh.u16Flags & AURA_HAS_SINE_WAVE)
		pAura->Distance += pAura->Amplitude * Math::Sin(pAura->SinAngle);
	if (pAura->Distance < 0.0f)
		pAura->Distance = 0.0f;	// distance <= 0.0f means no aura visible, 0.0f is easier to test for
}

