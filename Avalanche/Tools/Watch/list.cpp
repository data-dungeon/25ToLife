/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library  Release 1.4
 */
// Version 0.00
// ASCII
// 
// Copyright (C) 1999 Sony Computer Entertainment Inc. All Rights Reserved.
// 
// Version        Date            Design      Log
// --------------------------------------------------------------------
// 0.00           10/13/99        koji        1st version

#include "stdafx.h"
#include "list.h"

void ListInsertAfter(void *node, void *newnode)
{
	LISTNODE *n = (LISTNODE*)node;
	LISTNODE *nn = (LISTNODE*)newnode;
	
	nn->next = n->next;
	nn->prev = n;
	n->next = nn->next->prev = nn;
}


void *ListDelete(void *node)
{
    LISTNODE *n = (LISTNODE*)node;
	
    n->prev->next = n->next;
    n->next->prev = n->prev;
    return n;
}


void ListForEach(LIST *list, void (*proc)(LISTNODE *node))
{
    void *node;
	
    node = ListGetNext(list);
    while (!ListDone(node, list)) {
        (*proc)((LISTNODE*)node);
        node = ListGetNext(node);
    }
}

// Local variables:
// tab-width: 4
// End:
// vi:set tabstop=4:

