// GCNIconData.cpp: implementation of the CGCNIconData class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "GCNIconData.h"
#include "MakeGCNIcon.h"
#include "ReadImage.h"

typedef struct ts_CommandTable_tag
{
	const char							*pCommand;
	CGCNIconData::IconCommands		command;
} ts_CommandTable;

static ts_CommandTable commandTable[] =
{
	{ "comment1", CGCNIconData::COMMENT1},
	{ "comment2", CGCNIconData::COMMENT2},
	{ "animtype", CGCNIconData::ANIMTYPE},
	{ "banner", CGCNIconData::BANNER},
	{ "icon0", CGCNIconData::ICON0},
	{ "icon1", CGCNIconData::ICON1},
	{ "icon2", CGCNIconData::ICON2},
	{ "icon3", CGCNIconData::ICON3},
	{ "icon4", CGCNIconData::ICON4},
	{ "icon5", CGCNIconData::ICON5},
	{ "icon6", CGCNIconData::ICON6},
	{ "icon7", CGCNIconData::ICON7},
	{ "iconspeed0", CGCNIconData::ICONSPEED0},
	{ "iconspeed1", CGCNIconData::ICONSPEED1},
	{ "iconspeed2", CGCNIconData::ICONSPEED2},
	{ "iconspeed3", CGCNIconData::ICONSPEED3},
	{ "iconspeed4", CGCNIconData::ICONSPEED4},
	{ "iconspeed5", CGCNIconData::ICONSPEED5},
	{ "iconspeed6", CGCNIconData::ICONSPEED6},
	{ "iconspeed7", CGCNIconData::ICONSPEED7},
	{ "", CGCNIconData::ICONUNKNOWN}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGCNIconData::CGCNIconData( void)
{
	memset( m_acComment1, ' ', GCN_COMMENTS_SIZE);
	memset( m_acComment2, ' ', GCN_COMMENTS_SIZE);

	m_bBannerPresent = false;

	m_AnimType = CARD_STAT_ANIM_LOOP;

	m_bmBitsBanner = NULL;

	for( int iIcon = 0; iIcon < GCN_MAX_ICONS; iIcon++)
	{
		m_abmBitsIcon[iIcon] = NULL;
		m_abIconPresent[iIcon] = false;
		m_aIconSpeed[iIcon] = CARD_STAT_SPEED_END;
	}
}

CGCNIconData::~CGCNIconData( )
{
	for( int iIcon = 0; iIcon < GCN_MAX_ICONS; iIcon++)
	{
		if( m_abmBitsIcon[iIcon] != NULL)
		{
			free( m_abmBitsIcon[iIcon]);
		}
	}

	if( m_bmBitsBanner != NULL)
	{
		free( m_bmBitsBanner);
	}
}

void CGCNIconData::ReadCommandFile( const char *pCmdFileName)
{

	FILE		*pCmdFile;

	char		acCmd[400];

	OutputMsg( "Opening <InputCommandFile> %s\n", pCmdFileName);
	pCmdFile = fopen( pCmdFileName, "rt");
	if( pCmdFile == NULL)
	{
		FatalMsg( "Unable to open <InputCommandFile> %s\n", pCmdFileName);
	}

	while( !feof( pCmdFile))
	{
		fgets( acCmd, 400, pCmdFile);
		ProcessCommand( acCmd);
	}
	fclose( pCmdFile);
}

void CGCNIconData::ProcessCommand( char *pCmd)
{

	char		*pValue;

	bool		bErrors = false;

	switch( ParseCommandLine( pCmd, &pValue))
	{
		case ICONEMPTY:
			break;

		case COMMENT1:
			SetComment1( pValue);
			break;

		case COMMENT2:
			SetComment2( pValue);
			break;

		case BANNER:
			ReadBanner( pValue);
			break;

		case ANIMTYPE:
			SetAnimType( pValue);
			break;

		case ICON0:
			ReadIcon( pValue, 0);
			break;

		case ICON1:
			ReadIcon( pValue, 1);
			break;

		case ICON2:
			ReadIcon( pValue, 2);
			break;

		case ICON3:
			ReadIcon( pValue, 3);
			break;

		case ICON4:
			ReadIcon( pValue, 4);
			break;

		case ICON5:
			ReadIcon( pValue, 5);
			break;

		case ICON6:
			ReadIcon( pValue, 6);
			break;

		case ICON7:
			ReadIcon( pValue, 7);
			break;

		case ICONSPEED0:
			SetIconSpeed( pValue, 0);
			break;

		case ICONSPEED1:
			SetIconSpeed( pValue, 1);
			break;

		case ICONSPEED2:
			SetIconSpeed( pValue, 2);
			break;

		case ICONSPEED3:
			SetIconSpeed( pValue, 3);
			break;

		case ICONSPEED4:
			SetIconSpeed( pValue, 4);
			break;

		case ICONSPEED5:
			SetIconSpeed( pValue, 5);
			break;

		case ICONSPEED6:
			SetIconSpeed( pValue, 6);
			break;

		case ICONSPEED7:
			SetIconSpeed( pValue, 7);
			break;

		case ICONUNKNOWN:
			bErrors = true;
			break;
	}

	if( bErrors)
	{
		FatalMsg( "Errors in <InputCommandFile>... terminating\n");
	}
}

CGCNIconData::IconCommands CGCNIconData::ParseCommandLine( char *pCmd, char **ppValue)
{

	char				*pCmdSave,
						*pValueStart;

	IconCommands	result;

	pCmdSave = new char[ strlen( pCmd) + 1];

	OutputMsg( "Parsing <CommandInputFile> line %s\n", pCmd);

	strcpy( pCmdSave, pCmd);

	Strip( pCmd);

	if( strlen( pCmd) == 0)
	{
		delete [] pCmdSave;
		return( ICONEMPTY);
	}

	pValueStart = strchr( pCmd, '=');
	if( pValueStart == NULL)
	{
		ErrorMsg( "Invalid line in <InputCommandFile> -- %s\n", pCmdSave);
		delete [] pCmdSave;
		return( ICONUNKNOWN);
	}

	*ppValue = pValueStart + 1;

	*pValueStart = '\0';

	result = ICONUNKNOWN;
	for( int iEntry = 0; strlen( commandTable[iEntry].pCommand) != 0; iEntry++)
	{
		if( stricmp( pCmd, commandTable[iEntry].pCommand) == 0)
		{
			result = commandTable[iEntry].command;
			break;
		}
	}

	if( result == ICONUNKNOWN)
	{
		ErrorMsg( "Invalid line in <InputCommandFile> -- %s\n", pCmdSave);
	}

	delete [] pCmdSave;
 	return( result);
}

void CGCNIconData::Strip( char *pCmd)
{

	char		*pStart,
				*pComment;

	bool		bInString;

	pComment = strchr( pCmd, '#');
	if( pComment != NULL)
	{
		*pComment = '\0';
	}

	pStart = pCmd;
	bInString = false;
	while( (int)( pStart - pCmd) < (int)strlen( pCmd))
	{
		if( *pStart == '"')
		{
			bInString = !bInString;
			strcpy( pStart, pStart + 1);
		}
		else
		if( isspace( *pStart))
		{
			if( bInString)
			{
				*pStart = ' ';
				pStart++;
			}
			else
			{
				strcpy( pStart, pStart + 1);
			}
		}
		else
		{
			pStart++;
		}
	}
}

void CGCNIconData::SetComment1( const char *pValue)
{
	OutputMsg( "Setting COMMENT1 to %s\n", pValue);

	if( strlen( pValue) > GCN_COMMENTS_SIZE)
	{
		FatalMsg( "Value for COMMENT1 is too long -- %s\n", pValue);
	}

	strncpy( m_acComment1, pValue, strlen( pValue));
}

void CGCNIconData::SetComment2( const char *pValue)
{
	OutputMsg( "Setting COMMENT2 to %s\n", pValue);

	if( strlen( pValue) > GCN_COMMENTS_SIZE)
	{
		FatalMsg( "Value for COMMENT2 is too long -- %s\n", pValue);
	}

	strncpy( m_acComment2, pValue, strlen( pValue));
}

void CGCNIconData::ReadBanner( const char *pBannerFileName)
{

	HANDLE		hFile;

	short			sFrames;

	int			iResult;

	OutputMsg( "Opening BANNER %s\n", pBannerFileName);
	hFile = CreateFile(	pBannerFileName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
 
	if( hFile == INVALID_HANDLE_VALUE)
	{
		FatalMsg( "Could not open BANNER %s.\n", pBannerFileName);
	}
	
	OutputMsg( "Checking file type for BANNER %s\n", pBannerFileName);
	iResult = DetermineImageType( hFile, &sFrames);

	if( iResult == TYPE_ANM || iResult == TYPE_UNKNOWN)
	{
		CloseHandle( hFile);
		FatalMsg( "Unable to read BANNER %s -- image format not known or not supported\n", pBannerFileName);
	}

	OutputMsg( "Reading BANNER %s\n", pBannerFileName);
	if( !ReadImage( hFile, &m_bmiBanner, &m_bmBitsBanner))
	{
		FatalMsg( "Unable to read BANNER %s --	ReadImage failed\n", pBannerFileName);
	}

	if( m_bmiBanner->bmiHeader.biBitCount != 8)
	{
		FatalMsg( "Invalid BANNER %s -- only paletted images are supported\n", pBannerFileName);
	}

	if( m_bmiBanner->bmiHeader.biWidth != 96)
	{
		FatalMsg( "Invalid BANNER %s -- image is not 96 pixels in width\n", pBannerFileName);
	}

	if( m_bmiBanner->bmiHeader.biHeight != 32)
	{
		FatalMsg( "Invalid BANNER %s -- image is not 32 pixels in height\n", pBannerFileName);
	}

	m_bBannerPresent = true;
}

void CGCNIconData::ReadIcon( const char *pIconFileName, int iIcon)
{

	HANDLE		hFile;

	short			sFrames;

	int			iResult;

	OutputMsg( "Opening ICON%d %s\n", iIcon, pIconFileName);
	hFile = CreateFile(	pIconFileName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
 
	if( hFile == INVALID_HANDLE_VALUE)
	{
		FatalMsg( "Could not open ICON%d %s.\n", iIcon, pIconFileName);
	}
	
	OutputMsg( "Checking file type for ICON%d %s\n", iIcon, pIconFileName);
	iResult = DetermineImageType( hFile, &sFrames);

	if( iResult == TYPE_ANM || iResult == TYPE_UNKNOWN)
	{
		CloseHandle( hFile);
		FatalMsg( "Unable to read ICON%d %s -- image format not known or not supported\n", iIcon, pIconFileName);
	}

	OutputMsg( "Reading ICON%d %s\n", iIcon, pIconFileName);
	if( !ReadImage( hFile, &m_abmiIcon[iIcon], &m_abmBitsIcon[iIcon]))
	{
		FatalMsg( "Unable to read ICON%d %s --	ReadImage failed\n", iIcon, pIconFileName);
	}

	if( m_abmiIcon[iIcon]->bmiHeader.biBitCount != 8)
	{
		FatalMsg( "Invalid ICON%d %s -- only paletted images are supported\n", iIcon, pIconFileName);
	}

	if( m_abmiIcon[iIcon]->bmiHeader.biWidth != 32)
	{
		FatalMsg( "Invalid ICON%d %s -- image is not 32 pixels in width\n", iIcon, pIconFileName);
	}

	if( m_abmiIcon[iIcon]->bmiHeader.biHeight != 32)
	{
		FatalMsg( "Invalid ICON%d %s -- image is not 32 pixels in height\n", iIcon, pIconFileName);
	}

	m_abIconPresent[iIcon] = true;
}

void CGCNIconData::SetIconSpeed(const char *pValue, int iIcon)
{
	OutputMsg( "Setting icon %d speed to %s\n", iIcon, pValue);

	if( stricmp( pValue, "slow") == 0)
	{
		m_aIconSpeed[iIcon] = CARD_STAT_SPEED_SLOW;
	}
	else
	if( stricmp( pValue, "middle") == 0)
	{
		m_aIconSpeed[iIcon] = CARD_STAT_SPEED_MIDDLE;
	}
	else
	if( stricmp( pValue, "fast") == 0)
	{
		m_aIconSpeed[iIcon] = CARD_STAT_SPEED_FAST;
	}
	else
	{
		FatalMsg( "Invalid value for ICONSPEED%d -- %s\n", iIcon, pValue);
	}
}

void CGCNIconData::SetAnimType( const char *pValue)
{
	OutputMsg( "Setting anim type to %s\n", pValue);

	if( stricmp( pValue, "loop") == 0)
	{
		m_AnimType = CARD_STAT_ANIM_LOOP;
	}
	else
	if( stricmp( pValue, "bounce") == 0)
	{
		m_AnimType = CARD_STAT_ANIM_BOUNCE;
	}
	else
	{
		FatalMsg( "Invalid value for ANIMTYPE -- %s\n", pValue);
	}
}

void CGCNIconData::OutputIconFile(const char *pOutputName)
{

	ts_GCNIconHeader	gcnIconHdr;

	HANDLE				hFile;

	DWORD					nWritten;

	OutputMsg( "Opening <OutputFile> %s\n", pOutputName);
	hFile = CreateFile(	pOutputName,
								GENERIC_WRITE,
								0,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
 
	if( hFile == INVALID_HANDLE_VALUE)
	{
		FatalMsg( "Could not open <OutputFile> %s.\n", pOutputName);
	}
	
	memset( &gcnIconHdr, 0, sizeof(ts_GCNIconHeader));

	gcnIconHdr.bannerFormat = ( m_bBannerPresent) ? CARD_STAT_BANNER_C8 : CARD_STAT_BANNER_NONE;
	gcnIconHdr.iconFormat = CARD_STAT_ICON_C8;
	gcnIconHdr.animType = m_AnimType;
	memcpy( gcnIconHdr.iconSpeed, m_aIconSpeed, GCN_MAX_ICONS);

	OutputMsg( "Writing header to <OutputFile> %s.\n", pOutputName);
	if( !WriteFile( hFile, &gcnIconHdr, sizeof( ts_GCNIconHeader), &nWritten, NULL))
	{
		FatalMsg( "Could not write header to <OutputFile> %s.\n", pOutputName);
	}

	OutputMsg( "Writing comment1 to <OutputFile> %s\n", pOutputName);
	if( !WriteFile( hFile, m_acComment1, GCN_COMMENTS_SIZE, &nWritten, NULL))
	{
		FatalMsg( "Could not write comment1 to <OutputFile> %s.\n", pOutputName);
	}

	OutputMsg( "Writing comment2 to <OutputFile> %s\n", pOutputName);
	if( !WriteFile( hFile, m_acComment2, GCN_COMMENTS_SIZE, &nWritten, NULL))
	{
		FatalMsg( "Could not write comment2 to <OutputFile> %s.\n", pOutputName);
	}

	if( m_bBannerPresent)
	{
		OutputMsg( "Writing Banner bitmap to <OutputFile> %s.\n", pOutputName);
		WriteBitmap( hFile, m_bmiBanner, m_bmBitsBanner);

		OutputMsg( "Writing Banner palette to <OutputFile> %s.\n", pOutputName);
		WritePalette( hFile, m_bmiBanner);
	}

	for( int iIcon = 0; iIcon < GCN_MAX_ICONS; iIcon++)
	{
		if( m_abIconPresent[iIcon])
		{
			OutputMsg( "Writing Icon %d bitmap to <OutputFile> %s.\n", iIcon, pOutputName);
			WriteBitmap( hFile, m_abmiIcon[iIcon], m_abmBitsIcon[iIcon]);
		}
	}

	OutputMsg( "Writing Icon palette to <OutputFile> %s.\n", pOutputName);
	WritePalette( hFile, m_abmiIcon[0]);

	OutputMsg( "Closing <OutputFile> %s.\n", pOutputName);
	CloseHandle( hFile);
}

void CGCNIconData::WritePalette( HANDLE hFile, BITMAPINFO *pbmi)
{

	short		*psPaletteEntry;

	char		acData[2];

	DWORD		nWritten;

	psPaletteEntry = (short *)acData;
	for( int iColor = 0; iColor < 256; iColor++)
	{
		if( iColor == 0)
		{
			*psPaletteEntry = 0;
		}
		else
		{
			*psPaletteEntry =	0x8000 |
									( ( pbmi->bmiColors[iColor].rgbRed >> 3) << 10) |
									( ( pbmi->bmiColors[iColor].rgbGreen >> 3) << 5) |
									( ( pbmi->bmiColors[iColor].rgbBlue >> 3) << 0);
		}
		char temp = acData[0];
		acData[0] = acData[1];
		acData[1] = temp;
		WriteFile( hFile, acData, 2, &nWritten, NULL);
	}
}
			
void CGCNIconData::WriteBitmap( HANDLE hFile, BITMAPINFO *pbmi, char *bmBits)
{

	int		nBlocksX,
				nBlocksY,
				iStride;

	char		*pRowSrc,
				*pColSrc;

	DWORD		nWritten;

	nBlocksX = pbmi->bmiHeader.biWidth / 8;
	nBlocksY = pbmi->bmiHeader.biHeight / 4;

	iStride = -pbmi->bmiHeader.biWidth;

	pRowSrc = bmBits + ( pbmi->bmiHeader.biWidth * ( pbmi->bmiHeader.biHeight - 1));	//bmbits is inverted in y
	for( int iBlockY = 0; iBlockY < nBlocksY; iBlockY++)
	{
		pColSrc = pRowSrc;
		for( int iBlockX = 0; iBlockX < nBlocksX; iBlockX++)
		{
			for( int iRow = 0; iRow < 4; iRow++)
			{
				WriteFile( hFile, pColSrc + ( iRow * iStride), 8, &nWritten, NULL);
			}
			pColSrc += 8;
		}
		pRowSrc += 4 * iStride;
	}
}
