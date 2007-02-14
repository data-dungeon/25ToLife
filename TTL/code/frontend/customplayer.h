//-----------------------------------------------------------
//
// CustomPlayer.cpp
//
// Handles switching of player pieces, loading/saving to
// memcard etc
//
//-----------------------------------------------------------
#ifndef _CUSTOMPLAYER_H_
#define _CUSTOMPLAYER_H_

class GeneralCamera;

#define CS_MAX_CHARACTERS	( 8 )

struct ts_CustomProfile
{
	u32	CheckSum;
	int	CharType;				// cop or gangster
	char	Name[64];				// name of character
	char	InitString[256];		// his info
	char	Loadout[256];			// his loadout
};

// class
namespace CustomPlayer
{
	// CustomPlayer functions
	bool		Init();
	void		Intro();
	void		Exit();
	void		SetCamera( GeneralCamera* cam );
	void		SaveActor();
	void		RestoreActor();
	void		UpdateCamera();
	void		UpdateCamera( char *name );
	void     RotateBody( float angle );
	Vector3	GetBoneInWorld( CActor &actor, const char *boneName );
	void		SetProfileName( const char* pProfileName );
	void		Show( int profile );
	int		GetVisibleProfile();
	void		SetEditProfileString( const char* initString );
	void		SetEditProfile( int Profile );
	int		GetEditProfile();
	void		EquipLoadout( const char* loadoutString );
	void		EquipWeapon( int n );
	u32		CreateCheckSum(ts_CustomProfile* Buffer);
	void		ReleaseArtResources();
};

#endif // _CUSTOMPLAYER_H_
