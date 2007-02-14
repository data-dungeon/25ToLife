#ifndef SCREEN_OPTIONS_H
#define SCREEN_OPTIONS_H

#include "platform/defcontrolmap.h"
#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenOptions
//=============================================================================

class ScreenOptions : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenOptions );

	public:
		ScreenOptions( void );
		virtual ~ScreenOptions();

		void ToggleAiming( void );
		void ToggleVibration( void );
		void IncreaseLookSensitivity( int N );
		void IncreaseLookSpeed( int N );
		void ToggleStereoMode( void );
		void IncreaseSound( int N );
		void IncreaseMusic( int N );
		void IncreaseGamma( int N );
		void ToggleAutoSave( void );
		void ToggleBlood( void );

		void SetSoundVolume( float Volume );
		void SetMusicVolume( float Volume );
		float GetSoundVolume( void );
		float GetMusicVolume( void );

		int GetMeterCount( int SpriteId );
		int GetMeterCount( Sprite* pSprite );
		void InitMeter( int SpriteId );
		void InitAllMeters( void );
		void UpdateMeter( int SpriteId, float ZeroToOne );
		void UpdateMeter( Sprite *pSprite, float ZeroToOne );
		void UpdateAllMeters( void );

		void SetGammaValue( float Gamma );
		void ShowGammaBar( bool Show );
		void MakeGammaTakeEffect( void );

		AvatarControlMapper* GetAvatarMapper( void );

		bool m_InvertedAiming;
		bool m_VibrationEnabled;
		int m_LookSensitivity;
		int m_LookSpeed;
#ifdef DIRECTX_PC
		enum {
			STEREOMODE_MONO,
			STEREOMODE_STEREO,
			STEREOMODE_STEREO_REVERSE,
		};
		int m_StereoMode;
		bool m_BlurFX;
#else
		bool m_StereoMode;
#endif
		float m_SoundVolume;
		float m_MusicVolume;
		float m_GammaValue;
		bool m_Blood;
		bool m_AutoSave;
		bool m_KeepSettings;

	protected:
		t_Handle				m_GammaTexHandle;
		TextureDatabase*	m_GammaTexDatabase;
		Texture*				m_GammaTexPtr;
		Image*				m_GammaImage;
		Image*				m_NewGammaImage;

	protected:
		virtual void OnInitialize( void );
		virtual void OnTerminate( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual float GetScreenIntroTime( void ) { return 0.800f; }
		virtual float GetScreenExitTime( void ) { return 0.600f; }
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnChangeOption( int Option, int N );
		virtual void OnButtonCancel( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonUnselect( Button* pButton, Sprite* pSprite );
		virtual void OnScreenUpdate( void );
		virtual void OnNextScreen( Screen* pNextScreen );
		virtual void OnCommand( int Command );
		virtual void OnCommandDone( void );
		virtual void OnSaveGameResult( int Result );
#ifdef DIRECTX_PC
		virtual void OnButtonOK();
#endif

		void  SetGammaZeroToOne( float gamma );
		float GetGammaZeroToOne();
};

#endif // SCREEN_OPTIONS_H