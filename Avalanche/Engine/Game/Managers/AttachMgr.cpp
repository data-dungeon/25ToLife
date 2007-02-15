/*************************************/
/*                                   */
/*   AttachMgr.c                     */
/*   big juju   07/09/02             */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   things attached to actor bones  */
/*                                   */
/*************************************/

/* attaching objects attach to an actor at some bone location. they may be instances (like arrows)
or emitters (like fire), or maybe other things, like actors. attachers record which bone of which
actor they are attached to, and get updated after all actors have run their animation so that the
bone locations are correct, and they get rendered in the correct place */

/******************* includes ****************************/

#include "Game/GamePCH.h"



#include "EngineHelper/timer.h"

/******************* defines *****************************/

/* flags for ts_Attacher.u16Flags */

#define ATTACHER_INSTANCE		0x0001
#define ATTACHER_EMITTER		0x0002
#define ATTACHER_FORCEFIELD	0x0004
#define ATTACHER_TYPE_MASK		(ATTACHER_INSTANCE | ATTACHER_EMITTER | ATTACHER_FORCEFIELD)
#define ATTACHER_DISABLED		0x0008	// enabled by default, easier to leave bit clear at creation

/******************* macros ******************************/

#define ATTACH_GET_RECORD(Handle)		&AttachMgr.pAttacherAlloc[HANDLE_INDEX(Handle)]

/******************* structures **************************/

/* an individual attacher */

struct ts_Attacher
{
	t_Handle						Handle;				//	assigned handle for access, invalid handle checking
	ts_Attacher					*pNext;				// linked list pointer-- does not have to be first
	CProtagonist				*pProtagonist;		// protagonist attacher is attached to
	float							fLifetime;			// in seconds, 0.0 means forever
	Vector3						vWorldPos;			// calculated position of attach point, in world coords
	Vector3						vBonePos;			// attach position in bone space, so it can follow bone
	Vector3						vRotation;			// local rotation of instance
	ts_Bone						*pBone;				// bone attacher is attached to
	u16							u16Flags;

	/* different types have different data needs */

	union
	{
		t_Handle					EmitterHandle;		// for ATTACH_EMITTER type
		ts_PoolInstance		*pPoolInstance;	// for ATTACH_INSTANCE type
		t_Handle					ForceFieldHandle;	// for ATTACH_FORCEFIELD type
	};
};

/* attach manager */

struct ts_AttachMgr
{
	ts_SLinkList	AttacherFree;		// linked list of free records
	ts_SLinkList	AttacherActive;	// linked list of active records
	ts_Attacher		*pAttacherAlloc;	// pointer to malloc'd records
	u16				u16ID;				// for assigning handles
	int				nMaxAttachers;		// record passed value for debugging, not really necessary otherwise

#ifndef CONSUMER_BUILD
	/* debug */

	Vector3			vLastCollidePos;
	Vector3			vLastCollideVel;
#endif //CONSUMER_BUILD

};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* attach manager */

static ts_AttachMgr AttachMgr;

/******************* local prototypes ********************/

ts_Attacher *AttachMgr__AllocAttacher();
bool AttachMgr__AttachToBone(CProtagonist *pProtagonist, ts_Bone* pBone, float fDistance, ts_Attacher *pAttacher);
void Attacher__Kill(ts_Attacher *pAttacher);

/******************* functions ***************************/

/* initialize attach system */

int AttachMgr__Init(int nMaxAttachers)
{
	/* clear the manager */

	memset(&AttachMgr, 0, sizeof(ts_AttachMgr));

	/* done if no attachers */

	if (nMaxAttachers == 0)
		return(TRUE);
	AttachMgr.nMaxAttachers = nMaxAttachers;

	/* allocate records, initialize linked lists */

	MEM_SET_ALLOC_NAME("AttachMgr list");
	AttachMgr.pAttacherAlloc = (ts_Attacher *) memAlloc(AttachMgr.nMaxAttachers * sizeof(ts_Attacher));
	SLinkList__Init(&AttachMgr.AttacherFree, offsetof(ts_Attacher, pNext));
	SLinkList__LinkBlock(&AttachMgr.AttacherFree, AttachMgr.pAttacherAlloc, AttachMgr.nMaxAttachers, sizeof(ts_Attacher));
	SLinkList__Init(&AttachMgr.AttacherActive, offsetof(ts_Attacher, pNext));
	
	/* initialize records */

	ts_Attacher *pAttacher = AttachMgr.pAttacherAlloc;
	for (int i = 0; i < AttachMgr.nMaxAttachers; i++, pAttacher++)
	{
		pAttacher->Handle = INVALID_HANDLE;
		pAttacher->u16Flags = 0;
	}

	/* success */

	return(TRUE);
}

