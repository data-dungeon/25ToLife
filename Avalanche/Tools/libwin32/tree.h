#ifndef _CTREENODE_H_
#define _CTREENODE_H_

//=============================================================================
// CTreeNode : <class description>
//=============================================================================

class CTreeNode
{
	public:
		CTreeNode( void );
		virtual ~CTreeNode();

	public:
		void SetParent( CTreeNode *pParent );
		void SetChild( CTreeNode *pChild );
		void AddChild( CTreeNode *pChild );
		void Insert( CTreeNode *pObject );
		void InsertAfter( CTreeNode *pObject );
		void Remove( void );
		void SetObjectId( int Id );

		/////////////////////////////////////////////////////////////////////////

		void SetNext( CTreeNode* pObject ) { InsertAfter( pObject ); }
		void SetPrev( CTreeNode* pObject ) { Insert     ( pObject ); }

		/////////////////////////////////////////////////////////////////////////

		void AdoptChildren( CTreeNode* pParent );

		/////////////////////////////////////////////////////////////////////////

		inline int ObjectId( void )		{ return m_ObjectId; } //
		inline CTreeNode *Parent( void )	{ return m_pParent;  } //
		inline CTreeNode *Child( void )	{ return m_pChild;   } // want inlined 
		inline CTreeNode *Next( void )	{ return m_pNext;    } //
		inline CTreeNode *Prev( void )	{ return m_pPrev;    } //

		//////////////////////////////////////////////////////////////////////////

		CTreeNode* Parent( int ObjectId );
		CTreeNode* Child( int ObjectId );
		CTreeNode* Next( int ObjectId );
		CTreeNode* Prev( int ObjectId );

		//////////////////////////////////////////////////////////////////////////
		
		CTreeNode* GetLastChild( void );
		CTreeNode* GetNthChild( int n );
		CTreeNode* FindChild( int ObjectId );		

		//////////////////////////////////////////////////////////////////////////
		
		int GetNumChildren( void );
		int GetNumChildren( int ObjectId );
		int GetAllChildren( void );
		int GetNumParents( void );
		int GetTreePosition( void );

	public:
		CTreeNode *m_pParent;
		CTreeNode *m_pChild;
		CTreeNode *m_pNext;
		CTreeNode *m_pPrev;
		int		 m_ObjectId;
};

#define DECLARE_CTREENODE( ThisClass )\
	public:\
		inline ThisClass* Parent( void ) { return (ThisClass*) CTreeNode::Parent(); }\
		inline ThisClass* Child( void )  { return (ThisClass*) CTreeNode::Child();  }\
		inline ThisClass* Next( void )   { return (ThisClass*) CTreeNode::Next();   }\
		inline ThisClass* Prev( void )   { return (ThisClass*) CTreeNode::Prev();   }\
		inline ThisClass* Parent( int ObjectId ) { return (ThisClass*) CTreeNode::Parent(ObjectId); }\
		inline ThisClass* Child( int ObjectId )  { return (ThisClass*) CTreeNode::Child(ObjectId);  }\
		inline ThisClass* Next( int ObjectId )   { return (ThisClass*) CTreeNode::Next(ObjectId);   }\
		inline ThisClass* Prev( int ObjectId )   { return (ThisClass*) CTreeNode::Prev(ObjectId);   }\
		inline ThisClass* GetLastChild( void )   { return (ThisClass*) CTreeNode::GetLastChild();   }\
		inline ThisClass* GetNthChild( int n )   { return (ThisClass*) CTreeNode::GetNthChild(n);   }\
		inline ThisClass* FindChild( int ObjectId ) { return (ThisClass*) CTreeNode::FindChild(ObjectId); }

//=============================================================================
// NamedNode : <class description>
//=============================================================================

#include "tstring.h"

class NamedNode : public CTreeNode
{
	DECLARE_CTREENODE( NamedNode );

	public:
		void SetObjectName( const char* pName );
		const char* ObjectName( void );
	
		//////////////////////////////////////////////////////////////////////////
		
		NamedNode* FindChildByName( const char* pName );

	public:
		tstring m_ObjectName;
};

#endif // _CTREENODE_H_