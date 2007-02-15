#ifndef _PROPERTYHANDLER_H_
#define _PROPERTYHANDLER_H_

class PropertyRecord : public CTreeNode
{
	DECLARE_CTREENODE( PropertyRecord );
	
	public:
		PropertyRecord( void );
		PropertyRecord( const char* pKey, const char* pValue, int PropertyId );
		void Initialize( const char* pKey, const char* pValue, int PropertyId );

		tstring m_Key;
		tstring m_Value;
};

class PropertyHandler : public CTreeNode
{
	DECLARE_CLASSOBJECT( PropertyHandler );
	DECLARE_CTREENODE( PropertyHandler );

	public:
		PropertyHandler( void );
		virtual const char* GetKnownClass( void );

		virtual void SetKnownObject( void* pKnownObject );
		virtual void* GetKnownObject( void );
		virtual void SetProperty( const char* pKey, const char* pValue, int PropertyId );

		void AddProperty( const char* pKey, int PropertyId );
		void AddProperty( PropertyRecord* pProperty );
		void Branch( void );
		void Return( void );

		PropertyRecord* m_pTopProperty;
		PropertyRecord* m_pLastProperty;
		PropertyRecord m_Properties;

		void* m_pKnownObject;
};

#endif // _PROPERTYHANDLER_H_