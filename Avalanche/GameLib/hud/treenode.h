#ifndef _TREENODE_H_
#define _TREENODE_H_

//=============================================================================
// TreeNode : <class description>
//=============================================================================

class TreeNode
{
	public:
		TreeNode( void );
		virtual ~TreeNode();

	public:
		void SetParent( TreeNode *pParent );
		void SetChild( TreeNode *pChild );
		void AddChild( TreeNode *pChild );
		void Insert( TreeNode *pObject );
		void InsertAfter( TreeNode *pObject );
		void Remove( void );
		void SetObjectId( int Id );

		/////////////////////////////////////////////////////////////////////////

		void SetNext( TreeNode* pObject ) { InsertAfter( pObject ); }
		void SetPrev( TreeNode* pObject ) { Insert     ( pObject ); }

		/////////////////////////////////////////////////////////////////////////

		void AdoptChildren( TreeNode* pParent );

		/////////////////////////////////////////////////////////////////////////

		inline int ObjectId( void )		{ return m_ObjectId; } //
		inline TreeNode *Parent( void )	{ return m_pParent;  } //
		inline TreeNode *Child( void )	{ return m_pChild;   } // want inlined 
		inline TreeNode *Next( void )		{ return m_pNext;    } //
		inline TreeNode *Prev( void )		{ return m_pPrev;    } //

		//////////////////////////////////////////////////////////////////////////

		TreeNode* Parent( int ObjectId );
		TreeNode* Child( int ObjectId );
		TreeNode* Next( int ObjectId );
		TreeNode* Prev( int ObjectId );

		//////////////////////////////////////////////////////////////////////////
		
		TreeNode* GetLastChild( void );
		TreeNode* GetNthChild( int n );
		TreeNode* FindChild( int ObjectId );		

		//////////////////////////////////////////////////////////////////////////
		
		int GetNumChildren( void );
		int GetNumChildren( int ObjectId );
		int GetAllChildren( void );
		int GetNumParents( void );
		int GetTreePosition( void );

	public:
		TreeNode *m_pParent;
		TreeNode *m_pChild;
		TreeNode *m_pNext;
		TreeNode *m_pPrev;
		int		 m_ObjectId;
};

#define DECLARE_TREENODE( ThisClass )\
	public:\
		inline ThisClass* Parent( void ) { return (ThisClass*) TreeNode::Parent(); }\
		inline ThisClass* Child( void )  { return (ThisClass*) TreeNode::Child();  }\
		inline ThisClass* Next( void )   { return (ThisClass*) TreeNode::Next();   }\
		inline ThisClass* Prev( void )   { return (ThisClass*) TreeNode::Prev();   }\
		inline ThisClass* Parent( int ObjectId ) { return (ThisClass*) TreeNode::Parent(ObjectId); }\
		inline ThisClass* Child( int ObjectId )  { return (ThisClass*) TreeNode::Child(ObjectId);  }\
		inline ThisClass* Next( int ObjectId )   { return (ThisClass*) TreeNode::Next(ObjectId);   }\
		inline ThisClass* Prev( int ObjectId )   { return (ThisClass*) TreeNode::Prev(ObjectId);   }\
		inline ThisClass* GetLastChild( void )   { return (ThisClass*) TreeNode::GetLastChild();   }\
		inline ThisClass* GetNthChild( int n )   { return (ThisClass*) TreeNode::GetNthChild(n);   }\
		inline ThisClass* FindChild( int ObjectId ) { return (ThisClass*) TreeNode::FindChild(ObjectId); }

#endif // _TREENODE_H_
