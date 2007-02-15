//
// Author: TJC
// Purpose: This is a portion of the Water class...  See Water.h for
// complete details..
//

#include "Effects/EffectsPCH.h"

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterAffector.h"
#include "Effects/Water/WaterPool.h"

#ifdef DEBUG_WATER
#include "Display/Common/RenderMisc.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Water - Subdivision
///////////////////////////////////////////////////////////////////////////////////////

#ifdef SUPPORT_GENERIC_RENDERER

// Given a cell & a quadrant, mark it as subdivided.  this function is
// recursive to make sure that there are no T junctions in the
// resulting mesh. NOTE: if the subdivide fails, that doesnt mean that
// non of it happened.  neighbor cells may still have been subdivided,
// and those subdivisions are not backed out.
int Water::SubDivide(int cellNumber, int quadrant_to_divide)
{
	if(cellNumber == 0xFF) 
		return 0xFF;

	Node* cell = Cell(cellNumber);

	if(cell->quad[quadrant_to_divide] != 0xFF)
		return cell->quad[quadrant_to_divide];

	if(mCurrNodeIdx == mNodes)
		return OUT_OF_NODES;

	if(cell->size + 1 > mMaxSubDivDepth)
		return TOO_DEEP;

	// perform subdivision
	Node *newCell = mCurrNode++;
	u8 newCellIdx = mCurrNodeIdx++;
	cell->quad[quadrant_to_divide] = newCellIdx;
	newCell->parent = cellNumber;
	newCell->quadrant = quadrant_to_divide;
	newCell->quad[Q_UL] = 0xFF;
	newCell->quad[Q_UR] = 0xFF;
	newCell->quad[Q_DL] = 0xFF;
	newCell->quad[Q_DR] = 0xFF;
	newCell->size = cell->size + 1;

	// subdivide neighbors...
	int quadrant = cell->quadrant;
	int n_u, n_r, n_l, n_d;
	n_u = n_r = n_l = n_d = cellNumber;
	switch(quadrant_to_divide)
	{
	case Q_UR:
		n_u = SubDivide(cell->neighbor[N_U], quadrant ^ Q_XO);
		n_r = SubDivide(cell->neighbor[N_R], quadrant ^ Q_OX);
		break;
	case Q_DR:
		n_d = SubDivide(cell->neighbor[N_D], quadrant ^ Q_XO);
		n_r = SubDivide(cell->neighbor[N_R], quadrant ^ Q_OX);
		break;
	case Q_UL:
		n_u = SubDivide(cell->neighbor[N_U], quadrant ^ Q_XO);
		n_l = SubDivide(cell->neighbor[N_L], quadrant ^ Q_OX);
		break;
	case Q_DL:
		n_d = SubDivide(cell->neighbor[N_D], quadrant ^ Q_XO);
		n_l = SubDivide(cell->neighbor[N_L], quadrant ^ Q_OX);
		break;
	}

	// if we are out of nodes, undo this subdivide, and
	// back out of the recursion...
	if(n_u > SUBDIVISION_ERRORS ||
		n_r > SUBDIVISION_ERRORS ||
		n_l > SUBDIVISION_ERRORS ||
		n_d > SUBDIVISION_ERRORS)
	{
		// free the node we allocated...
		// (note that if one of the above subdivides succeeded,
		// its subdivide doesnt get freed).
		mCurrNode--;
		mCurrNodeIdx--;
		// mark the cell undivided
		cell->quad[quadrant_to_divide] = 0xFF;
		return Math::Max(Math::Max(n_u, n_d), Math::Max(n_l, n_r));
	}

	// remember new neighbors
	newCell->neighbor[N_U] = n_u;
	newCell->neighbor[N_R] = n_r;
	newCell->neighbor[N_D] = n_d;
	newCell->neighbor[N_L] = n_l;

	// return the new cell number & mark the subdivide valid
	return newCellIdx;
}

static void Tabs(int depth, char *string)
{
	char buffer[100];
	for(int i = 0; i < depth; i++)
	{
		buffer[i] = '\t';
	}
	buffer[depth] = '\0';
	printf("%s%s\n", buffer, string);
}

