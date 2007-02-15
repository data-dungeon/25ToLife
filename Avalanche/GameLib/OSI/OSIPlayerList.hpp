#ifndef _INCLUDED_OSI_PLAYER_LIST_
#define _INCLUDED_OSI_PLAYER_LIST_

#include "OSIBaseList.hpp"
#include "OSIPlayer.hpp"

class OSIPlayerList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIPlayerList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
		void moveCurrentEntryToFront();
		void moveCurrentEntryToBack();
        void push_front(OSIPlayer &data);
        void push_back(OSIPlayer &data);

        // Accessors
        inline virtual unsigned long count() const;
        OSIPlayer *getCurrent();
        OSIPlayer *operator[](unsigned long offset);
		OSIPlayer* findPlayerByID( OSIPlayerID& id );

		void clone( OSIPlayerList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIPlayer* s = other.getCurrent() )
			{
				push_back( *s );
				other.next();
			}
		}

		OSIPlayerList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIPlayerList( OSIPlayerList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		OSIPlayerList& operator=( OSIPlayerList& other )
		{
			clone( other );
			return *this;
		}

    private:
        struct listNode {
			static void* operator new( size_t size )		{ return OSImalloc( size ); }
			static void	 operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIPlayer data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_PLAYER_LIST_