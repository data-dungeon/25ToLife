#ifndef OLD_HUD_H
#define OLD_HUD_H

#include "Math/Matrix.h"
#include "hud/hudlib.h"
#include "shell/ps2pad.h"
#include "shell/gcnpad.h"

#include <stdarg.h>
#include <stdlib.h>

// DEBUG related things go into the GameDebug

#define NUM_COORD_LINES	3
#define NUM_STATS_LINES 6
#define NUM_DEBUG_LINES 8

class GameDebug : public Hud
{
	public:
		GameDebug();
		virtual ~GameDebug();

		virtual void Initialize();
		virtual void Update();

		void WriteDebugMessage( int line, const char* Text );

		void ToggleFPS();
		void ShowFPS( bool TrueOrFalse );

		void ToggleCoordinates();
		void ShowCoordinates( bool TrueOrFalse );

		void ToggleRenderStats();
		void ShowRenderStats( bool TrueOrFalse );

		void ToggleNetworkStats();
		void ShowNetworkStats( bool TrueOrFalse );

		void ToggleMemoryStats();
		void ShowMemoryStats( bool TrueOrFalse );

		void ToggleInstanceStats();
		void ShowInstanceStats( bool TrueOrFalse );

		void ToggleAIStats();
		void ShowAIStats( bool TrueOrFalse );

		void ToggleCollisionStats();
		void ShowCollisionStats( bool TrueOrFalse );
	public:
		Font			m_Font;
		DebugMenu	m_Menu;

		Text			m_FPS;
		Text			m_Coordinates[NUM_COORD_LINES];
		Text			m_StatsText[NUM_STATS_LINES];
		Text			m_DebugText[NUM_DEBUG_LINES];

		bool			m_ShowFPS;
		bool			m_ShowCoordinates;
		int			m_Panel;

private:
		void ShowPanel( int panel );
		void UpdateFPS();
		void UpdateCoordinates();
		void UpdatePanelMemory();
		void UpdatePanelNetwork();
		void UpdatePanelRender();
		void UpdatePanelInstance();
		void UpdatePanelAI();
		void UpdatePanelCollision();
};

class GameHud : public Hud
{
	public:
		GameHud( void );
		virtual ~GameHud();
		virtual void Initialize( void );
		virtual void Update( void );
		virtual void Shutdown( void );
		
	public:
#ifdef ALLOW_DEBUG_MENU
		GameDebug m_Debug;
#endif //ALLOW_DEBUG_MENU
};

#endif
