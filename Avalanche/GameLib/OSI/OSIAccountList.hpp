#ifndef _INCLUDED_OSI_ACCOUNT_LIST_
#define _INCLUDED_OSI_ACCOUNT_LIST_

#include "OSIBaseList.hpp"
#include "OSIAccount.hpp"

class OSIAccountList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIAccountList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(OSIAccount &data);
        void push_back(OSIAccount &data);

        // Accessors
        virtual unsigned long count() const;
        OSIAccount *getCurrent();
        OSIAccount *operator[](unsigned long offset);
		OSIAccount* findAccountByID( OSIAccountID& id );

		OSIAccountList& operator=( OSIAccountList& other )
		{
			clone( other );
			return *this;
		}

		OSIAccountList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIAccountList( OSIAccountList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIAccountList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIAccount* s = other.getCurrent() )
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
            OSIAccount data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_ACCOUNT_LIST_