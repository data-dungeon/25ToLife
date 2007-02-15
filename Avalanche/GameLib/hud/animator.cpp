/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/animator.h"

KeyFrame::KeyFrame( float Time, float Value )
{
	m_pPrev = 0;
	m_pNext = 0;
	m_Time = Time;
	m_Value = Value;
}

KeyFrame::~KeyFrame()
{
	if( m_pPrev ) m_pPrev->m_pNext = m_pNext;
	if( m_pNext ) m_pNext->m_pPrev = m_pPrev;
}

Animator *Animator::m_pActiveList = 0;

Animator::Animator()
{
	m_Value = 0.0f;
	m_Timer = 0.0f;
	m_LoopPoint = 0.0f;
	m_FrameTime = 0.0f;
	m_Scale = 1.0f;

	m_Status = ANIMSTATUS_LERPED;

	m_Id = 0;
	m_Priority = 0;

	m_pFrame1 = 0;
	m_pFrame2 = 0;
	m_pFrames = 0;
	m_pTargetf = 0;
	m_pTargeti = 0;
}

Animator::~Animator()
{
	Release();
}

static char SuspendLevel = 0;

void Animator::UpdateAll( float TimeStep )
{
	Animator *pAnimator = m_pActiveList;

	while( pAnimator )
	{
		if( pAnimator->m_Priority >= SuspendLevel )
			pAnimator->Update( TimeStep );

		pAnimator = pAnimator->m_pActiveNext;
	}

	return;
}

void Animator::SuspendAll( char Priority )
{
	SuspendLevel = Priority;
}

void Animator::ResumeAll( void )
{
	SuspendLevel = 0;
}

void Animator::Release( void )
{
	Clear();

	m_Status = ANIMSTATUS_LERPED;

	m_Id = 0;
	m_Priority = 0;

	m_pFrame1 = 0;
	m_pFrame2 = 0;
	m_pFrames = 0;
	m_pTargetf = 0;
	m_pTargeti = 0;
}

void Animator::Update( float TimeStep )
{
	if( Playing() )
	{
		m_Timer += TimeStep;

		if( m_pFrame2 && !m_pFrame2->m_pNext && m_pFrame2->m_Time <= m_Timer  )
		{
			// Reached End, what do we do??

			if( !Looped() )
				m_Timer = m_pFrame2->m_Time;
			else
			{
				while( m_Timer >= m_pFrame2->m_Time )
					m_Timer -= m_pFrame2->m_Time;

				m_Timer += m_LoopPoint;
			}
		}

		if( Playing() )
		{
			if( m_pFrame1 )
			{
				while( m_pFrame1->m_pPrev && m_pFrame1->m_Time > m_Timer )
					m_pFrame1 = m_pFrame1->m_pPrev;

				m_pFrame2 = m_pFrame1;

				while( m_pFrame2->m_pNext && m_pFrame2->m_Time <= m_Timer )
					m_pFrame2 = m_pFrame2->m_pNext;

				// Yes, it seems redundant but its not... Trust me =)
				m_pFrame1 = m_pFrame2->m_pPrev;

				if( !m_pFrame1 )
					m_pFrame1 = m_pFrame2;

				DoUpdate();
			}

			if( m_pFrame2->m_Time == m_Timer && !Looped() )
				Stop();
		}
	}

	return;
}

void Animator::SetTarget( float *pTarget )
{
	m_pTargetf = pTarget;
}

void Animator::SetTarget( int *pTarget )
{
	m_pTargeti = pTarget;
}

void Animator::SetPriority( char Priority )
{
	m_Priority = Priority;
}

char Animator::GetPriority( void )
{
	return m_Priority;
}

void Animator::SetAnimId( char Id )
{
	m_Id = Id;
}

char Animator::GetAnimId( void )
{
	return m_Id;
}

KeyFrame *Animator::AddFrame( float Time )
{
	if( !m_pFrames )
		m_pFrames = new KeyFrame( Time );

	KeyFrame *pFrame = m_pFrames;

	while( pFrame->m_Time != Time )
	{
		if( pFrame->m_Time > Time )
		{
			KeyFrame *pNewFrame = new KeyFrame( Time );

			if( pNewFrame )
			{
				pNewFrame->m_pPrev = pFrame->m_pPrev;
				pNewFrame->m_pNext = pFrame;

				if( pFrame->m_pPrev )
					pFrame->m_pPrev->m_pNext = pNewFrame;

				pFrame->m_pPrev = pNewFrame;

				if( !pNewFrame->m_pPrev )
					m_pFrames = pNewFrame;

				return pNewFrame;
			}
		}
		else if( !pFrame->m_pNext )
		{
			pFrame->m_pNext = new KeyFrame( Time );

			if( pFrame->m_pNext )
				pFrame->m_pNext->m_pPrev = pFrame;
		}

		pFrame = pFrame->m_pNext;
	}

	return pFrame;
}

