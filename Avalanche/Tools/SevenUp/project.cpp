#include "pch.h"
#include "propertyhandler.h"
#include "spriteproperties.h"
#include "textproperties.h"
#include "project.h"

#define LOADSAVE_VERSION1 (0x100)
#define LOADSAVE_LATEST_VERSION (LOADSAVE_VERSION1)

ProjectManager Project;

MAKE_CLASSFACTORY( Record );

const char* BlendModeToString( int BlendMode )
{
	switch( BlendMode )
	{
	case SPRITEBLEND_NORMAL:
		return "NORMAL";
		
	case SPRITEBLEND_ADD:
		return "ADD";

	case SPRITEBLEND_SUBTRACT:
		return "SUBTRACT";

	case SPRITEBLEND_MODULATE:
		return "MODULATE";

	case SPRITEBLEND_GLOSS:
		return "GLOSS";

	case SPRITEBLEND_DIFFUSE:
		return "DIFFUSE";

	case SPRITEBLEND_SATURATE:
		return "SATURATE";

	case SPRITEBLEND_DOUBLE:
		return "DOUBLE";

	case SPRITEBLEND_NEGATIVE:
		return "NEGATIVE";
	}

	return 0;
}

u8 BlendModeFromString( const char* pBlendMode )
{
	if( 0 == stricmp( pBlendMode, "SOLID" ) )
		return SPRITEBLEND_SOLID;

	if( 0 == stricmp( pBlendMode, "NORMAL" ) )
		return SPRITEBLEND_NORMAL;

	if( 0 == stricmp( pBlendMode, "ADD" ) )
		return SPRITEBLEND_ADD;

	if( 0 == stricmp( pBlendMode, "SUBTRACT" ) )
		return SPRITEBLEND_SUBTRACT;

	if( 0 == stricmp( pBlendMode, "MODULATE" ) )
		return SPRITEBLEND_MODULATE;

	if( 0 == stricmp( pBlendMode, "GLOSS" ) )
		return SPRITEBLEND_GLOSS;

	if( 0 == stricmp( pBlendMode, "DIFFUSE" ) )
		return SPRITEBLEND_DIFFUSE;

	if( 0 == stricmp( pBlendMode, "SATURATE" ) )
		return SPRITEBLEND_SATURATE;

	if( 0 == stricmp( pBlendMode, "DOUBLE" ) )
		return SPRITEBLEND_DOUBLE;

	if( 0 == stricmp( pBlendMode, "NEGATIVE" ) )
		return SPRITEBLEND_NEGATIVE;

	return SPRITEBLEND_NORMAL;
}

SpriteItem::SpriteItem( void )
{
	m_pSprite = 0;
}

SpriteItem::~SpriteItem()
{
	while( m_Records.Child() )
		delete m_Records.Child();

	return;
}

Record* SpriteItem::AddRecord( const char* pRecordType )
{
	Record* pRecord = GetRecord( pRecordType );

	if( !pRecord )
	{
		pRecord = Record::NewObject( pRecordType );

		if( pRecord )
			m_Records.AddChild( pRecord );
	}

	return pRecord;
}

Record* SpriteItem::GetRecord( const char* pRecordType )
{
	Record* pRecord = m_Records.Child();

	while( pRecord )
	{
		if( 0 == strcmp( pRecord->Classname(), pRecordType ) )
			break;

		pRecord = pRecord->Next();
	}

	return pRecord;
}

BOOL ProjectManager::Initialize( void )
{
	m_RunTimeKey = 1714;

	Record::RegisterClass( RUNTIME_CLASSOBJECT( SelectRecord ) );
	Record::RegisterClass( RUNTIME_CLASSOBJECT( FamilyRecord ) );
	Record::RegisterClass( RUNTIME_CLASSOBJECT( CopyRecord ) );

	m_PropertyHandlers.AddChild( new SpriteClassPropertyHandler );
	m_PropertyHandlers.AddChild( new TextClassPropertyHandler );

	return TRUE;
}

void ProjectManager::Shutdown( void )
{
	Record::UnregisterClass( RUNTIME_CLASSOBJECT( SelectRecord ) );
	Record::UnregisterClass( RUNTIME_CLASSOBJECT( FamilyRecord ) );
}

const char * ProjectManager::GetObjectClass( HUDOBJECTTYPE Type )
{
	const char *pClassname = 0;

	switch( Type )
	{
	case HUDOBJECT_SPRITE:
		pClassname = "Sprite";
		break;
		
	case HUDOBJECT_TEXT:
		pClassname = "Text";
		break;
	}

	return pClassname;
}

//=============================================================================
// ProjectManager::GetObjectType
//
//			Returns an enumerated id for the specified class.
//
//=============================================================================


HUDOBJECTTYPE ProjectManager::GetObjectType( const char *pClassname )
{
	if( 0 == strcmp( pClassname, "Sprite" ) )
		return HUDOBJECT_SPRITE;

	if( 0 == strcmp( pClassname, "Text" ) )
		return HUDOBJECT_TEXT;

	return HUDOBJECT_UNKNOWN;
}

//=============================================================================
// ProjectManager::GetObjectType
//
//			Returns an enumerated id that identifies the class of this object.
//
//=============================================================================

