#ifndef _INCLUDED_OSI_ClanInfo_LIST_
#define _INCLUDED_OSI_ClanInfo_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIClanInfoList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIClanInfoList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIClanInfo &data);
        void push_back(OSIClanInfo &data);
		void insert(OSIClanInfo &data);
		OSIClanInfo* findClanInfoByID( OSIClanID& id );

        // Accessors
        virtual unsigned long count() const;
        OSIClanInfo *getCurrent();
        const OSIClanInfo *operator[](unsigned long offset);

		OSIClanInfoList& operator=( OSIClanInfoList& other )
		{
			clone( other );
			return *this;
		}

		OSIClanInfoList::OSIClanInfoList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIClanInfoList( OSIClanInfoList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIClanInfoList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIClanInfo* s = other.getCurrent() )
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
            OSIClanInfo data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ClanInfo_LIST_