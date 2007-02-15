#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include "platform/BaseType.h"

//=============================================================================
// class KeyFrame || Key frames are generated as needed by the Animator class.
//						||	You should never have to access the key frames directly.
//=============================================================================

class KeyFrame
{
	public:
		KeyFrame( float Time, float Value = 0.0f );
		~KeyFrame();

	public:
		KeyFrame *m_pPrev;
		KeyFrame *m_pNext;
		float m_Time;
		float m_Value;
};

//=============================================================================
// class Animator || Animators simplify the process of creating variable length
//						|| key-frame based animation.
//						||===========================================================
//						|| Instructions for using an Animator:
//						||
//						|| Specify the channel to be animated by calling
//						|| Animator::SetTarget(), for example:
//						||
//						||	MySprite::MySprite() {
//						||		m_Animator1.SetTarget( &m_color.R );
//						||		m_Animator2.SetTarget( &m_color.G );
//						||		m_Animator3.SetTarget( &m_color.B );
//						|| }
//						||	Next, specify the individual key frames
//						||
//						||		m_Animator1.SetFrame( 0.0f, 0.0f );	// The red channel
//						||		m_Animator1.SetFrame( 2.0f, 1.0f );	// will fade in/out
//						||		m_Animator1.SetFrame( 4.0f, 0.0f );	// every 4 seconds
//						||
//						||		m_Animator2.SetFrame( 0.0f, 0.0f );	// The green channel
//						||		m_Animator2.SetFrame( 3.0f, 1.0f );	// will fade in/out
//						||		m_Animator2.SetFrame( 6.0f, 0.0f );	// every 6 seconds
//						||
//						||		m_Animator3.SetFrame( 0.0f, 0.0f );	// The blue channel
//						||		m_Animator3.SetFrame( 4.0f, 1.0f );	// will fade in/out
//						||		m_Animator3.SetFrame( 8.0f, 0.0f );	// every 8 seconds
//						||		
//						|| Finally, just play the animations
//						||
//						||		m_Animator1.Play( true );				// Looped
//						||		m_Animator2.Play( true );
//						||		m_Animator3.Play( true );
//						||
//						|| Animator::Play() doesn't actually play the animation, all
//						|| it does is set some flags that say its OK to animate this
//						|| channel.  Animations aren't stepped until the game engine
//						|| calls Animator::UpdateAll(), which must be done every
//						|| frame.
//=============================================================================

class Animator
{
	public:
		Animator();
		virtual ~Animator();

		static void UpdateAll( float TimeStep );										// Step all animations
		static void SuspendAll( char Priority );										// Suspend animations who's priority level is less than
		static void ResumeAll( void );													// Resume animations that were suspended
		virtual void Release( void );														// Delete keyframes and free up memory
		virtual void Update( float TimeStep );											// Step the animation
		
		virtual void SetTarget( float *pTarget );										// Specify the animation target (float)
		virtual void SetTarget( int *pTarget );										// Specify the animation target (integer)
		virtual void SetPriority( char Priority );
		virtual char GetPriority( void );
		virtual void SetAnimId( char Id );
		virtual char GetAnimId( void );
		
		// Key frame generation and management
		virtual KeyFrame *AddFrame( float Time );										// Insert a key frame at the specified time
		virtual void SetFrame( float Time, float Value );							// Set the value of a key frame at the specified time.  Key frames will be generated if necessary.
		virtual void SetRange( float FromTime, float ToTime, float Value );	// Set the value of two key frames, and delete any frames between them
		virtual void NextFrame( float Time, float Value );
		virtual void DelFrame( float Time );											// Delete a key frame
		virtual void DelRange( float FromTime, float ToTime );					// Delete a range of key frames
		virtual void Clear( void );
		virtual void CopyFrames( Animator *pSource, float TimeOffset = 0.0f, float ValueOffset = 0.0f );
	
		// Effects
		virtual void TimeStretch( float Scale );

		// Playback control
		virtual void SetLoopPoint( float Time );
		virtual float TotalPlayTime( void );		
		virtual void Interpolate( bool TrueOrFalse );								// Enable or disable linear interpolation
		virtual void TimeCurve( bool TrueOrFalse );									// T = Time * Time * (3.0f - 2.0f * Time);
		virtual void Accelerate( bool TrueOrFalse );
		virtual void Deccelerate( bool TrueOrFalse );

		// Playback functions
		virtual void Play( bool Looped );												// Play an animation
		virtual void Resume( bool Looped );												// Resume an animation
		virtual void Stop( void );															// Stop an animation
		virtual void Rewind( void );														// Reset the timer back to 0 (not necessarily the first key frame)
		virtual void Jump( float Time );													// Jump to a specific point in the animation

		// Playback information
		virtual bool Playing( void );
		virtual bool Stopped( void );
		virtual bool Looped( void );
		virtual float Value( void );

	protected:
		virtual void DoUpdate( void );
		
	public:
		float m_Value;
		float m_Timer;
		float m_LoopPoint;
		float m_FrameTime;
		float m_Scale;
		char	m_Status;
		char	m_Interpolation;
		char	m_Id;
		char	m_Priority;

	protected:
		KeyFrame *m_pFrame1;
		KeyFrame *m_pFrame2;

	private:
		static Animator *m_pActiveList;
		Animator *m_pActiveNext;
		KeyFrame *m_pFrames;
		float *m_pTargetf;
		int *m_pTargeti;
};

#define ANIMSTATUS_PLAYING			(1<<0)
#define ANIMSTATUS_LOOPED			(1<<1)
#define ANIMSTATUS_LERPED			(1<<2)
#define ANIMSTATUS_REVERSED		(1<<3)
#define ANIMSTATUS_CURVED			(1<<4)
#define ANIMSTATUS_ACCELERATE		(1<<5)
#define ANIMSTATUS_DECCELERATE	(1<<6)


class Animator8 : public Animator
{
	public:
		virtual void SetTarget( u8 *pTarget );

	protected:
		virtual void DoUpdate( void );

	private:
		u8 *m_pTarget;
};

class Animator16 : public Animator
{
	public:
		virtual void SetTarget( u16 *pTarget );

	protected:
		virtual void DoUpdate( void );

	private:
		u16 *m_pTarget;
};

class Animator32 : public Animator
{
	public:
		virtual void SetTarget( u32 *pTarget );

	protected:
		virtual void DoUpdate( void );

	private:
		u32 *m_pTarget;
};

#endif __ANIMATOR_H__