/* reset */

void AttachMgr__Reset(void)
{
	/* kill all attachers */

	AttachMgr__KillAllAttachers();

	/* all attachers should be in free list now, ready to go */

	ASSERT(SLinkList__Check(&AttachMgr.AttacherFree) == (signed) AttachMgr.nMaxAttachers);

	/* clear all handles */

	ts_Attacher *pAttacher = (ts_Attacher *) AttachMgr.AttacherFree.pHead;
	while (pAttacher)
	{
		pAttacher->Handle = INVALID_HANDLE;
		pAttacher = pAttacher->pNext;
	}
}

/* shutdown */

void AttachMgr__Shutdown(void)
{
	/* kill all attachers */

	AttachMgr__KillAllAttachers();

	/* shut down */

	if (AttachMgr.pAttacherAlloc)
		memFree(AttachMgr.pAttacherAlloc);
	memset(&AttachMgr, 0, sizeof(ts_AttachMgr));
}

ts_Attacher *AttachMgr__AllocAttacher()
{
	if (AttachMgr.AttacherFree.pHead == NULL)
		return(NULL);	// no free attacher record
	ts_Attacher *pAttacher = (ts_Attacher *) SLinkList__GetAndRemoveHead(&AttachMgr.AttacherFree);

	/* build handle */

	ASSERT(pAttacher->Handle == INVALID_HANDLE);				// record should not have a handle assigned
	HANDLE_INDEX(pAttacher->Handle) = pAttacher - AttachMgr.pAttacherAlloc;	// index into allocated array of records
	HANDLE_ID(pAttacher->Handle) = AttachMgr.u16ID++;
	if (pAttacher->Handle == INVALID_HANDLE)
		HANDLE_ID(pAttacher->Handle) = AttachMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pAttacher->Handle != INVALID_HANDLE);				// record should have a valid handle

	/* add to head of active list */

	SLinkList__AddLinkToHead(&AttachMgr.AttacherActive, pAttacher);

	/* clear record and done */

	pAttacher->u16Flags = 0;			// clear all flags
	return(pAttacher);
}

/* kill all attachers attached to a particular actor, typically when actor goes away */

void AttachMgr__KillProtagonistAttachers(CProtagonist *pProtagonist)
{
	ts_Attacher *pNextAttacher = (ts_Attacher *) AttachMgr.AttacherActive.pHead;
	while (pNextAttacher)
	{
		/* pre-load next attach since current one may get killed */

		ts_Attacher *pAttacher = pNextAttacher;
		pNextAttacher = pNextAttacher->pNext;

		/* this actor? */

		if (pAttacher->pProtagonist == pProtagonist)
			Attacher__Kill(pAttacher);
	}
}

/* enable/disable all attachers attached to a particular actor */

void AttachMgr__EnableProtagonistAttachers(CProtagonist *pProtagonist, bool bEnable)
{
	ts_Attacher *pAttacher = (ts_Attacher *) AttachMgr.AttacherActive.pHead;
	while (pAttacher)
	{
		/* this actor? */
		if (pAttacher->pProtagonist == pProtagonist)
			Attacher__Enable(pAttacher->Handle, bEnable);

		pAttacher = pAttacher->pNext;
	}
}

void AttachMgr__KillAllAttachers(void)
{
	/* loop through active attachers */

	while (AttachMgr.AttacherActive.pHead)
	{
		/* kill it */

		Attacher__Kill((ts_Attacher *) AttachMgr.AttacherActive.pHead);
	}

	/* all attachers should be in free list now, ready to go */

	ASSERT(SLinkList__Check(&AttachMgr.AttacherFree) == (signed) AttachMgr.nMaxAttachers);
}

