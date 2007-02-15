#include "hud/HudPCH.h"
#include "hud/treenode.h"

//=============================================================================
// TreeNode::TreeNode() - The constructor initializes all the data members
//								  and records itself in the master list.
//=============================================================================

TreeNode::TreeNode()
{
	m_pParent = 0;
	m_pChild  = 0;
	m_pNext   = 0;
	m_pPrev   = 0;
	m_ObjectId = 0;
}

//=============================================================================
// TreeNode::~TreeNode() - The destructor removes the node and its children
//									from the hierarchy.  Also removes the object from
//									the master list.
//=============================================================================

TreeNode::~TreeNode()
{
	Remove();

	if( m_pChild )
		m_pChild->Remove();

	return;
}

//=============================================================================
// TreeNode::SetParent() - Removes this node from its current position in the
//									hierarchy and attaches it to the new parent,
//									updating pointers as necessary.
//=============================================================================

void TreeNode::SetParent( TreeNode *pParent )
{
	Remove();

	if( pParent )
		pParent->AddChild( this );

	return;
}

//=============================================================================
// TreeNode::SetChild() - Removes a node from its current position in the
//								  hierarchy and attaches it to this node as the first
//								  child.  Pointers are updated as necessary.
//=============================================================================

void TreeNode::SetChild( TreeNode *pChild )
{
	if( m_pChild )
		m_pChild->Insert( pChild );
	else
		AddChild( pChild );

	return;
}

//=============================================================================
// TreeNode::AddChild() - Removes a node from its current position in the
//								  hierarchy and attaches it to this node updating
//								  pointers as necessary.
//=============================================================================

void TreeNode::AddChild( TreeNode *pChild )
{
	if( pChild && pChild != this )
	{
		pChild->Remove();

		if( !m_pChild )
			m_pChild = pChild;
		else
		{
			TreeNode *pObject = m_pChild;

			while( pObject->m_pNext )
				pObject = pObject->m_pNext;

			pObject->InsertAfter( pChild );
		}

		pChild->m_pParent = this;
	}

	return;
}

//=============================================================================
// TreeNode::Insert() - Inserts the specified object so that it comes BEFORE
//								this object.
//=============================================================================

void TreeNode::Insert( TreeNode *pObject )
{
	if( pObject && pObject != this )
	{
		pObject->Remove();

		pObject->m_pParent = m_pParent;
		pObject->m_pPrev   = m_pPrev;
		pObject->m_pNext   = this;

		if( m_pPrev )
			m_pPrev->m_pNext = pObject;

		m_pPrev = pObject;

		if( m_pParent )
		{
			while( m_pParent->m_pChild->m_pPrev )
				m_pParent->m_pChild = m_pParent->m_pChild->m_pPrev;
		}
	}

	return;
}

//=============================================================================
// TreeNode::InsertAfter() - Inserts the specified object so that it comes
//									  AFTER this object.
//=============================================================================

void TreeNode::InsertAfter( TreeNode *pObject )
{
	if( pObject && pObject != this )
	{
		pObject->Remove();
		pObject->m_pParent = m_pParent;
		pObject->m_pNext   = m_pNext;
		pObject->m_pPrev   = this;

		if( m_pNext )
			m_pNext->m_pPrev = pObject;

		m_pNext = pObject;
	}

	return;
}

//=============================================================================
// TreeNode::Remove() - Removes this object and its children from the hierarchy.
//								Adjacent objects are updated accordingly.
//=============================================================================

void TreeNode::Remove( void )
{
	if( m_pParent )
	{
		if( m_pParent->m_pChild == this )
			m_pParent->m_pChild = m_pNext;
	}

	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;

	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;

	m_pParent = 0;
	m_pNext = 0;
	m_pPrev = 0;
}

//=============================================================================
// TreeNode::SetObjectId() - Give the object an id.
//=============================================================================

void TreeNode::SetObjectId( int Id )
{
	m_ObjectId = Id;
}

//=============================================================================
// TreeNode::AdoptChildren()
//=============================================================================

void TreeNode::AdoptChildren( TreeNode* pParent )
{
	TreeNode* pLastChild = GetLastChild();
	TreeNode* pChild;

	if( pParent )
	{
		pChild = pParent->Child();

		if( pChild )
		{
			if( !pLastChild )
				m_pChild = pChild;
			else
			{
				pLastChild->m_pNext = pChild;
				pChild->m_pPrev = pLastChild;
			}

			while( pChild )
			{
				pChild->m_pParent = this;
				pChild = pChild->Next();
			}

			pParent->m_pChild = 0;
		}
	}

	return;
}

