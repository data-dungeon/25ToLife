
#include "Components/ComponentsPCH.h"

#include "Components/Property.h"

#include "GameHelper/Handle.h"

Property::Property( CActor &i_actor ) :
	actor( i_actor )
{
	needUpdate = true;
	updateFunc = NULL;
	dataLocation = NULL;
	type = Property_unknown;
	strcpy( name, "" );

	ownData = false;
}

void Property::Update( void )
{
	if( needUpdate && updateFunc )
	{
		if( updateFunc( actor, this ) )
			needUpdate = false;
	}
}

float Property::GetFloat( void )
{
	ASSERT( type == Property_float );
	ASSERT( dataLocation );

	Update();
	if( type != Property_float )
		return 0.0f;

	return *((float *)dataLocation);
}

int Property::GetInt( void )
{
	ASSERT( type == Property_int );
	ASSERT( dataLocation );

	Update();
	if( type != Property_int )
		return 0;

	return *((int *)dataLocation);
}

Vector3 Property::GetVector3( void )
{
	ASSERT( type == Property_Vector3 );
	ASSERT( dataLocation );

	Update();
	if( type != Property_Vector3 )
		return Vector3( 0.0f, 0.0f, 0.0f );

	return *((Vector3 *)dataLocation);
}

DirCos3x3 Property::GetDirCos3x3( void )
{
	ASSERT( type == Property_DirCos3x3 );
	ASSERT( dataLocation );

	Update();
	if( type != Property_DirCos3x3 )
	{
		DirCos3x3 m;
		m.Identity();
		return m;
	}

	return *((DirCos3x3 *)dataLocation);
}

bool Property::GetBool( void )
{
	ASSERT( type == Property_bool );
	ASSERT( dataLocation );

	Update();
	if( type != Property_bool )
		return 0;

	return *((bool *)dataLocation);
}

void *Property::GetPointer( void )
{
	ASSERT( type == Property_pointer );
	ASSERT( dataLocation );

	Update();
	if( type != Property_pointer )
		return NULL;

	return *((void **)dataLocation);
}

uint32 Property::GetHandle( void )
{
	ASSERT( type == Property_handle );
	ASSERT( dataLocation );

	Update();
	if( type != Property_handle )
		return INVALID_OBJECT_HANDLE;

	return *((uint32 *)dataLocation);
}

void Property::Set( float f )
{
	ASSERT( type == Property_float );
	ASSERT( dataLocation );

	if( type == Property_float )
		*((float *)dataLocation) = f;
}

void Property::Set( int i )
{
	ASSERT( type == Property_int );
	ASSERT( dataLocation );

	if( type == Property_int )
		*((int *)dataLocation) = i;
}

void Property::Set( bool b )
{
	ASSERT( type == Property_bool );
	ASSERT( dataLocation );

	if( type == Property_bool )
		*((bool *)dataLocation) = b;
}

void Property::Set( Vector3 v )
{
	ASSERT( type == Property_Vector3 );
	ASSERT( dataLocation );

	if( type == Property_Vector3 )
		*((Vector3 *)dataLocation) = v;
}

void Property::Set( DirCos3x3 m )
{
	ASSERT( type == Property_DirCos3x3 );
	ASSERT( dataLocation );

	if( type == Property_DirCos3x3 )
		*((DirCos3x3 *)dataLocation) = m;
}

void Property::Set( void *v )
{
	ASSERT( type == Property_pointer );
	ASSERT( dataLocation );

	if( type == Property_pointer )
		*((void **)dataLocation) = v;
}

void Property::Set( uint32 h )
{
	ASSERT( type == Property_handle );
	ASSERT( dataLocation );

	if( type == Property_handle )
		*((uint32 *)dataLocation) = h;
}

PropertyList::PropertyList() :
propertyTable( 3, 4, 0 )
{
}

void PropertyList::AddProperty( CActor &actor, const char *name, PropertyType type, void *dataLocation, PropertyUpdateFunc *updateFunc )
{
	ASSERT( strlen( name ) < 16 );
	if( HasProperty( name ) )
		return;

	Property *p = new Property( actor );
	strcpy( p->name, name );
	p->dataLocation = dataLocation;
	p->updateFunc = updateFunc;
	p->type = type;

	if( updateFunc )
		p->needUpdate = true;
	else
		p->needUpdate = false;

	if( p->dataLocation == NULL )
	{
		p->ownData = true;
		switch( type )
		{
			case Property_float:
				p->dataLocation = new float;
				break;
			case Property_int:
				p->dataLocation = new int;
				break;
			case Property_bool:
				p->dataLocation = new bool;
				break;
			case Property_Vector3:
				p->dataLocation = new Vector3;
				break;
			case Property_DirCos3x3:
				p->dataLocation = new DirCos3x3;
				break;
			case Property_pointer:
				p->dataLocation = new (void *);
				break;
			case Property_handle:
				p->dataLocation = new uint32;
				break;
		}
	}

	properties.Add( p );
	propertyTable.Add( name, p, p->name );
}

bool PropertyList::HasProperty( const char *name )
{
	if( GetProperty( name, true ) )
		return true;
	return false;
}

Property *PropertyList::GetProperty( const char *name, bool justchecking )
{
	Property **p;

	ASSERT( strlen( name ) < 16 );

	p = propertyTable.Get( name );
	if( p )
		return *p;
	
	// if we're just checking to see if its there, don't assert
	if( !justchecking )
		ASSERTS( 0, "Unable to find property in propertylist" );

	return NULL;
}

void PropertyList::RemoveAll( void ) 
{ 
	Property *p;
	int i;
	for( i = 0; i < properties.Count(); i++ )
	{
		p = properties.Get( i );
		propertyTable.Remove( p->name );
	}
	properties.Clear(); 
}

Property* PropertyList::QueryProperty( const char *name )
{
	return GetProperty( name, true );
}
