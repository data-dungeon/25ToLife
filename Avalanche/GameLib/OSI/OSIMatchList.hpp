#ifndef _INCLUDED_OSI_MATCH_LIST_
#define _INCLUDED_OSI_MATCH_LIST_

#include "OSIBaseList.hpp"
#include "OSIMatch.hpp"

class OSIMatchList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIMatchList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIMatch &data);
        void push_back(OSIMatch &data);
		void insert(OSIMatch &data);

        // Accessors
        virtual unsigned long count() const;
        OSIMatch *getCurrent();
        OSIMatch *operator[](unsigned long offset);
		OSIMatch* findMatchByID( OSIMatchID& id );

		OSIMatchList& operator=( OSIMatchList& other )
		{
			clone( other );
			return *this;
		}

		OSIMatchList::OSIMatchList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIMatchList( OSIMatchList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIMatchList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIMatch* s = other.getCurrent() )
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
            OSIMatch data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_MATCH_LIST_