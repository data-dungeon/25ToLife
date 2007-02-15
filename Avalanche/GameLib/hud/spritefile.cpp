#include "EngineHelper/filestream.h"
#include "hud/spritefile.h"

#define MAKEID(a,b,c,d) ((char)(a)<<24|(char)(b)<<16|(char)(c)|(char)(d))

#define SPRITEFILEHEADERTYPE MAKEID('S','P','R','T')
#define SPRITEFILEHEADERVERSION (1)

Sprite *Sprite__LoadFile( char *pFileName )
{
	return 0;
}

bool Sprite__WriteFile( FileStream &File, Sprite *pSprite )
{
	if( !pSprite )
		return false;

	int Top = File.Tell();

	SPRITEFILEHEADER FileHeader;

	FileHeader.Type = SPRITEFILEHEADERTYPE;
	FileHeader.Version = SPRITEFILEHEADERVERSION;
	FileHeader.Size = 0;
	FileHeader.Reserved0 = 0;
	FileHeader.Reserved1 = 0;

	if( !File.Write( &FileHeader, sizeof(FileHeader) ) )
		return false;

	SPRITEFILESTRUCT FileStruct;

	Sprite *p = pSprite;

	while( p )
	{
		FileStruct.State = p->m_state;
		FileStruct.NumChildren = p->GetNumChildren();

		if( !File.Write( &FileStruct, sizeof(FileStruct) ) )
			return false;

		if( p->m_child )
			p = (Sprite *) p->m_child;
		else if( p->m_next )
			p = (Sprite *) p->m_next;
		else if( p->m_parent && p->m_parent->m_next && p != pSprite )
			p = (Sprite *) p->m_parent->m_next;
		else
			p = 0;
	}

	int Bottom = File.Tell();

	FileHeader.Size = File.Size();

	File.Seek( Top );
	File.Write( &FileHeader, sizeof(FileHeader) );
	File.Seek( Bottom );

	return true;
}

bool Sprite__SaveFile( char *pFileName, Sprite *pSprite )
{
	if( !pFileName || !pSprite )
		return false;

	FileStream File;

	if( !File.Open( pFileName, FileStream::WRITE ) )
		return false;

	int Begin = File.Tell();

	SPRITEFILEHEADER FileHeader;

	FileHeader.Type = SPRITEFILEHEADERTYPE;
	FileHeader.Version = SPRITEFILEHEADERVERSION;
	FileHeader.Size = 0;
	FileHeader.Reserved0 = 0;
	FileHeader.Reserved1 = 0;

	if( !File.Write( &FileHeader, sizeof(FileHeader) ) )
		return false;

	return Sprite__WriteFile( File, pSprite );
}