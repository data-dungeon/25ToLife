#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <hud/hudlib.h>
#include "propertywnd.h"
#include "propertyhandler.h"

typedef enum
{
	HUDOBJECT_UNKNOWN,
	HUDOBJECT_SPRITE,
	HUDOBJECT_TEXT,
}
HUDOBJECTTYPE;

enum
{
	PROPERTYCODE_NAME,
	PROPERTYCODE_DESCRIPTION,
	PROPERTYCODE_SPRITE_POSITIONX,
	PROPERTYCODE_SPRITE_POSITIONY,
	PROPERTYCODE_SPRITE_POSITIONZ,
	PROPERTYCODE_SPRITE_WIDTH,
	PROPERTYCODE_SPRITE_HEIGHT,
	PROPERTYCODE_SPRITE_SCALE,
	PROPERTYCODE_SPRITE_ORIGIN,
	PROPERTYCODE_SPRITE_ROTATION,
	PROPERTYCODE_SPRITE_VISIBLE,
	PROPERTYCODE_SPRITE_PRIORITY,
	PROPERTYCODE_SPRITE_BLENDMODE,
	PROPERTYCODE_COLOR_BASECOLOR,
	PROPERTYCODE_COLOR_FAMILYCOLOR,
	PROPERTYCODE_COLOR_VERTEXCOLOR1,
	PROPERTYCODE_COLOR_VERTEXCOLOR2,
	PROPERTYCODE_COLOR_VERTEXCOLOR3,
	PROPERTYCODE_COLOR_VERTEXCOLOR4,
	PROPERTYCODE_VERTEX_TEXCOORD1,
	PROPERTYCODE_VERTEX_TEXCOORD2,
	PROPERTYCODE_VERTEX_TEXCOORD3,
	PROPERTYCODE_VERTEX_TEXCOORD4,
	PROPERTYCODE_TEXTURE_PATH,
	PROPERTYCODE_TEXTURE_NAME,
	PROPERTYCODE_TEXTURE_WIDTH,
	PROPERTYCODE_TEXTURE_HEIGHT,
	PROPERTYCODE_TEXTURE_FORMAT,
	PROPERTYCODE_TEXTURE_CELL,
	PROPERTYCODE_TEXTURE_ROWS,
	PROPERTYCODE_TEXTURE_COLUMNS,
	PROPERTYCODE_SHADOW_ENABLED,
	PROPERTYCODE_SHADOW_INTENSITY,
	PROPERTYCODE_SHADOW_OFFSET,
	PROPERTYCODE_TEXT,
	PROPERTYCODE_FONT_PATH,
	PROPERTYCODE_FONT_NAME,
	PROPERTYCODE_FONT_HEIGHT,
	PROPERTYCODE_TEXT_SPACING,
	PROPERTYCODE_TEXT_ALIGNMENT,
	PROPERTYCODE_TEXT_LINESPACING,
	PROPERTYCODE_TEXT_WORDWRAP,
	PROPERTYCODE_TEXT_TRANSLATE,
	PROPERTYCODE_TEXTEFFECT_ENABLE,
	PROPERTYCODE_TEXTEFFECT_TESSELATION,
	PROPERTYCODE_TEXTEFFECT_HORZOFFSET,
	PROPERTYCODE_TEXTEFFECT_VERTOFFSET,
	PROPERTYCODE_TEXTEFFECT_AMPLITUDE,
	PROPERTYCODE_TEXTEFFECT_FREQUENCY,
	PROPERTYCODE_TEXTEFFECT_PHASE,
	PROPERTYCODE_SCREEN_HANDLER,
};

typedef struct
{
	BOOL Locked;
	BOOL Hidden;
	HUDOBJECTTYPE Type;
}
PROJECTITEMSTRUCT;

class ItemName : public CTreeNode
{
	public:
		tstring m_Name;
};

class Record : public CTreeNode
{
	DECLARE_CLASSOBJECT( Record );
	DECLARE_CLASSFACTORY( Record );

	inline Record* Parent( void ) { return (Record*) CTreeNode::Parent(); }
	inline Record* Child( void )  { return (Record*) CTreeNode::Child(); }
	inline Record* Prev( void )   { return (Record*) CTreeNode::Prev(); }
	inline Record* Next( void )   { return (Record*) CTreeNode::Next(); }
};

