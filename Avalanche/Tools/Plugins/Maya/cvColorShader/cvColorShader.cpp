// Copyright (C) 2002 Alias|Wavefront, a division of Silicon Graphics Limited.
// 
// The information in this file is provided for the exclusive use of the
// licensees of Alias|Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias|Wavefront license agreement, without fee.
// 
// ALIAS|WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS|WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.


#include <math.h>

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MPointArray.h>
#include <maya/MFnPlugin.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MColor.h>
#include <maya/MItMeshVertex.h>

class cvColorShader : public MPxNode
{
	public:
                    cvColorShader();
    virtual         ~cvColorShader();

    virtual MStatus compute( const MPlug&, MDataBlock& );
	virtual void    postConstructor();

    static  void *  creator();
    static  MStatus initialize();

	//  Id tag for use with binary file format
    static  MTypeId id;

	private:

	static inline float dotProd(const MFloatVector &, const MFloatVector &); 

	// Input attributes

	static MObject aReverseAlpha;

	static MObject aPointObj;				// Implicit attribute
	static MObject aPrimitiveId;			// Implicit attribute

	// Output attributes
	static MObject aOutColor;
	static MObject aOutAlpha;

	int m_lastMeshfptr;
	int m_numTriangles;
	int *m_pTrianglePolyArray;
	int *m_pTrianglePolyIndexArray;

};

// Static data
MTypeId cvColorShader::id( 0x8000f );

// Attributes 
MObject cvColorShader::aReverseAlpha;
MObject cvColorShader::aPointObj;
MObject cvColorShader::aPrimitiveId;
MObject cvColorShader::aOutColor;
MObject cvColorShader::aOutAlpha;

void cvColorShader::postConstructor( )
{
	setMPSafe(true);
}

cvColorShader::cvColorShader() : m_numTriangles(0),m_pTrianglePolyArray(NULL),m_pTrianglePolyIndexArray(NULL),m_lastMeshfptr(-1)
{
}

cvColorShader::~cvColorShader()
{
	delete [] m_pTrianglePolyArray;
	delete [] m_pTrianglePolyIndexArray;
}

void * cvColorShader::creator()
{
    return new cvColorShader();
}

MStatus cvColorShader::initialize()
{
    MFnNumericAttribute nAttr;

	aReverseAlpha = nAttr.create( "reverseAlpha", "ra", 
								  MFnNumericData::kBoolean);
	nAttr.setDefault( true );

    aPointObj  = nAttr.createPoint( "pointObj", "po" );
	nAttr.setStorable(false);
    nAttr.setHidden(true);

	aPrimitiveId = nAttr.create( "primitiveId", "pi", MFnNumericData::kLong);
    nAttr.setHidden(true);

    aOutColor = nAttr.createColor( "outColor", "oc" );
    nAttr.setStorable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);

	aOutAlpha = nAttr.create( "outAlpha", "oa", MFnNumericData::kFloat);
	nAttr.setDisconnectBehavior(MFnAttribute::kReset);
    nAttr.setStorable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);

    addAttribute(aPointObj);
	addAttribute(aOutColor);
    addAttribute(aOutAlpha);
    addAttribute(aReverseAlpha);
    addAttribute(aPrimitiveId);

    attributeAffects(aPointObj,     aOutColor);
    attributeAffects(aPrimitiveId,  aOutColor);

    attributeAffects(aReverseAlpha, aOutAlpha);
    attributeAffects(aPointObj,     aOutAlpha);
    attributeAffects(aPrimitiveId,  aOutAlpha);

	return MS::kSuccess;
}

// dot product on vectors
inline float cvColorShader::dotProd(
	const MFloatVector & v1,
	const MFloatVector & v2) 
{
	return  v1.x*v2.x +  v1.y*v2.y + v1.z*v2.z;
}

