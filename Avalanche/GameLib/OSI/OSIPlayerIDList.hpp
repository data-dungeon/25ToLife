#ifndef _INCLUDED_OSI_PlayerID_LIST_
#define _INCLUDED_OSI_PlayerID_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIPlayerIDList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIPlayerIDList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIPlayerID &data);
        void push_back(OSIPlayerID &data);
		void insert(OSIPlayerID &data);

        // Accessors
        virtual unsigned long count() const;
        OSIPlayerID *getCurrent();
        const OSIPlayerID *operator[](unsigned long offset);

		OSIPlayerIDList& operator=( OSIPlayerIDList& other )
		{
			clone( other );
			return *this;
		}

		OSIPlayerIDList::OSIPlayerIDList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIPlayerIDList( OSIPlayerIDList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIPlayerIDList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIPlayerID* s = other.getCurrent() )
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
            OSIPlayerID data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_PlayerID_LIST_