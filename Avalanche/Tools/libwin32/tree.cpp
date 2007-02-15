#include "pch.h"
#include "tree.h"

//=============================================================================
// CTreeNode::CTreeNode() - The constructor initializes all the data members
//								  and records itself in the master list.
//=============================================================================

CTreeNode::CTreeNode()
{
	m_pParent = 0;
	m_pChild  = 0;
	m_pNext   = 0;
	m_pPrev   = 0;
	m_ObjectId = 0;
}

//=============================================================================
// CTreeNode::~CTreeNode() - The destructor removes the node and its children
//									  from the hierarchy.  Also removes the object from
//									  the master list.
//=============================================================================

CTreeNode::~CTreeNode()
{
	Remove();

	if( m_pChild )
		m_pChild->Remove();

	return;
}

//=============================================================================
// CTreeNode::SetParent() - Removes this node from its current position in the
//									 hierarchy and attaches it to the new parent,
//									 updating pointers as necessary.
//=============================================================================

void CTreeNode::SetParent( CTreeNode *pParent )
{
	Remove();

	if( pParent )
		pParent->AddChild( this );

	return;
}

//=============================================================================
// CTreeNode::SetChild() - Removes a node from its current position in the
//								   hierarchy and attaches it to this node as the first
//								   child.  Pointers are updated as necessary.
//=============================================================================

void CTreeNode::SetChild( CTreeNode *pChild )
{
	if( m_pChild )
		m_pChild->Insert( pChild );
	else
		AddChild( pChild );

	return;
}

//=============================================================================
// CTreeNode::AddChild() - Removes a node from its current position in the
//								   hierarchy and attaches it to this node updating
//								   pointers as necessary.
//=============================================================================

void CTreeNode::AddChild( CTreeNode *pChild )
{
	if( pChild && pChild != this )
	{
		pChild->Remove();

		if( !m_pChild )
			m_pChild = pChild;
		else
		{
			CTreeNode *pObject = m_pChild;

			while( pObject->m_pNext )
				pObject = pObject->m_pNext;

			pObject->InsertAfter( pChild );
		}

		pChild->m_pParent = this;
	}

	return;
}

//=============================================================================
// CTreeNode::Insert() - Inserts the specified object so that it comes BEFORE
//								 this object.
//=============================================================================

void CTreeNode::Insert( CTreeNode *pObject )
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
// CTreeNode::InsertAfter() - Inserts the specified object so that it comes
//									   AFTER this object.
//=============================================================================

void CTreeNode::InsertAfter( CTreeNode *pObject )
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
// CTreeNode::Remove() - Removes this object and its children from the hierarchy.
//								 Adjacent objects are updated accordingly.
//=============================================================================

void CTreeNode::Remove( void )
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
// CTreeNode::SetObjectId() - Give the object an id.
//=============================================================================

void CTreeNode::SetObjectId( int Id )
{
	m_ObjectId = Id;
}

//=============================================================================
// CTreeNode::AdoptChildren()
//=============================================================================