HUDOBJECTTYPE ProjectManager::GetObjectType( Sprite *pSprite )
{
	if( pSprite )
		return GetObjectType( pSprite->Classname() );

	return HUDOBJECT_UNKNOWN;
}


//=============================================================================
// ProjectManager::NewSprite
//
//			Creates a sprite of the type specified by Type and inserts it into the
//			project.  If a ProjectItem object is specified, it will be attached to
//			the sprite, or, if none is specified, one will be created and attached.
//
//=============================================================================


Sprite * ProjectManager::NewSprite( HUDOBJECTTYPE Type, ProjectItem *pProjectItem )
{
	const TCHAR *pClassname = GetObjectClass( Type );

	if( !pClassname )
		return 0;

	return NewSprite( pClassname, pProjectItem );
}

//=============================================================================
// ProjectManager::NewSprite
//
//			Creates a sprite of the type specified by pClassname and inserts it
//			into the project.  If a ProjectItem object is specified, it will be
//			attached to the sprite, or, if none is specified, one will be created
//			and attached.
//
//=============================================================================

Sprite * ProjectManager::NewSprite( const TCHAR *pClassname, ProjectItem *pProjectItem )
{
	Sprite *pSprite = (Sprite *) HudObject::NewObject( pClassname );

	if( pSprite )
	{
		if( !pProjectItem )
			CreateProjectItem( pSprite );
		else
			pSprite->m_pToolData = (void*) pProjectItem;
	}

	return pSprite;
}

//=============================================================================
// ProjectManager::DuplicateSprite
//
//			This function will make a copy of a sprite but does not duplicate its
//			children.
//			
//			DuplicateSpriteFamily() calls this function for each sprite it
//			duplicates.
//
//=============================================================================

Sprite * ProjectManager::DuplicateSprite( Sprite *pSprite )
{
	Sprite *pNewSprite = 0;

	if( pSprite && pSprite->m_pToolData )
	{
		pNewSprite = NewSprite( pSprite->Classname() );

		if( pNewSprite )
			pNewSprite->Copy( pSprite );

		ProjectItem* pDst = Project.GetProjectItem( pNewSprite );
		ProjectItem* pSrc = Project.GetProjectItem( pSprite );

		if( pDst && pSrc )
		{
			pDst->m_Name.set( pSrc->m_Name.get() );
			pDst->m_Description.set( pSrc->m_Description.get() );
		}
	}

	return pNewSprite;
}

//=============================================================================
// ProjectManager::DuplicateSpriteFamily
//					
//			This function will make a copy of a sprite and its children.
//=============================================================================


Sprite * ProjectManager::DuplicateSpriteFamily( Sprite *pSprite )
{
	Sprite *pNewSprite = DuplicateSprite( pSprite );
	Sprite *pNewChild;

	if( pNewSprite )
	{
		Sprite *pChild = (Sprite *) pSprite->Child();

		while( pChild )
		{
			pNewChild = DuplicateSpriteFamily( pChild );

			if( pNewChild )
				pNewSprite->AddChild( pNewChild );

			pChild = (Sprite *) pChild->Next();
		}
	}

	return pNewSprite;
}

//=============================================================================
// ProjectManager::DeleteSprite - Properly removes a sprite from the project.
//                              - It returns null so you can make an assignment
//                              - to the dead pointer if desired.
//=============================================================================

Sprite* ProjectManager::DeleteSprite( Sprite *pSprite )
{
	if( pSprite )
	{
		if( pSprite->m_pToolData )
		{
			delete pSprite->m_pToolData;
			pSprite->m_pToolData = 0;
		}

		delete pSprite;
	}

	return 0;
}

//=============================================================================
// ProjectManager::CreateProjectItem
//=============================================================================

ProjectItem* ProjectManager::CreateProjectItem( Sprite* pSprite )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( !pProjectItem )
		pProjectItem = new ProjectItem;

	if( pProjectItem )
	{
		pProjectItem->SetName( "Unnamed" );
		pProjectItem->SetDescription( "" );
		pProjectItem->SetId( 0 );
		pProjectItem->SetSprite( pSprite );
	}

	if( pSprite )
		pSprite->m_pToolData = (void*) pProjectItem;

	return pProjectItem;
}

//=============================================================================
// ProjectManager::GetProjectItem - Returns a pointer to the sprites
//											 - ProjectItem.
//=============================================================================

ProjectItem* ProjectManager::GetProjectItem( Sprite* pSprite )
{
	if( pSprite )
		return (ProjectItem*) pSprite->m_pToolData;

	return 0;
}

//=============================================================================
// ProjectManager::UniqueRunTimeKey
//=============================================================================

int ProjectManager::UniqueRunTimeKey( void )
{
	return m_RunTimeKey++;
}

//=============================================================================
// ProjectManager::SetName
//=============================================================================

void ProjectManager::SetName( Sprite* pSprite, const char *pName )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( pProjectItem )
		pProjectItem->SetName( pName );
	
	return;
}

//=============================================================================
// ProjectManager::SetDescription
//=============================================================================

void ProjectManager::SetDescription( Sprite* pSprite, const char* pDescription )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( pProjectItem )
		pProjectItem->SetDescription( pDescription );

	return;
}

//=============================================================================
// ProjectManager::SetId
//=============================================================================

void ProjectManager::SetId( Sprite* pSprite, int Id )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( pProjectItem )
		pProjectItem->SetId( Id );

	return;
}

