//*****************************************************************************
/*!	\file CExportDotXSI.h
 *	\brief CExportDotXSI object class.
 *
 * (c) Copyright 1996 - 2003 Softimage/Avid Corporation
 *
 */
//*****************************************************************************

#ifndef __CExportTup_H__
#define __CExportTup_H__

#include <xsi_status.h>
#include <xsi_model.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbscurvelist.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_application.h>
#include <xsi_material.h>
#include <xsi_projectitem.h>
#include <xsi_parameter.h>
#include <xsi_siobject.h>
#include <xsi_shader.h>
#include <SemanticLayer.h>// Part of the FTK.  Make sure you have the XSIFTK_ROOT env variable set.


//*****************************************************************************
/*! \class CExportDotXSI CExportDotXSI.h
 */
//*****************************************************************************
class CExportTup
{
public:
	/*! Default constructor. */
	CExportTup();

	XSI::CStatus ExportFile(const XSI::CString& in_FileName,const XSI::CString& in_ObjectToExport);
protected:	

   XSI::CStatus RecurseXSITree(int nParentNodeIndex, XSI::X3DObject &x3dobject);
   XSI::CStatus ExportGeometry(TupperwareAggregate *pNodeAgg, XSI::X3DObject &x3dobject);
   XSI::CStatus ExportPolygonMesh(TupperwareAggregate *pNodeAgg, XSI::X3DObject &x3dobject);
   void ExportNull(TupperwareAggregate *pNodeAgg, XSI::X3DObject &x3dobject);
   void ExportTransform(TupperwareAggregate *pNodeAgg,XSI::X3DObject &x3dobject);
   int ExportMultiSubObjectMaterial(XSI::X3DObject &x3dobject);
   XSI::CStatus ConvertCMatrix4ToTupMatrix(TupperwareAggregate *pTransformAgg,XSI::MATH::CMatrix4 &matrix);
   int ExportShader(XSI::Shader &shader,XSI::CRefArray &shaderRefArray);
   int ExportDefaultMaterial(XSI::Material material);

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

#endif // __CExportDotXSI_H__
