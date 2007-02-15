#ifndef _INCLUDED_OSI_ClanID_LIST_
#define _INCLUDED_OSI_ClanID_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIClanIDList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIClanIDList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIClanID &data);
        void push_back(OSIClanID &data);
		void insert(OSIClanID &data);
		bool find( OSIClanID& data );

        // Accessors
        virtual unsigned long count() const;
        OSIClanID *getCurrent();
        const OSIClanID *operator[](unsigned long offset);

		OSIClanIDList& operator=( OSIClanIDList& other )
		{
			clone( other );
			return *this;
		}

		OSIClanIDList::OSIClanIDList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIClanIDList( OSIClanIDList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIClanIDList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIClanID* s = other.getCurrent() )
			{
				push_back( *s );
				other.next();
			}
		}

    private:
        struct listNode {
			static void* operator new( size_t size )		{ return OSImalloc( size ); }
			static void	 operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIClanID data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ClanID_LIST_