void Water::DisplayCellGraph(int cellNumber, int depth)
{
	if(cellNumber == 0xFF) return;
	Node* cell = Cell(cellNumber);

	Tabs(depth, "CellUL");
	DisplayCellGraph(cell->quad[Q_UL], depth+1);
	Tabs(depth, "CellUR");
	DisplayCellGraph(cell->quad[Q_UR], depth+1);
	Tabs(depth, "CellLL");
	DisplayCellGraph(cell->quad[Q_DL], depth+1);
	Tabs(depth, "CellLR");
	DisplayCellGraph(cell->quad[Q_DR], depth+1);
}

void Water::UniformSubDivide(int depth, int cellNumber)
{
	if(Cell(cellNumber)->size > depth)
		return;

	int newCell;

	newCell = SubDivide(cellNumber, Q_UL);
	if(newCell != 0xFF)
		UniformSubDivide(depth, newCell);
	newCell = SubDivide(cellNumber, Q_UR);
	if(newCell != 0xFF)
		UniformSubDivide(depth, newCell);
	newCell = SubDivide(cellNumber, Q_DL);
	if(newCell != 0xFF)
		UniformSubDivide(depth, newCell);
	newCell = SubDivide(cellNumber, Q_DR);
	if(newCell != 0xFF)
		UniformSubDivide(depth, newCell);
}

int Water::SubDivideToRadius(int cellIdx, float centerX, float centerZ, float shift, float minPosX, float minPosZ, float maxPosX, float maxPosZ, int depth)
{
	int err = 0;

	float l = centerX - shift;
	float r = centerX + shift;
	float u = centerZ - shift;
	float d = centerZ + shift;

// 	float v1x = l - posX;
// 	float v1z = u - posZ;
// 	float v2x = centerX - posX;
// 	float v2z = centerZ - posZ;
// 	float v3x = r - posX;
// 	float v3z = d - posZ;
	
// 	float vd1 = radius - Math::Abs(v1x);
// 	float vd2 = radius - Math::Abs(v2x);
// 	float vd3 = radius - Math::Abs(v3x); 
// 	float hd1 = radius - Math::Abs(v1z);
// 	float hd2 = radius - Math::Abs(v2z);
// 	float hd3 = radius - Math::Abs(v3z);
	
// 	bool h1 = (hd1 >= 0);
// 	bool h2 = (hd2 >= 0);
// 	bool h3 = (hd3 >= 0);
// 	bool v1 = (vd1 >= 0);
// 	bool v2 = (vd2 >= 0);
// 	bool v3 = (vd3 >= 0);
	
// 	bool U = (h1 || h2 || (posZ > u && posZ < centerZ));
// 	bool D = (h2 || h3 || (posZ > centerZ && posZ < d));
// 	bool L = (v1 || v2 || (posX > l && posX < centerX));
// 	bool R = (v2 || v3 || (posX > centerX && posX < r));

	bool U = (minPosZ < centerZ && maxPosZ > u);
	bool D = (minPosZ < d && maxPosZ > centerZ);
	bool L = (minPosX < centerX && maxPosX > l);
	bool R = (minPosX < r && maxPosX > centerX);
	
	float halfshift = shift * 0.5f;
	
	if(U)
	{
		if(L)
		{
			// divide & recurse this quadrant...
			int cell = SubDivide(cellIdx, Q_UL);
			if(cell < SUBDIVISION_ERRORS && depth >= 1)
				cell = SubDivideToRadius(cell, centerX - halfshift, centerZ - halfshift, halfshift, minPosX, minPosZ, maxPosX, maxPosZ, depth-1);
			if(cell > SUBDIVISION_ERRORS)
				return cell;
		}
		if(R)
		{
			// divide & recurse this quadrant...
			int cell = SubDivide(cellIdx, Q_UR);
			if(cell < SUBDIVISION_ERRORS && depth >= 1)
				cell = SubDivideToRadius(cell, centerX + halfshift, centerZ - halfshift, halfshift, minPosX, minPosZ, maxPosX, maxPosZ, depth-1);
			if(cell > SUBDIVISION_ERRORS)
				return cell;
		}
	}
	if(D)
	{
		if(L)
		{
			// divide & recurse this quadrant...
			int cell = SubDivide(cellIdx, Q_DL);
			if(cell < SUBDIVISION_ERRORS && depth >= 1)
				cell = SubDivideToRadius(cell, centerX - halfshift, centerZ + halfshift, halfshift, minPosX, minPosZ, maxPosX, maxPosZ, depth-1);
			if(cell > SUBDIVISION_ERRORS)
				return cell;
		}
		if(R)
		{
			// divide & recurse this quadrant...
			int cell = SubDivide(cellIdx, Q_DR);
			if(cell < SUBDIVISION_ERRORS && depth >= 1)
				cell = SubDivideToRadius(cell, centerX + halfshift, centerZ + halfshift, halfshift, minPosX, minPosZ, maxPosX, maxPosZ, depth-1);
			if(cell > SUBDIVISION_ERRORS)
				return cell;
		}
	}

	return 0xFF;
}