class SelectRecord : public Record
{
	DECLARE_CLASSOBJECT( SelectRecord );

	public:
		float m_MousePosX;
		float m_MousePosY;
		SPRITESTATESTRUCT m_SpriteState;
};

class FamilyRecord : public Record
{
	DECLARE_CLASSOBJECT( FamilyRecord );

	public:
		Sprite* m_pSprite;
		Sprite* m_pParent;
		Sprite* m_pChild;
		Sprite* m_pPrev;
		Sprite* m_pNext;
};

class CopyRecord : public Record
{
	DECLARE_CLASSOBJECT( CopyRecord );

	public:
		float m_ScreenPosX;
		float m_ScreenPosY;
};

class SpriteItem : public CTreeNode
{
	DECLARE_CTREENODE( SpriteItem );

	public:
		SpriteItem( void );
		virtual ~SpriteItem();

		Record* AddRecord( const char* pRecordType );
		Record* GetRecord( const char* pRecordType );

		Sprite* m_pSprite;
		Record m_Records;
};

class ProjectItem : public SpriteItem
{
	DECLARE_CTREENODE( ProjectItem );
	DECLARE_CLASSOBJECT( ProjectItem );

	public:
		ProjectItem( void );
		virtual ~ProjectItem();

		static int GetUniqueObjectId( const char *pName );
		static void ClearItemNames( void );

		void SetName( const char *pName );
		void SetDescription( const char *pDescription );
		void SetId( int Id );
		void SetSprite( Sprite *pSprite );
		Sprite *GetSprite( void );
		const char* GetObjectClass( void );

		//////////////////////////////////////////////////////////////////////////
		
		virtual BOOL SaveObject( MemoryBank &m, LPCTSTR pTexturePath, BOOL GameFormat );
		virtual BOOL LoadObject( MemoryBank &m, Sprite *pParent );

		//////////////////////////////////////////////////////////////////////////

		virtual void SetProperty( int PropertyCode, const char *pPropertyString );
		virtual void SetProperties( PropertyWnd &Properties );
		virtual void GetProperties( PropertyWnd &Properties );

	public:
		static ItemName m_ItemNames;
		tstring m_Name;
		tstring m_Description;
		PROJECTITEMSTRUCT m_Item;
};

class ProjectManager
{
	public:
		BOOL Initialize( void );
		void Shutdown( void );
		const char* GetObjectClass( HUDOBJECTTYPE Type );
		HUDOBJECTTYPE GetObjectType( const char* pClassname );
		HUDOBJECTTYPE GetObjectType( Sprite* pSprite );
		Sprite* NewSprite( HUDOBJECTTYPE Type, ProjectItem* pProjectItem = 0 );
		Sprite* NewSprite( const TCHAR* pClassname, ProjectItem* pProjectItem = 0 );
		Sprite* DuplicateSprite( Sprite* pSprite );
		Sprite* DuplicateSpriteFamily( Sprite* pSprite );
		Sprite* DeleteSprite( Sprite* pSprite );
		ProjectItem* CreateProjectItem( Sprite* pSprite );
		ProjectItem* GetProjectItem( Sprite* pSprite );
		int UniqueRunTimeKey( void );
		
		//////////////////////////////////////////////////////////////////////////
		
		void SetName( Sprite* pSprite, const char*pName );
		void SetDescription( Sprite* pSprite, const char* pDescription );
		void SetId( Sprite* pSprite, int Id );
		void Lock( Sprite* pSprite, BOOL Recursive = FALSE );
		void Unlock( Sprite* pSprite, BOOL Recursive = FALSE );
		BOOL Locked( Sprite* pSprite );

		//////////////////////////////////////////////////////////////////////////

		BOOL GetProperties( PropertyWnd& Properties, ProjectItem* pProjectItem );
		void SetProperty( ProjectItem* p, const char* pKey, const char* pValue, int PropertyId );
		PropertyHandler* GetPropertyHandler( const char* pObjectClass );

		PropertyHandler m_PropertyHandlers;

private:
		int m_RunTimeKey;
};

extern ProjectManager Project;

#endif // _PROJECT_H_