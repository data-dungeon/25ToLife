
#ifndef _INCLUDED_USE_CASE_MY_SETTINGS_
#define _INCLUDED_USE_CASE_MY_SETTINGS_

#include "UseCase.hpp"
#include "Singleton.hpp"

namespace OnlineUI
{
	class MySettings : public UseCase, public Singleton< MySettings >
	{
	public:
		virtual ~MySettings( void )			{ }
		virtual bool update()				{ return true; }

	protected:
		friend class Singleton< MySettings >;
		MySettings( void )						{ }
		virtual bool _init()				{ return true; }
		virtual bool _deinit();
		virtual void _onOpen()				{ }
		virtual void _onPause()				{ }
	};
}

#endif //INCLUDED_USE_CASE_MY_SETTINGS_