///////////////////////////////////////////////////////////////////////////////
MStatus cvColorShader::compute( const MPlug& plug, MDataBlock& block ) 
{
	if ((plug != aOutColor) && (plug.parent() != aOutColor) && 
		(plug != aOutAlpha))
		return MS::kUnknownParameter;

	MStatus status;
	MObject thisNode = thisMObject();

	bool rev_flag = block.inputValue(aReverseAlpha).asBool();
	long triangleId = block.inputValue(aPrimitiveId).asLong();

	// Location of the point we are shading
	MFloatVector& pointObj = block.inputValue( aPointObj ).asFloatVector();

	// Find the Mesh object
	MPlug outColorPlug = MFnDependencyNode(thisNode).findPlug("outColor", 
															  &status);
	
	MItDependencyGraph depIt( outColorPlug, MFn::kShadingEngine,
							  MItDependencyGraph::kDownstream, 
							  MItDependencyGraph::kBreadthFirst,
							  MItDependencyGraph::kNodeLevel, 
							  &status);
	
	depIt.enablePruningOnFilter();

	MObject shadingEngineNode = depIt.thisNode(); // test
	
	MPlug dagSetMembersPlug = MFnDependencyNode(shadingEngineNode).findPlug(
		"dagSetMembers", &status );
	MPlug dagSetMembersElementPlug = dagSetMembersPlug.elementByLogicalIndex( 
		0, &status );
	
	MPlugArray meshPlugArray;
	dagSetMembersElementPlug.connectedTo( meshPlugArray, true, false, &status);
	
	MObject meshNode = meshPlugArray[0].node();

/////////////////////////////////////////////////////////////////////////////////////////////////////////

	MFnDependencyNode depnode(meshNode);
	const char *debug = depnode.name().asChar();

	// get connected shading group table with shading group/poly keys
	MObjectArray shaders;
	MIntArray indicies;
	shaders.clear(); indicies.clear();
	MFnMesh fnMesh(meshNode);

	int polyIndex;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// if this is a different mesh then rebuild the cache data
	if (((int)depnode.f_ptr)!=m_lastMeshfptr)
	{
		delete [] m_pTrianglePolyArray;
		delete [] m_pTrianglePolyIndexArray;

		m_pTrianglePolyArray = NULL;
		m_pTrianglePolyIndexArray = NULL;
		m_lastMeshfptr = (int)depnode.f_ptr;
	}

	// if we haven't built our remap table for the mesh
	if (m_pTrianglePolyArray==NULL)
	{
		MItMeshPolygon fIter( meshNode );
		
		int numPolygons = fnMesh.numPolygons();		// Number of faces
		int numTriangles = 0;
		int preIndex;
		for( polyIndex = 0; polyIndex < numPolygons; polyIndex++ )
		{
			int nTri;
			fIter.setIndex( polyIndex, preIndex );
			fIter.numTriangles(nTri);
			numTriangles += nTri;
		}
		if (numTriangles)
		{
			m_pTrianglePolyArray = new int[numTriangles];
			m_pTrianglePolyIndexArray = new int[numTriangles];
		}
		numTriangles = 0;
		for( polyIndex = 0; polyIndex < numPolygons; polyIndex++ )
		{
			int nTri;
			fIter.setIndex( polyIndex, preIndex );
			fIter.numTriangles(nTri);
			for (int triIndex=0;triIndex<nTri;triIndex++)
			{
				m_pTrianglePolyArray[numTriangles] = polyIndex;
				m_pTrianglePolyIndexArray[numTriangles] = triIndex;
				numTriangles++;
			}
		}
		m_numTriangles = numTriangles;
	}

	MColor resultColor(1.0,0.0,1.0,1.0); // start with pink
	if (triangleId>=0&&triangleId<m_numTriangles)
	{
		MItMeshPolygon fIter( meshNode );
		int preIndex;
		int polyIndex = m_pTrianglePolyArray[triangleId];
		int polyTriIndex = m_pTrianglePolyIndexArray[triangleId];

		MPointArray posArray;
		MIntArray vIndexArray;

		fIter.setIndex( polyIndex, preIndex );
		if (fIter.hasColor())
		{
			fIter.getTriangle(polyTriIndex, posArray, vIndexArray,MSpace::kObject);
		
			MFloatVector pos1;
			MFloatVector pos2;
			MFloatVector pos3;
			pos1.x = (float)posArray[0].x;
			pos1.y = (float)posArray[0].y;
			pos1.z = (float)posArray[0].z;
			pos2.x = (float)posArray[1].x;
			pos2.y = (float)posArray[1].y;
			pos2.z = (float)posArray[1].z;
			pos3.x = (float)posArray[2].x;
			pos3.y = (float)posArray[2].y;
			pos3.z = (float)posArray[2].z;

			MColor color1;
			MColor color2;
			MColor color3;

			MItMeshVertex vIter( meshNode );

			vIter.setIndex( vIndexArray[0], preIndex);
			vIter.getColor( color1, polyIndex );
			
			vIter.setIndex( vIndexArray[1], preIndex);
			vIter.getColor( color2, polyIndex );
			
			vIter.setIndex( vIndexArray[2], preIndex);
			vIter.getColor( color3, polyIndex );

			// Compute the barycentric coordinates of the sample.
			
			pointObj = pointObj - pos3;				// Translate pos3 to origin
			pos1 = pos1 - pos3;
			pos2 = pos2 - pos3;
			
			MFloatVector norm = pos1 ^ pos2;		// Triangle normal
			float len = dotProd(norm, norm);
			len = dotProd(norm, pointObj)/len;
			
			pointObj = pointObj - (len * norm);		// Make sure the point is
			// in the triangle
			
			float aa = dotProd(pos1, pos1);
			float bb = dotProd(pos2, pos2);
			float ab = dotProd(pos1, pos2);
			float am = dotProd(pos1, pointObj);
			float bm = dotProd(pos2, pointObj);
			float det = aa*bb - ab*ab;
			
			// a, b, c are the barycentric coordinates (assuming pnt
			// is in the triangle plane, best least square fit
			// otherwise.
			//
			float a = (am*bb - bm*ab) / det;
			float b = (bm*aa - am*ab) / det;
			float c = 1-a-b;

			resultColor = (a*color1) + (b*color2) + (c*color3);
		}
	}


	MDataHandle outColorHandle = block.outputValue( aOutColor );
	MFloatVector& outColor = outColorHandle.asFloatVector();
	outColor.x = resultColor.r;// * resultColor.a;
	outColor.y = resultColor.g;// * resultColor.a;
	outColor.z = resultColor.b;// * resultColor.a;
	outColorHandle.setClean();

	if( rev_flag == true )
		resultColor.a = 1.0f - resultColor.a;

	MDataHandle outAlphaHandle = block.outputValue( aOutAlpha );
	float& outAlpha = outAlphaHandle.asFloat();
	outAlpha = resultColor.a;
	outAlphaHandle.setClean();

	return MS::kSuccess;
}


/////////////////////////////////////////////////////////////////////////////////////////
MStatus initializePlugin( MObject obj )
{ 
	const MString UserClassify( "utility/color" );
	
	MFnPlugin plugin( obj, "Alias|Wavefront", "4.5", "Any");
	plugin.registerNode( "cvColorShader", cvColorShader::id, 
						 cvColorShader::creator, 
						 cvColorShader::initialize,
						 MPxNode::kDependNode, &UserClassify );

	return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );
	plugin.deregisterNode( cvColorShader::id );

	return MS::kSuccess;
}






