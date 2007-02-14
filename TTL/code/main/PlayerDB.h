//============================================================================
//=
//= PlayerDB.h - Keeps track of all the player art (pieces and textures)
//=              Database is built from a file in the data tree once at load
//=              time.
//=
//============================================================================

#ifndef _PLAYERDB_H
#define _PLAYERDB_H

//============================================================================

#define PLAYERDB_COP		1
#define PLAYERDB_GANG	2

#define PLAYERDB_CUSTOM	0x8000

//============================================================================

namespace PlayerDB
{
	// database
	void	Load( const char* filename );
	int	GetVersionID();

	// texture access
	char*	GetTextureFilename( int id );
	char* GetTexturePrefix( int id );

	// piece access
	char* GetPieceFilename( int id );
	char*	GetPieceSlot( int id );
	bool  PieceHasLores( int id );
	int	PieceLODReplacement( int id );

	// character access
	int	NumCharacters();
	char* CharacterInitString( int index );
	char*	CharacterName( int index );
	char* CharacterDisplayName( int index );
	int	CharacterType( int index );
	int	CharacterIndexFromName( const char* name );
	int	CharacterIndexFromId( int id );
	int	CharacterId( int index );
	bool	CharacterIsCustom( int id );
	int   ChangeCharacter( int index, int change, int typeMask );
	void	SetCharacter( int index, const char* name, const char* initString, int type, bool selectable );
	void  SetCharacterName( int index, const char* name );

	// menu access
	char*	MenuName( int menuId );
	int	MenuParent( int menuId );
	int	MenuNumChildren( int menuId );
	int	MenuChild( int menuId, int childIndex );
	int	MenuNumItems( int menuId );
	int	MenuItem( int menuId, int itemIndex );
	int	MenuNumUnlockableItems( int menuId );

	// item access
	char* ItemName( int id );

	// composer
	void	ComposerInit( int type, const char* initString );
	bool	IsComposerInited();
	void	ComposerShutdown();
	char*	ComposerGetInitString();
	int	ComposerMenu();
	void	ComposerSetMenu( const char* name );
	void	ComposerSetMenu( int id );
	void	ComposerSetColorshift(int h, int s, int v );
	bool	ComposerGetColorshift( int* h, int* s, int* v );

	int	ComposerSelected();
	void	ComposerSelect( int index );
	int	ComposerGetIDByName( const char* name );
};

//============================================================================

#endif //_PLAYERDB_H
