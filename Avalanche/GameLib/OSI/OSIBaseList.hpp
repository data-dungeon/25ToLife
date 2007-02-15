#ifndef _INCLUDED_OSI_BASE_LIST_
#define _INCLUDED_OSI_BASE_LIST_

#include "OSIPlatformTypes.h"
//#include "//Assert.h"

class OSIBaseList
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Iteration methods
        virtual void next() = 0;
        virtual void previous() = 0;
        virtual void begin() = 0;
        virtual void end() = 0;

        // List manipulation methods
        virtual void erase() = 0;
		virtual void clear() = 0;

        // Accessors
        virtual unsigned long count() const = 0;
        bool isNetworkDataReady();

        // Mutators
        void setNetworkDataReady(bool ready);

        // Each derived class needs to define operator [], since it has to return a value.
        // Each derived class needs to define a getCurrent() method, since it has to return a value.
        // Each derived class needs to define an push_front() method, since it takes a value.
        // Each derived class needs to define an push_back() method, since it takes a value.
    protected:
        OSIBaseList() : _dataReady(false) { }
        bool _dataReady;
};

inline void OSIBaseList::setNetworkDataReady(bool ready)
{
    _dataReady = ready;
}

inline bool OSIBaseList::isNetworkDataReady()
{
    return _dataReady;
}


#endif // _INCLUDED_OSI_BASE_LIST_

