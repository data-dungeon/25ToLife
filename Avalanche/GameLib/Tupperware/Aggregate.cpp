////////////////////////////////////////////////////////////////////////////
//
// TupperwareAggregate
//
// An aggregate entity (stores a collection of other atoms).
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Aggregate.h"

#include "Iterator.h"
#include "List.h"
#include "Log.h"
#include "Scalar.h"
#include "Comment.h"

////////////////////////////////////////////////////////////////////////////


TupperwareAggregate::TupperwareAggregate() 
{
	m_pFirstChild = m_pLastChild = NULL;
}


TupperwareAggregate::~TupperwareAggregate() 
{
	while ( m_pFirstChild )
	{
		TupperwareAtom* child = m_pFirstChild;
		child->Detatch();
		delete child;
	}
}


TupperwareIterator TupperwareAggregate::FindAll() const {
	return TupperwareIterator( m_pFirstChild );
}


TupperwareIterator TupperwareAggregate::FindByKey( const char* key ) const 
{
	return TupperwareIterator( key, m_pFirstChild );
}

TupperwareAtom* TupperwareAggregate::FindAtomByKey( const char* key) const 
{
	assert(key);
	if (m_pFirstChild)
	{
		for ( TupperwareIterator i = FindByKey(key); ! i.Finished(); i.Next() ) 
		{
			TupperwareAtom* a = i.GetAsAtom();
			return a;
		}
	}
	return NULL;
}

TupperwareAggregate* TupperwareAggregate::FindAggregateByKey( const char* key ) const 
{
	TupperwareAtom* a = FindAtomByKey(key);
	if ( a && a->GetAtomType() != ATOM_AGGREGATE ) 
	{
		TupperwareLog::Write( "FindAggregateByKey(): Atom type does not match requested type.\n" );
		return NULL;
	}
	return ( TupperwareAggregate* )a;
}


TupperwareList* TupperwareAggregate::FindListByKey( const char* key ) const 
{
	TupperwareAtom* a = FindAtomByKey( key );
	
	if ( a && a->GetAtomType() != ATOM_LIST ) 
	{
		TupperwareLog::Write( "FindListByKey(): Atom type does not match requested type.\n" );
		return NULL;
	}
	return ( TupperwareList* )a;
}

TupperwareScalar* TupperwareAggregate::FindScalarByKey( const char* key ) const 
{
	TupperwareAtom* a = FindAtomByKey( key );
	
	if ( a && a->GetAtomType() != ATOM_SCALAR ) 
	{
		TupperwareLog::Write( "FindScalarByKey(): Atom type does not match requested type.\n" );
		return NULL;
	}
	return ( TupperwareScalar* )a;
}

TupperwareComment* TupperwareAggregate::FindCommentByKey( const char* key ) const 
{
	TupperwareAtom* a = FindAtomByKey( key );
	
	if ( a && a->GetAtomType() != ATOM_COMMENT ) 
	{
		TupperwareLog::Write( "FindCommentByKey(): Atom type does not match requested type.\n" );
		return NULL;
	}
	return ( TupperwareComment* )a;
}


TupperwareAtom* TupperwareAggregate::FindAtomByKeyAndName( const char* key, const char* name ) const 
{
	assert(key);
	if (m_pFirstChild)
	{
		if (name) 
		{
			for ( TupperwareIterator i = FindByKey(key); ! i.Finished(); i.Next() ) 
			{
				TupperwareAtom* a = i.GetAsAtom();

				if ( strnullcmp( a->m_pName, name ) == 0 ) 
				{
					return a;
				}
			}
		}
		else 
		{
			for ( TupperwareIterator i = FindByKey(key); ! i.Finished(); i.Next() ) 
			{
				TupperwareAtom* a = i.GetAsAtom();
				return a;
			}
		}
	}

	return NULL;
}