//=============================================================================
// TreeNode::Parent - Returns a pointer to the first parent with the specified
//							 object id.
//=============================================================================

TreeNode* TreeNode::Parent( int ObjectId )
{
	TreeNode* pParent = Parent();

	while( pParent && pParent->ObjectId() != ObjectId )
		pParent = pParent->Parent();

	return pParent;
}

//=============================================================================
// TreeNode::Child - Returns a pointer to the first child with the specified
//						   object id.
//=============================================================================

TreeNode* TreeNode::Child( int ObjectId )
{
	TreeNode* pChild = Child();

	while( pChild && pChild->ObjectId() != ObjectId )
		pChild = pChild->Next();

	return pChild;
}

//=============================================================================
// TreeNode::Next - Returns a pointer to the next object with the specified
//						  object id.
//=============================================================================

TreeNode* TreeNode::Next( int ObjectId )
{
	TreeNode* pNext = Next();

	while( pNext && pNext->ObjectId() != ObjectId )
		pNext = pNext->Next();

	return pNext;
}

//=============================================================================
// TreeNode::Prev - Returns a pointer to the previous object with the specified
//						  object id.
//=============================================================================

TreeNode* TreeNode::Prev( int ObjectId )
{
	TreeNode* pPrev = Prev();

	while( pPrev && pPrev->ObjectId() != ObjectId )
		pPrev = pPrev->Prev();

	return pPrev;
}

//=============================================================================
// TreeNode::GetLastChild() - Returns a pointer to the last child, or 0 if none
//										exists.
//=============================================================================

TreeNode *TreeNode::GetLastChild( void )
{
	TreeNode *pObject = m_pChild;

	if( pObject )
	{
		while( pObject->m_pNext )
			pObject = pObject->m_pNext;
	}

	return pObject;
}

//=============================================================================
// TreeNode::GetNthChild() - Returns a pointer to the Nth child, or 0 if the
//									  child does not exist.
//=============================================================================

TreeNode *TreeNode::GetNthChild( int n )
{
	if( n < 0 )
		return 0;

	TreeNode *pObject = m_pChild;

	for( int i = 0; i < n; i++ )
	{
		if( pObject )
			pObject = pObject->m_pNext;
	}

	return pObject;
}

//=============================================================================
// TreeNode::FindChild() - Returns a pointer to the child who matches the
//									object id.
//=============================================================================

TreeNode * TreeNode::FindChild( int ObjectId )
{
	TreeNode *pChild = Child();

	while( pChild )
	{
		if( pChild->m_ObjectId == ObjectId )
			return pChild;

		pChild = pChild->Next();
	}

	return 0;
}

//=============================================================================
// TreeNode::GetNumChildren() - Returns the number of children immediately
//										  connected to this object.  Sub-children are not
//										  considered.
//=============================================================================

int TreeNode::GetNumChildren( void )
{
	int i;

	TreeNode *pObject = m_pChild;

	for( i = 0; pObject; i++ )
		pObject = pObject->m_pNext;

	return i;
}

//=============================================================================
// TreeNode::GetNumChildren() - Returns the number of children immediately
//										  connected to this object matching the specified
//										  object id.
//=============================================================================

int TreeNode::GetNumChildren( int ObjectId )
{
	int i, c;

	TreeNode *pObject = m_pChild;

	for( i = c = 0; pObject; i++ )
	{
		if( pObject->m_ObjectId == ObjectId )
			c++;

		pObject = pObject->m_pNext;
	}

	return c;
}

//=============================================================================
// TreeNode::GetAllChildren() - Returns the number of children connected to
//										  this object.  Sub-children are considered.
//=============================================================================

int TreeNode::GetAllChildren( void )
{
	int i;

	TreeNode *pObject = m_pChild;

	for( i = 0; pObject; pObject = pObject->m_pNext )
		i += 1 + pObject->GetAllChildren();

	return i;
}

//=============================================================================
// TreeNode::GetNumParents() - Returns the number of parents above this object.
//=============================================================================

int TreeNode::GetNumParents( void )
{
	int i;

	TreeNode *pObject = m_pParent;

	for( i = 0; pObject; i++ )
		pObject = pObject->m_pParent;

	return i;
}

//=============================================================================
// TreeNode::GetTreePosition() - Returns this objects position among its
//											siblings. For example, the first sibling will
//											return 0, the seventh sibling will return 6.
//=============================================================================

int TreeNode::GetTreePosition( void )
{
	int i;

	TreeNode *pObject = this;

	for( i = 0; pObject->m_pPrev; i++ )
		pObject = pObject->m_pPrev;

	return i;
}