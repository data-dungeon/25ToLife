
#include "OSIClanInfoList.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// Free the list
///////////////////////////////////////////////////////////////////////////////////////
OSIClanInfoList::~OSIClanInfoList()
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the next item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIClanInfoList::next()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->nextPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Iterate to the previous item in the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIClanInfoList::previous()
{
    if (_currentPtr)
    {
        _currentPtr = _currentPtr->prevPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the beginning of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIClanInfoList::begin()
{
    _currentPtr = _headPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Move the iterator to the end of the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIClanInfoList::end()
{
    _currentPtr = _tailPtr;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Remove the current item from the list
///////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OSIClanInfoList::erase()
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
void OSIClanInfoList::clear( void )
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
void OSIClanInfoList::push_front(OSIClanInfo &data)
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
/// Inserts and item to the list BEFORE the current pointer location
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIClanInfoList::insert(OSIClanInfo &data)
{
	listNode* newNode;
	newNode				= new listNode;
	newNode->data		= data;
	newNode->nextPtr	= NULL ;
	newNode->prevPtr	= NULL ;

    ++_count;

	// Head of the list insert
	if ( _currentPtr && _currentPtr->prevPtr == NULL )
	{
		newNode->nextPtr = _currentPtr;
		_headPtr = newNode;
		return;
	}

	// Mid-List insert
	if ( _currentPtr && _currentPtr->prevPtr )
	{
		_currentPtr->prevPtr->nextPtr = newNode ;	// previous guy points forward to new guy
		newNode->prevPtr = _currentPtr->prevPtr;	// new guy points backward to previous guy
		newNode->nextPtr = _currentPtr;				// new guy points forward to current guy
		_currentPtr->prevPtr = newNode;				// current guy points backward to new guy
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/// Add the given item to the end of the list
///
/// \param data New item to add to list
///////////////////////////////////////////////////////////////////////////////////////
void OSIClanInfoList::push_back(OSIClanInfo &data)
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
/*virtual*/ unsigned long OSIClanInfoList::count() const
{
    return _count;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Return the currently selected item in the list.
///
/// \return Pointer to item (NULL if no item selected).
///////////////////////////////////////////////////////////////////////////////////////
OSIClanInfo *OSIClanInfoList::getCurrent()
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
const OSIClanInfo *OSIClanInfoList::operator[](unsigned long offset)
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


OSIClanInfo* OSIClanInfoList::findClanInfoByID( OSIClanID& id )
{
	listNode* scan = _headPtr;
	while( scan && scan->data.clanID != id )
	{
		scan = scan->nextPtr;
	}
	
	if( scan ) 	{ return &scan->data; }
	else       	{ return 0; }
}




