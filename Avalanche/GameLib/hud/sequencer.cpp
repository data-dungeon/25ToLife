#include "Layers/Debug.h"
#include "hud/sequencer.h"

Sequencer *Sequencer::m_pActiveList = 0;

Sequencer::Sequencer( void )
{
	m_pNextActive = 0;
	m_pEventList = 0;
	m_pNextEvent = 0;
	m_pPrevEvent = 0;
	m_pEvent = 0;
	m_Time = 0.0f;
	m_LoopTime = 0.0f;
	m_Playing = false;
}

Sequencer::~Sequencer()
{
	Release();
}

void Sequencer::UpdateAll( float TimeStep )
{
	Sequencer *pNext = 0;
	Sequencer *pSequencer = m_pActiveList;

	while( pSequencer )
	{
		pNext = pSequencer->m_pNextActive;

		pSequencer->Update( TimeStep );

		pSequencer = pNext;
	}

	return;
}

Sequencer *Sequencer::AddEvent( float Time )
{
	if( !m_pEventList )
	{
		m_pEventList = NewEvent();

		if( m_pEventList )
			m_pEventList->m_Time = Time;
	}

	Sequencer *pEvent = m_pEventList;

	while( pEvent )
	{
		if( pEvent->m_Time == Time )
			return pEvent;
		else
		{
			if( pEvent->m_Time > Time )
			{
				Sequencer *pNewEvent = NewEvent();

				if( pNewEvent )
				{
					pNewEvent->m_Time = Time;
					pNewEvent->m_pNextEvent = pEvent;
					pNewEvent->m_pPrevEvent = pEvent->m_pPrevEvent;

					if( pEvent->m_pPrevEvent )
						pEvent->m_pPrevEvent->m_pNextEvent = pNewEvent;

					pEvent->m_pPrevEvent = pNewEvent;

					if( !pEvent->m_pPrevEvent )
						m_pEventList = pNewEvent;

					return pNewEvent;
				}
			}

			if( !pEvent->m_pNextEvent )
			{
				pEvent->m_pNextEvent = NewEvent();

				if( pEvent->m_pNextEvent )
				{
					pEvent->m_pNextEvent->m_Time = Time;
					pEvent->m_pNextEvent->m_pPrevEvent = pEvent;
				}

				return pEvent->m_pNextEvent;
			}
		}

		pEvent = pEvent->m_pNextEvent;
	}

	return 0;
}

void Sequencer::DelEvent( float Time )
{
	DelRange( Time, Time );
}

void Sequencer::DelRange( float FromTime, float ToTime )
{
	Sequencer *pEvent = m_pEventList;
	Sequencer *pNextEvent = 0;

	while( pEvent && m_pEvent->m_Time < FromTime )
		pEvent = pEvent->m_pNextEvent;

	while( pEvent && pEvent->m_Time >= FromTime && pEvent->m_Time <= ToTime )
	{
		pNextEvent = pEvent->m_pNextEvent;

		if( pEvent->m_pPrevEvent )
			pEvent->m_pPrevEvent->m_pNextEvent = pEvent->m_pNextEvent;
		else
			m_pEventList = pEvent->m_pNextEvent;

		if( pEvent->m_pNextEvent )
			pEvent->m_pNextEvent->m_pPrevEvent = pEvent->m_pPrevEvent;

		delete pEvent;

		pEvent = pNextEvent;
	}

	return;
}

void Sequencer::Release( void )
{
	Stop();

	while( m_pEventList )
		DelEvent( m_pEventList->m_Time );

	return;
}

