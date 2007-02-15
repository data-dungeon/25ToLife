//
// Author: Tyler Colbert
// Purpose: This is a part of the Water class.  Please see the Water.h header for details.  This
//    file has the code to actually render the water.
//

#include "Effects/EffectsPCH.h"


#ifdef PS2
#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "Display/PS2/Renderer/Common/VIFInline.h"
#include "Display/PS2/Renderer/Common/GIFTag.h"
#include "Display/PS2/Renderer/Common/PRIMReg.h"
#include "Effects/Water/WaterAffector.h"
#endif

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterPool.h"
#include "Effects/Water/WaterEquation.h"
#include "Effects/Water/WaterSprite.h"

#include "Effects/Water/WaterAffector.h"

#ifdef WATER_USE_FANS
static const u16 fan_plain[] = { 1, 0, 2, 4, 3, 0 };
static const u16 fan_split_1[] = { 0, 1, 2, 5, 3, 4, 1 };
static const u16 fan_split_2A[] = { 1, 0, 2, 6, 4, 3, 5, 0 };
static const u16 fan_split_2B[] = { 1, 0, 6, 2, 4, 3, 5, 0 };
static const u16 fan_split_3[] = { 1, 0, 6, 2, 4, 7, 3, 5, 0 };
static const u16 fan_split_4[] = { 1, 0, 6, 2, 7, 4, 8, 3, 5, 0 };
#else
static const u16 strip_plain[] = { 0, 2, 3, 4, 0, 1, 2, 4 };
static const u16 strip_split_1[] = { 1, 0, 2, 5, 3, 1, 2, 4, 3 };
static const u16 strip_split_2A[] = { 4, 3, 2, 5, 0, 0, 1, 2, 6, 4 };
static const u16 strip_split_2B[] = { 6, 0, 2, 5, 3, 6, 1, 2, 4, 3 };
static const u16 strip_split_3[] = { 1, 4, 2, 7, 3, 1, 2, 6, 0, 3, 2, 5, 0 };
static const u16 strip_split_4[] = { 3, 8, 2, 4, 7, 7, 1, 2, 6, 0, 0, 2, 5, 3 };
#endif

static const Matrix4x4* gCurrWaterLToC;

float WaterFuzzFactor = 0.5f;

#if (!defined(PS2)) || defined(SUPPORT_GENERIC_RENDERER) 

///////////////////////////////////////////////////////////////////////////////////////
// Water - Rendering
///////////////////////////////////////////////////////////////////////////////////////

void Water::BuildSplitInfo(u8 cellNumber, Node* cell, u8 quad)
{
	u8 neighbor[NUM_NEIGHBORS] = { 0xFF, 0xFF, 0xFF, 0xFF };
	u8 neighbor_parent[NUM_NEIGHBORS] = { 0xFF, 0xFF, 0xFF, 0xFF };

	static const u8 neighborquadmask[NUM_NEIGHBORS] = { Q_OX, Q_OX, Q_XO, Q_XO };
	
	switch(quad)
	{
	case Q_UL:
		neighbor[N_U] = neighbor[N_L] = 0xFF;
		neighbor[N_D] = neighbor[N_R] = cellNumber;
		neighbor_parent[N_U] = cell->neighbor[N_U];
		neighbor_parent[N_L] = cell->neighbor[N_L];
		break;
	case Q_UR:
		neighbor[N_U] = neighbor[N_R] = 0xFF;
		neighbor[N_D] = neighbor[N_L] = cellNumber;
		neighbor_parent[N_U] = cell->neighbor[N_U];
		neighbor_parent[N_R] = cell->neighbor[N_R];
		break;
	case Q_DL:
		neighbor[N_D] = neighbor[N_L] = 0xFF;
		neighbor[N_U] = neighbor[N_R] = cellNumber;
		neighbor_parent[N_D] = cell->neighbor[N_D];
		neighbor_parent[N_L] = cell->neighbor[N_L];
		break;
	case Q_DR:
		neighbor[N_D] = neighbor[N_R] = 0xFF;
		neighbor[N_U] = neighbor[N_L] = cellNumber;
		neighbor_parent[N_D] = cell->neighbor[N_D];
		neighbor_parent[N_R] = cell->neighbor[N_R];
		break;
	}

	u8 i;

	for(i = 0; i < NUM_NEIGHBORS; i++)
		if(neighbor_parent[i] != 0xFF)
			neighbor[i] = Cell(neighbor_parent[i])->quad[cell->quadrant ^ neighborquadmask[i]];

	cell->splitcount[quad] = 0;
	for(i = 0; i < NUM_NEIGHBORS; i++)
	{
		if(neighbor[i] != 0xFF)
		{
			cell->splits[quad][i] = Cell(neighbor[i])->quad[quad ^ neighborquadmask[i]];
			if(cell->splits[quad][i] != 0xFF)
				cell->splitcount[quad]++;
		}
		else
			cell->splits[quad][i] = 0xFF;
	}
}

