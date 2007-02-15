#pragma once
#include "../OSIBaseList.hpp"
//#include "OSIXboxPlayersListItem.hpp"
class OSIXboxPlayersListItem;

class OSIXboxPlayersList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Constructor
        OSIXboxPlayersList();

        // Destructor
        virtual ~OSIXboxPlayersList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIXboxPlayersListItem* data);
        void push_back(OSIXboxPlayersListItem* data);
		void insert(OSIXboxPlayersListItem* data);

        // Accessors
        virtual unsigned long count() const;
        OSIXboxPlayersListItem* getCurrent();
        OSIXboxPlayersListItem* operator[](unsigned long offset);

		OSIXboxPlayersList& operator=( OSIXboxPlayersList& other )
		{
			if( &other == this ) { return *this; }
			clear();
			other.begin();
			while( OSIXboxPlayersListItem* s = other.getCurrent() )
			{
				push_back( s );
				other.next();
			}
			return *this;
		}

    private:
        struct listNode {
			static void* operator new( size_t size )		{ return OSImalloc( size ); }
			static void	 operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIXboxPlayersListItem* data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

