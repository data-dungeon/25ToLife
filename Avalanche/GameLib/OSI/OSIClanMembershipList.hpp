#ifndef _INCLUDED_OSI_ClanMembership_LIST_
#define _INCLUDED_OSI_ClanMembership_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIClanMembershipList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIClanMembershipList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();
		OSIClanMembership* findByPlayerID( const OSIPlayerID& id );

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIClanMembership &data);
        void push_back(OSIClanMembership &data);
		void insert(OSIClanMembership &data);

        // Accessors
        virtual unsigned long count() const;
        OSIClanMembership *getCurrent();
        const OSIClanMembership *operator[](unsigned long offset);

		OSIClanMembershipList& operator=( OSIClanMembershipList& other )
		{
			clone( other );
			return *this;
		}

		OSIClanMembershipList::OSIClanMembershipList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIClanMembershipList( OSIClanMembershipList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIClanMembershipList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIClanMembership* s = other.getCurrent() )
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
            OSIClanMembership data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ClanMembership_LIST_