void Animator::SetFrame( float Time, float Value )
{
	KeyFrame *pFrame = AddFrame( Time );

	if( pFrame )
		pFrame->m_Value = Value;

	m_FrameTime = Time;

	return;
}

void Animator::SetRange( float FromTime, float ToTime, float Value )
{
	DelRange( FromTime, ToTime );		// Clear keyframes within this range

	SetFrame( FromTime, Value );
	SetFrame( ToTime, Value );

	return;
}

void Animator::NextFrame( float Time, float Value )
{
	SetFrame( m_FrameTime + Time, Value );
}

void Animator::DelFrame( float Time )
{
	KeyFrame *pFrame = m_pFrames;

	while( pFrame )
	{
		if( pFrame->m_Time == Time )
		{
			if( m_pFrames == pFrame )
				m_pFrames = pFrame->m_pNext;

			if( m_pFrame1 == pFrame )
				m_pFrame1 = m_pFrames;

			if( m_pFrame2 == pFrame )
				m_pFrame2 = m_pFrames;

			delete pFrame;

			break;
		}

		pFrame = pFrame->m_pNext;
	}

	return;
}

void Animator::DelRange( float FromTime, float ToTime )
{
	KeyFrame *pNext;
	KeyFrame *pFrame = m_pFrames;

	while( pFrame )
	{
		pNext = pFrame->m_pNext;

		if( pFrame->m_Time >= FromTime && pFrame->m_Time <= ToTime )
		{
			if( m_pFrames == pFrame )
				m_pFrames = pFrame->m_pNext;

			delete pFrame;
		}

		pFrame = pNext;
	}

	return;
}

void Animator::Clear( void )
{
	Stop();

	if( m_pFrames )
	{
		while( m_pFrames->m_pNext )
			delete m_pFrames->m_pNext;

		delete m_pFrames;

		m_pFrames = 0;
		m_pFrame1 = 0;
		m_pFrame2 = 0;
	}

	m_Timer = 0.0f;
	m_FrameTime = 0.0f;
	m_LoopPoint = 0.0f;
}

void Animator::CopyFrames( Animator *pSource, float TimeOffset, float ValueOffset )
{
	if( pSource )
	{
		KeyFrame *pFrame = pSource->m_pFrames;

		while( pFrame )
			SetFrame( pFrame->m_Time + TimeOffset, pFrame->m_Value + ValueOffset );
	}

	return;
}

void Animator::TimeStretch( float Scale )
{
	KeyFrame *pFrame = m_pFrames;

	while( pFrame )
		pFrame->m_Time = pFrame->m_Time * Scale;

	return;
}

void Animator::SetLoopPoint( float Time )
{
	m_LoopPoint = Time;
}

float Animator::TotalPlayTime( void )
{
	KeyFrame *pFrame = m_pFrame2;

	if( !pFrame )
	{
		pFrame = m_pFrame1;
		
		if( !pFrame )
		{
			pFrame = m_pFrames;

			if( !pFrame )
				return 0.0f;
		}
	}

	while( pFrame->m_pNext )
		pFrame = pFrame->m_pNext;

	return pFrame->m_Time;
}

void Animator::Interpolate( bool TrueOrFalse )
{
	m_Status &= ~ANIMSTATUS_LERPED;

	if( TrueOrFalse )
		m_Status |= ANIMSTATUS_LERPED;

	return;
}

void Animator::TimeCurve( bool TrueOrFalse )
{
	m_Status &= ~ANIMSTATUS_CURVED;

	if( TrueOrFalse )
		m_Status |= ANIMSTATUS_CURVED;

	return;
}

void Animator::Accelerate( bool TrueOrFalse )
{
	m_Status &= ~ANIMSTATUS_ACCELERATE;

	if( TrueOrFalse )
		m_Status |= ANIMSTATUS_ACCELERATE;

	return;
}

void Animator::Deccelerate( bool TrueOrFalse )
{
	m_Status &= ~ANIMSTATUS_DECCELERATE;

	if( TrueOrFalse )
		m_Status |= ANIMSTATUS_DECCELERATE;

	return;
}

void Animator::Play( bool Looped )
{
	Rewind();
	Resume( Looped );
}

