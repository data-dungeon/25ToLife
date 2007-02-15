#ifndef _INCLUDED_OSI_ClanInvitation_LIST_
#define _INCLUDED_OSI_ClanInvitation_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIClanInvitationList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIClanInvitationList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIClanInvitation &data);
        void push_back(OSIClanInvitation &data);
		void insert(OSIClanInvitation &data);

        // Accessors
        virtual unsigned long count() const;
        OSIClanInvitation *getCurrent();
        const OSIClanInvitation *operator[](unsigned long offset);

		OSIClanInvitationList& operator=( OSIClanInvitationList& other )
		{
			clone( other );
			return *this;
		}

		OSIClanInvitationList::OSIClanInvitationList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIClanInvitationList( OSIClanInvitationList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIClanInvitationList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIClanInvitation* s = other.getCurrent() )
			{
				push_back( *s );
				other.next();
			}
		}

		OSIClanInvitation* find( const OSIClanID& inviteSentFromClanID, const OSIPlayerID& inviteSentFromPlayerID )
		{
			for( listNode* iter = _headPtr;
				iter != NULL;
				iter = iter->nextPtr )
			{
				if( (iter->data.clanInfo.clanID == inviteSentFromClanID)
					&& (iter->data.senderID == inviteSentFromPlayerID ) )
				{
					return &(iter->data);
				}
			}
			return 0;
		}

    private:
        struct listNode {
			static void* operator new( size_t size )		{ return OSImalloc( size ); }
			static void	 operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIClanInvitation data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ClanInvitation_LIST_