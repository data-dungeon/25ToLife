///////////////////////////////////////////////////////////////////////////////
// The class to use defined tags...
// Used by single player and tag modes.
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "hud/hudlib.h"
#include "effects/modetag.h"
#ifdef PS2
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif

// This is hot!
DeclareSingleton(ModeTag);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ModeTag::ModeTag()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ModeTag::~ModeTag()
{
	Terminate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ModeTag::Init( const char* tagName )
{
	// Clear our default decal set
	m_decalSet.Destroy();

	// Load the specified player's decal set
	char fileName[30];
	sprintf( fileName, "objects/%s.dbl", tagName );
	m_decalSet.Load( fileName );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ModeTag::Init( void )
{
	// Create the outline and team emitters
	m_decalSet.Load("objects/tagframe.dbl");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ModeTag::Terminate( void )
{
	m_decalSet.Destroy();
}

///////////////////////////////////////////////////////////////////////////////
// Create a tag
///////////////////////////////////////////////////////////////////////////////
Tizag* ModeTag::CreateTag(
	CTeamingModel::ERole role,
	Vector3CRef position,
	Vector3CRef normal,
	Vector3CRef up,
	float width,
	float height,
	float time )
{
	// Find our tag index
	int index;
	if (role == CTeamingModel::BLUE)
		index = BLUE_TAG;
	else if (role == CTeamingModel::RED)
		index = RED_TAG;
	else if (role == CTeamingModel::AMBIENT)
		index = NEUTRAL_TAG;
	else
		return NULL;

	// Test for a valid decal set
	ASSERT(m_decalSet.Okay() && m_decalSet.Textures() > 0);
	if (!m_decalSet.Okay() || m_decalSet.Textures() == 0)
		return NULL;

	// Create our tag
	Tizag* tag;
	const int xPoints = 12;
	const int yPoints = 6;
	tag = new Tizag(m_decalSet, index, xPoints, yPoints);

	// Make the corners
	Vector3 corner[4];
	const float finalAlpha = 0.9f;
	if (!tag->Setup(position, normal, up, width, height, time, finalAlpha))
		return NULL;

	return tag;
}

///////////////////////////////////////////////////////////////////////////////
// Delete the given tag
///////////////////////////////////////////////////////////////////////////////
void ModeTag::DeleteTag(
	Tizag* tag )
{
	// Remove the tag if a valid index is used and the tag exists.
	if (tag)
	{
		delete tag;
		tag = NULL;
	}
}