/* update attach system-- position all attachers to actor bone they are attached to. this routine needs to be called
after the actors are updated (by g_stage.AdvanceSimulation), but before scene is rendered. it works to have this called
by GameEngine__Advance, since that is called in between the two */

void AttachMgr__Advance(void)
{
	/* quick exit */

	if (AttachMgr.AttacherActive.pHead == NULL)
		return;

	/* get time slice */

	float fFrameSeconds = g_timer.GetFrameSec();

	/* loop through attachers */

	ts_Attacher *pNextAttacher = (ts_Attacher *) AttachMgr.AttacherActive.pHead;
	while (pNextAttacher)
	{
		/* pre-load next attach since current one may get killed */

		ts_Attacher *pAttacher = pNextAttacher;
		pNextAttacher = pNextAttacher->pNext;

		/* lifetime */

		if (pAttacher->fLifetime != 0.0f)	// 0.0 means infinite
		{
			pAttacher->fLifetime -= fFrameSeconds;
			if (pAttacher->fLifetime <= 0.0f)
			{
				Attacher__Kill(pAttacher);
				continue;	// dead, nothing more to do
			}
		}

		/* disabled? */

		if (pAttacher->u16Flags & ATTACHER_DISABLED)
			continue;

		/* update attacher position. we have a position in bone space, need to transform to world space */

		Graphics4x4 mBoneToWorld;
		mBoneToWorld.FastMultiply(pAttacher->pBone->Matrix, pAttacher->pProtagonist->Instance()->Matrix());
		pAttacher->vWorldPos = pAttacher->vBonePos * mBoneToWorld;

		/* special cases? */

		switch(pAttacher->u16Flags & ATTACHER_TYPE_MASK)
		{
		case ATTACHER_INSTANCE:
			/* calculate new mLocalToWorld transform */

			Graphics4x4 mLocal;
			mLocal.SetXYZRotation(pAttacher->vRotation);
			mLocal.CatTranslation(pAttacher->vBonePos.x(), pAttacher->vBonePos.y(), pAttacher->vBonePos.z());
			pAttacher->pPoolInstance->Instance.Matrix() = mLocal * mBoneToWorld;
			pAttacher->pPoolInstance->Instance.UpdateBoundingVolumes();
			break;
		}
	}
}

/* create attachers */

