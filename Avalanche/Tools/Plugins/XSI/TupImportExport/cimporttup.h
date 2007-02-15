#ifndef __CImportTup_H__
#define __CImportTup_H__
#include <xsi_status.h>
#include <xsi_model.h>
#include <xsi_application.h>
#include <xsi_clusterproperty.h>
#include <SemanticLayer.h>

//*****************************************************************************
/*! \class CImportTup CImportTup.h
 */
//*****************************************************************************
class CImportTup
{
public:
	/*! Default constructor. */
	CImportTup();

	XSI::CStatus ImportFile( const XSI::CString& in_FileName );
	protected:	

	XSI::CStatus CreateProjectionCmd( XSI::X3DObject p3DObj )const;
	XSI::CStatus FreezeObjCmd( XSI::ClusterProperty& Obj )const;

   TupObject::ObjectTypes GetObjectType(TupperwareAggregate *pNodeAgg);
   XSI::CStatus RecurseTupTree(TupperwareAggregate *pNodeAgg, XSI::X3DObject in_Parent);
   XSI::CStatus ImportPolyMesh(TupperwareAggregate *pNodeAgg,XSI::X3DObject in_Parent,XSI::X3DObject &x3dobject);
   void ImportTransform(TupperwareAggregate *pNodeAgg,XSI::X3DObject &x3dobject);

   XSI::CStatus ConvertTupMatrixToCMatrix4(TupperwareAggregate *pTransformAgg,XSI::MATH::CMatrix4 &matrix);

	private:

	XSI::Application m_app;
	XSI::Model       m_sceneRoot;

   float m_fUnitRatio;

   TupperwareAggregate  *m_pSceneAgg;
   TupScene             *m_pTupScene;
   TupNodePool          *m_pTupNodePool;
   TupObjectPool        *m_pTupObjectPool;
   TupMapPool           *m_pTupMapPool;
   TupImagePool         *m_pTupImagePool;
   TupMaterialPool      *m_pTupMaterialPool;
   TupModifierPool      *m_pTupModifierPool;
// TupAnimSetPool       *m_pTupAnimSetPool;
// TupperwareAggregate  *m_pTupAnimChannelPoolAgg; 
};

#endif // __CImportDotXSI_H__
