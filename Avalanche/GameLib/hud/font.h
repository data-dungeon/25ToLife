#ifndef FONT_H
#define FONT_H

#include "Game/Database/DBFile.h"
#include "hud/hudobject.h"
#include "hud/stringex.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

//=============================================================================
// To generate a font, run $Avalanche\Tools\FontRip\Release\Font.exe.
// Export the image to .TGA format and run the file through ImportExport.
// You will need to copy the .fnt and .dbl to the \\data directory.
//=============================================================================

//=============================================================================
// class Font : Facilitates management of textures that contain closely packed
//					 variable sized cells, each containing one glyph.
//=============================================================================

class Font : public HudObject
{
	DECLARE_TREENODE( Font );

	public:
		Font( void );
		virtual ~Font();

		typedef struct
		{
			t_Handle textureset;
			int texture;

			int width;
			int height;
			int x1;
			int y1;
			int x2;
			int y2;

			float widthf;
			float heightf;
			float u1;
			float v1;
			float u2;
			float v2;

			float cx;
			float cy;
			float cw;
			float ch;
			float cs;
		}
		CustomChar;

	public:
		// The System Font is the default font that anyone can use.

		static Font *LoadSystemFont( const char* pFileName ); // System font is permanent
		static Font *FindFont( const char *pFileName );
		static Font *LoadFont( const char *pFileName, bool permanent = false );
		static Font *GetFirstFont( void );
		static void FlushAllFonts( void );

		// When loading fonts, do not specify the file extention.
		bool Load( const char *pFontName, bool permanent );
		void Release( void );
																// Returns the texture coordinates for the specified character
		const char* Name( void );						// Returns the font name
		float GetTextLength( const char *text );	// Computes the length of a string
		float GetTextHeight( const char *text );	// Computes the height of a string
		float GetCharWidth( char ch );				// Returns the width of a single character
		float GetCharHeight( char ch );				// Returns the height of a single character
		float GetFontHeight( void );					// Returns the height of the font
		float GetAscent( void );						// Returns the baseline
		float GetDescent( void );						// Returns the space below the baseline
		float GetInternalLeading( void );
		float GetExternalLeading( void );			// Returns the space between linefeeds BELOW the font

		void SetTextureRect( int ch, int u1, int v1, int u2, int v2 );
		void GetTextureRect( int ch, int &u1, int &v1, int &u2, int &v2 );
		void SetTextureRect( int ch, float u1, float v1, float u2, float v2 );
		void GetTextureRect( int ch, float &u1, float &v1, float &u2, float &v2 );
		void SetCustomChar( int ch, t_Handle textureset, int texture, int u1, int v1, int u2, int v2 );
		CustomChar* GetCustomChar( int ch );

	private:
		void EndianSwap( void );

	public:
		static Font m_FontList;
		xstring m_FontName;
		bool m_permanent;

		struct {
			t_Handle handle;			// m_texture.handle
			int texture;				// m_texture.texture
			float width;				// m_texture.width
			float height;				// m_texture.height
		} m_texture;
		struct {
			struct {
				s16 x1;					// m_metrics.table.x1	// texture coordinates
				s16 y1;					// m_metrics.table.y1	// texture coordinates
				s16 x2;					// m_metrics.table.x2	// texture coordinates
				s16 y2;					// m_metrics.table.y2	// texture coordinates
				s32 cx;					// m_metrics.table.cx	// this is the space to the left of the character
				s32 cy;					// m_metrics.table.cw	// this is the space above the character (not the same as m_metrics.leadin)
				s32 cw;					// m_metrics.table.cw	// this is the physical width of the character
				s32 ch;					// m_metrics.table.ch	// this is the physical height of the character
				s32 cs;					// m_metrics.table.cs	// this is the extra space added to the end of the character
			} table[256];
			s32 height;					// m_metrics.height		// physical height (m_metrics.ascent + m_metrics.descent)
			s32 ascent;					// m_metrics.ascent		// this is the baseline
			s32 descent;				// m_metrics.descent		// this is the space below the baseline--g's, j's, p's and q's occupy this space
			s32 leadin;					// m_metrics.leadin		// this is the space between linefeeds ABOVE the font
			s32 leadout;				// m_metrics.leadout		// this is the space between linefeeds BELOW the font
		} m_metrics;
		struct {
			struct {
				float x1;				// m_metricsf.table.x1	// texture coordinates
				float y1;				// m_metricsf.table.y1  // texture coordinates
				float x2;				// m_metricsf.table.x2  // texture coordinates
				float y2;				// m_metricsf.table.y2  // texture coordinates
				float cx;				// m_metricsf.table.cx  // character offset
				float cy;				// m_metricsf.table.cy  // character offset
				float cw;				// m_metricsf.table.cw  // character width
				float ch;				// m_metricsf.table.ch  // character height
				float cs;				// m_metricsf.table.cs
			} table[256];
			float height;				// m_metricsf.height
			float ascent;				// m_metricsf.ascent
			float descent;				// m_metricsf.descent
			float leadin;				// m_metricsf.leadin
			float leadout;				// m_metricsf.leadout
		} m_metricsf;

		CustomChar* m_CustomChars[256];

		struct NameMap
		{
			const char *request;
			const char *use;
		};
		static const NameMap s_map[];
		static const char *MapFontName(const char *fontName);
};

#endif

