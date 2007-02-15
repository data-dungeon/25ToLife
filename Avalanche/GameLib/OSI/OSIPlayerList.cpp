#include <stdlib.h>
#include "OSIPlayerList.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// Free the list
///////////////////////////////////////////////////////////////////////////////////////
OSIPlayerList::~OSIPlayerList()
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the next item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIPlayerList::next()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->nextPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the previous item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIPlayerList::previous()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->prevPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the beginning of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIPlayerList::begin()
{
    _currentPtr = _headPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the end of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIPlayerList::end()
{
    _currentPtr = _tailPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Remove the current item from the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIPlayerList::erase()
{
//	printf( "OSIPlayerList::erase() : called (%d items in list)\n", (int)_count );

    if (_currentPtr)
    {
        listNode *tempPtr = _currentPtr;

        // Reset pointers correctly
        if (_currentPtr == _headPtr)
        {
            _headPtr = _headPtr->nextPtr;
        }
        if (_currentPtr == _tailPtr)
        {
            _tailPtr = _tailPtr->prevPtr;
        }
        if (_currentPtr->prevPtr)
        {
            _currentPtr->prevPtr->nextPtr = _currentPtr->nextPtr;
        }
        if (_currentPtr->nextPtr)
        {
            _currentPtr->nextPtr->prevPtr = _currentPtr->prevPtr;
        }

        // Set the position of the current node
        if (_currentPtr->nextPtr)
        {
            _currentPtr = _currentPtr->nextPtr;
        }
        else
        {
            _currentPtr = _currentPtr->prevPtr;
        }

        delete tempPtr;
        --_count;

//		printf( "OSIPlayerList::erase() : removed one item; %d items remain\n", (int)_count );

        // Reset all pointers if no pointer
        if (!_currentPtr)
        {
            _headPtr = _tailPtr = NULL;
//			printf( "OSIPlayerList::erase() : list is now empty!\n" );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
/// Clears the list of all elements.
////////////////////////////////////////////////////////////////////////////////////////
void OSIPlayerList::clear( void )
{
//	printf( "OSIPlayerList::clear() : called (%d items in list)\n", (int)_count );
    listNode *tempPtr = _headPtr;

    while (_headPtr)
    {
        tempPtr = _headPtr;
        _headPtr = _headPtr->nextPtr;

        delete tempPtr;
    }

	_headPtr = NULL;
	_tailPtr = NULL;
	_currentPtr = NULL;
	_count = 0;
//	printf( "OSIPlayerList::clear() : done (%d items in list)\n", (int)_count );
}


///////////////////////////////////////////////////////////////////////////////////////
/// Add the given item to the front of the list
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIPlayerList::push_front(OSIPlayer &data)
{
//	printf( "OSIPlayerList::push_front() : called (%d items in list)\n", (int)_count );
    if (!_headPtr)
    {
        _headPtr = new listNode;
        _headPtr->prevPtr = NULL;
        _headPtr->nextPtr = NULL;
        _headPtr->data = data;

        _tailPtr = _headPtr;
        _currentPtr = _headPtr;
    }
    else
    {
        _headPtr->prevPtr = new listNode;
        _headPtr->prevPtr->prevPtr = NULL;
        _headPtr->prevPtr->nextPtr = _headPtr;
        _headPtr->prevPtr->data = data;
        _headPtr = _headPtr->prevPtr;
    }

    ++_count;
//	printf( "OSIPlayerList::push_front() : done (%d items in list)\n", (int)_count );
}

///////////////////////////////////////////////////////////////////////////////////////
/// Add the given item to the end of the list
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIPlayerList::push_back(OSIPlayer &data)
{
//	printf( "OSIPlayerList::push_back() : called (%d items in list)\n", (int)_count );
    if (!_tailPtr)
    {
        _tailPtr = new listNode;
        _tailPtr->prevPtr = NULL;
        _tailPtr->nextPtr = NULL;
        _tailPtr->data = data;

        _headPtr = _tailPtr;
        _currentPtr = _tailPtr;
    }
    else
    {
        _tailPtr->nextPtr = new listNode;
        _tailPtr->nextPtr->prevPtr = _tailPtr;
        _tailPtr->nextPtr->nextPtr = NULL;
        _tailPtr->nextPtr->data = data;
        _tailPtr = _tailPtr->nextPtr;
    }

    ++_count;
//	printf( "OSIPlayerList::push_back() : done (%d items in list)\n", (int)_count );
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return number of items in list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ inline unsigned long OSIPlayerList::count() const
{
    return _count;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return the currently selected item in the list.
///
/// \return Pointer to item (NULL if no item selected).
///////////////////////////////////////////////////////////////////////////////////////
OSIPlayer *OSIPlayerList::getCurrent()
{
    if (_currentPtr)
    {
        return &_currentPtr->data;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return the nth item of the list.
///
/// \return Pointer to item (NULL if it doesn't exist).
///////////////////////////////////////////////////////////////////////////////////////
OSIPlayer *OSIPlayerList::operator[](unsigned long offset)
{
    unsigned long count = 0;
    listNode *curPtr = _headPtr;

    if (curPtr)
    {
        while (curPtr && count != offset)
        {
            curPtr = curPtr->nextPtr;
            ++count;
        }

        if (curPtr)
        {
            return &curPtr->data;
        }

        return NULL;
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////
// Finds a player by ID
///////////////////////////////////////////////////////////////////////////////////////
OSIPlayer* OSIPlayerList::findPlayerByID( OSIPlayerID& id )
{
	listNode *curPtr = _headPtr ;
	if ( curPtr )
	{

		while ( curPtr && curPtr->data.id != id )
		{
			curPtr = curPtr->nextPtr ;
		}
		if ( curPtr )
		{
			return &curPtr->data ;
		}
	}

	return NULL ;
}


///////////////////////////////////////////////////////////////////////////////////////
// Moves the current object in the list to the front of the list
///////////////////////////////////////////////////////////////////////////////////////
void OSIPlayerList::moveCurrentEntryToFront()
{
	if( !_currentPtr )
		return;

	/// Quietly remove ourselves from our current place in the list
	if( _currentPtr->prevPtr )
	{
		_currentPtr->prevPtr->nextPtr = _currentPtr->nextPtr;
	}

	if( _currentPtr->nextPtr )
	{
		_currentPtr->nextPtr->prevPtr = _currentPtr->prevPtr;
	}

	_currentPtr->nextPtr = _headPtr;
	_headPtr = _currentPtr;

	/// Add ourselves at the front of the list
//	_currentPtr->prevPtr_headPtr;
}


///////////////////////////////////////////////////////////////////////////////////////
// Moves the current object in the list to the back of the list
///////////////////////////////////////////////////////////////////////////////////////
void OSIPlayerList::moveCurrentEntryToBack()
{
	if( !_currentPtr )
		return;
}