t_Handle AttachMgr__CreateAttacher(ts_PatchSOM *pSOM, CProtagonist *pProtagonist, Vector3CRef vCollidePos, Vector3CRef vCollideVel, const float fLifetime /*= 0.0f*/)
{
	/* alloc an attacher-- should always succeed-- steals oldest if none available */

	ts_Attacher *pAttacher = AttachMgr__AllocAttacher();
	if (pAttacher == NULL)
		return(INVALID_HANDLE);

	/* get an instance, add to scene tree */

	pAttacher->pPoolInstance = InstancePool__Alloc(pAttacher);
	if (pAttacher->pPoolInstance == NULL)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}
	ts_Geometry geom;
	geom.pLOD = NULL;
	geom.pSOM = pSOM;
	pAttacher->pPoolInstance->Instance.InitFromGeometry( geom );
	pAttacher->pPoolInstance->Instance.Matrix().SetTranslation(pAttacher->vWorldPos);
	pAttacher->pPoolInstance->Instance.UpdateBoundingVolumes();
	SceneMgr::AddInstance(&pAttacher->pPoolInstance->Instance);

	/* record data */

	pAttacher->u16Flags = ATTACHER_INSTANCE;
	pAttacher->pProtagonist = pProtagonist;
	pAttacher->fLifetime = fLifetime;

	/* done */

	return(pAttacher->Handle);
}
t_Handle AttachMgr__CreateAttacher(ts_PatchSOM *pSOM, CProtagonist *pProtagonist, ts_Bone *pBone, float fDistance, Vector3CRef vRotation, const float fLifetime /*= 0.0f*/)
{
	/* alloc an attacher-- should always succeed-- steals oldest if none available */

	/* this attacher uses a bone as an anchor point.  the fDistance parameter 
	   determines where along the bone the pSOM will be located and the 
		vRotation parameter is a rotation that is applied to the pSOM in its 
		local space */

	ts_Attacher *pAttacher = AttachMgr__AllocAttacher();
	if (pAttacher == NULL)
		return(INVALID_HANDLE);

	/* determine attachment point */

	if (AttachMgr__AttachToBone(pProtagonist, pBone, fDistance, pAttacher) == false)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}

	/* get an instance, add to scene tree */

	pAttacher->pPoolInstance = InstancePool__Alloc(pAttacher);
	if (pAttacher->pPoolInstance == NULL)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}
	ts_Geometry geom;
	geom.pLOD = NULL;
	geom.pSOM = pSOM;
	pAttacher->pPoolInstance->Instance.InitFromGeometry( geom );
	pAttacher->pPoolInstance->Instance.Matrix().SetTranslation(pAttacher->vWorldPos);
	pAttacher->pPoolInstance->Instance.UpdateBoundingVolumes();
	SceneMgr::AddInstance(&pAttacher->pPoolInstance->Instance);

	/* record data */

	pAttacher->u16Flags = ATTACHER_INSTANCE;
	pAttacher->pProtagonist = pProtagonist;
	pAttacher->fLifetime = fLifetime;
	pAttacher->vRotation = vRotation;

	/* done */

	return(pAttacher->Handle);
}
t_Handle AttachMgr__CreateAttacher(char *pEmitterName, CProtagonist *pProtagonist, Vector3CRef vCollidePos, Vector3CRef vCollideVel, const float fLifetime /*= 0.0f*/)
{
	/* alloc an attacher-- should always succeed-- steals oldest if none available */

	ts_Attacher *pAttacher = AttachMgr__AllocAttacher();
	if (pAttacher == NULL)
		return(INVALID_HANDLE);

	/* get an emitter-- may not be one available, or name might be invalid */

	pAttacher->EmitterHandle = ParticleEmitter__CreateFromRecord(
		pEmitterName,				// char *pEmitterName,
		&pAttacher->vWorldPos,	// Vector3 *pvEmitterPos,
		NULL,							// Vector3 *pvEmitterVel,
		NULL,							// Vector3 *pvEjectVel,
		false,						// bool bEjectVelChanges,
		NULL,							// Vector3 *pvUp,
		false);						// bool bStationary)
	if (pAttacher->EmitterHandle == INVALID_HANDLE)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}

	/* record data */

	pAttacher->u16Flags = ATTACHER_EMITTER;
	pAttacher->pProtagonist = pProtagonist;
	pAttacher->fLifetime = fLifetime;

	/* done */

	return(pAttacher->Handle);
}

// --- DAS just attach emitter to the specified bone
t_Handle AttachMgr__CreateAttacher( char *pEmitterName, CProtagonist *pProtagonist, ts_Bone *pBone, const float fLifetime /*= 0.0f*/ )
{
	/* alloc an attacher-- should always succeed-- steals oldest if none available */

	ts_Attacher *pAttacher = AttachMgr__AllocAttacher();
	if (pAttacher == NULL)
		return(INVALID_HANDLE);

	/* determine attachment point */
	Vector3 pos = pProtagonist->GetBoneWorldStartPosition( pBone->boneID );

	pAttacher->vWorldPos = pos;
	pAttacher->pBone = pBone;

	/* calculate hit point in bone space so it can track bone (retain correct offset from bone) in future */

	Graphics4x4 mWorldToBone;
	mWorldToBone.FastMultiply(pAttacher->pBone->Matrix, pProtagonist->Instance()->Matrix());	// start with bone-to-world
	bool bInvertSuccess = mWorldToBone.Invert();	// and invert
	ASSERT(bInvertSuccess);
	pAttacher->vBonePos = pAttacher->vWorldPos * mWorldToBone;

	/* get an emitter-- may not be one available, or name might be invalid */

	pAttacher->EmitterHandle = ParticleEmitter__CreateFromRecord(
		pEmitterName,				// char *pEmitterName,
		&pAttacher->vWorldPos,	// Vector3 *pvEmitterPos,
		NULL,							// Vector3 *pvEmitterVel,
		NULL,							// Vector3 *pvEjectVel,
		false,						// bool bEjectVelChanges,
		NULL,							// Vector3 *pvUp,
		false);						// bool bStationary)
	if (pAttacher->EmitterHandle == INVALID_HANDLE)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}

	/* record data */

	pAttacher->u16Flags = ATTACHER_EMITTER;
	pAttacher->pProtagonist = pProtagonist;
	pAttacher->fLifetime = fLifetime;

	/* done */

	return(pAttacher->Handle);
}

