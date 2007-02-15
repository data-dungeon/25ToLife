/**********************************************************************
 *<
	FILE:		    ApplyVC.h
	DESCRIPTION:	Vertex Color Utility
	CREATED BY:		Christer Janson
	HISTORY:		Created Monday, June 02, 1997

 *>	Copyright (c) 1997 Kinetix, All Rights Reserved.
 **********************************************************************/

#ifndef __APPLYVC__H
#define __APPLYVC__H

#if 0
#include "Max.h"
#include "resource.h"
#include "utilapi.h"
#include "istdplug.h"
#include "EvalCol.h"
#endif

#define APPLYVC_MOD_CLASS_ID	Class_ID(0x104170cc, 0x66373204)

typedef Tab<Color*> ColorTab;

class FaceColor 
{
public:
	Color colors[3];
};

typedef Tab<FaceColor*> FaceColorTab;


class ApplyVCMod : public Modifier {	
	public:
		static IObjParam*	ip;
		HWND			hPanel;
		ColorTab		mixedVertexColors;
		//VertexColorTab	vertexColors;
		FaceColorTab	faceColors;
		Interval		iValid;

		ApplyVCMod(BOOL create);
		void		InitControl(ModContext &mc,TriObject *obj,int type,TimeValue t);

		// From Animatable
		void		DeleteThis();
		void		GetClassName(TSTR& s);
		Class_ID	ClassID();
		void		BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void		EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);
		TCHAR*		GetObjectName();
		CreateMouseCallBack*	GetCreateMouseCallBack();
		BOOL		CanCopyAnim();
		BOOL		DependOnTopology(ModContext &mc);

		ChannelMask	ChannelsUsed();
		ChannelMask	ChannelsChanged();
		Class_ID	InputType();
		void		ModifyTriObject(TriObject* tobj, TimeValue t);
		void		ModifyPolyObject(PolyObject* pPolyObj, TimeValue t);
		void		ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval	LocalValidity(TimeValue t);

		int			NumRefs();
		RefTargetHandle	GetReference(int i);
		void		SetReference(int i, RefTargetHandle rtarg);

		int			NumSubs();
		Animatable* SubAnim(int i);
		TSTR		SubAnimName(int i);

		RefTargetHandle	Clone(RemapDir& remap = NoRemap());
		RefResult	NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);

		IOResult	Load(ILoad *iload);
		IOResult	Save(ISave *isave);

		void		ResetColTab()
{
	int i;
	// Reset table of mixed colors
	for (i=0; i<mixedVertexColors.Count(); i++) {
		delete mixedVertexColors[i];
	}
	mixedVertexColors.ZeroCount();
	mixedVertexColors.Shrink();

	// Reset table of vertex colors
	for (i=0; i<faceColors.Count(); i++) {
		delete faceColors[i];
	}
	faceColors.ZeroCount();
	faceColors.Shrink();

	iValid.SetEmpty();
}

//		void		SetColors(VertexColorTab& colorTab);
		void		SetColors(FaceColorTab& colorTab)
{
	ResetColTab();

	for (int i=0; i<colorTab.Count(); i++) 
	{
		FaceColor* faceCol = new FaceColor;

		for (int j=0; j<3; j++) 
		{
			faceCol->colors[j] = colorTab[i]->colors[j];
		}
		faceColors.Append(1, &faceCol, 25);
	}

//	NotifyDependents(FOREVER, PART_VERTCOLOR & PART_EXCLUDE_RADIOSITY, REFMSG_CHANGE);
	NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
}

		void		SetMixedColors(ColorTab& colorTab);
};

#endif // __APPLYVC__H
