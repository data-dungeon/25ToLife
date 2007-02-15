#include <stdlib.h>
#include "OSIHardwareProfileList.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// Free the list
///////////////////////////////////////////////////////////////////////////////////////
OSIHardwareProfileList::~OSIHardwareProfileList()
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the next item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIHardwareProfileList::next()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->nextPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the previous item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIHardwareProfileList::previous()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->prevPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the beginning of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIHardwareProfileList::begin()
{
    _currentPtr = _headPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the end of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIHardwareProfileList::end()
{
    _currentPtr = _tailPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Remove the current item from the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIHardwareProfileList::erase()
{
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

        // Reset all pointers if no pointer
        if (!_currentPtr)
        {
            _headPtr = _tailPtr = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
/// Clears the list of all elements.
////////////////////////////////////////////////////////////////////////////////////////
void OSIHardwareProfileList::clear( void )
{
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
}

///////////////////////////////////////////////////////////////////////////////////////
/// Add the given item to the front of the list
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIHardwareProfileList::push_front(OSIHardwareProfile &data)
{
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
}

///////////////////////////////////////////////////////////////////////////////////////
/// Add the given item to the end of the list
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIHardwareProfileList::push_back(OSIHardwareProfile &data)
{
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
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return number of items in list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ unsigned long OSIHardwareProfileList::count() const
{
    return _count;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return the currently selected item in the list.
///
/// \return Pointer to item (NULL if no item selected).
///////////////////////////////////////////////////////////////////////////////////////
OSIHardwareProfile *OSIHardwareProfileList::getCurrent()
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
OSIHardwareProfile *OSIHardwareProfileList::operator[](unsigned long offset)
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

OSIHardwareProfile* OSIHardwareProfileList::findHardwareProfileByID( OSIHardwareProfileID& id )
{
	listNode *curPtr = _headPtr ;
	if ( curPtr )
	{

		while ( curPtr && curPtr->data.id.ps2 != id.ps2 && curPtr->data.id.xBox != id.xBox )
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

