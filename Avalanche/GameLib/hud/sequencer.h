#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include "hud/node.h"

class Sequencer
{
	DECLARE_CLASS( Sequencer );

	public:
		Sequencer( void );
		virtual ~Sequencer();

		static void UpdateAll( float TimeStep );

		virtual Sequencer *AddEvent( float Time );
		virtual void DelEvent( float Time );
		virtual void DelRange( float FromTime, float ToTime );
		virtual void Release( void );
		virtual void Update( float TimeStep );
		virtual void Trigger( float TimeDelta );
		virtual void SetLoopTime( float Time );
		virtual float GetLoopTime( void );
		virtual float PlayTime( void );

		virtual Sequencer *Event( void );
		virtual Sequencer *NextEvent( void );
		virtual Sequencer *PrevEvent( void );

	public:
		virtual void Play( bool Looped );
		virtual void Resume( bool Looped );
		virtual void Stop( void );
		virtual void Rewind( void );
		virtual void Jump( float Time );

	protected:
		virtual Sequencer *NewEvent( void ) { return (Sequencer *) New(); }

	public:
		float m_Time;
		float m_LoopTime;
		bool m_Playing;
		bool m_Looped;

	protected:
		static Sequencer *m_pActiveList;
		Sequencer *m_pNextActive;
		Sequencer *m_pEventList;
		Sequencer *m_pPrevEvent;
		Sequencer *m_pNextEvent;
		Sequencer *m_pEvent;
};

typedef void (*EVENTCALLBACK)( float Time );

class EventSequencer : public Sequencer
{
	DECLARE_CLASS( EventSequencer );

	public:
		EventSequencer( void );

		virtual void SetEvent( float Time, EVENTCALLBACK pfnCallBack );
		virtual void Trigger( float TimeDelta );

	private:
		EVENTCALLBACK m_pfnCallBack;
};

#endif // __SEQUENCER_H__