void Animator::Resume( bool Looped )
{
	if( m_pFrames )
	{
		if( Stopped() )
		{
			m_pActiveNext = m_pActiveList;
			m_pActiveList = this;

			if( m_pActiveNext == this )
				return;
		}
		
		m_Status &= ~ANIMSTATUS_LOOPED;
		m_Status |= ANIMSTATUS_PLAYING;

		if( Looped )
			m_Status |= ANIMSTATUS_LOOPED;

		Update( 0.0f );
	}

	return;
}

void Animator::Stop( void )
{
	if( Playing() )
	{
		if( m_pActiveList == this )
			m_pActiveList = m_pActiveNext;
		else
		{
			Animator *pAnimator = m_pActiveList;

			while( pAnimator->m_pActiveNext )
			{
				if( pAnimator->m_pActiveNext == this )
				{
					pAnimator->m_pActiveNext = m_pActiveNext;
					break;
				}

				pAnimator = pAnimator->m_pActiveNext;
			}
		}

		m_Status &= ~ANIMSTATUS_PLAYING;
	}

	return;
}

void Animator::Rewind( void )
{
	m_pFrame1 = 0;			// Setting to zero before jumping makes Rewind() faster
	m_pFrame2 = 0;	

	Jump( 0.0f );
}

void Animator::Jump( float Time )
{
	m_Timer = Time;

	if( !m_pFrame1 )
		m_pFrame1 = m_pFrames;

	if( !m_pFrame1 )
		return;

	if( m_pFrame1->m_Time > Time )
	{
		while( m_pFrame1->m_pPrev && m_pFrame1->m_Time > Time )
			m_pFrame1 = m_pFrame1->m_pPrev;

		m_pFrame2 = m_pFrame1;

		if( m_pFrame2->m_pNext && m_pFrame2->m_Time <= Time )
			m_pFrame2 = m_pFrame2->m_pNext;
	}
	else
	{
		while( m_pFrame1->m_pNext && m_pFrame1->m_Time < Time )
			m_pFrame1 = m_pFrame1->m_pNext;

		m_pFrame2 = m_pFrame1->m_pNext;

		if( !m_pFrame2 )
			m_pFrame2 = m_pFrame1;
	}

	return;
}

bool Animator::Playing( void )
{
	if( m_Status & ANIMSTATUS_PLAYING )
		return true;

	return false;
}

bool Animator::Stopped( void )
{
	return !Playing();
}

bool Animator::Looped( void )
{
	if( m_Status & ANIMSTATUS_LOOPED )
		return true;

	return false;
}

float Animator::Value( void )
{
	return m_Value;
}

void Animator::DoUpdate( void )
{
	m_Value = m_pFrame2->m_Value;

	if( m_pFrame2->m_Time != m_pFrame1->m_Time )
	{
		float Range = m_pFrame2->m_Value - m_pFrame1->m_Value;
		float Time = (m_Timer - m_pFrame1->m_Time) / (m_pFrame2->m_Time - m_pFrame1->m_Time);

		if( m_Status & ANIMSTATUS_ACCELERATE )
			Time = Time * Time;

		if( m_Status & ANIMSTATUS_DECCELERATE )
			Time = 1.0f - (1.0f - Time) * (1.0f - Time);
		
		if( m_Status & ANIMSTATUS_CURVED )
			Time = Time * Time * (3.0f - 2.0f * Time);

		if( m_Status & ANIMSTATUS_LERPED )
			// lerping enabled
			m_Value = m_pFrame1->m_Value + Range * Time;
		else
			// lerping disabled
			m_Value = m_pFrame1->m_Value;
	}

	if( m_pTargetf )
		*m_pTargetf = m_Value;

	if( m_pTargeti )
		*m_pTargeti = (int) m_Value;

	return;
}

void Animator8::SetTarget( u8 *pTarget )
{
	m_pTarget = pTarget;
}

void Animator8::DoUpdate( void )
{
	Animator::DoUpdate();

	if( m_pTarget )
		*m_pTarget = (u8) (m_Value * 255.0f);

	return;
}

void Animator16::SetTarget( u16 *pTarget )
{
	m_pTarget = pTarget;
}

void Animator16::DoUpdate( void )
{
	Animator::DoUpdate();

	if( m_pTarget )
		*m_pTarget = (u16) m_Value;

	return;
}

void Animator32::SetTarget( u32 *pTarget )
{
	m_pTarget = pTarget;
}

void Animator32::DoUpdate( void )
{
	Animator::DoUpdate();

	if( m_pTarget )
		*m_pTarget = (u32) m_Value;

	return;
}