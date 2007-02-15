#include "pch.h"
#include "propertyhandler.h"

PropertyRecord::PropertyRecord( void )
{
}

PropertyRecord::PropertyRecord( const char* pKey, const char* pValue, int PropertyId )
{
	Initialize( pKey, pValue, PropertyId );
}

void PropertyRecord::Initialize( const char* pKey, const char* pValue, int PropertyId )
{
	m_Key.set( pKey );
	m_Value.set( pValue );
	SetObjectId( PropertyId );
}

PropertyHandler::PropertyHandler( void )
{
	m_pTopProperty = &m_Properties;
	m_pLastProperty = 0;
}

const char* PropertyHandler::GetKnownClass( void )
{
	return PropertyHandler::ClassnameStatic();
}

void PropertyHandler::SetKnownObject( void* pKnownObject )
{
	m_pKnownObject = pKnownObject;
}

void* PropertyHandler::GetKnownObject( void )
{
	return m_pKnownObject;
}

void PropertyHandler::SetProperty( const char* pKey, const char* pValue, int PropertyId )
{
}

void PropertyHandler::AddProperty( const char* pKey, int PropertyId )
{
	AddProperty( new PropertyRecord( pKey, 0, PropertyId ) );
}

void PropertyHandler::AddProperty( PropertyRecord* pProperty )
{
	if( pProperty )
	{
		if( m_pTopProperty )
			m_pTopProperty->AddChild( pProperty );

		m_pLastProperty = pProperty;
	}

	return;
}

void PropertyHandler::Branch( void )
{
	m_pTopProperty = m_pLastProperty;
}

void PropertyHandler::Return( void )
{
	if( m_pTopProperty->Parent() )
		m_pTopProperty = m_pTopProperty->Parent();

	return;
}