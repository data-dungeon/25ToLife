//============================================================================
//=
//= PlayerDB.cpp - Keeps track of all the player art (pieces and textures)
//=                Database is built from a file in the data tree once at load
//=                time.
//=
//============================================================================

#include "TTLPCH.h"
#include "PlayerDB.h"
#include "Tupperware/memorymanager.h"
#include "Tupperware/Aggregate.h"
#include "Tupperware/Scalar.h"
#include "Tupperware/loadsave.h"
#include "Tupperware/pool.h"
#include "TupperAttrib/AccessRoot.h"
#include "TupperAttrib/AccessInstance.h"
#include "TupperAttrib/AccessPath.h"
#include "TupperAttrib/ConfigRoot.h"
#include "TupperAttrib/KeyValue.h"
#include "TupperAttrib/KeyValueContext.h"
#include "frontend/customplayer.h"
#include "GameHelper/GameLocks.h"

//============================================================================

namespace PlayerDB
{
	class DBPrefix;
	class DBLock;
	class DBTexture;
	class DBPiece;
	class DBPrebuilt;
	class DBMenu;
	class DBItem;
	class DBSlot;
	class DBExclude;
	class DBComposer;

	// local private functions
	void Dump();
	void DumpMenus( DBMenu* root, int level );
	void ImportVersion( TupperwareAggregate* tupDatabase );
	void ImportPrefixes( TupperwareAggregate* tupDatabase );
	void ImportLocks( TupperwareAggregate* tupDatabase );
	void ImportTextures( TupperwareAggregate* tupDatabase );
	void ImportPieces( TupperwareAggregate* tupDatabase );
	void ImportPrebuilt( TupperwareAggregate* tupDatabase );
	void ImportMenus( TupperwareAggregate* tupDatabase );
	void ImportItems( TupperwareAggregate* tupDatabase );
	void ImportExclude( TupperwareAggregate* tupDatabase );
	void ImportReverseExclude( TupperwareAggregate* tupDatabase );
	DBTexture*	FindTextureById( int id );
	DBPiece*		FindPieceById( int id );
	DBMenu*		FindMenuById( int id );
	DBItem*		FindItemById( int id );
	DBLock*		FindLockById( int id );
	DBPrefix*	FindPrefixByName( const char* name );
	int			PieceIndexFromPtr( DBPiece* piece );
	int			TextureIndexFromPtr( DBTexture* tex );
	void ComposerSetChoices( const char* initString );
	void SetCustomToDefault();
	int GetPrebuiltByDefaultSlot( int type, int slot );

	// local private storage
	static int								s_databaseVersion = 0;

	int										s_numPrefixes = 0;
	int										s_numTextures = 0;
	int										s_numPieces   = 0;
	int										s_numPrebuilt = 0;
	int										s_numMenus    = 0;
	int										s_numItems    = 0;
	int										s_numLocks    = 0;
	int										s_numExcludes = 0;
	int										s_numReverseExcludes = 0;

	DBPrefix*								s_prefixes    = NULL;
	DBTexture*								s_textures    = NULL;
	DBPiece*									s_pieces      = NULL;
	DBPrebuilt*								s_prebuilt    = NULL;
	DBMenu*									s_menus       = NULL;
	DBItem*									s_items       = NULL;
	DBLock*									s_locks       = NULL;
	DBExclude*								s_excludes    = NULL;
	DBExclude*								s_reverseExcludes = NULL;

	static DBComposer*					s_composer = NULL;
	static DBItem*							s_emptyItem = NULL;

	enum {
		EXCLUDE_COMMAND_SIZE	= 5,
		ITEM_COMMAND_SIZE    = 4,
		CMD_COMMAND_MASK		= 0xE000,
		CMD_ID_MASK          = 0x1FFF,
		CMD_TEXTURE          = (0 << 15),
		CMD_PIECE            = (1 << 15),
		CMD_KEY              = (0 << 13),
		CMD_DELETE           = (1 << 13),
		CMD_ADD              = (2 << 13),
		CMD_INVALID          = (3 << 13)
	};
};

//============================================================================

namespace PlayerDB
{
	class DBPrefix
	{
		public:
		DBPrefix( const char* i_name )
		{
			ASSERT( strlen( i_name ) + 1 <= sizeof( name ) );
			strcpy( name, i_name );
		}
		char name[ 24 ];
	};

	class DBLock
	{
	public:
		DBLock( int i_id, const char* i_key, const char* i_desc, const char* i_unlock )
		{
			id = i_id;
			lock = GameLocks::AddLock( i_key, i_desc, i_unlock );
			ASSERT_PTR( lock );
		}
		int					id;
		GameLocks::Lock*	lock;
	};

	class DBTexture
	{
		public:
		DBTexture( int i_id, DBPrefix* i_prefix, int i_type, u32 i_lock )
		{
			id = i_id;
			prefix = i_prefix;
			type = i_type;
			lock = FindLockById( i_lock );
		}
		bool IsAvailable( bool allowLocked );
		int				id;
		DBPrefix*		prefix;
		int				type;
		DBLock*			lock;
	};

	class DBPiece
	{
		public:
		DBPiece( int i_id, const char* i_slot, int i_type, int i_lock, bool i_hasLores, int i_lod )
		{
			id = i_id;
			ASSERT( strlen( i_slot ) + 1 <= sizeof( slot ) );
			strcpy( slot, i_slot );
			for ( int i = 0 ; i < sizeof(prefixes)/sizeof(prefixes[0]) ; i++ )
				prefixes[ i ] = NULL;
			numPrefixes = 0;
			type = i_type;
			lock = FindLockById( i_lock );
			hasLores = i_hasLores;
			lod = i_lod;
		}
		bool HasPrefix( DBPrefix* i_prefix )
		{
			for ( int i = 0 ; i < numPrefixes ; i++ )
				if ( prefixes[ i ] == i_prefix )
					return true;
			return false;
		}
		void AddPrefix( DBPrefix* i_prefix )
		{
			if ( !HasPrefix( i_prefix ) )
			{
				ASSERT( numPrefixes < sizeof(prefixes)/sizeof(prefixes[0]) );
				prefixes[ numPrefixes++ ] = i_prefix;
			}
		}
		bool IsAvailable( bool allowLocked );
		int			id;
		char			slot[ 16 ];
		DBPrefix*	prefixes[ 8 ];
		int			numPrefixes;
		DBLock*		lock;
		int			type;
		short			lod;
		char			hasLores;
	};

	class DBPrebuilt
	{
		public:
		DBPrebuilt( int i_id, const char* i_name, const char* i_displayname, const char* i_init, int i_type, int i_selectable, int i_lock, int i_default )
		{
			id = i_id;
			Set( i_name, i_displayname, i_init, i_type, i_selectable, i_lock, i_default );
		}
		void Set( const char* i_name, const char* i_displayname, const char* i_init, int i_type, int i_selectable, int i_lock, int i_default )
		{
			SetName( i_name );
			SetDisplayName( i_displayname );
			ASSERT( strlen( i_init ) + 1 <= sizeof( init ) );
			strcpy( init, i_init );
			type = i_type;
			selectable = i_selectable;
			lock = FindLockById( i_lock );
			def = i_default;
		}
		void SetName( const char* i_name )
		{
			ASSERT( strlen( i_name ) + 1 <= sizeof( name ) );
			strcpy( name, i_name );
		}
		void SetDisplayName( const char* i_displayname )
		{
			ASSERT( strlen( i_displayname ) + 1 <= sizeof( displayname ) );
			strcpy( displayname, i_displayname );
		}
		int		id;
		char		name[ 28 ];
		char		displayname[ 28 ];
		char		init[ 160 ];
		int		type;
		DBLock*	lock;
		char		selectable;
		char		def;
	};

