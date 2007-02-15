#ifndef _HUDOBJECT_H_
#define _HUDOBJECT_H_

#include "hud/memorybank.h"
#include "hud/treenode.h"

//=============================================================================
// DECLARE_CLASSOBJECT defines several functions for your class.
//
//=============================================================================
//		char *PRIVATECLASS() // Returns the name of the class.  This member is
//									// made private to enforce type checking.
//=============================================================================
//		char *Classname()    // Returns the name of the class by calling
//									// PRIVATECLASS().  These two functions work
//									// together to ensure a bad class names in the
//									// DECLARE_CLASSOBJECT macro will generate a
//									// compiler error.
//=============================================================================
//		void * New()			// Returns a pointer to a new object.
//									// The object is created by calling the new operator.
//=============================================================================
//		int Sizeof()			// Returns the size of the object, in bytes.
//=============================================================================
//		New##classname()		// Returns a pointer to a new object.  This function
//									// is declared static and used in combination with
//									// the NEW_RUNTIMEE_CLASSOBJECT macro to create the
//									// object.  The object is created by calling the
//									// new opeartor.
//=============================================================================

#define DECLARE_CLASSOBJECT( classname )\
	private: char * PRIVATECLASS( void ) { return ClassnameStatic(); };\
	public:  static  char* ClassnameStatic( void ) { return (char *)(classname *) #classname; };\
	public:  virtual char * Classname( void ) { return classname::PRIVATECLASS(); };\
	public:  virtual void * New( void ) { return classname::New##classname(); };\
	public:  virtual int Sizeof( void ) { return sizeof(classname); };\
	public:  static  void * New##classname( void ) { return new classname; };\

//=============================================================================
// RUNTIME_CLASSOBJECT gives you a pointer to the New##classname function.
// The return value is a NEWCLASSOBJECTCALLBACK( void ).
// This function is normally invoked by calling NEW_RUNTIME_CLASSOBJECT.
// *This macro will fail to compile if DECLARE_CLASSOBJECT is not included in
// the class declaration.
//=============================================================================

typedef void * (*NEWCLASSOBJECTCALLBACK)( void );
#define RUNTIME_CLASSOBJECT( classname ) classname::New##classname

//=============================================================================
// NEW_RUNETIME_CLASSOBJECT invokes the New##classname function for that class.
// *This macro will fail to compile if DECLARE_CLASSOBJECT is not included in
// the class declaration.
//=============================================================================

#define NEW_RUNTIME_CLASSOBJECT( classname ) RUNTIME_CLASSOBJECT( classname )()

//=============================================================================
// ClassRecord
//=============================================================================

class ClassRecord : public TreeNode
{
	DECLARE_TREENODE( ClassRecord );

	public:
		ClassRecord( void );
		ClassRecord( const char *pClassName, NEWCLASSOBJECTCALLBACK CallBack );

	public:
		static void RegisterClass( ClassRecord &ClassFactory, const char *pClassName, NEWCLASSOBJECTCALLBACK CallBack );

	public:
		xstring m_Classname;
		NEWCLASSOBJECTCALLBACK m_CallBack;
};

//==================================================================================
// DECLARE_CLASSFACTORY       Use this macro in your class definition if you want
//                            the ability to register class types and create them
//                            on the fly.
//==================================================================================
//    RegisterClass()         This will register the class.  It takes the callback
//                            function returned by the RUNTIME_CLASSOBJECT macro.
//
//    UnregisterClass()       This will remove the class from the registry. It takes
//                            the function returned by the RUNTIME_CLASSOBJECT macro.
//
//    UnregisterAllClasses()  Removes all class registrations from memory.
//
//    Newobject()             Creates an object of the type you specify.
//==================================================================================

#define DECLARE_CLASSFACTORY( classname )\
	public:\
		static ClassRecord m_ClassRecords;\
		static ClassRecord* FindClassRecord( const char* pClassname );\
		static ClassRecord* FindClassRecord( NEWCLASSOBJECTCALLBACK CallBack );\
		static void RegisterClass( NEWCLASSOBJECTCALLBACK CallBack, bool TrueOrFalse = true );\
		static void UnregisterClass( NEWCLASSOBJECTCALLBACK CallBack );\
		static void UnregisterAllClasses( void );\
		static classname* NewObject( const char *pClassname );

//=============================================================================
// MAKE_CLASSFACTORY - This macro implements the functions declared by the
//							  DECLARE_CLASSFACTORY macro.
//=============================================================================

#define MAKE_CLASSFACTORY( classname )\
	ClassRecord classname::m_ClassRecords;\
	ClassRecord* classname::FindClassRecord( const char* pClassname )\
	{\
		ClassRecord *pClassRecord = m_ClassRecords.Child();\
		while( pClassRecord )\
		{\
			if( pClassRecord->m_Classname.compare( pClassname ) )\
				return pClassRecord;\
			pClassRecord = (ClassRecord*) pClassRecord->Next();\
		}\
		return 0;\
	}\
	ClassRecord* classname::FindClassRecord( NEWCLASSOBJECTCALLBACK CallBack )\
	{\
		ClassRecord* pClassRecord = m_ClassRecords.Child();\
		while( pClassRecord )\
		{\
			if( pClassRecord->m_CallBack == CallBack )\
				return pClassRecord;\
			pClassRecord = (ClassRecord*) pClassRecord->Next();\
		}\
		return 0;\
	}\
	void classname::RegisterClass( NEWCLASSOBJECTCALLBACK CallBack, bool TrueOrFalse )\
	{\
		if( !TrueOrFalse )\
			UnregisterClass( CallBack );\
		else\
		{\
			classname* pNew = (classname*) CallBack();\
			if( pNew )\
			{\
				ClassRecord::RegisterClass( m_ClassRecords, pNew->Classname(), CallBack );\
				delete pNew;\
			}\
		}\
	}\
	void classname::UnregisterClass( NEWCLASSOBJECTCALLBACK CallBack )\
	{\
		ClassRecord* pClassRecord = FindClassRecord( CallBack );\
		if( pClassRecord )\
			delete pClassRecord;\
	}\
	void classname::UnregisterAllClasses( void )\
	{\
		while( m_ClassRecords.Child() )\
			delete m_ClassRecords.Child();\
	}\
	classname* classname::NewObject( const char *pClassname )\
	{\
		ClassRecord* pClassRecord = FindClassRecord( pClassname );\
		if( pClassRecord )\
			return (classname*) pClassRecord->m_CallBack();\
		return 0;\
	}

//=============================================================================
// HudObject : This is the base class for all hud objects.
//=============================================================================

class HudObject : public TreeNode
{
	DECLARE_CLASSOBJECT( HudObject );
	DECLARE_CLASSFACTORY( HudObject );
	DECLARE_TREENODE( HudObject );

	public:
		HudObject( void );
		virtual ~HudObject();

	public:	
		virtual void Update( void );		
		virtual bool SaveObject( MemoryBank &m );
		virtual bool LoadObject( MemoryBank &m );

	#ifdef _WIN32
		void *	m_pToolData;
	#endif // _WIN32
};


#endif // _HUDOBJECT_H_