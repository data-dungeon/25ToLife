#ifndef __UI_H__
#define __UI_H__

#include "Layers/LayersStruct.h"

class UserInterface
{
	public:
		UserInterface( void );
		virtual ~UserInterface();

	public:
		virtual void Initialize( void );
		virtual bool LoadTextureSet( const char *pFileName );
		virtual void FreeTextureSet( void );
		virtual t_Handle TextureSet( void );
		virtual void Update( void );
		virtual void ProcessControllerInput( void );
		virtual void Activate( void );
		virtual void Deactivate( void );
		virtual bool Active( void );
		virtual bool Inactive( void );

	protected:
		virtual void OnPadUp( void );
		virtual void OnPadDown( void );
		virtual void OnPadLeft( void );
		virtual void OnPadRight( void );
		virtual void OnPadTriangle( void );
		virtual void OnPadCircle( void );
		virtual void OnPadSquare( void );
		virtual void OnPadX( void );
		virtual void OnPadStart( void );
		virtual void OnPadSelect( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnStartButton( void );
		virtual void OnSelectButton( void );
		virtual void OnActivate( void );
		virtual void OnDeactivate( void );

	public:
		bool	m_Active;

	private:
		t_Handle m_TextureSet;
};

#endif // __UI_H__