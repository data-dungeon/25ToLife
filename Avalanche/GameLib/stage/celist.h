/***************************************************************************/
// A cluster element list
/***************************************************************************/
#ifndef CEELIST_H
#define CELIST_H

#include "clustere.h"

class ClusterElementList
{
public:
	// The constructor
	inline ClusterElementList(ClusterElement *head = NULL);

	// This will delete all the items in the list
	inline ~ClusterElementList(void);

	// Add an item at the head/tail of the list
	inline void AddHead(ClusterElement *newItem);
	inline void AddTail(ClusterElement *newItem);

	// Remove an item from the list
	inline void Remove(ClusterElement *item);
	inline void RemoveAll(void);

	// Get at the head
	inline ClusterElement *Head(void);

	// Get the head and remove it from the list
	inline ClusterElement *PopHead(void);

	// Get at the tail
	inline ClusterElement *Tail(void);

	// Is the list empty?
	inline int IsEmpty(void);

	// Does the list have a single element?
	inline bool OnlyOneElement(void);

	// Get the next element
	inline ClusterElement *GetNextElement(ClusterElement *currentElement);

	// Get the previous element
	inline ClusterElement *GetPreviousElement(ClusterElement *currentElement);

private:
	ClusterElement	*head, *tail;		// head/tail of the list
};

// Implementation
#include "celist.hpp"

#endif
