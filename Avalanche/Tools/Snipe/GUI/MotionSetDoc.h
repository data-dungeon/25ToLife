#pragma once
#include "SceneDoc.h"

class SceneObject;
class MotionInstance;
class TransformableObject;
class CommandInstance;
class MotionSet;
class Motion;

class CMotionSetDoc : public CSceneDoc
{
protected:
	DECLARE_BASEDOC(CMotionSetDoc, CSceneDoc)

// Attributes
public:
   SnipeObjectTypeArray  m_MotionSetSelectableObjectTypeArray;

         	CMotionSetDoc();
	virtual ~CMotionSetDoc();

// Operations
   void AddMotion( Motion *pMotion );
   MotionSet *GetMotionSet() { return (MotionSet *)GetCommandObject(); }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CMotionSetDoc; }
   virtual void SetObjectHead( HierObject *pObjectHead );
};
