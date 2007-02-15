#ifndef __COMPONENTLIST_H_INCLUDED__
#define __COMPONENTLIST_H_INCLUDED__

#include "container/dllist.h"
#include "container/pool.h"

class CActorComponent;
class CCompActor;

class CActorComponentListNode
{
public:
	CActorComponentListNode() { component = NULL; }

	CActorComponent *component;

public:
	// needed to be a member of a DoublyLinkedList
	CActorComponentListNode *prev, *next;
};

class CActorComponentList
{
public:
	CActorComponentList( bool i_callDestructors = false );
	~CActorComponentList();
	
	CActorComponent *Attach( CCompActor &actor, const char *componentname );
	void Attach( CCompActor &actor, CActorComponent *component );
	void Remove( CActorComponent *component );
	void RemoveAll( void );

	// this should only be used to add components to temporary lists.  Normally,
	// you should be using Attach instead of Add.
	void Add( CActorComponent *component );

	static void Cleanup( void );

	CActorComponentListNode *Head( void ) { return list.Head(); }
	CActorComponentListNode *Tail( void ) { return list.Tail(); }

	CActorComponent *GetByName( const char *componentname );

	bool GetListChanged(void) {return(listChanged);}
	void ClearListChanged(void) {listChanged = false;}

private:

	DoublyLinkedList<CActorComponentListNode> list;
	bool callDestructors;
	CLinearHash<CActorComponent *> componentTable;
	bool listChanged;
};

bool operator<( const CActorComponentListNode &left, const CActorComponentListNode &right );
bool operator>( const CActorComponentListNode &left, const CActorComponentListNode &right );

#endif // __COMPONENTLIST_H_INCLUDED__