void Water::SetSpecularCoords(int layer, int index, Vector3& vtx, Vector3& normal, Vector3& toEye)
{
	Vector3 r = normal * (2.0f * Vector3::Dot(normal, mLayerModes[layer].mLight)) - mLayerModes[layer].mLight;
	float a = Vector3::Dot(r, toEye);
	float d = r.Cross(toEye).Dot(normal);
	float s = Math::Clamp((Math::Square(a) - 0.75f) * 2.0f);
	mVtxBuffer[layer].TexCoordv2(0, index).Set(s, s);
}

void Water::SetEnvironmentCoords(int layer, int index, Vector3& normal, Vector3& toEye)
{
	Vector3 r = normal * (2.0f * Vector3::Dot(normal, toEye)) - toEye;
	mVtxBuffer[layer].TexCoordv2(0, index).Set(r.x(), r.z());
}

void Water::DO_VERTEX(int vtx, Vector3& srcvtx, Vector3& normalvtx)
{
   int index = (vtx);
	int numSprites = mPool->Sprites().Count();
	int i;

	for(i = 0; i < mNumLayers; i++)
	{
		mVtxBuffer[i].Vertex(index) = (srcvtx);
	}

	for(i = 0; i < numSprites; i++)
	{
		mSpriteVtxBuffers[i].Vertex(index) = (srcvtx);
		mSpriteVtxBuffers[i].TexCoordv2(0, index).Set((srcvtx).x( ) + mXOffset, (srcvtx).z( ) + mZOffset);
		mSpriteVtxBuffers[i].Diffuse(index) = VtxBuffer::cvtRGBA(mPool->Sprites()[i]->Color());
	}

	Vector3 __n = (normalvtx);
	__n.x(__n.x() * mInvXScale);		/* correct for non-uniform scaled normal */
	__n.z(__n.z() * mInvZScale);		/* correct for non-uniform scaled normal */
	__n.Normalize();

	Vector3 wvtx = srcvtx;
	wvtx.x(wvtx.x() * mXScale);
	wvtx.z(wvtx.z() * mZScale);

	Vector3 toEye = mEyePoint - wvtx;
	toEye.Normalize();


	Vector3 n, svtx;
	for(int layer = 0; layer < mNumLayers; layer++){
		n = __n * mLayerModes[layer].mNormAdjParameter;
		n.Normalize();
		switch(mLayerModes[layer].mLayerType)
		{
		case WaterLayerModes::LAYER_DISTORT:
		{
			const Matrix4x4 &m = *MatrixStack__Get(MS_PROJECTION);
			svtx = srcvtx;
// 			svtx.x += Math::Min(n.x, 0.001);
// 			svtx.z += Math::Min(n.z, 0.001);
			svtx.x(svtx.x() + n.x());
			svtx.z(svtx.z() + n.z());
			Vector4 v1;
			v1 = svtx.Transform3(*gCurrWaterLToC);
			v1 = v1.Transform(m);
			v1.w( 1.0f / v1.w());
			v1 *= v1.w();
			// x has -1 to 1 in it, need 0 to 1
			v1.x(v1.x() * 0.5f);
			v1.x(v1.x() + 0.5f);
			// y has -1 to 1 in it, need 0 to 1
			v1.y(v1.y() * 0.5f);
			v1.y(v1.y() + 0.5f);

#ifdef DIRECTX
			// y is upside down..
			v1.y(1.0f - v1.y());
#endif
#ifdef PS2
			// y scale includes 0 to 255, need 0 to 224 portion of that
 			v1.y(v1.y() * (224.0f / 256.0f));
#endif

			// clamp to valid values... this should be done with texture clamping,
			// but that not how Im doing it for now...
			v1.x(Math::Clamp(v1.x(), 0.0f, 1.0f));
			v1.y(Math::Clamp(v1.y(), 0.0f, 1.0f));
			mVtxBuffer[layer].TexCoordv2(0, index).Set(v1.x(), v1.y());
			break;
		}
		case WaterLayerModes::LAYER_DECAL:
			svtx = (srcvtx) * mLayerModes[layer].mVtxAdjParameter;
			mVtxBuffer[layer].TexCoordv2(0, index).Set(
				svtx.x() + n.x(),
				svtx.z() + n.z());
			break;
		case WaterLayerModes::LAYER_SPECULAR:
			SetSpecularCoords(layer, index, svtx, n, toEye);
			break;
		case WaterLayerModes::LAYER_ENVIRONMENT:
			SetEnvironmentCoords(layer, index, n, toEye);
			break;
		case WaterLayerModes::LAYER_SHADOW:
			svtx = (srcvtx) * mLayerModes[layer].mVtxAdjParameter;
			mVtxBuffer[layer].TexCoordv2(0, index).Set(
				svtx.z() + n.z(),
				svtx.x() + n.x());
			break;
		}

		float alpha = 255.0f;
		if((mLayerModes[layer].mFlags & WaterLayerModes::LAYER_FADE_HEIGHT) == WaterLayerModes::LAYER_FADE_HEIGHT)
		{
			alpha = svtx.y() * 255.0f;
		}
		else if((mLayerModes[layer].mFlags & WaterLayerModes::LAYER_FADE_EDGES) == WaterLayerModes::LAYER_FADE_EDGES)
		{
			alpha = mDampeningGrid(srcvtx.x(), srcvtx.z());
		}
		else if((mLayerModes[layer].mFlags & WaterLayerModes::LAYER_FADE_CENTER) == WaterLayerModes::LAYER_FADE_CENTER)
		{
			alpha = 255.0f - mDampeningGrid(srcvtx.x(), srcvtx.z());
		}
		alpha *= mLayerModes[layer].mOpacity;
		u8 val = (u8)(Math::Clamp(alpha, 0.0f, 255.0f));
		mVtxBuffer[layer].Diffuse(index) = VtxBuffer::cvtRGBA(0xFFFFFF | (val << 24));
	}
}

