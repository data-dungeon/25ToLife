#ifndef _INCLUDED_OSI_ClanNotice_LIST_
#define _INCLUDED_OSI_ClanNotice_LIST_

#include "OSIBaseList.hpp"
#include "OSIClans.hpp"

class OSIClanNoticeList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIClanNoticeList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIClanNotice &data);
        void push_back(OSIClanNotice &data);
		void insert(OSIClanNotice &data);

        // Accessors
        virtual unsigned long count() const;
        OSIClanNotice *getCurrent();
        const OSIClanNotice *operator[](unsigned long offset);

		OSIClanNoticeList& operator=( OSIClanNoticeList& other )
		{
			clone( other );
			return *this;
		}

		OSIClanNoticeList::OSIClanNoticeList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIClanNoticeList( OSIClanNoticeList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIClanNoticeList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIClanNotice* s = other.getCurrent() )
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
            OSIClanNotice data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ClanNotice_LIST_