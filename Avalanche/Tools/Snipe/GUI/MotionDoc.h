#pragma once
#include "SceneDoc.h"

class SceneObject;
class MotionInstance;
class TransformableObject;
class CommandInstance;
class Motion;

class CMotionDoc : public CSceneDoc
{
protected:
	DECLARE_BASEDOC(CMotionDoc, CSceneDoc)

// Attributes
public:
         	CMotionDoc();
	virtual ~CMotionDoc();

// Operations
   void ApplyModel( TransformableObject *pModelCache );
   Motion *GetMotion() { return (Motion *)GetCommandObject(); }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CMotionDoc; }
   virtual void SetObjectHead( HierObject *pObjectHead );
};