	class DBItem
	{
		public:
		DBItem( int i_id, const char* i_name, int i_menuId, int i_showIfAll )
		{
			id = i_id;
			menuId = i_menuId;
			ASSERT( strlen( i_name ) + 1 <= sizeof( name ) );
			strcpy( name, i_name );
			next = NULL;
			showIfAll = i_showIfAll;
			memset( command, 0xFF, sizeof(command) );
		}
		int NextEntry()
		{
			for ( int j = 0 ; j < ITEM_COMMAND_SIZE ; j++ )
				if ( command[ j ] == 0xFFFF )
					return j;
			ASSERTS( false, "DBItem no space for new piece or texture" );
			return -1;
		}
		void AddPiece( DBPiece* piece )
		{
			ASSERT_PTR( piece );
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_PIECE | CMD_ADD | piece->id;
		}
		void AddTexture( DBTexture* texture )
		{
			ASSERT_PTR( texture );
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_TEXTURE | CMD_ADD | texture->id;
		}
		int BestMatch( DynList<DBPiece>& i_pieces, DynList<DBTexture>& i_textures )
		{
			int matching = 0;
			int i, j;
			for ( j = 0 ; j < ITEM_COMMAND_SIZE ; j++ )
			{
				if ( ( command[ j ] & CMD_COMMAND_MASK ) == ( CMD_PIECE | CMD_ADD ) )
					for ( i = 0 ; i < i_pieces.Count() ; i++ )
						if ( i_pieces[ i ]->id == ( command[ j ] & CMD_ID_MASK ) )
							matching++;
				if ( ( command[ j ] & CMD_COMMAND_MASK ) == ( CMD_TEXTURE | CMD_ADD ) )
					for ( i = 0 ; i < i_textures.Count() ; i++ )
						if ( i_textures[ i ]->id == ( command[ j ] & CMD_ID_MASK ) )
							matching++;
			}
			return matching;
		}
		int NumTextures()
		{
			int count = 0;
			for ( int i = 0 ; i < ITEM_COMMAND_SIZE ; i++ )
				if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_TEXTURE | CMD_ADD ) )
					count++;
			return count;
		}
		int NumPieces()
		{
			int count = 0;
			for ( int i = 0 ; i < ITEM_COMMAND_SIZE ; i++ )
				if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_PIECE | CMD_ADD ) )
					count++;
			return count;
		}
		bool IsRelevant();
		bool IsAvailable( bool allowLocked );
		int			id;
		int			menuId;
		char			name[ 32 ];
//		int			numPieces;
//		int			numTextures;
//		DBPiece*		pieces[ 4 ];
//		DBTexture*	textures[ 4 ];
		unsigned short command[ ITEM_COMMAND_SIZE ];
		DBItem*		next;
		char			showIfAll;
	};

	class DBMenu
	{
		public:
		DBMenu( int i_id,  const char* i_name, int i_parentId, int i_allowNone )
		{
			id = i_id;
			parentId = i_parentId;
			ASSERT( strlen( i_name ) + 1 <= sizeof( name ) );
			strcpy( name, i_name );
			allowNone = ( i_allowNone != 0 );
			parent    = NULL;
			next      = NULL;
			childHead = NULL;
			childTail = NULL;
			itemHead  = NULL;
			itemTail  = NULL;
		}
		void AddChildMenu( DBMenu* menu )
		{
			ASSERT( menu->parent == NULL );
			ASSERT( menu->next == NULL );
			menu->parent = this;
			if ( childTail )	childTail->next = menu;
			else					childHead = menu;
			childTail = menu;
		}
		void AddItem( PlayerDB::DBItem* item )
		{
//			ASSERT( item->menu == NULL );
			ASSERT( item->next == NULL );
//			item->menu = this;
			if ( itemTail )	itemTail->next = item;
			else					itemHead = item;
			itemTail = item;
		}
		int MenuCount()
		{
			int count = 0;
			for ( DBMenu* m = childHead ; m ; m = m->next )
				if ( m->IsRelevant() )
					count++;
			return count;
		}
		DBMenu* GetMenu( int index )
		{
			for ( DBMenu* m = childHead ; m ; m = m->next )
				if ( m->IsRelevant() )
					if ( index-- == 0 )
						return m;
			return NULL;
		}
		DBItem* GetItem( int index )
		{
			if ( allowNone )
				if ( index-- == 0 )
					return s_emptyItem;
			for ( DBItem* item = itemHead ; item ; item = item->next )
				if ( item->IsAvailable( false ) )
					if ( index-- == 0 )
						return item;
			return NULL;
		}
		DBItem* GetItemById( int id )
		{
			if ( allowNone )
				if ( s_emptyItem->id == id )
					return s_emptyItem;
			for ( DBItem* item = itemHead ; item ; item = item->next )
				if ( item->IsAvailable( false ) )
					if ( item->id == id )
						return item;
			return NULL;
		}
		int ItemCount( bool allowLocked )
		{
			int count = 0;
			for ( DBItem* item = itemHead ; item ; item = item->next )
				if ( item->IsAvailable( allowLocked ) )
					count++;
			if ( count && allowNone )
				count++;
			return count;
		}
		bool IsRelevant();
		int		id;
		int		parentId;
		char		name[ 32 ];
		bool		allowNone;
		DBMenu*	parent;
		DBMenu*	next;
		DBMenu*	childHead;
		DBMenu*	childTail;
		DBItem*	itemHead;
		DBItem*	itemTail;
	};

	class DBSlot
	{
		public:
		DBSlot( DBMenu* i_menu )
		{
			ASSERT_PTR( i_menu );
			menu = i_menu;
			numItems = 0;
			for ( int i = 0 ; i < sizeof(items) / sizeof(items[0]) ; i++ )
				items[ i ] = NULL;
		}
		void AddItem( DBItem* item )
		{
			ASSERT_PTR( item );
			if ( item && numItems < sizeof(items) / sizeof(items[0]) )
				items[ numItems++ ] = item;
			else
				ASSERTF( false, ( "slot '%s' too many items", menu->name ) );
		}
		DBMenu*	menu;
		int		numItems;
		DBItem*	items[ 36 ];
	};

	class DBExclude
	{
		public:
		DBExclude( int i_id )
		{
			memset( command, 0xFF, sizeof(command) );
		}
		int NextEntry()
		{
			for ( int i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
				if ( command[ i ] == 0xffff )
					return i;
			ASSERTS( false, "no space for new entry" );
			return -1;
		}
		void AddPieceKey( DBPiece* piece )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_KEY | CMD_PIECE | piece->id;
		}
		void AddPieceDel( DBPiece* piece )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_DELETE | CMD_PIECE | piece->id;
		}
		void AddPieceAdd( DBPiece* piece )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_ADD | CMD_PIECE | piece->id;
		}
		void AddTextureKey( DBTexture* texture )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_KEY | CMD_TEXTURE | texture->id;
		}
		void AddTextureDel( DBTexture* texture )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_DELETE | CMD_TEXTURE | texture->id;
		}
		void AddTextureAdd( DBTexture* texture )
		{
			int entry = NextEntry();
			if ( entry >= 0 )
				command[ entry ] = CMD_ADD | CMD_TEXTURE | texture->id;
		}
		void Apply( DynList<DBPiece>* pieces, DynList<DBTexture>* textures )
		{
			if ( IsValid( pieces, textures ) )
			{
				int i, j;

				// remove pieces and textures
				for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
					if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_DELETE | CMD_TEXTURE ) )
						if ( ( j = Find( textures, command[ i ] & CMD_ID_MASK ) ) >= 0 )
							textures->Delete( j );
				for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
					if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_DELETE | CMD_PIECE ) )
						if ( ( j = Find( pieces, command[ i ] & CMD_ID_MASK ) ) >= 0 )
							pieces->Delete( j );

				// add in new pieces or textures
				DBTexture* tex;
				DBPiece* piece;
				for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
					if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_ADD | CMD_TEXTURE ) )
						if ( Find( textures, command[ i ] & CMD_ID_MASK ) < 0 )
							if ( ( tex = FindTextureById( command[ i ] & CMD_ID_MASK ) ) != NULL )
								textures->Add( tex );
				for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
					if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_ADD | CMD_PIECE ) )
						if ( Find( pieces, command[ i ] & CMD_ID_MASK ) < 0 )
							if ( ( piece = FindPieceById( command[ i ] & CMD_ID_MASK ) ) != NULL )
								pieces->Add( piece );
			}
		}
		bool IsValid( DynList<DBPiece>* pieces, DynList<DBTexture>* textures )
		{
			int i;
			for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
				if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_KEY | CMD_TEXTURE ) )
					if ( Find( textures, command[ i ] & CMD_ID_MASK ) < 0 )
						return false;
			for ( i = 0 ; i < EXCLUDE_COMMAND_SIZE ; i++ )
				if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_KEY | CMD_PIECE ) )
					if ( Find( pieces, command[ i ] & CMD_ID_MASK ) < 0 )
						return false;
			return true;
		}
		int Find( DynList<DBPiece>* pieces, int id )
		{
			int j;
			for ( j = pieces->Count() - 1 ; j >= 0 ; j-- )
				if ( pieces->Get( j )->id == id )
					break;
			return j;
		}
		int Find( DynList<DBTexture>* textures, int id )
		{
			int j;
			for ( j = textures->Count() - 1 ; j >= 0 ; j-- )
				if ( textures->Get( j )->id == id )
					break;
			return j;
		}
		unsigned short command[ EXCLUDE_COMMAND_SIZE ];
	};

	class DBComposerChoice
	{
		public:
		DBComposerChoice()
		{
			selected = -1;
		}
		int selected;
	};

	class DBComposerTextures
	{
		public:
		DBComposerTextures()
		{
			hasShift   = 0;
			hue        = 0;
			saturation = 0;
			value      = 0;
		}
		void SetHSV( int h, int s, int v )
		{
			ASSERT( h >= 0 && h <= 360  );
			ASSERT( s >= 0 && s <= 0xff );
			ASSERT( v >= 0 && v <= 0xff );
			hue        = h;
			saturation = s;
			value      = v;
			hasShift   = 1;
		}
		bool GetHSV( int* h, int* s, int* v )
		{
			*h = hue;
			*s = saturation;
			*v = value;
			return hasShift != 0;
		}
		int	hasShift;
		int	hue;
		int	saturation;
		int	value;
	};

	class DBComposer
	{
		public:
		DBComposer() : neededPrefixes( 64, 0, false )
		{
			choices = NULL;
			textures = NULL;
			currentMenu = -1;
			type			= PLAYERDB_COP;
		}
		~DBComposer()
		{
			delete choices;
			delete textures;
		}
		void RefreshPrefixes()
		{
			neededPrefixes.Clear();
			for ( int i = 0 ; i < s_numMenus ; i++ )
			{
				DBMenu* menu = &s_menus[ i ];
				int selected = choices[ i ].selected;
				if ( menu->ItemCount( false ) > 0 && selected >= 0 )
				{
					DBItem* item = menu->GetItemById( selected );
					if ( item )
					{
						for ( int j = 0 ; j < ITEM_COMMAND_SIZE ; j++ )
						{
							if ( ( item->command[ j ] & CMD_COMMAND_MASK ) == ( CMD_PIECE | CMD_ADD ) )
							{
								DBPiece* piece = FindPieceById( item->command[ j ] & CMD_ID_MASK );
								for ( int p = 0 ; p < piece->numPrefixes ; p++ )
									if ( neededPrefixes.Find( piece->prefixes[ p ] ) < 0 )
										neededPrefixes.Add( piece->prefixes[ p ] );
							}
						}
					}
				}
			}
		}
		DBComposerChoice*		choices;
		DBComposerTextures*	textures;
		int						currentMenu;
		DynList<DBPrefix>		neededPrefixes;
		int						type;		// cop or gang
	};
};

