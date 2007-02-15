/***************************************************************************/
// Tire track instance
/***************************************************************************/

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





// some constants we need
float TireTrack::totalLength;
float TireTrack::vSeg[TTRACK_VERTPAIRS];
float TireTrack::invSegmentLength;
float TireTrack::invSegments = 1.0f / (float)(TTRACK_SEGMENTS);
u16 TireTrack::indices[TTRACK_VERTS];

/***************************************************************************/
// Setup our initial data
/***************************************************************************/
TireTrack::TireTrack(void)
	: InstancedDynamicObject("TireTrack")
{
	uint creationFlags = CTRISTRIP_ZEROFILLHDR;

	// Init these.
	lock = 0;
	drawnLastFrame = 0;
	pairs = 0;

	// init the vertex buffer
	u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS];
	memset(texFlags, 0, sizeof(u32) * VTX_BUFFER_TEXTURE_LAYERS);
	texFlags[0] = VtxBuffer::VERT_TEX_DIM2;
	bool success = vbuf.Init(TTRACK_VERTS, VtxBuffer::VERT_HAS_DIFFUSE, texFlags);
	ASSERT(success);

	// init the strip buffer	
	success = sbuf.Init();
	ASSERT(success);
	sbuf.SetVtxBuffer(&vbuf);

	stripIndex = -1;
}



/***************************************************************************/
/***************************************************************************/
TireTrack::~TireTrack(void)
{ 
}

/***************************************************************************/
// Set the constants
/***************************************************************************/
void TireTrack::Setup(
float trackLength,
int textureRepeat)
{
	ASSERT(textureRepeat > 0);

	float repeat = (float)textureRepeat;
	totalLength = trackLength;
	float invSegments = 1.0f / (float)TTRACK_SEGMENTS;
	float segmentLength = totalLength * invSegments;
	invSegmentLength = 1.0f / segmentLength;
	for (int i = 0; i < TTRACK_VERTPAIRS; i++)
		vSeg[i] = (float)i * invSegments * repeat;
	invSegments *= repeat;

	int index;
	for(index = 0; index < TTRACK_VERTS; index++)
	{
		indices[index] = index;
	}
}

/***************************************************************************/
// Render it
/***************************************************************************/
void TireTrack::Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
	if(g_trackSystem.RenderOn() && pairs > 1)
	{
		if(stripIndex == -1)
			stripIndex = sbuf.AddStrip(indices, pairs*2);
		else
			stripIndex = sbuf.ReplaceStrip(stripIndex, indices, pairs*2);
		sbuf.Render(pInstance, pmLocalToCamera, fZCamDist);
	}
}

/***************************************************************************/
/***************************************************************************/
TireTrackPad16::~TireTrackPad16(void)
{ 
}