t_Handle AttachMgr__CreateAttacher( float fForceFieldRadius, CProtagonist *pProtagonist, ts_Bone *pBone, const float fLifetime /*= 0.0f*/ )
{
	/* alloc an attacher-- should always succeed-- steals oldest if none available */

	ts_Attacher *pAttacher = AttachMgr__AllocAttacher();
	if (pAttacher == NULL)
		return(INVALID_HANDLE);

	/* determine attachment point */
	Vector3 pos = pProtagonist->GetBoneWorldStartPosition( pBone->boneID );

	pAttacher->vWorldPos = pos;
	pAttacher->pBone = pBone;

	/* calculate hit point in bone space so it can track bone (retain correct offset from bone) in future */

	Graphics4x4 mWorldToBone;
	mWorldToBone.FastMultiply(pAttacher->pBone->Matrix, pProtagonist->Instance()->Matrix());	// start with bone-to-world
	bool bInvertSuccess = mWorldToBone.Invert();	// and invert
	ASSERT(bInvertSuccess);
	pAttacher->vBonePos = pAttacher->vWorldPos * mWorldToBone;

	/* get an emitter-- may not be one available, or name might be invalid */

	pAttacher->ForceFieldHandle = DistortionMgr__AddForcefield(
		&pAttacher->vWorldPos,	// Vector3 *pvPos
		fForceFieldRadius, 		// fRadius
		fLifetime,			 		// fLifetime (0.0 means forever)
		0,						 		// u16DistortionFlags
		0);					 		// u8Flags

	if (pAttacher->ForceFieldHandle == INVALID_HANDLE)
	{
		Attacher__Kill(pAttacher);
		return(INVALID_HANDLE);
	}

	/* record data */

	pAttacher->u16Flags = ATTACHER_FORCEFIELD;
	pAttacher->pProtagonist = pProtagonist;
	pAttacher->fLifetime = fLifetime;

	/* done */

	return(pAttacher->Handle);
}

/* attach to a bone - fDistance is a relative position along the bone (between 0 and 1). */

bool AttachMgr__AttachToBone(CProtagonist *pProtagonist, ts_Bone* pBone, float fDistance, ts_Attacher *pAttacher)
{
	/* attach the bone */

	pAttacher->pBone = pBone;

	/* compute the bone-pivot relative offset */

	Vector3 vBone = pBone->pModelBone->vBoneEnd - pBone->pModelBone->vPivot;
	pAttacher->vBonePos = pBone->pModelBone->vPivot + vBone * fDistance;

	/* transform the bone-pivot relative offset into world space */

	Graphics4x4 mBoneToWorld;
	mBoneToWorld.FastMultiply(pBone->Matrix, pProtagonist->Instance()->Matrix());
	pAttacher->vWorldPos = pAttacher->vBonePos * mBoneToWorld;

	/* success */

	return(true);
}

#ifndef CONSUMER_BUILD
/* debug only-- called from scene render-- show world location of attachers so you can see results */

void AttachMgr__ShowAttachers()
{
	static ts_bRGBA Red = {255, 0, 0, 255};
	ts_Attacher *pAttacher = (ts_Attacher *) AttachMgr.AttacherActive.pHead;
	while (pAttacher)
	{
		gfxDrawMarkWC(&pAttacher->vWorldPos, 0.05f, &Red);
		pAttacher = pAttacher->pNext;
	}

	/* draw last collision segment */

	Vector3 vEndPoint = AttachMgr.vLastCollideVel;
	vEndPoint.SafeNormalize();
	vEndPoint *= 2.0f;	// 2 meters long should be enough
	vEndPoint += AttachMgr.vLastCollidePos;
	gfxDrawLineWC(&AttachMgr.vLastCollidePos, &vEndPoint, &Red);
}
#endif //CONSUMER_BUILD