//============================================================================

void PlayerDB::Load( const char* filename )
{
	ASSERT_PTR( filename );
	ASSERT( filename[ 0 ] );

	s32		fileLen;
	t_Error	error;
	char* fileBuf = (char*) Media::Load( filename, &fileLen, &error );
	ASSERTS( fileBuf, "fatal error - can't load player database" );

	if ( fileBuf )
	{
		TupperwareAggregate* tupDatabase = NULL;
		if ( TupperwareLoadSaveManager::Read( fileBuf, fileLen, &tupDatabase ) )
		{
			ImportVersion( tupDatabase );
			ImportPrefixes( tupDatabase );
			ImportLocks( tupDatabase );
			ImportTextures( tupDatabase );
			ImportPieces( tupDatabase );
			ImportPrebuilt( tupDatabase );
			ImportItems( tupDatabase );
			ImportMenus( tupDatabase );
			ImportExclude( tupDatabase );
			ImportReverseExclude( tupDatabase );

			delete tupDatabase;
			memFree( fileBuf );

			SetCustomToDefault();
		}
	}

//	Dump();
}

//============================================================================

void PlayerDB::ImportVersion( TupperwareAggregate* tupDatabase )
{
	TupperwareIterator version = tupDatabase->FindByKey( "Version" );
	if ( !version.Finished() )
	{
		s_databaseVersion = version.GetAsScalar()->GetAsInt();
	}
	else
	{
		ASSERTS( false, "player database version not found" );
		s_databaseVersion = 0;
	}
}

//============================================================================