// this is the REAL subdivision method.. it walks the affectors and determines how to subdivide based on them..
void Water::AdaptiveSubDivide(void)
{
	// do we even have any affectors causing subdivisions?
	if(mPool->Affectors().Count() == 0) return;

	// run through each affector (in reverse order [see comment in WaterAffector:: in .h])
	for(int affectorIdx = mPool->Affectors().Count()-1; affectorIdx >= 0; affectorIdx--)
	{
		// get the affector in question...
		WaterAffector *affector = mPool->Affectors()[affectorIdx];

		// does this affector cause subdivision?
		if(affector->mWaterMaxSubDivisionDepth == 0)
			continue;

		// TODO: use mVerticalTessReduction in radius calculations below

		// prepare to interpolate radii
		float radius = affector->mWaterMaxRadius;
		float radiusStep = 0.0f;
		if(affector->mWaterMaxSubDivisionDepth > 1)
			radiusStep = (affector->mWaterMaxRadius - affector->mWaterMinRadius) / (affector->mWaterMaxSubDivisionDepth - 1);

		// get position of affector
		Vector3 pos = affector->WorldPosition();

		// convert to water local space...
		pos = pos * mWorldToLocal;

		// interpolate radii and subdivide to the appropriate depths..
		for(int depth = 0; depth < affector->mWaterMaxSubDivisionDepth; depth++)
		{
			float radiusx = radius * mInvXScale;
			float radiusz = radius * mInvZScale;
#ifdef DEBUG_WATER
			if(Water_Dbg.mDrawSubDivisionRings)
			{
				// draw ring...
				ts_bRGBA color = { 0xFF, 0x00, 0x00, 0xFF };
				float depthweight = depth / (float)(affector->mWaterMaxSubDivisionDepth - 1);
				color.G = (int)(depthweight * 255.0f);
				color.R = 255 - color.G;
				Vector3 a;
				Vector3 b;
				Vector3 center;
				center = affector->WorldPosition();
				center = center * mWorldToLocal;
				#define PHI_STEP (Math::Pi / 16.0f)
				float phi = PHI_STEP;
				b.x( center.x());
				b.z( center.z() + radiusz);
				b.y( GetHeightLocal(b.x(), b.z()));
				for(;phi <= Math::TwoPi; phi+=PHI_STEP)
				{
					a = b;
					b.x( center.x() + radiusx * Math::Sin(phi));
					b.z( center.z() + radiusz * Math::Cos(phi));
					b.y( GetHeightLocal(b.x(), b.z()));
					gfxDrawLineMC(&mInstance->Matrix(), &a, &b, &color);
				}
			}
#endif

			if(SubDivideToRadius(0, 0.5f, 0.5f, 0.5f, pos.x() - radiusx, pos.z() - radiusz, pos.x() + radiusx, pos.z() + radiusz, depth) > SUBDIVISION_ERRORS)
				return;

			radius -= radiusStep;
		}
	}
}

void Water::ReSubDivide(void)
{
	// reset mCurrNode (skip root node...)
	mCurrNode = mNodeArray+1;
	mCurrNodeIdx = 1;

	// reset root node
	mNodeArray->quad[Q_UL] = 0xFF;
	mNodeArray->quad[Q_UR] = 0xFF;
	mNodeArray->quad[Q_DL] = 0xFF;
	mNodeArray->quad[Q_DR] = 0xFF;
	mNodeArray->parent = 0xFF;
	mNodeArray->quadrant = 0;
	mNodeArray->neighbor[N_U] = 0xFF;
	mNodeArray->neighbor[N_D] = 0xFF;
	mNodeArray->neighbor[N_L] = 0xFF;
	mNodeArray->neighbor[N_R] = 0xFF;
	mNodeArray->size = 0;

	// perform uniform subdivision
	if(mMinSubDivDepth > 0)
		UniformSubDivide(mMinSubDivDepth);

	// now do adaptive subdivision on top of that...
	AdaptiveSubDivide();
}

#endif // SUPPORT_GENERIC_RENDERER
