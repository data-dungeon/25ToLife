// Netapi32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Netapi32.h"
#include "my-nb30.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define APP_NAME "fake Netapi32.dll"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//			MessageBox( NULL, APP_NAME " DllMain, PROCESS_ATTACH", APP_NAME, MB_OK );
			break;
		case DLL_THREAD_ATTACH:
//			MessageBox( NULL, APP_NAME " DllMain, THREAD_ATTACH", APP_NAME, MB_OK );
			break;
		case DLL_THREAD_DETACH:
//			MessageBox( NULL, APP_NAME " DllMain, THREAD_DETATCH", APP_NAME, MB_OK );
			break;
		case DLL_PROCESS_DETACH:
//			MessageBox( NULL, APP_NAME " DllMain, PROCESS_DETATCH", APP_NAME, MB_OK );
			break;
	}
	return TRUE;
}

//static unsigned char MACaddress[] = { 0x00, 0x0c, 0x6e, 0x93, 0xb2, 0x4a };
static unsigned char MACaddresses[ MAX_LANA ][ 6 ];
static int numMACaddresses = 0;
static bool MACisValid = false;

bool ParseMAC( FILE* fp )
{
	char line[ 2048 ];
	while ( fgets( line, sizeof( line ) - 1, fp ) )
	{
		int A, B, C, D, E, F;
		char* p = strchr( line, '-' );
		if ( p && isxdigit(p[-1]) && isxdigit(p[-2]) && sscanf( p-2, "%x-%x-%x-%x-%x-%x", &A, &B, &C, &D, &E, &F ) == 6 )
		{
			if ( numMACaddresses < MAX_LANA )
			{
				MACaddresses[ numMACaddresses ][ 0 ] = A;
				MACaddresses[ numMACaddresses ][ 1 ] = B;
				MACaddresses[ numMACaddresses ][ 2 ] = C;
				MACaddresses[ numMACaddresses ][ 3 ] = D;
				MACaddresses[ numMACaddresses ][ 4 ] = E;
				MACaddresses[ numMACaddresses ][ 5 ] = F;
				numMACaddresses++;
				return true;
			}
		}
	}
	return false;
}

void ReadMACFromFile()
{
	if ( ! MACisValid )
	{
		const char* path = getenv( "SN_PATH" );
		if ( path && path[0] && strlen( path ) < 495 )
		{
			char filename[ 512 ];
			for ( int i = 0 ; i < 10 ; i++ )
			{
				FILE* fp;
				sprintf( filename, "%s\\snl%04d.txt", path, i );
				if ( ( fp = fopen( filename, "rt" ) ) != NULL )
				{
					ParseMAC( fp );
					fclose( fp );
				}
			}
		}
		MACisValid = true;

		//char buf[ 512 ], *p = buf;
		//p += sprintf( p, "n=%d ", numMACaddresses );
		//for ( int i = 0 ; i < numMACaddresses ; i++ )
		//{
		//	p += sprintf( p, "[%02x-%02x-%02x-%02x-%02x-%02x] ",
		//						MACaddresses[ i ][ 0 ], MACaddresses[ i ][ 1 ], MACaddresses[ i ][ 2 ],
		//						MACaddresses[ i ][ 3 ], MACaddresses[ i ][ 4 ], MACaddresses[ i ][ 5 ] );
		//}
		//MessageBox( NULL, buf, "hi", MB_OK );
	}
}

// fake NetBios function
extern "C"
{
	NETAPI32_API UCHAR Netbios( PNCB pncb )
	{
//		char buf[ 256 ];
//		sprintf( buf, "%s Netbios command: %d", APP_NAME, pncb->ncb_command );
//		MessageBox( NULL, buf, APP_NAME, MB_OK );

		switch ( pncb->ncb_command )
		{
			case NCBENUM:
				{
					ReadMACFromFile();
					PLANA_ENUM lana = ( PLANA_ENUM ) pncb->ncb_buffer;
					lana->length = numMACaddresses;
					for ( int i = 0 ; i < numMACaddresses ; i++ )
						lana->lana[ i ] = i;
				}
				break;
			case NCBRESET:
				break;
			case NCBASTAT:
				{
					ReadMACFromFile();
					//char buf[ 512 ];
					//sprintf( buf, "query LANA_NUM=%d", pncb->ncb_lana_num );
					//MessageBox( NULL, buf, "hi", MB_OK );
					PADAPTER_STATUS stat = ( PADAPTER_STATUS ) pncb->ncb_buffer;
					memcpy( stat->adapter_address, MACaddresses[ pncb->ncb_lana_num ], 6 );
				}
				break;
		}

		return NRC_GOODRET;
	}
}