void PlayerDB::ImportPrefixes( TupperwareAggregate* tupDatabase )
{
	s_numPrefixes = 0;
	TupperwareIterator prefixes = tupDatabase->FindByKey( "Prefix" );
	for ( ; !prefixes.Finished() ; prefixes.Next() )
		s_numPrefixes++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_prefixes = (DBPrefix*) memAlloc( s_numPrefixes * sizeof(DBPrefix) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB PREFIXES", s_numPrefixes * sizeof(DBPrefix), s_numPrefixes, sizeof(DBPrefix) );

	prefixes = tupDatabase->FindByKey( "Prefix" );
	for ( int i = 0 ; !prefixes.Finished() ; prefixes.Next(), i++ )
	{
		new ( &s_prefixes[ i ] ) DBPrefix( prefixes.GetAsAtom()->GetName() );
	}
}

//============================================================================

void PlayerDB::ImportLocks( TupperwareAggregate* tupDatabase )
{
	// allocate space for locks
	s_numLocks = 0;
	TupperwareIterator locks = tupDatabase->FindByKey( "Lock" );
	for ( ; !locks.Finished() ; locks.Next() )
		s_numLocks++;
	if ( s_numLocks )
	{
		memSetAllocMethod( ALC_BTM );
		memSetFitMethod( FIT_FIRST );
		s_locks = (DBLock*) memAlloc( s_numLocks * sizeof(DBLock) );
		memSetAllocMethod( ALC_TOP );
		memSetFitMethod( FIT_LAST );
		dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB LOCKS", s_numLocks * sizeof(DBLock), s_numLocks, sizeof(DBLock) );
	}

	locks = tupDatabase->FindByKey( "Lock" );
	for ( int i = 0 ; !locks.Finished() ; locks.Next(), i++ )
	{
		int id = atoi( locks.GetAsAtom()->GetName() );
		TupperwareIterator key    = locks.GetAsAggregate()->FindByKey( "key" );
		TupperwareIterator desc   = locks.GetAsAggregate()->FindByKey( "desc" );
		TupperwareIterator unlock = locks.GetAsAggregate()->FindByKey( "tounlock" );
		new ( &s_locks[ i ] ) DBLock( id,
			                           key.GetAsScalar()->GetAsString(),
			                           desc.GetAsScalar()->GetAsString(),
												unlock.GetAsScalar()->GetAsString() );
	}

	// now lock it all up
	GameLocks::LockAll();
}

//============================================================================

void PlayerDB::ImportTextures( TupperwareAggregate* tupDatabase )
{
	// allocate space for all textures
	s_numTextures = 0;
	TupperwareIterator textures = tupDatabase->FindByKey( "Texture" );
	for ( ; !textures.Finished() ; textures.Next() )
		s_numTextures++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_textures = (DBTexture*) memAlloc( s_numTextures * sizeof(DBTexture) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB TEXTURES", s_numTextures * sizeof(DBTexture), s_numTextures, sizeof(DBTexture) );

	int lastId = -1;
	textures = tupDatabase->FindByKey( "Texture" );
	for ( int i = 0 ; !textures.Finished() ; textures.Next(), i++ )
	{
		int id = atoi( textures.GetAsAtom()->GetName() );
		ASSERT( id > lastId );	// for binary search
		lastId = id;
		TupperwareIterator prefix = textures.GetAsAggregate()->FindByKey( "tex" );
		TupperwareIterator type = textures.GetAsAggregate()->FindByKey( "type" );
		TupperwareIterator lock = textures.GetAsAggregate()->FindByKey( "lock" );
		int lockId = lock.Finished() ? -1 : lock.GetAsScalar()->GetAsInt();
		const char* prefixName = prefix.GetAsScalar()->GetAsString();
		DBPrefix* p = FindPrefixByName( prefixName );
		ASSERT_PTR( p );
		new ( &s_textures[i] ) DBTexture( id, p, type.GetAsScalar()->GetAsInt(), lockId );
	}
}

//============================================================================

void PlayerDB::ImportPieces( TupperwareAggregate* tupDatabase )
{
	// alloc space for all pieces
	s_numPieces = 0;
	TupperwareIterator pieces = tupDatabase->FindByKey( "Piece" );
	for ( ; !pieces.Finished() ; pieces.Next() )
		s_numPieces++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_pieces = (DBPiece*) memAlloc( s_numPieces * sizeof(DBPiece) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB PIECES", s_numPieces * sizeof(DBPiece), s_numPieces, sizeof(DBPiece) );

	int lastId = -1;
	pieces = tupDatabase->FindByKey( "Piece" );
	for ( int i = 0 ; !pieces.Finished() ; pieces.Next(), i++ )
	{
		int id = atoi( pieces.GetAsAtom()->GetName() );
		ASSERT( id > lastId );	// for binary search
		lastId = id;
		TupperwareIterator slot = pieces.GetAsAggregate()->FindByKey( "slot" );
		TupperwareIterator type = pieces.GetAsAggregate()->FindByKey( "type" );
		TupperwareIterator lock = pieces.GetAsAggregate()->FindByKey( "lock" );
		TupperwareIterator lores = pieces.GetAsAggregate()->FindByKey( "lores" );
		TupperwareIterator lod = pieces.GetAsAggregate()->FindByKey( "lod" );
		int lockId = lock.Finished() ? -1 : lock.GetAsScalar()->GetAsInt();
		bool hasLores = lores.Finished() ? false : ( lores.GetAsScalar()->GetAsInt() != 0 );
		int lodId = lod.Finished() ? -1 : lod.GetAsScalar()->GetAsInt();
		DBPiece* piece = new ( &s_pieces[ i ] ) DBPiece( id, slot.GetAsScalar()->GetAsString(),
			                                              type.GetAsScalar()->GetAsInt(), lockId, hasLores, lodId );
		TupperwareIterator tex = pieces.GetAsAggregate()->FindByKey( "tex" );
		for ( ; !tex.Finished() ; tex.Next() )
		{
			const char* prefixName = tex.GetAsScalar()->GetAsString();
			DBPrefix* p = FindPrefixByName( prefixName );
			ASSERT_PTR( p );
			piece->AddPrefix( p );
		}
	}
}

//============================================================================

void PlayerDB::ImportPrebuilt( TupperwareAggregate* tupDatabase )
{
	// allocate space for prebuilt
	s_numPrebuilt = 0;
	TupperwareIterator prebuilt = tupDatabase->FindByKey( "Prebuilt" );
	for ( ; !prebuilt.Finished() ; prebuilt.Next() )
		s_numPrebuilt++;
	s_numPrebuilt += CS_MAX_CHARACTERS;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_prebuilt = (DBPrebuilt*) memAlloc( s_numPrebuilt * sizeof(DBPrebuilt) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB PREBUILT", s_numPrebuilt * sizeof(DBPrebuilt), s_numPrebuilt, sizeof(DBPrebuilt) );

	prebuilt = tupDatabase->FindByKey( "Prebuilt" );
	int i;
	for ( i = 0 ; !prebuilt.Finished() ; prebuilt.Next(), i++ )
	{
		int id = atoi( prebuilt.GetAsAtom()->GetName() );
		TupperwareIterator name = prebuilt.GetAsAggregate()->FindByKey( "name" );
		TupperwareIterator displayname = prebuilt.GetAsAggregate()->FindByKey( "displayname" );
		TupperwareIterator init = prebuilt.GetAsAggregate()->FindByKey( "init" );
		TupperwareIterator type = prebuilt.GetAsAggregate()->FindByKey( "type" );
		TupperwareIterator restricted = prebuilt.GetAsAggregate()->FindByKey( "restricted" );
		TupperwareIterator lock = prebuilt.GetAsAggregate()->FindByKey( "lock" );
		TupperwareIterator def  = prebuilt.GetAsAggregate()->FindByKey( "default" );
		int lockId = lock.Finished() ? -1 : lock.GetAsScalar()->GetAsInt();
		int defId  = def.Finished() ? 0 : def.GetAsScalar()->GetAsInt();
		new ( &s_prebuilt[ i ] ) DBPrebuilt( id, name.GetAsScalar()->GetAsString(),
														 displayname.GetAsScalar()->GetAsString(),
			                                  init.GetAsScalar()->GetAsString(),
													    type.GetAsScalar()->GetAsInt(),
													    !restricted.GetAsScalar()->GetAsInt(), lockId, defId );
	}

	// add in preallocated custom character slots
	for ( int c = 0 ; c < CS_MAX_CHARACTERS ; c++, i++ )
		new ( &s_prebuilt[ i ] ) DBPrebuilt( PLAYERDB_CUSTOM | c, "", "", "", PLAYERDB_COP, false, -1, 0 );
}

//============================================================================

void PlayerDB::ImportMenus( TupperwareAggregate* tupDatabase )
{
	s_numMenus = 0;
	TupperwareIterator menus = tupDatabase->FindByKey( "Menu" );
	for ( ; !menus.Finished() ; menus.Next() )
		s_numMenus++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_menus = (DBMenu*) memAlloc( s_numMenus * sizeof(DBMenu) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB MENUS", s_numMenus * sizeof(DBMenu), s_numMenus, sizeof(DBMenu) );

	menus = tupDatabase->FindByKey( "Menu" );
	int i;
	for ( i = 0 ; !menus.Finished() ; menus.Next(), i++ )
	{
		int id = atoi( menus.GetAsAtom()->GetName() );
		TupperwareIterator name = menus.GetAsAggregate()->FindByKey( "name" );
		TupperwareIterator parent = menus.GetAsAggregate()->FindByKey( "menu" );
		TupperwareIterator allowNone = menus.GetAsAggregate()->FindByKey( "allownone" );
		int parentId = ( parent.Finished() ) ? -1 : parent.GetAsScalar()->GetAsInt();
		new ( &s_menus[ i ] ) DBMenu( id, name.GetAsScalar()->GetAsString(), parentId,
											   allowNone.GetAsScalar()->GetAsInt() );
	}

	// link menus
	for ( int i = 0 ; i < s_numMenus ; i++ )
	{
		DBMenu* parent = FindMenuById( s_menus[ i ].parentId );
		if ( parent )
			parent->AddChildMenu( &s_menus[ i ] );
	}

	// link items into menus
	for ( int i = 0 ; i < s_numItems ; i++ )
	{
		DBMenu* menu = FindMenuById( s_items[ i ].menuId );
		if ( menu )
			menu->AddItem( &s_items[ i ] );
	}
}

//============================================================================

void PlayerDB::ImportItems( TupperwareAggregate* tupDatabase )
{
	s_numItems = 1; // always have empty item
	TupperwareIterator items = tupDatabase->FindByKey( "Item" );
	for ( ; !items.Finished() ; items.Next() )
		s_numItems++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_items = (DBItem*) memAlloc( s_numItems * sizeof(DBItem) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB ITEMS", s_numItems * sizeof(DBItem), s_numItems, sizeof(DBItem) );

	items = tupDatabase->FindByKey( "Item" );
	int i;
	for ( i = 0 ; !items.Finished() ; items.Next(), i++ )
	{
		int id = atoi( items.GetAsAtom()->GetName() );
		TupperwareIterator name = items.GetAsAggregate()->FindByKey( "name" );
		TupperwareIterator menu = items.GetAsAggregate()->FindByKey( "menu" );
		TupperwareIterator showIfAll = items.GetAsAggregate()->FindByKey( "showifall" );
		DBItem* item = new ( &s_items[ i ] ) DBItem( id, name.GetAsScalar()->GetAsString(), menu.GetAsScalar()->GetAsInt(),
			                                  showIfAll.GetAsScalar()->GetAsInt() );

		// pieces
		TupperwareIterator piece = items.GetAsAggregate()->FindByKey( "piece" );
		for ( ; !piece.Finished() ; piece.Next() )
			item->AddPiece( FindPieceById( piece.GetAsScalar()->GetAsInt() ) );

		// textures
		TupperwareIterator tex = items.GetAsAggregate()->FindByKey( "tex" );
		for ( ; !tex.Finished() ; tex.Next() )
			item->AddTexture( FindTextureById( tex.GetAsScalar()->GetAsInt() ) );
	}

	// create an empty item
	s_emptyItem = new ( &s_items[ i ] ) DBItem( -1, "None", -1, 0 );
}

//============================================================================

void PlayerDB::ImportExclude( TupperwareAggregate* tupDatabase )
{
	s_numExcludes = 0;
	TupperwareIterator excludes = tupDatabase->FindByKey( "Exclude" );
	for ( ; !excludes.Finished() ; excludes.Next() )
		s_numExcludes++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_excludes = (DBExclude*) memAlloc( s_numExcludes * sizeof(DBExclude) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB EXCLUDES", s_numExcludes * sizeof(DBExclude), s_numExcludes, sizeof(DBExclude) );

	excludes = tupDatabase->FindByKey( "Exclude" );
	for ( int i = 0 ; !excludes.Finished() ; excludes.Next(), i++ )
	{
		int id = atoi( excludes.GetAsAtom()->GetName() );
		DBExclude* exclude = new ( &s_excludes[ i ] ) DBExclude( id );

		TupperwareIterator x;

		for ( x = excludes.GetAsAggregate()->FindByKey( "piecekey" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceKey( FindPieceById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "piecedel" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceDel( FindPieceById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "pieceadd" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceAdd( FindPieceById( x.GetAsScalar()->GetAsInt() ) );

		for ( x = excludes.GetAsAggregate()->FindByKey( "texkey" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureKey( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "texdel" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureDel( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "texadd" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureAdd( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
	}
}

//============================================================================

void PlayerDB::ImportReverseExclude( TupperwareAggregate* tupDatabase )
{
	s_numReverseExcludes = 0;
	TupperwareIterator excludes = tupDatabase->FindByKey( "ReverseExclude" );
	for ( ; !excludes.Finished() ; excludes.Next() )
		s_numReverseExcludes++;
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	s_reverseExcludes = (DBExclude*) memAlloc( s_numReverseExcludes * sizeof(DBExclude) );
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );
	dbgPrint( "%-20s: %6d (%4d @ %4d)\n", "CUSTOMDB REVERSE EXCLUDES", s_numReverseExcludes * sizeof(DBExclude), s_numReverseExcludes, sizeof(DBExclude) );

	excludes = tupDatabase->FindByKey( "ReverseExclude" );
	for ( int i = 0 ; !excludes.Finished() ; excludes.Next(), i++ )
	{
		int id = atoi( excludes.GetAsAtom()->GetName() );
		DBExclude* exclude = new ( &s_reverseExcludes[ i ] ) DBExclude( id );

		TupperwareIterator x;

		for ( x = excludes.GetAsAggregate()->FindByKey( "piecekey" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceKey( FindPieceById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "piecedel" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceDel( FindPieceById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "pieceadd" ) ; !x.Finished() ; x.Next() )
			exclude->AddPieceAdd( FindPieceById( x.GetAsScalar()->GetAsInt() ) );

		for ( x = excludes.GetAsAggregate()->FindByKey( "texkey" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureKey( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "texdel" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureDel( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
		for ( x = excludes.GetAsAggregate()->FindByKey( "texadd" ) ; !x.Finished() ; x.Next() )
			exclude->AddTextureAdd( FindTextureById( x.GetAsScalar()->GetAsInt() ) );
	}
}

//============================================================================

void PlayerDB::SetCustomToDefault()
{
	int i;
	char name[ 16 ];

	int numCops = CS_MAX_CHARACTERS / 2;
	int numGang = ( CS_MAX_CHARACTERS ) - numCops;
	ASSERT( numCops == numGang );

	// add in preallocated custom character slots
	for ( i = 0 ; i < numCops ; i++ )
	{
		int index = CharacterIndexFromId( PLAYERDB_CUSTOM | i );
		sprintf( name, "Custom Cop %d", i + 1 );
		SetCharacter( index, name, CharacterInitString( GetPrebuiltByDefaultSlot( PLAYERDB_COP, i + 1 ) ), PLAYERDB_COP, true );
	}
	for ( i = 0 ; i < numGang ; i++ )
	{
		int index = CharacterIndexFromId( PLAYERDB_CUSTOM | ( numCops + i ) );
		sprintf( name, "Custom Gang %d", i + 1 );
		SetCharacter( index, name, CharacterInitString( GetPrebuiltByDefaultSlot( PLAYERDB_GANG, i + 1 ) ), PLAYERDB_GANG, true );
	}
}

//============================================================================

void PlayerDB::Dump()
{
	int i;

	dbgPrint( "PlayerDB Version=%d\n", s_databaseVersion );

	for ( i = 0 ; i < s_numPrefixes ; i++ )
	{
		dbgPrint( "Prefix %d - '%s'\n", i, s_prefixes[ i ].name );
	}

	for ( i = 0 ; i < s_numTextures ; i++ )
	{
		dbgPrint( "Texture ID=%d prefix='%s'\n", s_textures[ i ].id, s_textures[ i ].prefix );
	}

	for ( i = 0 ; i < s_numPieces ; i++ )
	{
		dbgPrint( "Piece ID=%d slot='%s'\n", s_pieces[ i ].id, s_pieces[ i ].slot );
	}

	for ( i = 0 ; i < s_numPrebuilt ; i++ )
	{
		dbgPrint( "Prebuilt ID=%d '%s' type=%d init='%s'\n", s_prebuilt[ i ].id, s_prebuilt[ i ].name, s_prebuilt[ i ].type, s_prebuilt[ i ].init );
	}

	for ( i = 0 ; i < s_numMenus ; i++ )
	{
		dbgPrint( "Menu ID=%d '%s' parent=%d\n", s_menus[ i ].id, s_menus[ i ].name, s_menus[ i ].parentId );
	}

	DumpMenus( FindMenuById( 0 ), 0 );

//	for ( i = 0 ; i < s_numItems ; i++ )
//	{
//		dbgPrint( "Item ID=%d '%s' menu=%d\n", s_items[ i ].id, s_items[ i ].name, s_items[ i ].menuId );
//		int j;
//		for ( j = 0 ; j < s_items[ i ].numPieces ; j++ )
//			dbgPrint( "  P:%d", s_items[ i ].pieces[ j ]->id );
//		for ( j = 0 ; j < s_items[ i ].numTextures ; j++ )
//			dbgPrint( "  T:%d", s_items[ i ].textures[ j ]->id );
//		dbgPrint( "\n" );
//	}
//
//	for ( i = 0 ; i < s_numExcludes ; i++ )
//	{
//		int j;
//		DBExclude* exc = &s_excludes[ i ];
//		dbgPrint( "Exclude ID=%d\n", exc->id );
//		for ( j = 0 ; j < exc->numPieceKey ; j++ ) { dbgPrint( " if P[%d]\n", exc->pieceKey[j]->id ); }
//		for ( j = 0 ; j < exc->numTexKey   ; j++ ) { dbgPrint( " if T[%d]\n", exc->texKey[j]->id ); }
//		for ( j = 0 ; j < exc->numPieceDel ; j++ ) { dbgPrint( "  - P[%d]\n", exc->pieceDel[j]->id ); }
//		for ( j = 0 ; j < exc->numTexDel   ; j++ ) { dbgPrint( "  - T[%d]\n", exc->texDel[j]->id ); }
//		for ( j = 0 ; j < exc->numPieceAdd ; j++ ) { dbgPrint( "  + P[%d]\n", exc->pieceAdd[j]->id ); }
//		for ( j = 0 ; j < exc->numTexAdd   ; j++ ) { dbgPrint( "  + T[%d]\n", exc->texAdd[j]->id ); }
//	}
}

//============================================================================

void PlayerDB::DumpMenus( PlayerDB::DBMenu* root, int level )
{
	for ( ; root ; root = root->next )
	{
		for ( int i = 0 ; i < level ; i++ ) dbgPrint( ". . " );
		dbgPrint( "[%s] %d\n", root->name, root->id );
		for ( DBItem* item = root->itemHead ; item ; item = item->next )
		{
			for ( int i = 0 ; i < level ; i++ ) dbgPrint( ". . " );
			dbgPrint( "- %s\n", item->name );
		}
		DumpMenus( root->childHead, level + 1 );
	}
}

//============================================================================

int PlayerDB::NumCharacters()
{
	return s_numPrebuilt;
}

//============================================================================

char* PlayerDB::CharacterInitString( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return s_prebuilt[ index ].init;
}

//============================================================================

char* PlayerDB::CharacterName( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return s_prebuilt[ index ].name;
}

//============================================================================

char* PlayerDB::CharacterDisplayName( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return s_prebuilt[ index ].displayname;
}

//============================================================================

int PlayerDB::CharacterType( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return s_prebuilt[ index ].type;
}

//============================================================================

int PlayerDB::CharacterIndexFromName( const char* name )
{
	ASSERT_PTR( name );
	ASSERT( name[0] );
	for ( int i = 0 ; i < s_numPrebuilt ; i++ )
		if ( !stricmp( s_prebuilt[ i ].name, name ) )
			return i;
	ASSERTF( false, ( "character '%s' not found", name ) );
	return -1;
}

//============================================================================

int PlayerDB::CharacterIndexFromId( int id )
{
	for ( int i = 0 ; i < s_numPrebuilt ; i++ )
		if ( s_prebuilt[ i ].id == id )
			return i;
	ASSERTF( false, ( "character id=%d not found", id ) );
	return -1;
}

//============================================================================

int PlayerDB::CharacterId( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return s_prebuilt[ index ].id;
}

//============================================================================

bool PlayerDB::CharacterIsCustom( int index )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	return ( s_prebuilt[ index ].id & PLAYERDB_CUSTOM ) != 0;
}

//============================================================================

int PlayerDB::ChangeCharacter( int index, int change, int type  )
{
	int dir = ( change > 0 ) ? 1 : -1;
	int count = Math::Abs( change );

	while ( count )
	{
		index += dir;
		if ( index >= s_numPrebuilt )
			index = 0;
		else if ( index < 0 )
			index = s_numPrebuilt - 1;

		DBPrebuilt& pre = s_prebuilt[ index ];
		if ( pre.selectable && ( pre.type & type ) && ( !pre.lock || pre.lock->lock->unlocked ) )
			count--;
	}

	return index;
}

//============================================================================

void PlayerDB::SetCharacter( int index, const char* name, const char* initString, int type, bool selectable )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	ASSERT( s_prebuilt[ index ].id & PLAYERDB_CUSTOM );
	s_prebuilt[ index ].Set( name, name, initString, type, selectable, -1, 0 );
}

//============================================================================

void PlayerDB::SetCharacterName( int index, const char* name )
{
	ASSERT( index >= 0 && index < s_numPrebuilt );
	ASSERT( s_prebuilt[ index ].id & PLAYERDB_CUSTOM );
	s_prebuilt[ index ].SetName( name );
	s_prebuilt[ index ].SetDisplayName( name );
}

//============================================================================

	int PlayerDB::GetVersionID()
{
	return s_databaseVersion;
}

//============================================================================

char* PlayerDB::GetTextureFilename( int id )
{
	static char buf[ 16 ];
	sprintf( buf, "t%d.dbl", id );
	return buf;
}

//============================================================================

char* PlayerDB::GetTexturePrefix( int id )
{
	DBTexture* tex = FindTextureById( id );
	if ( tex )
		return tex->prefix->name;
	else
	{
		ASSERT( false );
		return NULL;
	}
}

//============================================================================

char* PlayerDB::GetPieceFilename( int id )
{
	static char buf[ 16 ];
	sprintf( buf, "p%d.dbl", id );
	return buf;
}

//============================================================================

char* PlayerDB::GetPieceSlot( int id )
{
	DBPiece* piece = FindPieceById( id );
	if ( piece )
		return piece->slot;
	else
	{
		ASSERT( false );
		return NULL;
	}
}

//============================================================================

bool PlayerDB::PieceHasLores( int id )
{
	DBPiece* piece = FindPieceById( id );
	return piece ? ( piece->hasLores != 0 ) : false;
}

//============================================================================

int PlayerDB::PieceLODReplacement( int id )
{
	DBPiece* piece = FindPieceById( id );
	return piece ? piece->lod : -1;
}

//============================================================================

PlayerDB::DBTexture* PlayerDB::FindTextureById( int id )
{
	int lo = 0;
	int hi = s_numTextures - 1;
	while ( lo <= hi )
	{
		int md = ( lo + hi ) / 2;
		if      ( s_textures[ md ].id < id )	lo = md + 1;
		else if ( s_textures[ md ].id > id )   hi = md - 1;
		else                                   return &s_textures[ md ];
	}
	return NULL;
}

//============================================================================

PlayerDB::DBPiece* PlayerDB::FindPieceById( int id )
{
	int lo = 0;
	int hi = s_numPieces - 1;
	while ( lo <= hi )
	{
		int md = ( lo + hi ) / 2;
		if      ( s_pieces[ md ].id < id )	lo = md + 1;
		else if ( s_pieces[ md ].id > id )	hi = md - 1;
		else											return &s_pieces[ md ];
	}
	return NULL;
}

//============================================================================

PlayerDB::DBMenu* PlayerDB::FindMenuById( int id )
{
	// menus aren't sorted by ID, so we can't binary search
	for ( int i = 0 ; i < s_numMenus ; i++ )
		if ( s_menus[ i ].id == id )
			return &s_menus[ i ];
	return NULL;
}

//============================================================================

PlayerDB::DBItem* PlayerDB::FindItemById( int id )
{
	// items aren't sorted by ID, so we can't binary search
	for ( int i = 0 ; i < s_numItems ; i++ )
		if ( s_items[ i ].id == id )
			return &s_items[ i ];
	return NULL;
}

//============================================================================

PlayerDB::DBLock* PlayerDB::FindLockById( int id )
{
	for ( int i = 0 ; i < s_numLocks ; i++ )
		if ( s_locks[ i ].id == id )
			return &s_locks[ i ];
	return NULL;
}

//============================================================================

PlayerDB::DBPrefix* PlayerDB::FindPrefixByName( const char* name )
{
	ASSERT_PTR( name );
	for ( int i = 0 ; i < s_numPrefixes ; i++ )
		if ( !stricmp( s_prefixes[ i ].name, name ) )
			return &s_prefixes[ i ];
	return NULL;
}

//============================================================================

int PlayerDB::MenuParent( int menuId )
{
	DBMenu* menu = FindMenuById( menuId );
	return ( menu ) ? menu->parentId : -1;
}

//============================================================================

char* PlayerDB::MenuName( int menuId )
{
	DBMenu* menu = FindMenuById( menuId );
	return ( menu ) ? menu->name : NULL;
}

//============================================================================

int PlayerDB::MenuNumChildren( int menuId )
{
	DBMenu* menu = FindMenuById( menuId );
	return ( menu ) ? menu->MenuCount() : 0;
}

//============================================================================

int PlayerDB::MenuChild( int menuId, int childIndex )
{
	DBMenu* menu = FindMenuById( menuId );
	if ( menu )
	{
		DBMenu* m = menu->GetMenu( childIndex );
		if ( m )
			return m->id;
	}
	ASSERT( false );
	return -1;
}

//============================================================================

int PlayerDB::MenuNumItems( int menuId )
{
	DBMenu* menu = FindMenuById( menuId );
	return ( menu ) ? menu->ItemCount( false ) : 0;
}

//============================================================================

int PlayerDB::MenuNumUnlockableItems( int menuId )
{
	DBMenu* menu = FindMenuById( menuId );
	return ( menu ) ? menu->ItemCount( true ) : 0;
}

//============================================================================

int PlayerDB::MenuItem( int menuId, int itemIndex )
{
	DBMenu* menu = FindMenuById( menuId );
	if ( menu )
	{
		DBItem* item = menu->GetItem( itemIndex );
		if ( item )
			return item->id;
	}
	ASSERT( false );
	return -1;
}

//============================================================================

char* PlayerDB::ItemName( int id )
{
	DBItem* item = FindItemById( id );
	return ( item ) ? item->name : NULL;
}

//============================================================================

void PlayerDB::ComposerInit( int type, const char* initString )
{
	// init composer
	ASSERT( s_composer == NULL );
	s_composer = new DBComposer;
	s_composer->choices = new DBComposerChoice[ s_numMenus ];
	s_composer->textures = new DBComposerTextures[ s_numTextures ];
	s_composer->type = type;

	// set initial choices
	ComposerSetChoices( initString );
}

//============================================================================

void PlayerDB::ComposerSetChoices( const char* initString )
{
	ParameterSet p( initString );
	int i;

	// convert "pieces=..." and "tex=..." into lists of pieces
	// and textures
	DynList<DBPiece> pieces( 64, 64, false );
	DynList<DBTexture> textures( 64, 64, false );
	char* pieceStr = p.GetString( "pieces" );
	while ( pieceStr )
	{
		DBPiece* piece = FindPieceById( atoi( pieceStr ) );
		if ( piece )
			pieces.Add( piece );
		if ( ( pieceStr = strchr( pieceStr, ',' ) ) != NULL ) pieceStr++;
	}
	char* texStr = p.GetString( "tex" );
	while ( texStr )
	{
		DBTexture* tex = FindTextureById( atoi( texStr ) );
		if ( tex )
			textures.Add( tex );
		if ( ( texStr = strchr( texStr, ',' ) ) != NULL ) texStr++;
	}

	// apply reverse excludes
	for ( i = 0 ; i < s_numReverseExcludes ; i++ )
		s_reverseExcludes[ i ].Apply( &pieces, &textures );
	
	for ( i = 0 ; i < s_numMenus ; i++ )
	{
		DBMenu* menu = &s_menus[ i ];

		ASSERT( !menu->childHead || !menu->itemHead );

		if ( menu->childHead )
		{
//			s_composer->choices[ i ].selected = MenuChild( menu->id, 0 );
			s_composer->choices[ i ].selected = -1;
		}
		else
		{
			// default selection
			int numItems = menu->ItemCount( false );
			if ( numItems == 0 )
			{
				s_composer->choices[ i ].selected = -1;
			}
			else
			{
				// default selection
				s_composer->choices[ i ].selected = menu->GetItem( 0 )->id;

				// find the best contributing match
				int bestCount = 0;
				for ( int j = 0 ; j < numItems ; j++ )
				{
					DBItem* item = menu->GetItem( j );
					int matchCount = item->BestMatch( pieces, textures );
					if ( matchCount > bestCount )
					{
						bestCount = matchCount;
						s_composer->choices[ i ].selected = item->id;
					}
				}

				if ( bestCount == 0 )
				{
					dbgPrint( "no good match: menu %d %s\n", menu->id, menu->name );
				}

				// DEBUG
				//{
				//	DBItem* select = menu->GetItemById( s_composer->choices[ i ].selected );
				//	dbgPrint( "menu %d '%s' : item %d '%s'\n", menu->id, menu->name, select->id, select->name );
				//}
			}
		}
	}

	s_composer->RefreshPrefixes();

	dbgPrint( "----- ORIGINAL STRING -----\n" );
	dbgPrint( "ORG: %s\n", initString );
	dbgPrint( "NEW: %s\n", ComposerGetInitString() );

#if 0
	// DEBUG - why don't these match?
	{
		ParameterSet p1( initString );
		ParameterSet p2( ComposerGetInitString() );
		char* p;
		int i;

		// make map of all used pieces
		char* pieceMap = new char[ s_numPieces ];
		memset( pieceMap, 0, s_numPieces );
		p = p1.GetString( "pieces" );
		while ( p && isdigit(*p) )
		{
			DBPiece* piece = FindPieceById( atoi( p ) );
			ASSERT_PTR( piece );
			pieceMap[ PieceIndexFromPtr( piece ) ] |= 1;
			p = strchr( p, ',' );  if (p) p++;
		}
		p = p2.GetString( "pieces" );
		while ( p && isdigit(*p) )
		{
			DBPiece* piece = FindPieceById( atoi( p ) );
			ASSERT_PTR( piece );
			pieceMap[ PieceIndexFromPtr( piece ) ] |= 2;
			p = strchr( p, ',' );  if (p) p++;
		}

		// make map of all used textures
		char* texMap = new char[ s_numTextures ];
		memset( texMap, 0, s_numTextures );
		p = p1.GetString( "tex" );
		while ( p && isdigit(*p) )
		{
			DBTexture* tex = FindTextureById( atoi( p ) );
			ASSERT_PTR( tex );
			texMap[ TextureIndexFromPtr( tex ) ] |= 1;
			p = strchr( p, ',' );  if (p) p++;
		}
		p = p2.GetString( "tex" );
		while ( p && isdigit(*p) )
		{
			DBTexture* tex = FindTextureById( atoi( p ) );
			ASSERT_PTR( tex );
			texMap[ TextureIndexFromPtr( tex ) ] |= 2;
			p = strchr( p, ',' );  if (p) p++;
		}

		int errors = 0;
		for ( i = 0 ; i < s_numPieces ; i++ )
		{
			if ( pieceMap[i] != 0 && pieceMap[i] != 3 )
			{
				DBPiece* piece = &s_pieces[i];
				errors++;
				dbgPrint( "- piece '%d' in slot %s only used by '%d'\n", piece->id, piece->slot, pieceMap[i] );
			}
		}
		for ( i = 0 ; i < s_numTextures ; i++ )
		{
			if ( texMap[i] != 0 && texMap[i] != 3 )
			{
				DBTexture* tex = &s_textures[i];
				errors++;
				dbgPrint( "- tex '%d' in prefix %s only used by '%d'\n", tex->id, tex->prefix->name, texMap[i] );
			}
		}

		delete [] pieceMap;
		delete [] texMap;
	}
#endif
}

//============================================================================

char* PlayerDB::ComposerGetInitString()
{
	int i;
	ASSERT_PTR( s_composer );

	// get a list of all pieces and textures selected
	DynList<DBPiece> pieces( 64, 64, false );
	DynList<DBTexture> textures( 64, 64, false );
	for ( i = 0 ; i < s_numMenus ; i++ )
	{
		DBMenu* menu = &s_menus[ i ];
		int selected = s_composer->choices[ i ].selected;
		if ( menu->ItemCount( false ) > 0 && selected >= 0 )
		{
			DBItem* item = menu->GetItemById( selected );
			if ( item )
			{
				for ( int j = 0 ; j < ITEM_COMMAND_SIZE ; j++ )
				{
					if ( ( item->command[ j ] & CMD_COMMAND_MASK ) == ( CMD_PIECE | CMD_ADD ) )
					{
						DBPiece* piece = FindPieceById( item->command[ j ] & CMD_ID_MASK );
						if ( pieces.Find( piece ) < 0 )
							pieces.Add( piece );
					}
					if ( ( item->command[ j ] & CMD_COMMAND_MASK ) == ( CMD_TEXTURE | CMD_ADD ) )
					{
						DBTexture* tex = FindTextureById( item->command[ j ] & CMD_ID_MASK );
						if ( textures.Find( tex ) < 0 )
							textures.Add( tex );
					}
				}
			}
		}
	}

	// determine voice set based on hand color, HACK HACK HACK
	const char* voiceset = NULL;
	{
		if ( textures.Find( FindTextureById( 27 ) ) >= 0 ) // ASIAN
		{
			if(g_random.InRange(0,1) == 0)
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "casian1";
				else
					voiceset = "gasian1";
			}
			else
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "casian2";
				else
					voiceset = "gasian2";
			}
		}
		else if ( textures.Find( FindTextureById( 30 ) ) >= 0 ) // LIGHT
		{
			if(g_random.InRange(0,1) == 0)
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cwhite1";
				else
					voiceset = "gwhite1";
			}
			else
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cwhite2";
				else
					voiceset = "gwhite2";
			}
		}
		else if ( textures.Find( FindTextureById( 32 ) ) >= 0 ) // MED
		{
			if(g_random.InRange(0,1) == 0)
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "chisp1";
				else
				{
					if(g_random.InRange(0,3) == 0)
						voiceset = "ghisp3";
					else
						voiceset = "ghisp1";
				}
			}
			else
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "chisp2";
				else
				{
					if(g_random.InRange(0,3) == 0)
						voiceset = "ghisp3";
					else
						voiceset = "ghisp2";
				}
			}
		}
		else if ( textures.Find( FindTextureById( 33 ) ) >= 0 ) // MEDDRK
		{
			if(g_random.InRange(0,1) == 0)
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cblack2";
				else
					voiceset = "gblack3";
			}
			else
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cblack3";
				else
					voiceset = "gblack4";
			}
		}
		else if ( textures.Find( FindTextureById( 36 ) ) >= 0 ) // RDARK
		{
			if(g_random.InRange(0,1) == 0)
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cblack1";
				else
					voiceset = "gblack1";
			}
			else
			{
				if(s_composer->type == PLAYERDB_COP)
					voiceset = "cblack2";
				else
					voiceset = "gblack2";
			}
		}
	}

	// apply exclusions
	for ( i = 0 ; i < s_numExcludes ; i++ )
		s_excludes[ i ].Apply( &pieces, &textures );

	// remove unnecessary textures
	for ( i = textures.Count() - 1 ; i >= 0 ; i-- )
	{
		bool used = false;
		for ( int j = 0 ; j < pieces.Count() ; j++ )
			if ( pieces[ j ]->HasPrefix( textures[ i ]->prefix ) )
				used = true;
		if ( !used )
			textures.Delete( i );
	}

	// build init string from options
	static char buf[ 256 ];
	char* p = buf;
	p += sprintf( p, "role=%s", ( s_composer->type == PLAYERDB_COP ) ? "cop" : "gang" );
	if ( voiceset )
		p += sprintf( p, ";voiceset=%s", voiceset );
	for ( i = 0 ; i < pieces.Count() ; i++ )
		p += sprintf( p, "%s%d", i == 0 ? ";pieces=" : ",", pieces[ i ]->id );
	for ( i = 0 ; i < textures.Count() ; i++ )
		p += sprintf( p, "%s%d", i == 0 ? ";tex=" : ",", textures[ i ]->id );
	for ( i = 0 ; i < textures.Count() ; i++ )
	{
		DBComposerTextures* c = &s_composer->textures[ TextureIndexFromPtr( textures[ i ] ) ];
		if ( c->hasShift )
		{
			ASSERT( c->hue        >= 0 && c->hue        <= 360  );
			ASSERT( c->saturation >= 0 && c->saturation <= 0xff );
			ASSERT( c->value      >= 0 && c->value      <= 0xff );
			p += sprintf( p, ";tex%d=%04x%02x%02x", textures[ i ]->id, c->hue, c->saturation, c->value );
		}
	}

	ASSERT( p < buf + sizeof(buf) );

	return buf;
}

//============================================================================

void PlayerDB::ComposerShutdown()
{
	if ( s_composer )
	{
		delete s_composer;
		s_composer = NULL;
	}
}

//============================================================================

void PlayerDB::ComposerSetMenu( const char* name )
{
	ASSERT_PTR( s_composer );
	s_composer->currentMenu = -1;
	for ( int i = 0 ; i < s_numMenus ; i++ )
		if ( !stricmp( s_menus[ i ].name, name ) )
			s_composer->currentMenu = i;
	ASSERT( s_composer->currentMenu >= 0 );
}

//============================================================================

void PlayerDB::ComposerSetMenu( int id )
{
	ASSERT_PTR( s_composer );
	s_composer->currentMenu = -1;
	for ( int i = 0 ; i < s_numMenus ; i++ )
		if ( s_menus[ i ].id == id )
			s_composer->currentMenu = i;
	ASSERT( s_composer->currentMenu >= 0 );
}

//============================================================================

void PlayerDB::ComposerSetColorshift( int h, int s, int v )
{
	DBItem*	item;
	DBMenu*	menu;

	ASSERT_PTR( s_composer );

	menu = &s_menus[ s_composer->currentMenu ];
	item = menu->GetItemById( s_composer->choices[ s_composer->currentMenu ].selected );
//	if ( item->numTextures != 0 )
//		for ( int i = 0 ; i < item->numTextures ; i++ )
//			s_composer->textures[ TextureIndexFromPtr( item->textures[ i ] ) ].SetHSV( h, s, v );
}

//============================================================================

bool PlayerDB::ComposerGetColorshift( int* h, int* s, int* v )
{
	DBItem*	item;
	DBMenu*	menu;

	ASSERT_PTR( s_composer );
	
	menu = &s_menus[ s_composer->currentMenu ];
	item = menu->GetItemById( s_composer->choices[ s_composer->currentMenu ].selected );
//	if ( item->numTextures != 0 )
//		return s_composer->textures[ TextureIndexFromPtr( item->textures[ 0 ] ) ].GetHSV( h, s, v );
//	else
		return false;
}

//============================================================================

int PlayerDB::ComposerGetIDByName( const char* name )
{
	ASSERT_PTR( s_composer );
	for ( int i = 0 ; i < s_numMenus ; i++ )
	{
		if ( !stricmp( s_menus[ i ].name, name ) )
		{
			return( s_composer->choices[ i ].selected );
			//return s_menus[ i ].id;
		}
	}
	
	return -1;//??
}

//============================================================================

bool PlayerDB::IsComposerInited()
{
	if( s_composer == NULL )
	{
		return false;
	}

	return true;
}

//============================================================================

int PlayerDB::ComposerMenu()
{
	ASSERT_PTR( s_composer );
	ASSERT( s_composer->currentMenu >= 0 );
	return s_menus[ s_composer->currentMenu ].id;
}

//============================================================================

int PlayerDB::ComposerSelected()
{
	ASSERT_PTR( s_composer );
	ASSERT( s_composer->currentMenu >= 0 );
	return s_composer->choices[ s_composer->currentMenu ].selected;
}

//============================================================================

void PlayerDB::ComposerSelect( int index )
{
	ASSERT_PTR( s_composer );
	ASSERT( s_composer->currentMenu >= 0 );
	s_composer->choices[ s_composer->currentMenu ].selected = index;
	s_composer->RefreshPrefixes();
}

//============================================================================

bool PlayerDB::DBItem::IsRelevant()
{
	ASSERT( s_composer );

	if ( !IsAvailable( false ) )
		return false;
	else if ( NumPieces() )
		return true;
	else if ( NumTextures() )
	{
		int relevant = 0;
		for ( int i = 0 ; i < ITEM_COMMAND_SIZE ; i++ )
		{
			if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_TEXTURE | CMD_ADD ) )
			{
				DBTexture* tex = FindTextureById( command[ i ] & CMD_ID_MASK );
				if ( s_composer->neededPrefixes.Find( tex->prefix ) >= 0 )
					relevant++;
			}
		}
		if ( ( showIfAll && relevant == NumTextures() ) || ( !showIfAll && relevant > 0 ) )
			return true;
	}
	return false;
}

//============================================================================

bool PlayerDB::DBMenu::IsRelevant()
{
	if ( MenuCount() )
		return true;

	int itemCount = ItemCount( false );
	for ( int i = 0 ; i < itemCount ; i++ )
		if ( GetItem( i )->IsRelevant() )
			return true;

	return false;
}

//============================================================================

bool PlayerDB::DBPiece::IsAvailable( bool allowLocked )
{
	ASSERT_PTR( s_composer );
	if ( ( s_composer->type & type ) == 0 )
		return false;
	if ( !allowLocked && lock && !lock->lock->unlocked )
		return false;
	return true;
}

//============================================================================

bool PlayerDB::DBTexture::IsAvailable( bool allowLocked )
{
	ASSERT_PTR( s_composer );
	if ( ( s_composer->type & type ) == 0 )
		return false;
	if ( !allowLocked && lock && !lock->lock->unlocked )
		return false;
	return true;
}

//============================================================================

bool PlayerDB::DBItem::IsAvailable( bool allowLocked )
{
	for ( int i = 0 ; i < ITEM_COMMAND_SIZE ; i++ )
	{
		if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_PIECE | CMD_ADD ) )
		{
			DBPiece* piece = FindPieceById( command[ i ] & CMD_ID_MASK );
			if ( !piece->IsAvailable( allowLocked ) )
				return false;
		}
		else if ( ( command[ i ] & CMD_COMMAND_MASK ) == ( CMD_TEXTURE | CMD_ADD ) )
		{
			DBTexture* texture = FindTextureById( command[ i ] & CMD_ID_MASK );
			if ( !texture->IsAvailable( allowLocked ) )
				return false;
		}
	}
	return true;
}

//============================================================================

int PlayerDB::PieceIndexFromPtr( DBPiece* piece )
{
	int index = piece - s_pieces;
	ASSERT( index >= 0 && index < s_numPieces );
	ASSERT( piece == &s_pieces[ index ] );
	return index;
}

//============================================================================

int PlayerDB::TextureIndexFromPtr( DBTexture* tex )
{
	int index = tex - s_textures;
	ASSERT( index >= 0 && index < s_numTextures );
	ASSERT( tex == &s_textures[ index ] );
	return index;
}

//============================================================================

int PlayerDB::GetPrebuiltByDefaultSlot( int type, int slot )
{
	for ( int i = 0 ; i < s_numPrebuilt ; i++ )
	{
		if ( s_prebuilt[ i ].type == type && s_prebuilt[ i ].def == slot )
			return i;
	}

	ASSERTF( false, ( "prebuilt default type=%d slot=%d not found", type, slot ) );
	return -1;
}

//============================================================================
