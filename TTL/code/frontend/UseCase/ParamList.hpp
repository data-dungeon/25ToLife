#ifndef _INCLUDED_Param_LIST_
#define _INCLUDED_Param_LIST_

#include "OSI/OSIBaseList.hpp"
#include "frontend/UseCase/GameSettingConfig.hpp"

class ParamList : public OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Constructor
        ParamList();

        // Destructor
        virtual ~ParamList();

        // Iteration methods
        virtual void next();
        virtual void previous();
        virtual void begin();
        virtual void end();

        // List manipulation methods
        virtual void erase();
		virtual void clear();
        void push_front(ParamType *data);
        void push_back(ParamType *data);

        // Accessors
        virtual unsigned long count() const;
        ParamType *getCurrent();
        ParamType *operator[](unsigned long offset);
        ParamType *getBack() { return _tailPtr->data; }
        ParamType *getFront() { return _headPtr->data; }
        unsigned long getPosition() { return _position; }

		ParamList& operator=( ParamList& other )
		{
			clear();
			other.begin();
			while( ParamType* s = other.getCurrent() )
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
            ParamType *data;
        };

        unsigned long _count;
        unsigned long _position;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_Param_LIST_