//=============================================================================
// ProjectManager::Lock
//=============================================================================

void ProjectManager::Lock( Sprite* pSprite, BOOL Recursive )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( pProjectItem )
		pProjectItem->m_Item.Locked = TRUE;

	if( Recursive )
	{
		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			Lock( pChild, TRUE );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// ProjectItem::Unlock
//=============================================================================

void ProjectManager::Unlock( Sprite* pSprite, BOOL Recursive )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( pProjectItem )
		pProjectItem->m_Item.Locked = FALSE;

	if( Recursive )
	{
		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			Unlock( pChild, TRUE );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// ProjectManager::IsLocked
//=============================================================================

BOOL ProjectManager::Locked( Sprite* pSprite )
{
	ProjectItem* pProjectItem = GetProjectItem( pSprite );

	if( !pProjectItem )
		return false;

	return pProjectItem->m_Item.Locked;
}

//=============================================================================
// ProjectManager::GetProperties
//=============================================================================

BOOL ProjectManager::GetProperties( PropertyWnd &Properties, ProjectItem* pProjectItem )
{
	if( !pProjectItem )
		return FALSE;

	PropertyHandler* pPropertyHandler = GetPropertyHandler( pProjectItem->GetObjectClass() );
	PropertyRecord* pPropertyRecord;

	if( pPropertyHandler )
	{
		pPropertyRecord = pPropertyHandler->m_Properties.Child();

		while( pPropertyRecord )
		{
			if( pPropertyRecord->Child() )
				Properties.AddProperty( pPropertyRecord->m_Key.get(), 0, PROPERTYTYPE_CATAGORY, 0 );
			else
				Properties.AddProperty( pPropertyRecord->m_Key.get(), 0, PROPERTYTYPE_STRING, pPropertyRecord->ObjectId() );

			if( pPropertyRecord->Child() )
				pPropertyRecord = pPropertyRecord->Child();
			else
			if( pPropertyRecord->Next() )
				pPropertyRecord = pPropertyRecord->Next();
			else
			{
				while( pPropertyRecord && !pPropertyRecord->Next() )
					pPropertyRecord = pPropertyRecord->Parent();

				if( pPropertyRecord )
					pPropertyRecord = pPropertyRecord->Next();
			}
		}
	}

	return TRUE;
}

void ProjectManager::SetProperty( ProjectItem* p, const char* pKey, const char* pValue, int PropertyId )
{
	if( !p )
		return;

	PropertyHandler* pHandler2 = GetPropertyHandler( p->GetObjectClass() );

	if( pHandler2 )
	{
		pHandler2->SetKnownObject( p->GetSprite() );
		pHandler2->SetProperty( pKey, pValue, PropertyId );
		pHandler2->SetKnownObject( 0 );
	}

	return;
}

PropertyHandler* ProjectManager::GetPropertyHandler( const char* pObjectClass )
{
	PropertyHandler* pHandler = m_PropertyHandlers.Child();
	const char* pKnownClass;

	while( pHandler )
	{
		pKnownClass = pHandler->GetKnownClass();

		if( pKnownClass && 0 == strcmp( pKnownClass, pObjectClass ) )
			break;

		pHandler = pHandler->Next();
	}

	return pHandler;
}

//=============================================================================
// ProjectItem::ProjectItem
//=============================================================================

ProjectItem::ProjectItem( void )
{
	m_pSprite = 0;
	m_Item.Locked = FALSE;
	m_Item.Hidden = FALSE;
	m_Item.Type = HUDOBJECT_UNKNOWN;
}

//=============================================================================
// ProjectItem::~ProjectItem
//=============================================================================

ProjectItem::~ProjectItem()
{
	__asm
	{
		mov		eax, eax
		and		eax, eax
		nop
	}

	return;
}

//=============================================================================
// ProjectItem::ClearItemNames
//=============================================================================

void ProjectItem::ClearItemNames( void )
{
	while( m_ItemNames.Child() )
		delete m_ItemNames.Child();

	return;
}

//=============================================================================
// ProjectItem::GetUniqueObjectId
//=============================================================================

ItemName ProjectItem::m_ItemNames;

int ProjectItem::GetUniqueObjectId( const char *pName )
{
	ItemName *pItemName = (ItemName *) m_ItemNames.Child();

	while( pItemName )
	{
		if( 0 == pItemName->m_Name.stricmp(pName) )
			return pItemName->ObjectId();

		pItemName = (ItemName *) pItemName->Next();
	}

	ItemName *pNewItem = new ItemName;

	if( !pNewItem )
		return 0;

	pNewItem->m_Name.set( pName );
	pNewItem->m_Name.toupper();

	int NextId = 0x100;

	pItemName = (ItemName *) m_ItemNames.Child();

	if( !pItemName )
	{
		m_ItemNames.AddChild( pNewItem );
		pNewItem->SetObjectId( NextId );
	}
	else
	{
		while( pItemName )
		{
			if( pItemName->ObjectId() != NextId )
			{
				pItemName->Insert( pNewItem );
				pNewItem->SetObjectId( NextId );
				break;
			}

			NextId++;

			if( !pItemName->Next() )
			{
				pItemName->InsertAfter( pNewItem );
				pNewItem->SetObjectId( NextId );
				break;
			}

			pItemName = (ItemName *) pItemName->Next();
		}
	}

	return pNewItem->m_ObjectId;
}

//=============================================================================
// ProjectItem::SetName
//=============================================================================

void ProjectItem::SetName( const char *pName )
{
	m_Name.set( pName );
	
	SetId( GetUniqueObjectId( pName ) );
}

//=============================================================================
// ProjectItem::SetDescription
//=============================================================================

void ProjectItem::SetDescription( const char *pDescription )
{
	m_Description.set( pDescription );
}

//=============================================================================
// ProjectItem::SetId
//=============================================================================

void ProjectItem::SetId( int Id )
{
	if( m_pSprite )
		m_pSprite->SetObjectId( Id );

	return;
}

//=============================================================================
// ProjectItem::SetSprite
//=============================================================================

void ProjectItem::SetSprite( Sprite *pSprite )
{
	m_pSprite = pSprite;
	m_Item.Type = Project.GetObjectType( pSprite );
}

//=============================================================================
// ProjectItem::GetSprite
//=============================================================================

Sprite* ProjectItem::GetSprite( void )
{
	return m_pSprite;
}

//=============================================================================
// ProjectItem::GetObjectClass
//=============================================================================

const char* ProjectItem::GetObjectClass( void )
{
	Sprite* pSprite = GetSprite();

	if( pSprite )
		return pSprite->Classname();

	return 0;
}

//=============================================================================
// ProjectItem::GetSaveData
//=============================================================================

BOOL ProjectItem::SaveObject( MemoryBank &m, LPCTSTR pTexturePath, BOOL GameFormat )
{
	if( !m_pSprite )
		return FALSE;

	if( !pTexturePath )
		pTexturePath = "";

	filename TextureFile;
	char *pTextureName;
	ProjectItem *p;
	Sprite *pChild;
	int Children;
	int SanityCheck;

	Children = 0;
	pChild = m_pSprite->Child();

	while( pChild )
	{
		if( pChild->m_pToolData )
			Children++;

		pChild = pChild->Next();
	}

	if( GameFormat )
		SanityCheck = 0x3fffffff - Children;
	else
		SanityCheck = 0x4fffffff - Children;

	if( !m.Write( &SanityCheck, sizeof(SanityCheck) ) )
		return FALSE;

	if( !m.Write( &Children, sizeof(Children) ) )
		return FALSE;

	if( !m.WriteString( m_pSprite->Classname() ) )
		return FALSE;

	if( !m.WriteString( m_Name.get() ) )
		return FALSE;

	if( !GameFormat )
	{
		if( !m.WriteString( m_Description.get() ) )
			return FALSE;

		u32 Version = LOADSAVE_LATEST_VERSION;

		if( !m.Write( &Version, sizeof(Version) ) )
			return FALSE;

		if( !m.Write( &m_Item, sizeof(m_Item) ) )
			return FALSE;
	}

	pTextureName = 0;

	if( GameFormat )
	{
		TCHAR *pFileName = dblGetFileName( m_pSprite->m_state.textureset.handle );

		if( pFileName )
		{
			filename tmp;
			tmp.getfilename( pFileName );
			tmp.delext();

			if( tstrlen(pTexturePath) > 0)
				TextureFile.set( Printf( "%s\\%s.dbl", pTexturePath, tmp.get() ) );
			else
				TextureFile.set( Printf( "%s.dbl", tmp.get() ) );

			pTextureName = TextureFile.get();
		}
	}

	if( !m_pSprite->SaveObject( m, pTextureName ) )
		return FALSE;

	pChild = m_pSprite->Child();

	while( pChild )
	{
		p = (ProjectItem *) pChild->m_pToolData;

		if( p )
		{
			if( !p->SaveObject( m, pTexturePath, GameFormat ) )
				return FALSE;
		}

		pChild = pChild->Next();
	}

	return TRUE;
}

//=============================================================================
// ProjectItem::LoadObject
//=============================================================================

BOOL ProjectItem::LoadObject( MemoryBank &m, Sprite *pParent )
{
	xstring classname;
	xstring name;
	xstring description;

	int SanityCheck;
	int Children;
	u32 Version = 0;

	if( !m.Read( &SanityCheck, sizeof(SanityCheck) ) )
		return FALSE;

	if( !m.Read( &Children, sizeof(Children) ) )
		return FALSE;

	if( SanityCheck != 0x4fffffff - Children )
		if( SanityCheck != (0x3fffffff - Children) )
			return FALSE;

	if( !m.ReadString( classname ) )
		return FALSE;

	if( !m.ReadString( name ) )
		return FALSE;

	if( !m.ReadString( description ) )
		return FALSE;

	if( SanityCheck == 0x3fffffff - Children )
	{
		if( !m.Read( &m_Item.Locked, sizeof(m_Item.Locked) ) )
			return FALSE;

		if( !m.Read( &m_Item.Type, sizeof(m_Item.Type) ) )
			return FALSE;

		m_Item.Hidden = FALSE;
	}
	else
	if( SanityCheck == 0x4fffffff - Children )
	{
		if( !m.Read( &Version, sizeof(Version) ) )
			return FALSE;

		if( !m.Read( &m_Item, sizeof(m_Item) ) )
			return FALSE;
	}

	m_pSprite = Project.NewSprite( classname.get(), this );

	if( !m_pSprite )
		return FALSE;

	if( !m_pSprite->LoadObject( m, 0 ) )
		return FALSE;

	SetName( name.get() );
	SetDescription( description.get() );

	if( pParent )
		pParent->AddChild( m_pSprite );

	/* FIX BAD OFFSET HACK
	if( pParent && 0 == strcmp( pParent->Classname(), "Text" ) )
	{
		TextOutput to;
		Text* pText = (Text*) pParent;
		pText->ComputeTextOutput( &to );

		float x = to.Width  * pParent->m_state.origin.x;
		float y = to.Height * pParent->m_state.origin.y;

		m_pSprite->Move( x, y );
	}
	*/

	ProjectItem *p;

	for( int i = 0; i < Children; i++ )
	{
		p = new ProjectItem;

		if( !p )
			return FALSE;

		if( !p->LoadObject( m, m_pSprite ) )
			return FALSE;
	}

	return TRUE;
}

//=============================================================================
// ProjectItem::GetProperties
//=============================================================================

int clamp( int i, int min, int max )
{
	if( min > max )
		return clamp( i, max, min );

	if( i > max )
		return max;

	if( i < min )
		return min;

	return i;
}

float fclamp( float f, float min, float max )
{
	if( min > max )
		return fclamp( f, max, min );

	if( f > max )
		return max;

	if( f < min )
		return min;

	return f;
}

void ProjectItem::SetProperty( int PropertyCode, const char* pPropertyString )
{
	if( m_pSprite )
	{
		tstring LowerCase;
		Sprite &s = *m_pSprite;
		Text *pText = 0;
		
		if( tstrcmp( s.Classname(), "Text" ) )
			pText = (Text *) &s;

		int Input = atoi( pPropertyString );
		float Inputf = (float) atof( pPropertyString );
		
		bool Inputb = false;

		if( 0 == stricmp( pPropertyString, "true" ) )
			Inputb = true;

		if( Input == 1 )
			Inputb = true;

		int alpha = 0;
		int red = 0;
		int green = 0;
		int blue = 0;

		sscanf( pPropertyString, "%3d %3d %3d %3d", &alpha, &red, &green, &blue );

		alpha = clamp( alpha, 0, 255 );
		red   = clamp( red, 0, 255 );
		green = clamp( green, 0, 255 );
		blue  = clamp( blue, 0, 255 );



		float tu = 0.0f;
		float tv = 0.0f;

		sscanf( pPropertyString, "%f %f", &tu, &tv );

		switch( PropertyCode )
		{
		case PROPERTYCODE_NAME:
			SetName( pPropertyString );
			break;
		
		case PROPERTYCODE_DESCRIPTION:
			m_Description.set( pPropertyString );
			break;

		case PROPERTYCODE_SPRITE_POSITIONX:
			s.m_state.position.x = Inputf;
			break;
			
		case PROPERTYCODE_SPRITE_POSITIONY:
			s.m_state.position.y = Inputf;
			break;

		case PROPERTYCODE_SPRITE_WIDTH:
			s.m_state.size.x = Inputf;
			break;

		case PROPERTYCODE_SPRITE_HEIGHT:
			s.m_state.size.y = Inputf;
			break;

		case PROPERTYCODE_SPRITE_SCALE:
			s.SetScale( tu, tv );
			break;
			
		case PROPERTYCODE_SPRITE_ORIGIN:
			s.SetOrigin( tu, tv );
			break;

		case PROPERTYCODE_SPRITE_ROTATION:
			s.m_state.rotation.z = Inputf;
			break;

		case PROPERTYCODE_SPRITE_VISIBLE:
			break;

		case PROPERTYCODE_SPRITE_PRIORITY:
			s.SetPriority( Input );
			break;

		case PROPERTYCODE_SPRITE_BLENDMODE:
			s.SetBlendMode( BlendModeFromString( pPropertyString ) );
			break;
	
		case PROPERTYCODE_COLOR_BASECOLOR:
			s.SetBaseColor( red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_COLOR_FAMILYCOLOR:
			s.SetFamilyColor( red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_COLOR_VERTEXCOLOR1:
			s.SetVertexColor( 0, red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_COLOR_VERTEXCOLOR2:
			s.SetVertexColor( 1, red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_COLOR_VERTEXCOLOR3:
			s.SetVertexColor( 2, red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_COLOR_VERTEXCOLOR4:
			s.SetVertexColor( 3, red, green, blue, alpha );
			break;
	
		case PROPERTYCODE_VERTEX_TEXCOORD1:
			s.SetTextureCoordinates( 0, tu, tv );
			break;
	
		case PROPERTYCODE_VERTEX_TEXCOORD2:
			s.SetTextureCoordinates( 1, tu, tv );
			break;
	
		case PROPERTYCODE_VERTEX_TEXCOORD3:
			s.SetTextureCoordinates( 2, tu, tv );
			break;
	
		case PROPERTYCODE_VERTEX_TEXCOORD4:
			s.SetTextureCoordinates( 3, tu, tv );
			break;
	
		case PROPERTYCODE_TEXTURE_PATH:
			break;
	
		case PROPERTYCODE_TEXTURE_NAME:
			break;
	
		case PROPERTYCODE_TEXTURE_WIDTH:
			break;
	
		case PROPERTYCODE_TEXTURE_HEIGHT:
			break;
	
		case PROPERTYCODE_TEXTURE_FORMAT:
			break;
	
		case PROPERTYCODE_TEXTURE_CELL:
			s.m_state.textureset.cell = Input;
			break;
	
		case PROPERTYCODE_TEXTURE_ROWS:
			s.m_state.textureset.rows = Input;
			break;
	
		case PROPERTYCODE_TEXTURE_COLUMNS:
			s.m_state.textureset.cols = Input;
			break;

		case PROPERTYCODE_SHADOW_ENABLED:
			s.m_state.shadow.enabled = Inputb;
			break;

		case PROPERTYCODE_SHADOW_INTENSITY:
			s.m_state.shadow.intensity = Input;
			break;

		case PROPERTYCODE_SHADOW_OFFSET:
			s.m_state.shadow.x = (u8) tu;
			s.m_state.shadow.y = (u8) tv;
			break;
	
		case PROPERTYCODE_TEXT:
			if( pText )
				pText->SetText( pPropertyString );
			break;
	
		case PROPERTYCODE_FONT_PATH:
			break;
	
		case PROPERTYCODE_FONT_NAME:
			break;
	
		case PROPERTYCODE_FONT_HEIGHT:
			if( pText )
			{
				// HACK: make sure the sprite height and text height stay synced
				if( pText->GetTextHeight() == pText->GetHeight() )
					pText->SetHeight( Inputf );

				pText->SetTextHeight( Inputf );
			}
			break;

		case PROPERTYCODE_TEXT_SPACING:
			if( pText )
				pText->SetTextSpacing( Inputf );
			break;
	
		case PROPERTYCODE_TEXT_LINESPACING:
			if( pText )
				pText->SetLineSpacing( Inputf );
			break;
	
		case PROPERTYCODE_TEXT_ALIGNMENT:
			if( pText )
				pText->SetTextAlignment( Inputf );
			break;
	
		case PROPERTYCODE_TEXT_WORDWRAP:
			if( pText )
				pText->SetWordWrap( Inputb );
			break;

		case PROPERTYCODE_TEXT_TRANSLATE:
			if( pText )
				pText->SetTranslator( Inputb );
			break;
	
		case PROPERTYCODE_TEXTEFFECT_ENABLE:
			if( pText )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_TESSELATION:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_HORZOFFSET:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_VERTOFFSET:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_AMPLITUDE:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_FREQUENCY:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;
	
		case PROPERTYCODE_TEXTEFFECT_PHASE:
			if( pText && pText->GetFirstEffect() )
			{
			}
			break;

		case PROPERTYCODE_SCREEN_HANDLER:
			{
				Screen* pScreen = (Screen*) s.GetInterface( Screen::ClassnameStatic() );

				if( pScreen )
					pScreen->SetScreenHandlerClass( pPropertyString );
			}
		}
	}

	return;
}

void ProjectItem::GetProperties( PropertyWnd &p )
{
	if( m_pSprite )
	{
		Sprite &s = *m_pSprite;
		Text *pText = 0;

		TCHAR *pClassname = s.Classname();

		p.Clear();
		p.SetUserParam( m_pSprite );

		Screen* pScreen = (Screen*) s.GetInterface( Screen::ClassnameStatic() );

		p.AddCatagory( "Properties" );

		if( tstricmp( pClassname, Text::ClassnameStatic() ) )
			pText = (Text *) &s;

		if( pText )
			p.AddProperty( "Text", 0, PROPERTYTYPE_STRING, PROPERTYCODE_TEXT, 0, FALSE );

		p.AddProperty( "Name", 0, PROPERTYTYPE_STRING, PROPERTYCODE_NAME, 1, FALSE );

		if( pScreen )
		{
			p.AddProperty( "Screen Handler", 0, PROPERTYTYPE_STRING, PROPERTYCODE_SCREEN_HANDLER, 1, FALSE );
			p.SetPropertyString( PROPERTYCODE_SCREEN_HANDLER, pScreen->GetScreenHandlerClass() );
		}

		p.AddProperty( "Description", 0, PROPERTYTYPE_STRING, PROPERTYCODE_DESCRIPTION, 0, FALSE );
		p.AddCatagory( "Sprite" );
		p.AddProperty( "Position X", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_POSITIONX, 1, FALSE );
		p.AddProperty( "Position Y", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_POSITIONY, 1, FALSE );
		p.AddProperty( "Width", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_WIDTH, 1, FALSE );
		p.AddProperty( "Height", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_HEIGHT, 1, FALSE );
		p.AddProperty( "Scale", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_SCALE, 2, FALSE );
		p.AddProperty( "Origin", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_ORIGIN, 2, FALSE );
		p.AddProperty( "Rotation", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SPRITE_ROTATION, 1, FALSE );
		p.AddProperty( "Visible", 0, PROPERTYTYPE_BOOL, PROPERTYCODE_SPRITE_VISIBLE, 1, FALSE );
		p.AddProperty( "Priority", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_SPRITE_PRIORITY, 1, FALSE );
		p.AddProperty( "Blend Mode", 0, PROPERTYTYPE_STRING, PROPERTYCODE_SPRITE_BLENDMODE, 1, FALSE );
		p.AddCatagory( "Color" );
		p.AddProperty( "Base Color", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_BASECOLOR, 4, FALSE );
		p.AddProperty( "Family Color", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_FAMILYCOLOR, 4, FALSE );
		p.AddProperty( "Vertex 1", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_VERTEXCOLOR1, 4, FALSE );
		p.AddProperty( "Vertex 2", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_VERTEXCOLOR2, 4, FALSE );
		p.AddProperty( "Vertex 3", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_VERTEXCOLOR3, 4, FALSE );
		p.AddProperty( "Vertex 4", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_COLOR_VERTEXCOLOR4, 4, FALSE );
		p.AddCatagory( "Texture" );
		p.AddProperty( "Path", 0, PROPERTYTYPE_STRING, PROPERTYCODE_TEXTURE_PATH, 0, TRUE );
		p.AddProperty( "Name", 0, PROPERTYTYPE_STRING, PROPERTYCODE_TEXTURE_NAME, 0, TRUE );
		p.AddProperty( "Width", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_TEXTURE_WIDTH, 1, TRUE );
		p.AddProperty( "Height", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_TEXTURE_HEIGHT, 1, TRUE );
		p.AddProperty( "Format", 0, PROPERTYTYPE_STRING, PROPERTYCODE_TEXTURE_FORMAT, 1, TRUE );
		p.AddProperty( "Cell", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_TEXTURE_CELL, 1, FALSE );
		p.AddProperty( "Rows", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_TEXTURE_ROWS, 1, FALSE );
		p.AddProperty( "Columns", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_TEXTURE_COLUMNS, 1, FALSE );
		p.AddCatagory( "Texture Coordinates" );
		p.AddProperty( "Vertex 1", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_VERTEX_TEXCOORD1, 4, FALSE );
		p.AddProperty( "Vertex 2", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_VERTEX_TEXCOORD2, 4, FALSE );
		p.AddProperty( "Vertex 3", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_VERTEX_TEXCOORD3, 4, FALSE );
		p.AddProperty( "Vertex 4", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_VERTEX_TEXCOORD4, 4, FALSE );
		p.AddCatagory( "Drop Shadow" );
		p.AddProperty( "Enabled", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_SHADOW_ENABLED, 1, FALSE );
		p.AddProperty( "Intensity", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_SHADOW_INTENSITY, 1, FALSE );
		p.AddProperty( "Offset", 0, PROPERTYTYPE_INTEGER, PROPERTYCODE_SHADOW_OFFSET, 1, FALSE );

	//	p.Collapse( "Color" );
	//	p.Collapse( "Texture" );
	//	p.Collapse( "Texture Coordinates" );

		filename TexturePath;
		filename TextureName;

		TexturePath.filepath( dblGetFileName( s.m_state.textureset.handle ) );
		TextureName.getfilename( dblGetFileName( s.m_state.textureset.handle ) );

		p.SetPropertyString( PROPERTYCODE_NAME, m_Name.get() );
		p.SetPropertyString( PROPERTYCODE_DESCRIPTION, m_Description.get() );
		p.SetPropertyString( PROPERTYCODE_SPRITE_POSITIONX, Printf( "%0.5f", s.m_state.position.x ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_POSITIONY, Printf( "%0.5f", s.m_state.position.y ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_WIDTH, Printf( "%0.5f", s.m_state.size.x ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_HEIGHT, Printf( "%0.5f", s.m_state.size.y ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_SCALE, Printf( "%0.5f %0.5f", s.m_state.scale.x, s.m_state.scale.y ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_ORIGIN, Printf( "%0.5f %0.5f", s.m_state.origin.x, s.m_state.origin.y ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_ROTATION, Printf( "%0.5f", s.m_state.rotation.z ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_VISIBLE, TrueOrFalse( s.m_state.sprite.visible == 1 ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_PRIORITY, Printf( "%d", s.m_state.sprite.priority ) );
		p.SetPropertyString( PROPERTYCODE_SPRITE_BLENDMODE, BlendModeToString( s.GetBlendMode() ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_BASECOLOR, Printf( "%d %d %d %d", s.m_state.color.a, s.m_state.color.r, s.m_state.color.g, s.m_state.color.b ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_FAMILYCOLOR, Printf( "%d %d %d %d", s.m_state.familycolor.a, s.m_state.familycolor.r, s.m_state.familycolor.g, s.m_state.familycolor.b ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_VERTEXCOLOR1, Printf( "%d %d %d %d", s.m_state.vertexcolor[0].a, s.m_state.vertexcolor[0].r, s.m_state.vertexcolor[0].g, s.m_state.vertexcolor[0].b ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_VERTEXCOLOR2, Printf( "%d %d %d %d", s.m_state.vertexcolor[1].a, s.m_state.vertexcolor[1].r, s.m_state.vertexcolor[1].g, s.m_state.vertexcolor[1].b ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_VERTEXCOLOR3, Printf( "%d %d %d %d", s.m_state.vertexcolor[2].a, s.m_state.vertexcolor[2].r, s.m_state.vertexcolor[2].g, s.m_state.vertexcolor[2].b ) );
		p.SetPropertyString( PROPERTYCODE_COLOR_VERTEXCOLOR4, Printf( "%d %d %d %d", s.m_state.vertexcolor[3].a, s.m_state.vertexcolor[3].r, s.m_state.vertexcolor[3].g, s.m_state.vertexcolor[3].b ) );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_PATH, TexturePath.get() );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_NAME, TextureName.get() );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_WIDTH, Printf( "%d", s.m_state.textureset.sizex ) );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_HEIGHT, Printf( "%d", s.m_state.textureset.sizey ) );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_FORMAT, "Not Available" );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_CELL, Printf( "%d", (int) s.m_state.textureset.cell ) );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_ROWS, Printf( "%d", (int) s.m_state.textureset.rows ) );
		p.SetPropertyString( PROPERTYCODE_TEXTURE_COLUMNS, Printf( "%d", (int) s.m_state.textureset.cols ) );
		p.SetPropertyString( PROPERTYCODE_VERTEX_TEXCOORD1, Printf( "%0.5f, %0.5f", s.m_state.texcoords[0].x, s.m_state.texcoords[0].y ) );
		p.SetPropertyString( PROPERTYCODE_VERTEX_TEXCOORD2, Printf( "%0.5f, %0.5f", s.m_state.texcoords[1].x, s.m_state.texcoords[1].y ) );
		p.SetPropertyString( PROPERTYCODE_VERTEX_TEXCOORD3, Printf( "%0.5f, %0.5f", s.m_state.texcoords[2].x, s.m_state.texcoords[2].y ) );
		p.SetPropertyString( PROPERTYCODE_VERTEX_TEXCOORD4, Printf( "%0.5f, %0.5f", s.m_state.texcoords[3].x, s.m_state.texcoords[3].y ) );
		p.SetPropertyString( PROPERTYCODE_SHADOW_ENABLED, TrueOrFalse( s.m_state.shadow.enabled == 1 ) );
		p.SetPropertyString(	PROPERTYCODE_SHADOW_INTENSITY, Printf( "%d", s.m_state.shadow.intensity ) );
		p.SetPropertyString( PROPERTYCODE_SHADOW_OFFSET, Printf( "%d %d", s.m_state.shadow.x, s.m_state.shadow.y ) );

		if( pText )
		{
			p.AddCatagory( "Text Appearance" );
			p.AddProperty( "Font Name", 0, PROPERTYTYPE_STRING, PROPERTYCODE_FONT_NAME, 0, TRUE );
			p.AddProperty( "Font Height", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_FONT_HEIGHT, 1, FALSE );
			p.AddProperty( "Spacing", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXT_SPACING, 1, FALSE );
			p.AddProperty( "Line Spacing", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXT_LINESPACING, 1, FALSE );
			p.AddProperty( "Alignment", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXT_ALIGNMENT, 1, FALSE );
			p.AddProperty( "Word Wrap", 0, PROPERTYTYPE_BOOL, PROPERTYCODE_TEXT_WORDWRAP, 1, FALSE );
			p.AddProperty( "Translate", 0, PROPERTYTYPE_BOOL, PROPERTYCODE_TEXT_TRANSLATE, 1, FALSE );
			p.AddCatagory( "Text Effects" );
			p.AddProperty( "Enable", 0, PROPERTYTYPE_BOOL, PROPERTYCODE_TEXTEFFECT_ENABLE, 1, FALSE );
			p.AddProperty( "Horz. Offset", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXTEFFECT_HORZOFFSET, 1, TRUE );
			p.AddProperty( "Vert. Offset", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXTEFFECT_VERTOFFSET, 1, TRUE ); 
			p.AddProperty( "Amplitude", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXTEFFECT_AMPLITUDE, 1, TRUE );
			p.AddProperty( "Frequency", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXTEFFECT_FREQUENCY, 1, TRUE );
			p.AddProperty( "Phase", 0, PROPERTYTYPE_FLOAT, PROPERTYCODE_TEXTEFFECT_PHASE, 1, TRUE );

			p.SetPropertyString( PROPERTYCODE_FONT_NAME, pText->GetFont()->m_FontName.get() );
			p.SetPropertyString( PROPERTYCODE_TEXT, pText->GetText() );
			p.SetPropertyString( PROPERTYCODE_FONT_HEIGHT, Printf( "%0.5f", pText->GetTextHeight() ) );
			p.SetPropertyString( PROPERTYCODE_TEXT_SPACING, Printf( "%0.5f", pText->GetTextSpacing() ) );
			p.SetPropertyString( PROPERTYCODE_TEXT_LINESPACING, Printf( "%0.5f", pText->GetLineSpacing() ) );
			p.SetPropertyString( PROPERTYCODE_TEXT_ALIGNMENT, Printf( "%0.5f", pText->GetTextAlignment() ) );
			p.SetPropertyString( PROPERTYCODE_TEXT_WORDWRAP, TrueOrFalse( pText->GetWordWrap() ) );
			p.SetPropertyString( PROPERTYCODE_TEXT_TRANSLATE, TrueOrFalse( pText->GetTranslator() ) );

			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_ENABLE, TrueOrFalse(false) );
			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_HORZOFFSET, "0.00000" );
			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_VERTOFFSET, "0.00000" );
			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_AMPLITUDE, "0.00000" );
			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_FREQUENCY, "0.00000" );
			p.SetPropertyString( PROPERTYCODE_TEXTEFFECT_PHASE, "0.00000" );
		}
	}

	return;
}

//=============================================================================
// ProjectItem::SetProperties
//=============================================================================

void ProjectItem::SetProperties( PropertyWnd &Properties )
{
}