/************************ individual attacher manipulation *******************************/

/* kill via handle */

void Attacher__Kill(t_Handle AttachHandle)
{
	/* point to attach, bail if invalid id */

	ASSERT(AttachHandle != INVALID_HANDLE);
	ts_Attacher *pAttacher = ATTACH_GET_RECORD(AttachHandle);
	if (pAttacher->Handle != AttachHandle)
		return;
	Attacher__Kill(pAttacher);
}

/* kill via pointer */

void Attacher__Kill(ts_Attacher *pAttacher)
{
	/* some attachers have allocated other things */

	switch(pAttacher->u16Flags & ATTACHER_TYPE_MASK)
	{
	case ATTACHER_INSTANCE:
		SceneMgr::RemoveInstance(&pAttacher->pPoolInstance->Instance);
		InstancePool__Free(pAttacher->pPoolInstance);
		pAttacher->pPoolInstance = NULL;
		break;
	case ATTACHER_EMITTER:
		ParticleEmitter__Kill(pAttacher->EmitterHandle);
		pAttacher->EmitterHandle = INVALID_HANDLE;
		break;
	case ATTACHER_FORCEFIELD:
		Distortion__Kill(pAttacher->ForceFieldHandle);
		pAttacher->EmitterHandle = INVALID_HANDLE;
		break;
	}

	/* clear record */

	pAttacher->Handle = INVALID_HANDLE;
	pAttacher->u16Flags = 0;	// clear all flags

	/* move from active to free list */

	SLinkList__RemoveLink(&AttachMgr.AttacherActive, pAttacher);
	SLinkList__AddLinkToHead(&AttachMgr.AttacherFree, pAttacher);
}

/* enable/disable an attacher-- attachers are enabled by default at creation */

void Attacher__Enable(t_Handle AttachHandle, bool bEnable)
{
	/* point to attach, bail if invalid id */

	ASSERT(AttachHandle != INVALID_HANDLE);
	ts_Attacher *pAttacher = ATTACH_GET_RECORD(AttachHandle);
	if (pAttacher->Handle != AttachHandle)
		return;

	/* check current state for quick bail */

	if (bEnable == ((pAttacher->u16Flags & ATTACHER_DISABLED) == 0))
		return;
	if (bEnable)
		pAttacher->u16Flags &= ~ATTACHER_DISABLED;
	else
		pAttacher->u16Flags |= ATTACHER_DISABLED;

	/* enable/disable */

	switch(pAttacher->u16Flags & ATTACHER_TYPE_MASK)
	{
	case ATTACHER_INSTANCE:
		pAttacher->pPoolInstance->Instance.Hide( !bEnable );
		break;
	case ATTACHER_EMITTER:
		if (bEnable)
			ParticleEmitter__Activate(pAttacher->EmitterHandle);
		else
			ParticleEmitter__Deactivate(pAttacher->EmitterHandle);
		break;
	case ATTACHER_FORCEFIELD:
		Distortion__Enable( pAttacher->ForceFieldHandle, bEnable );
		break;
	}
}

t_Handle Attacher__GetEmitterHandle(t_Handle AttachHandle)
{
	/* point to attach, bail if invalid id */

	ASSERT(AttachHandle != INVALID_HANDLE);
	ts_Attacher *pAttacher = ATTACH_GET_RECORD(AttachHandle);
	if (pAttacher->Handle != AttachHandle)
		return(INVALID_HANDLE);
	return(pAttacher->EmitterHandle);
}

t_Handle Attacher__GetForceFieldHandle(t_Handle AttachHandle)
{
	/* point to attach, bail if invalid id */

	ASSERT(AttachHandle != INVALID_HANDLE);
	ts_Attacher *pAttacher = ATTACH_GET_RECORD(AttachHandle);
	if (pAttacher->Handle != AttachHandle)
		return(INVALID_HANDLE);
	return(pAttacher->ForceFieldHandle);
}

