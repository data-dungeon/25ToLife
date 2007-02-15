#ifndef _INCLUDED_OSI_PLAYER_STATS_LIST_
#define _INCLUDED_OSI_PLAYER_STATS_LIST_

#include "OSIBaseList.hpp"
#include "OSIPlayerStats.hpp"

class OSIPlayerStatsList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIPlayerStatsList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIPlayerStats &data);
        void push_back(OSIPlayerStats &data);
		void insert(OSIPlayerStats &data);

        // Accessors
        virtual unsigned long count() const;
        OSIPlayerStats *getCurrent();
        const OSIPlayerStats *operator[](unsigned long offset);
		OSIPlayerStats* findPlayerByID( OSIPlayerID& id );
		OSIPlayerStats* getPlayerAtIndex(unsigned long offset);


		OSIPlayerStatsList& operator=( OSIPlayerStatsList& other )
		{
			clone( other );
			return *this;
		}

		OSIPlayerStatsList::OSIPlayerStatsList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIPlayerStatsList( OSIPlayerStatsList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIPlayerStatsList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIPlayerStats* s = other.getCurrent() )
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
            OSIPlayerStats data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_PLAYER_STATS_LIST_