TupperwareAggregate* TupperwareAggregate::FindAggregateByKeyAndName( const char* key, const char* name ) const {
	TupperwareAtom* a = FindAtomByKeyAndName( key, name );

	if ( a && a->GetAtomType() != ATOM_AGGREGATE ) 
	{
		TupperwareLog::Write( "FindAggregateByKeyAndName(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return ( TupperwareAggregate* )a;
}


TupperwareList* TupperwareAggregate::FindListByKeyAndName( const char* key, const char* name ) const {
	TupperwareAtom* a = FindAtomByKeyAndName( key, name );
	
	if ( a && a->GetAtomType() != ATOM_LIST ) 
	{
		TupperwareLog::Write( "FindListByKeyAndName(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return ( TupperwareList* )a;
}

TupperwareScalar* TupperwareAggregate::FindScalarByKeyAndName( const char* key, const char* name ) const {
	TupperwareAtom* a = FindAtomByKeyAndName( key, name );
	
	if ( a && a->GetAtomType() != ATOM_SCALAR ) 
	{
		TupperwareLog::Write( "FindScalarByKeyAndName(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return ( TupperwareScalar* )a;
}

TupperwareComment* TupperwareAggregate::FindCommentByKeyAndName( const char* key, const char* name ) const 
{
	TupperwareAtom* a = FindAtomByKeyAndName( key, name );
	
	if ( a && a->GetAtomType() != ATOM_COMMENT ) 
	{
		TupperwareLog::Write( "FindCommentByKeyAndName(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return ( TupperwareComment* )a;
}


void TupperwareAggregate::DeleteAtom(TupperwareAtom* atom) 
{
	assert(atom->m_pParent == this); // we should be the parent
	atom->Detatch();
	delete atom;
}

void TupperwareAggregate::DeleteAtomsByKey(const char * key)
{
	TupperwareAtom *pAtom = FindAtomByKey(key);
	while (pAtom)
	{
		pAtom->Detatch();
		delete pAtom;
		pAtom = FindAtomByKey(key);
	}
}

void TupperwareAggregate::DeleteAtomsByKeyAndName(const char * key,const char * name)
{
	TupperwareAtom *pAtom = FindAtomByKeyAndName(key,name);
	while (pAtom)
	{
		pAtom->Detatch();
		delete pAtom;
		pAtom = FindAtomByKeyAndName(key,name);
	}
}


void TupperwareAggregate::AddChild(TupperwareAtom* child, bool addToEnd) 
{
	assert(child);
	child->Detatch();
	child->m_pParent = this;
	if (addToEnd) 
	{
		if (m_pLastChild) // if there is a child at the end
		{ 
			child->m_pPrev = m_pLastChild;
			m_pLastChild->m_pNext = child;
			m_pLastChild = child; // I am now at the end
		}
		else // if there are no children
		{
			m_pFirstChild = child;
			m_pLastChild = child;
		}
	}
	else
	{
		child->m_pNext = m_pFirstChild;
		if (m_pFirstChild)
		{
			m_pFirstChild->m_pPrev = child;
		}
		m_pFirstChild = child;
		if (!m_pLastChild)
			m_pLastChild = child;
	}
}

void TupperwareAggregate::InsertChild( TupperwareAtom* child, TupperwareAtom* sibling, bool insertAfter)
{
	assert(child);
	if (!sibling) // if sibling is null (just add child like normal)
	{
		AddChild(child);
	}
	else
	{
		assert(sibling->m_pParent == this); // this better point to me
		child->Detatch();
		child->m_pParent = this;
		if (insertAfter)
		{
			child->m_pNext = sibling->m_pNext;
			sibling->m_pNext = child;
			child->m_pPrev = sibling;
			if (child->m_pNext)
				child->m_pNext->m_pPrev = child;
			else
			{
				assert(m_pLastChild == sibling);
				m_pLastChild = child;
			}
		}
		else
		{
			child->m_pPrev = sibling->m_pPrev;
			sibling->m_pPrev = child;
			child->m_pNext = sibling;
			if (child->m_pPrev)
				child->m_pPrev->m_pNext = child;
			else
			{
				assert(m_pFirstChild == sibling);
				m_pFirstChild = child;
			}
		}
	}
}

TupperwareAtom *TupperwareAggregate::GetLastChildAtom(void)
{ 
	return m_pLastChild;
}

TupperwareAtom *TupperwareAggregate::GetFirstChildAtom(void)
{ 
	return m_pFirstChild;
}


TupperwareAggregate* TupperwareAggregate::AddAggregate( const char* key, const char* name, bool addToEnd ) 
{
	TupperwareAggregate* child = new TupperwareAggregate();

	child->SetKey( key );
	child->SetName( name );

	AddChild( child, addToEnd );

	return child;
}


TupperwareList* TupperwareAggregate::AddList( const char* key, const char* name, bool addToEnd ) 
{
	TupperwareList* child = new TupperwareList();

	child->SetKey( key );
	child->SetName( name );

	AddChild( child, addToEnd );

	return child;
}


TupperwareList* TupperwareAggregate::AddListFloat( const char* key, const char* name, const float* values, int num_values, bool addToEnd ) 
{
	TupperwareList* child = AddList( key, name, addToEnd );
	
	child->SetAsFloat( values, num_values );

	return child;
}

TupperwareList* TupperwareAggregate::AddListFloat( const char* key, const char* name, const TupArray<float> &values, bool addToEnd ) 
{
	TupperwareList* child = AddList( key, name, addToEnd );
	
	child->SetAsFloat( &values[0], values.GetSize());

	return child;
}



TupperwareList* TupperwareAggregate::AddListInt( const char* key, const char* name, const int* values, int num_values, bool addToEnd ) 
{
	TupperwareList* child = AddList( key, name, addToEnd );
	
	child->SetAsInt( values, num_values );

	return child;
}

TupperwareList* TupperwareAggregate::AddListInt( const char* key, const char* name, const TupArray<int> &values, bool addToEnd ) 
{
	TupperwareList* child = AddList( key, name, addToEnd );
	
	if (values.GetSize())
		child->SetAsInt( &values[0], values.GetSize());

	return child;
}


TupperwareList* TupperwareAggregate::AddListString( const char* key, const char* name, const char** values, int num_values, bool addToEnd ) 
{
	TupperwareList* child = AddList( key, name, addToEnd );
	child->SetAsString( values, num_values );
	return child;
}

TupperwareList* TupperwareAggregate::AddListString( const char* key, const char* name, const TupArray<TupString> &strings, bool addToEnd)
{
	TupperwareList* child = AddList( key, name, addToEnd );
	child->SetAsString( strings );
	return child;
}


TupperwareScalar* TupperwareAggregate::AddScalar( const char* key, const char* name, bool addToEnd ) 
{
	TupperwareScalar* child = new TupperwareScalar();

	child->SetKey( key );
	child->SetName( name );

	AddChild( child, addToEnd );

	return child;
}


TupperwareScalar* TupperwareAggregate::AddScalarFloat( const char* key, const char* name, float value, bool addToEnd ) 
{
	TupperwareScalar* child = AddScalar( key, name, addToEnd );

	child->SetAsFloat( value );

	return child;
}


TupperwareScalar* TupperwareAggregate::AddScalarInt( const char* key, const char* name, int value, bool addToEnd ) 
{
	TupperwareScalar* child = AddScalar( key, name, addToEnd );

	child->SetAsInt( value );

	return child;
}


TupperwareScalar* TupperwareAggregate::AddScalarString( const char* key, const char* name, const char* value, bool addToEnd ) 
{
	TupperwareScalar* child = AddScalar( key, name, addToEnd );

	child->SetAsString( value );

	return child;
}


TupperwareScalar* TupperwareAggregate::AddScalarBinary( const char* key, const char* name, const void* value, int length, bool addToEnd ) 
{
	TupperwareScalar* child = AddScalar( key, name, addToEnd );

	child->SetAsBinary( value, length);

	return child;
}


TupperwareComment* TupperwareAggregate::AddComment( const char* key, const char* name, const char* value, bool addToEnd ) 
{
	TupperwareComment* child = new TupperwareComment();

	child->SetKey( key );
	child->SetName( name );
	child->SetValue( value );

	AddChild( child, addToEnd );

	return child;
}

void TupperwareAggregate::MoveChildren( TupperwareAggregate *pSrc)
{
	while (pSrc->m_pFirstChild)
	{
		AddChild(pSrc->m_pFirstChild);
	}
}

void TupperwareAggregate::CopyChildren(const TupperwareAggregate *pSrc)
{
	for (TupperwareIterator i = pSrc->FindAll(); !i.Finished(); i.Next()) 
	{
		TupperwareAtom *child = i.GetAsAtom();
		Copy(child);
	}
}

void TupperwareAggregate::Copy(const TupperwareAtom *atom) 
{
	AtomType type = atom->GetAtomType();

	if (type == ATOM_AGGREGATE) 
	{
		TupperwareAggregate *source = (TupperwareAggregate *)atom;
		TupperwareAggregate *child = AddAggregate(atom->GetKey(), atom->GetName());

		for (TupperwareIterator i = source->FindAll(); !i.Finished(); i.Next()) 
		{
			TupperwareAtom *grandchild = i.GetAsAtom();
			child->Copy(grandchild);
		}
	} 
	else if (type == ATOM_LIST) 
	{
		TupperwareList *source = (TupperwareList *)atom;
		TupperwareList *child = AddList(atom->GetKey(), atom->GetName());

		DataType data_type = source->GetDataType();
		if (data_type == DATA_FLOAT) 
		{
			child->SetAsFloat(source->GetAsFloat(), source->GetLength());
		}
		else if (data_type == DATA_INT) 
		{
			child->SetAsInt(source->GetAsInt(), source->GetLength());
		}
		else if (data_type == DATA_STRING) 
		{
			child->SetAsString((const char **)source->GetAsString(), source->GetLength());
		}
	} 
	else if (type == ATOM_SCALAR) 
	{
		TupperwareScalar *source = (TupperwareScalar *)atom;
		TupperwareScalar *child = AddScalarInt(atom->GetKey(), atom->GetName(), 0);

		DataType data_type = source->GetDataType();
		if (data_type == DATA_FLOAT) 
		{
			child->SetAsFloat(source->GetAsFloat());
		}
		else if (data_type == DATA_INT) 
		{
			child->SetAsInt(source->GetAsInt());
		}
		else if (data_type == DATA_STRING) 
		{
			child->SetAsString(source->GetAsString());
		}
		else if (data_type == DATA_BINARY) 
		{
			const void *data;
			int len;

			data = source->GetAsBinary(&len);
			child->SetAsBinary(data, len);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
