																				  /*******************************************************************************
 HoldVC.h
 Utility/modifier for saving the current vertex colors into a modifier.

 Author   :   Dave Astle
 Created  :   1/14/2002
  
 Copyright (C) 2002 Avalanche Software 
********************************************************************************
   Date      By    Desc
   ----     -----  -------------------------------------------------------------
 01/17/02   Dave   Initial release
 01/22/02   Dave   Fixed crash happening when the preceding modifiers are
                   deleted by creating a new MeshMap if the mesh doesn't already
                   have one for the vertcol channel.
*******************************************************************************/

#ifndef __HOLDVC__H
#define __HOLDVC__H

#if 0
#include "tupperware/aggregate.h"
#include "tupperware/list.h"
#include "tupperware/loadsave.h"
#include "tupper3d/Keeper.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "utilapi.h"

#include <fstream>

#endif

using namespace std;

extern TCHAR *GetString(int id);

extern HINSTANCE g_hInstance;

#define HOLDVC_MOD_CLASS_ID	  Class_ID(0x79e3052f, 0x6d641ee)


#define PBLOCK_REF	0


typedef Tab<Color*> ColorTab;
typedef Tab<TVFace*> FaceTab;

class HoldVCMod : public Modifier
{
public:
  static IObjParam *ip;

  ColorTab  colorTable;
  FaceTab   faceTable;
  HWND      hPanel;

  //Constructor/Destructor
  HoldVCMod();
  ~HoldVCMod();

  // From Animatable
  void DeleteThis() { delete this; }
  SClass_ID SuperClassID()        { return OSM_CLASS_ID; }
  Class_ID  ClassID()             { return HOLDVC_MOD_CLASS_ID; }
  RefTargetHandle Clone( RemapDir &remap );
  IOResult Load( ILoad *iload );
  IOResult Save( ISave *isave );


  //From Modifier
  ChannelMask ChannelsUsed()    { return VERTCOLOR_CHANNEL | TOPO_CHANNEL; }
  ChannelMask ChannelsChanged() { return VERTCOLOR_CHANNEL | TOPO_CHANNEL; }
  Class_ID    InputType()       { return mapObjectClassID; }
  Interval LocalValidity( TimeValue t ) { return FOREVER; }
  void NotifyInputChanged( Interval changeInt, PartID partID, RefMessage message, ModContext *mc );
  void ModifyObject( TimeValue t, ModContext &mc, ObjectState *os, INode *node );

  void SetColors(TVFace faces[], int numFaces, VertColor* vertexColors, int numVertices);
  void ResetTables();
  void ModifyTriObject(TriObject* obj, TimeValue t);
  void ModifyPolyObject(PolyObject* pPolyObj, TimeValue t);
  void TurnVCOn(BOOL shaded);

  // From BaseObject
  BOOL ChangeTopology() { return FALSE; }
  BOOL DependOnTopology(ModContext &mc) { return TRUE; }

  int GetParamBlockIndex(int id) { return id; }

  //From ReferenceMaker
  int NumRefs() { return 0; }
  RefTargetHandle GetReference(int i) { return NULL; }
  void SetReference(int i, RefTargetHandle rtarg) { }

  int         NumSubs()             { return 0; }
  Animatable* SubAnim(int i)        { return NULL; }
  TSTR        SubAnimName( int i )  { return _T(""); }

  RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);

  CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

  void BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev );
  void EndEditParams( IObjParam *ip, ULONG flags, Animatable *next );
};

#endif // __HOLDVC__H