#ifdef DEBUG_WATER

#define COPY_VERTEX(vtx, srcvtx, normalvtx)                                             \
{                                                                                       \
    DO_VERTEX(vtx, srcvtx, normalvtx);                                                  \
    if(Water_Dbg.mDrawNormalLines)                                                      \
    {                                                                                   \
        static ts_bRGBA color = { 255, 0, 0, 255 };                                     \
                                                                                        \
        Vector3 __n = (normalvtx);                                                      \
        __n.x(__n.x() * mInvXScale);        /* correct for non-uniform scaled normal */         \
        __n.z(__n.z() * mInvZScale);        /* correct for non-uniform scaled normal */         \
        __n.Normalize();                                                                \
                                                                                        \
        __n.x(__n.x() * mInvXScale);        /* undo non-uniform scale */                        \
        __n.z(__n.z() * mInvZScale);        /* undo non-uniform scale */                        \
        Vector3 __pos = (srcvtx);                                                       \
        Vector3 __nvtx = (__pos) + (__n * 0.25f);                                       \
        gfxDrawLineMC(&mInstance->Matrix(), &__pos, &__nvtx, &color);              \
    }                                                                                   \
}
#else
#define COPY_VERTEX(vtx, srcvtx, normalvtx)                     \
{                                                               \
    DO_VERTEX(vtx, srcvtx, normalvtx);                          \
}
#endif

void Water::FillBaseTile(int rotations, int offset, int cellNumber, Node* cell, u8 quad, u8 quadrant)
{
	static const int centervtxidx[4][NUM_QUADRANTS] = {{ 4, 3, 1, 0 }, { 1, 4, 0, 3 }, { 0, 1, 3, 4 }, { 3, 0, 4, 1 }};
	static const int cornervtxidx[4][NUM_QUADRANTS] = {{ 0, 1, 3, 4 }, { 3, 0, 4, 1 }, { 4, 3, 1, 0 }, { 1, 4, 0, 3 }};
	static const int udedgevtxidx[4][NUM_QUADRANTS] = {{ 1, 0, 4, 3 }, { 0, 3, 1, 4 }, { 3, 4, 0, 1 }, { 4, 1, 3, 0 }};
	static const int lredgevtxidx[4][NUM_QUADRANTS] = {{ 3, 4, 0, 1 }, { 4, 1, 3, 0 }, { 1, 0, 4, 3 }, { 0, 3, 1, 4 }};
	int udedge;
	int lredge;

	if((quad & Q_OX) == Q_OL)
		lredge = N_L;
	else
		lredge = N_R;
	
	if((quad & Q_XO) == Q_UO)
		udedge = N_U;
	else
		udedge = N_D;
		
	COPY_VERTEX(offset+2, mRenderNodeArray[cellNumber].mCenters[quad], mRenderNodeArray[cellNumber].mCenterNormals[quad]);
	COPY_VERTEX(offset+udedgevtxidx[rotations][quad], mRenderNodeArray[cellNumber].mEdges[udedge], mRenderNodeArray[cellNumber].mEdgeNormals[udedge]);
	COPY_VERTEX(offset+lredgevtxidx[rotations][quad], mRenderNodeArray[cellNumber].mEdges[lredge], mRenderNodeArray[cellNumber].mEdgeNormals[lredge]);

	// EEK!!! I have to go way back up the chain....
	Node* searchcell = cell;
	Vector3* vtx;
	Vector3* nvtx;
	while(searchcell->quadrant == quad && searchcell->parent != 0xFF)
		searchcell = Cell(searchcell->parent);

	// no parent, use the corner
	if(searchcell->parent == 0xFF)
	{
		vtx = &mRendMasterNode.mCorners[quad];
		nvtx = &mRendMasterNode.mCornerNormals[quad];
	}
	// same ud edge
	else if((searchcell->quadrant & Q_XO) == (quad & Q_XO))
	{
		vtx = &mRenderNodeArray[searchcell->parent].mEdges[udedge];
		nvtx = &mRenderNodeArray[searchcell->parent].mEdgeNormals[udedge];
	}
	// same lr edge
	else if((searchcell->quadrant & Q_OX) == (quad & Q_OX))
	{
		vtx = &mRenderNodeArray[searchcell->parent].mEdges[lredge];
		nvtx = &mRenderNodeArray[searchcell->parent].mEdgeNormals[lredge];
	}
	// opposite corner...
	else
	{
		searchcell = Cell(searchcell->parent);
		if(searchcell->parent == 0xFF)
		{
			vtx = &mRendMasterNode.mCenter;
			nvtx = &mRendMasterNode.mCenterNormal;
		}
		else
		{
			vtx = &mRenderNodeArray[searchcell->parent].mCenters[searchcell->quadrant];
			nvtx = &mRenderNodeArray[searchcell->parent].mCenterNormals[searchcell->quadrant];
		}
	}

	COPY_VERTEX(offset+cornervtxidx[rotations][quad], *vtx, *nvtx);

	if(cell->parent == 0xFF)
	{
		vtx = &mRendMasterNode.mCenter;
		nvtx = &mRendMasterNode.mCenterNormal;
	}
	else
	{
		vtx = &mRenderNodeArray[cell->parent].mCenters[quadrant];
		nvtx = &mRenderNodeArray[cell->parent].mCenterNormals[quadrant];
	}
	COPY_VERTEX(offset+centervtxidx[rotations][quad], *vtx, *nvtx);
}