void Sequencer::Update( float TimeStep )
{
	if( m_Playing )
	{
		float Time = m_Time + TimeStep;

		if( m_pEventList )
		{
			if( !m_pEvent )
				m_pEvent = m_pEventList;

			while( m_Playing && m_pEvent->m_Time <= Time )
			{
				// Trigger the event
				m_pEvent->Trigger( Time - m_pEvent->m_Time );

				// Have we reached the end?
				if( m_pEvent->m_pNextEvent )
					m_pEvent = m_pEvent->m_pNextEvent;
				else
				{
					if( !m_Looped )
						Stop();
					else
					{
						float OverStep = Time;

						while( OverStep >= m_pEvent->m_Time )
							OverStep -= m_pEvent->m_Time;

						Jump( m_LoopTime );

						Time = OverStep + m_LoopTime;
					}
				}
			}
		}
		m_Time = Time;
	}

	return;
}

void Sequencer::Play( bool Looped )
{
	Rewind();
	Resume( Looped );
}

void Sequencer::Resume( bool Looped )
{
	if( !m_pActiveList )
		m_pActiveList = this;
	else
	{
		Sequencer *pSequencer = m_pActiveList;

		while( pSequencer->m_pNextActive )
			pSequencer = pSequencer->m_pNextActive;

		pSequencer->m_pNextActive = this;
	}

	m_Playing = true;
	m_Looped = Looped;
}

void Sequencer::Stop( void )
{
	if( m_pActiveList == this )
		m_pActiveList = m_pNextActive;
	else
	{
		Sequencer *pSequencer = m_pActiveList;

		while( pSequencer && pSequencer->m_pNextActive != this )
			pSequencer = pSequencer->m_pNextActive;

		if( pSequencer && pSequencer->m_pNextActive == this )
			pSequencer->m_pNextActive = m_pNextActive;
	}

	m_pNextActive = 0;
	m_Playing = false;
}

void Sequencer::Rewind( void )
{
	Jump( 0.0f );
}

void Sequencer::Jump( float Time )
{
	if( m_pEventList )
	{
		if( Time <= m_pEventList->m_Time )
			m_pEvent = m_pEventList;
		else
		{
			Sequencer *pEvent = m_pEvent;

			if( !pEvent )
				pEvent = m_pEventList;

			if( pEvent->m_Time > Time )
			{
				while( pEvent->m_Time > Time )
					pEvent = pEvent->m_pPrevEvent;
			}
			else if( pEvent->m_Time < Time )
			{
				while( pEvent->m_pNextEvent && pEvent->m_pNextEvent->m_Time < Time )
					pEvent = pEvent->m_pNextEvent;
			}

			m_pEvent = pEvent;
		}
	}

	m_Time = Time;
}

void Sequencer::Trigger( float TimeDelta )
{
}

void Sequencer::SetLoopTime( float Time )
{
	m_LoopTime = Time;
}

float Sequencer::GetLoopTime( void )
{
	return m_LoopTime;
}

float Sequencer::PlayTime( void )
{
	if( !m_pEventList )
		return 0.0f;

	Sequencer *pEvent = m_pEvent;

	if( !pEvent )
		pEvent = m_pEventList;

	while( pEvent->m_pNextEvent )
		pEvent = pEvent->m_pNextEvent;

	return pEvent->m_Time;
}

Sequencer *Sequencer::Event( void )
{
	return m_pEvent;
}

Sequencer *Sequencer::NextEvent( void )
{
	if( m_pEvent )
		return m_pEvent->m_pNextEvent;

	return 0;
}

Sequencer *Sequencer::PrevEvent( void )
{
	if( m_pEvent )
		return m_pEvent->m_pPrevEvent;

	return 0;
}

EventSequencer::EventSequencer( void )
{
	m_pfnCallBack = 0;
}

void EventSequencer::SetEvent( float Time, EVENTCALLBACK pfnCallBack )
{
	EventSequencer *pEvent = (EventSequencer *) AddEvent( Time );

	if( pEvent )
		pEvent->m_pfnCallBack = pfnCallBack;

	return;
}

void EventSequencer::Trigger( float TimeDelta )
{
	(*m_pfnCallBack)( m_Time + TimeDelta );
}
