#ifndef _INCLUDED_OSI_String_LIST_
#define _INCLUDED_OSI_String_LIST_

#include "OSIBaseList.hpp"
//#include "OSI.hpp"

class OSIStringList : public OSIBaseList
{
    public:
		static const unsigned int MAX_STRING_LENGTH = 255;

		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIStringList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(const OSIchar *data);
        void push_back(const OSIchar *data);

        // Accessors
        virtual unsigned long count() const;
        OSIchar *getCurrent();
        OSIchar *operator[](unsigned long offset);

		OSIStringList& operator=( OSIStringList& other )
		{
			clone( other );
			return *this;
		}

		OSIStringList::OSIStringList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIStringList( OSIStringList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIStringList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIchar* s = other.getCurrent() )
			{
				push_back( s );
				other.next();
			}
		}

    private:
        struct listNode {
			static void* operator new( size_t size )		{ return OSImalloc( size ); }
			static void	 operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIchar data[MAX_STRING_LENGTH];
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_String_LIST_