void CTreeNode::AdoptChildren( CTreeNode* pParent )
{
	CTreeNode* pLastChild = GetLastChild();
	CTreeNode* pChild;

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
// CTreeNode::Parent - Returns a pointer to the first parent with the specified
//							  object id.
//=============================================================================

CTreeNode* CTreeNode::Parent( int ObjectId )
{
	CTreeNode* pParent = Parent();

	while( pParent && pParent->ObjectId() != ObjectId )
		pParent = pParent->Parent();

	return pParent;
}

//=============================================================================
// CTreeNode::Child - Returns a pointer to the first child with the specified
//						    object id.
//=============================================================================

CTreeNode* CTreeNode::Child( int ObjectId )
{
	CTreeNode* pChild = Child();

	while( pChild && pChild->ObjectId() != ObjectId )
		pChild = pChild->Next();

	return pChild;
}

//=============================================================================
// CTreeNode::Next - Returns a pointer to the next object with the specified
//						   object id.
//=============================================================================

CTreeNode* CTreeNode::Next( int ObjectId )
{
	CTreeNode* pNext = Next();

	while( pNext && pNext->ObjectId() != ObjectId )
		pNext = pNext->Next();

	return pNext;
}

//=============================================================================
// CTreeNode::Prev - Returns a pointer to the previous object with the specified
//						   object id.
//=============================================================================

CTreeNode* CTreeNode::Prev( int ObjectId )
{
	CTreeNode* pPrev = Prev();

	while( pPrev && pPrev->ObjectId() != ObjectId )
		pPrev = pPrev->Prev();

	return pPrev;
}

//=============================================================================
// CTreeNode::GetLastChild() - Returns a pointer to the last child, or 0 if none
//										 exists.
//=============================================================================

CTreeNode *CTreeNode::GetLastChild( void )
{
	CTreeNode *pObject = m_pChild;

	if( pObject )
	{
		while( pObject->m_pNext )
			pObject = pObject->m_pNext;
	}

	return pObject;
}

//=============================================================================
// CTreeNode::GetNthChild() - Returns a pointer to the Nth child, or 0 if the
//									   child does not exist.
//=============================================================================

CTreeNode *CTreeNode::GetNthChild( int n )
{
	if( n < 0 )
		return 0;

	CTreeNode *pObject = m_pChild;

	for( int i = 0; i < n; i++ )
	{
		if( pObject )
			pObject = pObject->m_pNext;
	}

	return pObject;
}

//=============================================================================
// CTreeNode::FindChild() - Returns a pointer to the child who matches the
//									 object id.
//=============================================================================

CTreeNode * CTreeNode::FindChild( int ObjectId )
{
	CTreeNode *pChild = Child();

	while( pChild )
	{
		if( pChild->m_ObjectId == ObjectId )
			return pChild;

		pChild = pChild->Next();
	}

	return 0;
}

//=============================================================================
// CTreeNode::GetNumChildren() - Returns the number of children immediately
//										   connected to this object.  Sub-children are not
//										   considered.
//=============================================================================

int CTreeNode::GetNumChildren( void )
{
	int i;

	CTreeNode *pObject = m_pChild;

	for( i = 0; pObject; i++ )
		pObject = pObject->m_pNext;

	return i;
}

//=============================================================================
// CTreeNode::GetNumChildren() - Returns the number of children immediately
//										   connected to this object matching the specified
//										   object id.
//=============================================================================

int CTreeNode::GetNumChildren( int ObjectId )
{
	int i, c;

	CTreeNode *pObject = m_pChild;

	for( i = c = 0; pObject; i++ )
	{
		if( pObject->m_ObjectId == ObjectId )
			c++;

		pObject = pObject->m_pNext;
	}

	return c;
}

//=============================================================================
// CTreeNode::GetAllChildren() - Returns the number of children connected to
//										   this object.  Sub-children are considered.
//=============================================================================

int CTreeNode::GetAllChildren( void )
{
	int i;

	CTreeNode *pObject = m_pChild;

	for( i = 0; pObject; pObject = pObject->m_pNext )
		i += 1 + pObject->GetAllChildren();

	return i;
}

//=============================================================================
// CTreeNode::GetNumParents() - Returns the number of parents above this object.
//=============================================================================

int CTreeNode::GetNumParents( void )
{
	int i;

	CTreeNode *pObject = m_pParent;

	for( i = 0; pObject; i++ )
		pObject = pObject->m_pParent;

	return i;
}

//=============================================================================
// CTreeNode::GetTreePosition() - Returns this objects position among its
//											 siblings. For example, the first sibling will
//											 return 0, the seventh sibling will return 6.
//=============================================================================

int CTreeNode::GetTreePosition( void )
{
	int i;

	CTreeNode *pObject = this;

	for( i = 0; pObject->m_pPrev; i++ )
		pObject = pObject->m_pPrev;

	return i;
}

//=============================================================================
// NamedNode::SetObjectName
//=============================================================================

void NamedNode::SetObjectName( const char* pName )
{
	m_ObjectName.set( pName );
}

//=============================================================================
// NamedNode::ObjectName
//=============================================================================

const char* NamedNode::ObjectName( void )
{
	return m_ObjectName.get();
}

//=============================================================================
// NamedNode::FindChildByName
//=============================================================================

NamedNode* NamedNode::FindChildByName( const char* pName )
{
	NamedNode* pNode = Child();

	while( pNode )
	{
		if( pNode->m_ObjectName.equals( pName ) )
			break;

		pNode = pNode->Next();
	}

	return pNode;
}