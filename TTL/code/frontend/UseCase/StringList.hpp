#ifndef _INCLUDED_STRING_LIST_
#define _INCLUDED_STRING_LIST_

#include "OSI/OSIBaseList.hpp"

typedef char charString[50];

#define MAX_STRING_SIZE 256

class StringList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Constructor
        StringList();

        // Destructor
        virtual ~StringList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(const char *data);
        void push_back(const char *data);

        // Accessors
        virtual unsigned long count() const;
        const char *getCurrent();
        const char *operator[](unsigned long offset);
        const char *getBack() { return _tailPtr->data; }
        const char *getFront() { return _headPtr->data; }
        unsigned long getPosition() { return _position; }

		StringList& operator=( StringList& other )
		{
			clear();
			other.begin();
			while( const char* s = other.getCurrent() )
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
            char data[MAX_STRING_SIZE];
        };

        unsigned long _count;
        unsigned long _position;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_STRING_LIST_
