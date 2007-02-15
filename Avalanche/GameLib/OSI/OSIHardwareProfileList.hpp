#ifndef _INCLUDED_OSI_HardwareProfile_LIST_
#define _INCLUDED_OSI_HardwareProfile_LIST_

#include "OSIBaseList.hpp"
#include "OSIHardwareProfile.hpp"

class OSIHardwareProfileList : public OSIBaseList
{
    public:
		static void*				operator new( size_t size )								{ return OSImalloc( size ); }
		static void					operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        // Destructor
        virtual ~OSIHardwareProfileList();

        // Iteration methods
        virtual void				next();
        virtual void				previous();
        virtual void				begin();
        virtual void				end();

        // List manipulation methods
        virtual void				erase();
		virtual void				clear();
        void						push_front(OSIHardwareProfile &data);
        void						push_back(OSIHardwareProfile &data);

        // Accessors
        virtual unsigned long		count() const;
        OSIHardwareProfile*	getCurrent();
		OSIHardwareProfile*			findHardwareProfileByID( OSIHardwareProfileID& id );
        OSIHardwareProfile*	operator[](unsigned long offset);

		OSIHardwareProfileList& operator=( OSIHardwareProfileList& other )
		{
			clone( other );
			return *this;
		}

		OSIHardwareProfileList::OSIHardwareProfileList() :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
		}

		OSIHardwareProfileList( OSIHardwareProfileList& other ) :
			_count(0),
			_currentPtr(NULL),
			_headPtr(NULL),
			_tailPtr(NULL)
		{
			clone( other );
		}

		void clone( OSIHardwareProfileList& other )
		{
			if( &other == this ) { return; }
			clear();
			other.begin();
			while( OSIHardwareProfile* s = other.getCurrent() )
			{
				push_back( *s );
				other.next();
			}
		}

    private:
        struct listNode {
			static void*	operator new( size_t size )		{ return OSImalloc( size ); }
			static void		operator delete( void* memPtr )	{ OSIfree( memPtr ); }
            listNode *prevPtr;
            listNode *nextPtr;
            OSIHardwareProfile data;
        };

        unsigned long _count;
        listNode *_currentPtr;
        listNode *_headPtr;
        listNode *_tailPtr;
};

#endif // _INCLUDED_OSI_HardwareProfile_LIST_