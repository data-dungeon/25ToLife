#ifndef _INCLUDED_VALUE_LIST_
#define _INCLUDED_VALUE_LIST_

#include "OSI/OSIBaseList.hpp"

typedef char charString[50];

struct Value
{
    int        intValue;
    charString stringValue;

    Value() { stringValue[0] = 0; }
    Value(Value &rhs)
    {
        intValue = rhs.intValue;
        strncpy(stringValue, rhs.stringValue, 50);
    }
};

class ValueList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Constructor
        ValueList();

        // Destructor
        virtual ~ValueList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(Value &data);
        void push_back(Value &data);

        // Accessors
        virtual unsigned long count() const;
        Value *getCurrent();
        Value *operator[](unsigned long offset);
        Value *getBack() { return &_tailPtr->data; }
        Value *getFront() { return &_headPtr->data; }
        unsigned long getPosition() { return _position; }

		ValueList& operator=( ValueList& other )
		{
			clear();
			other.begin();
			while( Value* s = other.getCurrent() )
			{
				push_back( *s );
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
            Value data;
        };

        unsigned long _count;
        unsigned long _position;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_VALUE_LIST_
