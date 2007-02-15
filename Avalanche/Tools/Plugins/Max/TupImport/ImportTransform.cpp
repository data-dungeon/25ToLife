////////////////////////////////////////////////////////////////////////////
//
// ImportTransform
//
// Class to handle the transforms
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportTransform.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"
#include "TupImport.h"


void TupImport::AddTransform(TupNode &tupNode,INode *pNode)
{
	Matrix3 tm = pNode->GetObjTMAfterWSM(m_pInterface->GetTime());
	Graphics4x4 matrix;
	ConvertMaxMatrix(tm,matrix);
	TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
	TupTransform tupTransform(pTransformAgg);
	tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
	TupStaticMatrix tupStaticMatrix(pTransformAgg);
	tupStaticMatrix.AddMatrix(&matrix.r00);
}

void TupImport::ConvertMaxMatrix(const Matrix3 &srcMatrix,Graphics4x4 &dstMatrix)
{
	const float *pSrcMatrix = (const float *)srcMatrix.GetAddr();

	dstMatrix.r00 = pSrcMatrix[0];
	dstMatrix.r01 = pSrcMatrix[1];
	dstMatrix.r02 = pSrcMatrix[2];

	dstMatrix.r10 = pSrcMatrix[3];
	dstMatrix.r11 = pSrcMatrix[4];
	dstMatrix.r12 = pSrcMatrix[5];

	dstMatrix.r20 = pSrcMatrix[6];
	dstMatrix.r21 = pSrcMatrix[7];
	dstMatrix.r22 = pSrcMatrix[8];

	dstMatrix.tx  = pSrcMatrix[9];
	dstMatrix.ty  = pSrcMatrix[10];
	dstMatrix.tz  = pSrcMatrix[11];

	dstMatrix.u0  = dstMatrix.u1 = dstMatrix.u2 = 0.0f;
	dstMatrix.w = 1.0f;
}
