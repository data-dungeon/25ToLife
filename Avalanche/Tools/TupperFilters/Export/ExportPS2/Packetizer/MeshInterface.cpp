#include "ExportPch.h"
#include "MeshInterface.h"

static Position3<float> 			ZeroPoint3(0.0f,0.0f,0.0f);
static Position2<float> 			ZeroPoint2(0.0f, 0.0f);
static BaseColor<unsigned short> 	ZeroColor(0, 0, 0, 0);



const unsigned int MeshInterface::IDX_MASK = 0x07fffffff;
const unsigned int MeshInterface::IDX_END_OF_STRIP = 0x80000000;

// None of these functions should be called, except in cases where it is
// possible to get a reasonable default.  See more detailed comment
// in MeshInterface.h

unsigned int MeshInterface::numStates( void ) const
		{ return 0; }
unsigned short MeshInterface::numPasses( unsigned int /*uistate*/ ) const
		{ return 0; }

unsigned int MeshInterface::numVertices( void ) const
		{	return 0; }

const	unsigned int MeshInterface::idx( unsigned int uivtx) const
		{ return uivtx; }

const Position3<float> & MeshInterface::vtx( unsigned int /*uivtx*/) const
		{ return ZeroPoint3; }
const Position3<float> & MeshInterface::nrm( unsigned short /*usStage*/, unsigned int /*uivtx*/) const
		{ return ZeroPoint3; }
const Position3<float> MeshInterface::min( void ) const
		{ return ZeroPoint3; }
const Position3<float> MeshInterface::max( void ) const
		{ return ZeroPoint3; }
const Position3<float> MeshInterface::dim( void ) const
		{ return ZeroPoint3; }
const unsigned int MeshInterface::idxMatrix( unsigned int /*uivtx*/) const
		{ return 0; }
const	unsigned int MeshInterface::numMatrices( void) const
		{ return 1; }
const Position2<float> & MeshInterface::uv( unsigned short /*usStage*/, unsigned int /*uivtx*/) const
		{ return ZeroPoint2; }
const Position2<float> & MeshInterface::st( unsigned short /*usStage*/, unsigned int /*uivtx*/) const
		{ return ZeroPoint2; }

const BaseColor<unsigned short> & MeshInterface::clr(unsigned short /*usStage*/, unsigned int /*uivtx*/) const
{

	return ZeroColor;
}

// TJC - get the # of valid color sets for this model
const	unsigned int MeshInterface::numColorSets(void) const
{
	return 1;
}

void MeshInterface::enableColorSet( unsigned int /*uiSet*/)
{

}
// TJC - get the minimum # of alpha bits required (should currently always return 1 or 8, default is 8)
const	unsigned int MeshInterface::numRequiredAlphaBits( ) const
{
	return 8;
}
