#ifndef _INCLUDED_SINGLETON_
#define _INCLUDED_SINGLETON_

#include <Assert.h>
#include "OSI/OSIPlatformTypes.h"

namespace OnlineUI
{
    template <typename CLASSNAME>
    class Singleton
    {
        public:
            Singleton() { }

            /** \name Singleton methods */
            //\{
            inline static bool		isSingletonCreated( void );
			static CLASSNAME*		createSingleton( void );
            static CLASSNAME*		getSingletonPtr( void );
            static void				destroySingleton( void );
			static void*			operator new( size_t size )		{ return OSImalloc( size ); }
			static void				operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            //\}

        protected:
            static CLASSNAME *_instancePtr;
    };

    template <typename CLASSNAME>
    CLASSNAME *Singleton<CLASSNAME>::_instancePtr = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create the singleton object.
    ///
    /// \return Pointer to singleton object.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename CLASSNAME>
    CLASSNAME *Singleton<CLASSNAME>::createSingleton()
    {
        if (!_instancePtr)
        {
            _instancePtr = new CLASSNAME();
        }

        return _instancePtr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get pointer to the current singleton object, and create it if it doesn't exist.
    ///
    /// \return Pointer to singleton object.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename CLASSNAME>
    CLASSNAME *Singleton<CLASSNAME>::getSingletonPtr()
    {
        if (!_instancePtr)
        {
            _instancePtr = new CLASSNAME();
        }

#ifndef WIN32
        assert(_instancePtr != 0);
#endif
        return _instancePtr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy the singleton object.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename CLASSNAME>
    void Singleton<CLASSNAME>::destroySingleton()
    {
        delete _instancePtr;
        _instancePtr = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Just query whether the singleton has been created yet or not.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename CLASSNAME>
    bool Singleton<CLASSNAME>::isSingletonCreated()
	{
		return (_instancePtr != 0);
	}

}

#endif // _INCLUDED_SINGLETON_