int Water::FillTile(u8 cellNumber, Node* cell, u8 quad, int offset, u8& buffertype)
{
	int suboffset = 5;
	
	switch(cell->splitcount[quad])
	{
	case 0:
		FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
		buffertype = SB_PLAIN;
		break;
	case 1:
		buffertype = SB_SPLIT_1;
		if(cell->splits[quad][N_U] != 0xFF)
		{
			FillBaseTile(1, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++), 
							mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D], 
							mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
		}
		else if(cell->splits[quad][N_D] != 0xFF)
		{
			FillBaseTile(3, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++), 
							mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U], 
							mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
		}
		else if(cell->splits[quad][N_R] != 0xFF)
		{
			FillBaseTile(2, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++), 
							mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L], 
							mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
		}
		else
		{
			FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++), 
							mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R], 
							mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
		}
		break;
	case 2:
		buffertype = SB_SPLIT_2B;
		if(cell->splits[quad][N_U] != 0xFF)
		{
			if(cell->splits[quad][N_D] != 0xFF)
			{
				// N_U && N_D
				buffertype = SB_SPLIT_2A;
				FillBaseTile(1, offset, cellNumber, cell, quad, cell->quadrant);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
								mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
								mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
			}
			else if(cell->splits[quad][N_L] != 0xFF)
			{
				// N_U && N_L
				FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
								mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
								mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
			}
			else
			{
				// N_U && N_R
				FillBaseTile(1, offset, cellNumber, cell, quad, cell->quadrant);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
								mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
								mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
			}
		}
		else if(cell->splits[quad][N_L] != 0xFF)
		{
			if(cell->splits[quad][N_R] != 0xFF)
			{
				// N_L && N_R
				buffertype = SB_SPLIT_2A;
				FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
								mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
								mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
			}
			else
			{
				// N_D && N_L
				FillBaseTile(3, offset, cellNumber, cell, quad, cell->quadrant);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
								mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
				COPY_VERTEX(offset+(suboffset++),
								mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
								mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
			}
		}
		else
		{
			// N_D && N_R
			FillBaseTile(2, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
							mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
							mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
		}
		break;
	case 3:
		if(cell->splits[quad][N_U] == 0xFF)
		{
			FillBaseTile(3, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
							mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
							mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
							mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
		}
		else if(cell->splits[quad][N_D] == 0xFF)
		{
			FillBaseTile(1, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
							mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
							mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
							mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
		}
		else if(cell->splits[quad][N_R] == 0xFF)
		{
			FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
							mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
							mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
							mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
		}
		else
		{
			FillBaseTile(2, offset, cellNumber, cell, quad, cell->quadrant);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
							mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
							mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
			COPY_VERTEX(offset+(suboffset++),
							mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
							mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
		}
		buffertype = SB_SPLIT_3;
		break;
	case 4:
		FillBaseTile(0, offset, cellNumber, cell, quad, cell->quadrant);
		COPY_VERTEX(offset+(suboffset++),
						mRenderNodeArray[cell->splits[quad][N_L]].mEdges[N_R],
						mRenderNodeArray[cell->splits[quad][N_L]].mEdgeNormals[N_R]);
		COPY_VERTEX(offset+(suboffset++),
						mRenderNodeArray[cell->splits[quad][N_U]].mEdges[N_D],
						mRenderNodeArray[cell->splits[quad][N_U]].mEdgeNormals[N_D]);
		COPY_VERTEX(offset+(suboffset++),
						mRenderNodeArray[cell->splits[quad][N_R]].mEdges[N_L],
						mRenderNodeArray[cell->splits[quad][N_R]].mEdgeNormals[N_L]);
		COPY_VERTEX(offset+(suboffset++),
						mRenderNodeArray[cell->splits[quad][N_D]].mEdges[N_U],
						mRenderNodeArray[cell->splits[quad][N_D]].mEdgeNormals[N_U]);
		buffertype = SB_SPLIT_4;
		break;
	}

	return suboffset;
}

void Water::FillTileNormals(u8 cellNumber, Node* cell, u8 quad)
{
	static const int neighquad[NUM_NEIGHBORS][2] = {{ Q_UL, Q_DL }, { Q_DR, Q_UR }, { Q_UR, Q_UL }, { Q_DL, Q_DR }};
	int udedge;
	int lredge;

	if((quad & Q_OX) == Q_OL)
		lredge = N_L;
	else
		lredge = N_R;
	
	if((quad & Q_XO) == Q_UO)
		udedge = N_U;
	else
		udedge = N_D;
		
	// EEK!!! I have to go way back up the chain....
	Node* searchcell = cell;
	Vector3* cornervtx;
	Vector3* centervtx;
	Vector3* cornernormal;
	Vector3* centernormal;

	while(searchcell->quadrant == quad && searchcell->parent != 0xFF)
		searchcell = Cell(searchcell->parent);

	// no parent, use the corner
	if(searchcell->parent == 0xFF)
	{
		cornervtx = &mRendMasterNode.mCorners[quad];
		cornernormal = &mRendMasterNode.mCornerNormals[quad];
	}
	// same ud edge
	else if((searchcell->quadrant & Q_XO) == (quad & Q_XO))
	{
		cornervtx = &mRenderNodeArray[searchcell->parent].mEdges[udedge];
		cornernormal = &mRenderNodeArray[searchcell->parent].mEdgeNormals[udedge];
	}
	// same lr edge
	else if((searchcell->quadrant & Q_OX) == (quad & Q_OX))
	{
		cornervtx = &mRenderNodeArray[searchcell->parent].mEdges[lredge];
		cornernormal = &mRenderNodeArray[searchcell->parent].mEdgeNormals[lredge];
	}
	// opposite corner...
	else
	{
		searchcell = Cell(searchcell->parent);
		if(searchcell->parent == 0xFF)
		{
			cornervtx = &mRendMasterNode.mCenter;
			cornernormal = &mRendMasterNode.mCenterNormal;
		}
		else
		{
			cornervtx = &mRenderNodeArray[searchcell->parent].mCenters[searchcell->quadrant];
			cornernormal = &mRenderNodeArray[searchcell->parent].mCenterNormals[searchcell->quadrant];
		}
	}

	if(cell->parent == 0xFF)
	{
		centervtx = &mRendMasterNode.mCenter;
		centernormal = &mRendMasterNode.mCenterNormal;
	}
	else
	{
		int quadrant = cell->quadrant;
		centervtx = &mRenderNodeArray[cell->parent].mCenters[quadrant];
		centernormal = &mRenderNodeArray[cell->parent].mCenterNormals[quadrant];
	}


	// ok, now we know the 5 base verts.. 
	// we always need the 4 base vectors that these form (these are the X in the middle of the cells quadrant...
	RendNode* rNode = &mRenderNodeArray[cellNumber];
	Vector3 v[NUM_QUADRANTS];
	v[quad ^ Q_OX] = rNode->mEdges[udedge] - rNode->mCenters[quad];
	v[quad ^ Q_XO] = rNode->mEdges[lredge] - rNode->mCenters[quad];
	v[quad ^ Q_XX] = *centervtx - rNode->mCenters[quad];
	v[quad] = *cornervtx - rNode->mCenters[quad];
	v[Q_UL].Normalize();
	v[Q_UR].Normalize();
	v[Q_DL].Normalize();
	v[Q_DR].Normalize();
	Vector3 *normPtr[NUM_QUADRANTS];
	normPtr[quad ^ Q_OX] = &rNode->mEdgeNormals[udedge];
	normPtr[quad ^ Q_XO] = &rNode->mEdgeNormals[lredge];
	normPtr[quad ^ Q_XX] = centernormal;
	normPtr[quad] = cornernormal;

	Vector3 cross;

	for(int neigh = N_L; neigh < NUM_QUADRANTS; neigh++)
	{
		if(cell->splits[quad][neigh] != 0xFF)
		{
			// there are 2 triangles that make up this quadrant...
			for(int tri = 0; tri < 2; tri++)
			{
				Vector3 splitedge = mRenderNodeArray[cell->splits[quad][neigh]].mEdges[neigh ^ 1] - rNode->mCenters[quad];
				// surface normal of the triangle in question
				cross = v[neighquad[neigh][tri]].Cross(splitedge);
				// one eight of the total surface contributing to all verts...
				cross *= 0.125f;
				*normPtr[neighquad[neigh][tri]] += cross;
				rNode->mCenterNormals[quad] += cross;
			}
		}
		else
		{
			// surface normal of the triangle in question
			cross = v[neighquad[neigh][0]].Cross(v[neighquad[neigh][1]]);
			// one quarter of the total surface contributing to the center vert
			rNode->mCenterNormals[quad] += cross * 0.25f;
			// one eight of the total surface contributing to these corner verts...
			cross *= 0.125f;
			*normPtr[neighquad[neigh][0]] += cross;
			*normPtr[neighquad[neigh][1]] += cross;
		}
	}
}

void Water::AddStrip(StripBuffer& stripBuffer, const u16* indexlist, int count, int offset)
{
	u16 index[10];
	
	for(int i = 0; i < count; i++)
	{
		index[i] = *indexlist++ + offset;
	}
	stripBuffer.AddStrip(&index[0], count);
}

void Water::AddStrips(int buffertype, StripBuffer& stripBuffer, int offset)
{
	switch(buffertype)
	{
#ifdef WATER_USE_FANS
	case SB_PLAIN:
// 		AddStrip(stripBuffer, fan_plain, 6, offset);
		break;
	case SB_SPLIT_1:
		AddStrip(stripBuffer, fan_split_1, 5, offset);
		break;
	case SB_SPLIT_2A:
// 		AddStrip(stripBuffer, fan_split_2A, 8, offset);
		break;
	case SB_SPLIT_2B:
// 		AddStrip(stripBuffer, fan_split_2B, 8, offset);
		break;
	case SB_SPLIT_3:
// 		AddStrip(stripBuffer, fan_split_3, 9, offset);
		break;
	case SB_SPLIT_4:
// 		AddStrip(stripBuffer, fan_split_4, 10, offset);
		break;
#else
	case SB_PLAIN:
		AddStrip(stripBuffer, strip_plain, 4, offset);
		AddStrip(stripBuffer, &strip_plain[4], 4, offset);
		break;
	case SB_SPLIT_1:
		AddStrip(stripBuffer, strip_split_1, 5, offset);
		AddStrip(stripBuffer, &strip_split_1[5], 4, offset);
		break;
	case SB_SPLIT_2A:
		AddStrip(stripBuffer, strip_split_2A, 5, offset);
		AddStrip(stripBuffer, &strip_split_2A[5], 5, offset);
		break;
	case SB_SPLIT_2B:
		AddStrip(stripBuffer, strip_split_2B, 5, offset);
		AddStrip(stripBuffer, &strip_split_2B[5], 5, offset);
		break;
	case SB_SPLIT_3:
		AddStrip(stripBuffer, strip_split_3, 5, offset);
		AddStrip(stripBuffer, &strip_split_3[5], 4, offset);
		AddStrip(stripBuffer, &strip_split_3[9], 4, offset);
		break;
	case SB_SPLIT_4:
		AddStrip(stripBuffer, strip_split_4, 5, offset);
		AddStrip(stripBuffer, &strip_split_4[5], 5, offset);
		AddStrip(stripBuffer, &strip_split_4[10], 4, offset);
		break;
#endif
	}
}

void Water::RenderRecurse(int cellNumber, float shift)
{
	static const float xsigns[] = { -1.0f, +1.0f, -1.0f, +1.0f };
	static const float zsigns[] = { -1.0f, -1.0f, +1.0f, +1.0f };
	static int offset;
	float halfshift = shift * 0.5f;
	int numSprites = mPool->Sprites().Count();
	int i;

	// get the cell we are rendering...
	Node* cell = Cell(cellNumber);
	RendNode *node = &mRenderNodeArray[cellNumber];

	if(cellNumber == 0)
		offset = 0;

	for(u8 quad = Q_UL; quad < NUM_QUADRANTS; quad++)
	{
		if(cell->quad[quad] != 0xFF)
		{
			RenderRecurse(
				cell->quad[quad],
				halfshift);
		}
		else
		{
			u8 buffertype;
			u32 offsetshift = FillTile(
				cellNumber,
				cell,
				quad,
				offset,
				buffertype);

			for(i = 0; i < mNumLayers; i++)
				AddStrips(buffertype, mStripBuffer[i], offset);

			for(i = 0; i < numSprites; i++)
			{
				// TODO: deal with activating the texture... (cant be done with stripbuffers...)
				AddStrips(buffertype, mSpriteStripBuffers[i], offset);
			}

			offset += offsetshift;
		}
	}
}

// fill in the render nodes...
#define FILL_VERTEX(vtx, xval, zval) (vtx).Set(xval,GetHeightLocal(xval, zval),zval)

void Water::EvaluateRenderTile(u8 index, float centerx, float centerz, float shift)
{
	static const float xsigns[] = { -1.0f, +1.0f, -1.0f, +1.0f };
	static const float zsigns[] = { -1.0f, -1.0f, +1.0f, +1.0f };

	RendNode *node = &mRenderNodeArray[index];

	float u = centerz - shift;
	float d = centerz + shift;
	float l = centerx - shift;
	float r = centerx + shift;

	// centers of edges of the cell
	FILL_VERTEX(node->mEdges[N_U], centerx, u);
	FILL_VERTEX(node->mEdges[N_D], centerx, d);
	FILL_VERTEX(node->mEdges[N_L], l, centerz);
	FILL_VERTEX(node->mEdges[N_R], r, centerz);

	// clear the normals while we are here...
	node->mEdgeNormals[N_U].Set(0.0f, 0.0f, 0.0f);
	node->mEdgeNormals[N_D].Set(0.0f, 0.0f, 0.0f);
	node->mEdgeNormals[N_L].Set(0.0f, 0.0f, 0.0f);
	node->mEdgeNormals[N_R].Set(0.0f, 0.0f, 0.0f);

	// centers of sub cells...
	float halfshift = shift * 0.5f;
	Node* cell = Cell(index);
	for(int quad = Q_UL; quad < NUM_QUADRANTS; quad++)
	{
		// fill the center...
		FILL_VERTEX(node->mCenters[quad], centerx + (halfshift*xsigns[quad]), centerz + (halfshift*zsigns[quad]));

		// clear the normals while we are here...
		node->mCenterNormals[quad].Set(0.0f, 0.0f, 0.0f);

		// recurse into subnode info...
		if(cell->quad[quad] != 0xFF)
			EvaluateRenderTile(cell->quad[quad], node->mCenters[quad].x(), node->mCenters[quad].z(), halfshift);

		BuildSplitInfo(index, cell, quad);
	}
}

void Water::EvaluateRenderTileNormals(int index)
{
	Node *cell = &mNodeArray[index];
	RendNode *node = &mRenderNodeArray[index];

	for(int quad = Q_UL; quad < NUM_QUADRANTS; quad++)
	{
		// recurse into subnode info...
		if(cell->quad[quad] != 0xFF)
			EvaluateRenderTileNormals(cell->quad[quad]);
		else
		{
			//
			// normals at centers of edges of the cell
			// this is handled as though drawing the cell..., but the normals
			// are accumulated.  Edge normals are scaled to |n|=0.5f, while corner
			// normals are |n|=0.25f.  that way, the sum of the |n|=1.0f
			// 
			FillTileNormals(index, cell, quad);
		}
	}
}

void Water::MergeDuplicateNormals(int cellNumber)
{
	// some normals in the grid are duplicated across up to 2 nodes... this function
	// hunts them down and averages them. They must be averaged because the normals
	// are constructed separately..
	Node* cell = Cell(cellNumber);

	for(int quad = Q_UL; quad < NUM_QUADRANTS; quad++)
	{
		if(cell->quad[quad] != 0xFF)
		{
			MergeDuplicateNormals(cell->quad[quad]);

			for(int neighbors = N_L; neighbors < NUM_NEIGHBORS; neighbors++)
			{
				if(cell->splits[quad][neighbors] != 0xFF)
				{
					// do the merge...
					RendNode* rnode = &mRenderNodeArray[cell->quad[quad]];
					RendNode* otherNode = &mRenderNodeArray[cell->splits[quad][neighbors]];
					rnode->mEdgeNormals[neighbors] += otherNode->mEdgeNormals[neighbors ^ 1];
					otherNode->mEdgeNormals[neighbors ^ 1] = rnode->mEdgeNormals[neighbors];
				}
			}
		}
	}
}

void Water::EvaluateMesh()
{
	// fill the master nodes vertices...
	FILL_VERTEX(mRendMasterNode.mCorners[Q_UL], 0.0f, 0.0f);
	FILL_VERTEX(mRendMasterNode.mCorners[Q_UR], 1.0f, 0.0f);
	FILL_VERTEX(mRendMasterNode.mCorners[Q_DL], 0.0f, 1.0f);
	FILL_VERTEX(mRendMasterNode.mCorners[Q_DR], 1.0f, 1.0f);
	FILL_VERTEX(mRendMasterNode.mCenter, 0.5f, 0.5f);

	// clear master normals...
	mRendMasterNode.mCornerNormals[Q_UL].Set(0.0f, 0.0f, 0.0f);
	mRendMasterNode.mCornerNormals[Q_UR].Set(0.0f, 0.0f, 0.0f);
	mRendMasterNode.mCornerNormals[Q_DL].Set(0.0f, 0.0f, 0.0f);
	mRendMasterNode.mCornerNormals[Q_DR].Set(0.0f, 0.0f, 0.0f);
	mRendMasterNode.mCenterNormal.Set(0.0f, 0.0f, 0.0f);

	// fill the remaining verts...
	EvaluateRenderTile(0, 0.5f, 0.5f, 0.5f);

	// fill the normals...
// 	mRendMasterNode.mCornerNormals[Q_UL];
	
	EvaluateRenderTileNormals(0);

	MergeDuplicateNormals(0);
}


void Water::ResetStripBuffers(int numSprites)
{
	int i;

	for(i = 0; i < mNumLayers; i++)
		mStripBuffer[i].Reset();


	for(i = 0; i < numSprites; i++)
	{
		WaterSprite* sprite = mPool->Sprites()[i];

		mSpriteStripBuffers[i].Reset();
		mSpriteStripBuffers[i].SetPassTexture(0, sprite->mTexture, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR, sprite->GetAlphaBlendFunc());
	}
}

#endif // if (!defined(PS2)) || defined(SUPPORT_GENERIC_RENDERER) 

void Water::Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
	gCurrWaterLToC = pmLocalToCamera;

#if 0
	// TJC -  draw the bounding box...
	DrawUtility::Box(GetBoundingBox().Min, GetBoundingBox().Max, pInstance->mLocalToWorld);
	DrawUtility::Box(pInstance->BoundingBox.Min, pInstance->BoundingBox.Max, NULL, NULL, DrawUtility::RED);
#endif

#if defined(DEBUG_WATER) && defined(SUPPORT_GENERIC_RENDERER)
	if(Water_Dbg.mUseGenericCode)
		Water::RenderGeneric(pInstance, pmLocalToCamera, fZCamDist);
	else
#endif
		Water::RenderPlatform(pInstance, pmLocalToCamera, fZCamDist);
}

void Water::RenderGeneric(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
#if (defined(PS2)) && (!defined(SUPPORT_GENERIC_RENDERER))
	RenderPlatform(pInstance, pmLocalToCamera, fZCamDist);
#else // if (!defined(PS2)) || defined(DEBUG_WATER) 

	ReSubDivide();

	EvaluateMesh();
	
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableDrawWater)
#endif
	{

		// render it all
		// recursive again... sigh...
		Graphics4x4 tex[8];
		for(int layer = 0; layer < mNumLayers; layer++)
		{
			tex[layer].SetScale(mLayerModes[layer].mScaleU, mLayerModes[layer].mScaleV, 1.0f);
			tex[layer].CatTranslation(Math::Mod(mT * mLayerModes[layer].mFlowU, 1.0f), Math::Mod(mT * mLayerModes[layer].mFlowV, 1.0f), 0.0f);

			if(mLayerModes[layer].mLayerType == WaterLayerModes::LAYER_DISTORT)
			{
			}
		}

		int numSprites = mPool->Sprites().Count();
		int i;

		for(i = 0; i < numSprites; i++)
			mSpriteVtxBuffers[i].Lock();
		for(i = 0; i < mNumLayers; i++)
			mVtxBuffer[i].Lock();


		ResetStripBuffers(numSprites);

		RenderRecurse(0, 0.5f);

		for(i = 0; i < mNumLayers; i++)
		{
			mVtxBuffer[i].UnLock();
			mStripBuffer[i].Render(*pmLocalToCamera, &tex[i]);
		}

		for(i = 0; i < numSprites; i++)
		{
			mSpriteVtxBuffers[i].UnLock();
			WaterSprite* sprite = mPool->Sprites()[i];
			mSpriteStripBuffers[i].Render(*pmLocalToCamera, &sprite->mTexMatrix);
		}

	}

	// draw grids...
#ifdef DEBUG_WATER
	if(Water_Dbg.mDrawPhysicsGrid)
	{
		ts_bRGBA color = { 0xFF, 0x00, 0x00, 0xFF };
		mPhysicsGrid.QuickDrawGrid(&mInstance->Matrix(), &color);
	}
	if(Water_Dbg.mDrawDampeningGrid)
	{
		ts_bRGBA color1 = { 0x00, 0xFF, 0x00, 0xFF };
		ts_bRGBA color2 = { 0xFF, 0x00, 0x00, 0xFF };
		mDampeningGrid.DrawGridWeightedColors(
			&mInstance->Matrix(), 
			&color1, 
			&color2, 
			true, 
			(1.0f / 255.0f));
	}
#endif // DEBUG_WATER
#endif //if (!defined(PS2)) || defined(SUPPORT_GENERIC_RENDERER) 
}



#ifndef PS2


// Main function to render the water..

void Water::RenderPlatform(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{

	///////////////////////////////////////
	// NORMAL GENERIC RENDERER FOR WATER
	///////////////////////////////////////
	RenderGeneric(pInstance, pmLocalToCamera, fZCamDist);
}

// PS2 version is in WaterRenderPS2.cpp (it got kind of big...)

#endif // PS2


