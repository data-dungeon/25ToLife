///////////////////////////////////////////////////////////////////////////////
// The class to use defined tags...
// Used by single player and tag modes.
///////////////////////////////////////////////////////////////////////////////
#ifndef MODE_TAG_H
#define MODE_TAG_H

#include "EngineHelper/Singleton.h"
#include "Interfaces/Interface.h"
#include "Math/MathClass.h"
#include "Math/fastrand.h"
#include "sim/TeamingModel.h"

#define NEUTRAL_TAG  0	// Index for the netural team texture
#define BLUE_TAG     1	// Index for the generic blue team tag
#define RED_TAG      2	// Index for the generic red team tag
#define AVATAR_TAG   1  // Index for the avatar's unique tag

class ModeTag
{
public:
	ModeTag();
	~ModeTag();

	// Initialize everything
	void Init( const char* tagName );
	void Init( void );
	void Terminate( void );

	// Create a tag
	Tizag* CreateTag( CTeamingModel::ERole role, Vector3CRef position, 
		Vector3CRef normal, Vector3CRef up, float width, float height, float time );

	// Delete a tag
	void DeleteTag( Tizag* tag );

private:
	// Our internal stuff
	DecalSet m_decalSet;
};

// There can be only one!
DefineSingleton(ModeTag)
#define g_tag GetSingleton(ModeTag)

#endif
