
#ifndef _INCLUDED_USE_CASE_READY_UP_
#define _INCLUDED_USE_CASE_READY_UP_

#include "UseCase.hpp"
#include "Singleton.hpp"

namespace OnlineUI
{
	class ReadyUp : public UseCase, public Singleton< ReadyUp >
	{
	public:
		virtual ~ReadyUp( void )			{ }
		virtual bool update()				{ return true; }

	protected:
		friend class Singleton< ReadyUp >;
		ReadyUp( void )						{ }
		virtual bool _init()				{ return true; }
		virtual bool _deinit()				{ return true; }
		virtual void _onOpen()				{ }
		virtual void _onPause()				{ }
	};
}

#endif //INCLUDED_USE_CASE_