#ifndef __PROPERTY_H_INCLUDED__
#define __PROPERTY_H_INCLUDED__

#include "container/dynlist.h"
#include "container/LinearHash.h"

typedef enum
{
	Property_unknown = -1,
	Property_int = 0,
	Property_float,
	Property_Vector3,
	Property_bool,
	Property_DirCos3x3,
	Property_pointer,
	Property_handle,
} PropertyType;

class Property;
typedef bool *PropertyUpdateFunc( CActor &, Property * );

class Property
{
public:
	Property( CActor &i_actor );
	~Property() { if( ownData ) delete (char*) dataLocation; }

	float GetFloat( void );
	int GetInt( void );
	Vector3 GetVector3( void );
	bool GetBool( void );
	DirCos3x3 GetDirCos3x3( void );
	void *GetPointer( void );
	uint32 GetHandle( void );

	void Set( float f );
	void Set( int i );
	void Set( bool b );
	void Set( Vector3 v );
	void Set( DirCos3x3 m );
	void Set( void *v );
	void Set( uint32 h );

	void Update( void );

	CActor &actor;
	bool needUpdate;
	PropertyUpdateFunc *updateFunc;
	void *dataLocation;
	PropertyType type;
	char name[16];

	// do we own the data location?
	bool ownData;
};

class PropertyList
{
public:
	PropertyList();

	void AddProperty( CActor &actor, const char *name, PropertyType type, void *dataLocation, PropertyUpdateFunc *updateFunc );
	bool HasProperty( const char *name );
	Property *GetProperty( const char *name, bool justchecking = false );
	Property *QueryProperty( const char *name );

	void RemoveAll( void );

private:
	DynList<Property> properties;
	CLinearHash<Property *> propertyTable;
};

#endif __PROPERTY_H_INCLUDED__
