// GCNIconData.h: interface for the CGCNIconData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GCNICONDATA_H__E9C9BC22_A753_43E6_89C4_36F4FCD05837__INCLUDED_)
#define AFX_GCNICONDATA_H__E9C9BC22_A753_43E6_89C4_36F4FCD05837__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <dolphin/card.h>

#define GCN_COMMENTS_SIZE		32
#define GCN_MAX_ICONS			8

struct ts_GCNIconHeader
{
	unsigned char		bannerFormat,
							iconFormat,
							animType,
							iconSpeed[GCN_MAX_ICONS],
							filler[32 - 11];
};

class CGCNIconData  
{
public:
	void OutputIconFile( const char *pOutputName);
	enum				IconCommands
	{
		COMMENT1,
		COMMENT2,
		BANNER,
		ANIMTYPE,
		ICON0,
		ICON1,
		ICON2,
		ICON3,
		ICON4,
		ICON5,
		ICON6,
		ICON7,
		ICONSPEED0,
		ICONSPEED1,
		ICONSPEED2,
		ICONSPEED3,
		ICONSPEED4,
		ICONSPEED5,
		ICONSPEED6,
		ICONSPEED7,
		ICONUNKNOWN,
		ICONEMPTY
	};

	void ReadCommandFile( const char *pCmdFileName);
	CGCNIconData( void);
	virtual ~CGCNIconData( );

private:
	void WriteBitmap( HANDLE hFile, BITMAPINFO *pbmi, char *bmBits);
	void WritePalette( HANDLE hFile, BITMAPINFO *pbmi);
	void SetAnimType( const char *pValue);
	void Strip( char *pCmd);
	void SetIconSpeed( const char *pValue, int iIcon);
	void ReadIcon( const char *pIconFileName, int iIcon);
	void ReadBanner( const char *pBannerFileName);
	void SetComment2( const char *pValue);
	void SetComment1( const char *pValue);
	CGCNIconData::IconCommands ParseCommandLine( char *pCmd, char **ppValue);
	void				ProcessCommand( char *pCmd);

	bool					m_abIconPresent[GCN_MAX_ICONS];
	char					m_acComment1[GCN_COMMENTS_SIZE];
	char					m_acComment2[GCN_COMMENTS_SIZE];

	BITMAPINFO			*m_abmiIcon[GCN_MAX_ICONS],
							*m_bmiBanner;

	char					*m_abmBitsIcon[GCN_MAX_ICONS],
							*m_bmBitsBanner;

	int					m_aIconSpeed[GCN_MAX_ICONS];
	int					m_AnimType;

	bool					m_bBannerPresent;
};

#endif // !defined(AFX_GCNICONDATA_H__E9C9BC22_A753_43E6_89C4_36F4FCD05837__INCLUDED_)
