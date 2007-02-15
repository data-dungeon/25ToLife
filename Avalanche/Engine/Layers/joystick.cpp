/* --------------------------------------------------
 * File    : JOYSTICK.C
 * Created : Fri Feb 26 08:16:28 1999
 * Descript: PSX Joystick Stuff
 * --------------------------------------------------*/

/******************* includes ****************************/

#include "Layers/LayersPCH.h"





/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
#define ANALOG_CENTER 0x80
#define ANALOG_THRESH 0x30

#define PORTMAX		2
#define PADBUFFLEN   34

#define PADID_CON    0x41
#define PADID_DS		0x73
#define PADID_MOUSE  0x12
#define PADID_ANALOG 0x23
#define PADID_MULTI  0x80

// Declared here
ts_JoyStk 	Joy[_MAX_CONTROLLERS];

struct ts_PadDef
{
	unsigned char acPadBuff[PADBUFFLEN];
};

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
ts_PadDef 	Pad[PORTMAX];
unsigned short			sNumControllers;

unsigned short gbl_KeyBDIn;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
unsigned short ProcessPort (unsigned char *portData, unsigned short sControllers, unsigned short sStart, unsigned short sPort);
void PlaceButtonData(unsigned short sData, short sIndex);
void PlaceAnalogData(unsigned char *pData, short sIndex);

//----------------------------------------------------------------------------
// nJoy is the number of joysticks you want processed for the game.
//----------------------------------------------------------------------------
void joyInit(int nJoy)
{
//	PadInitMtap( Pad[0].acPadBuff, Pad[1].acPadBuff);
//	PadStartCom();
	sNumControllers = nJoy;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void joyLaunchRead(void)
{
}

//----------------------------------------------------------------------------
// Note:  There is currently no remap handling.
//----------------------------------------------------------------------------
void joyGetData(void)
{
	unsigned short	nPort, nController;

	for (nPort = 0, nController = 0; nPort < PORTMAX && nController < sNumControllers; nPort++)
	{
		nController += ProcessPort (Pad[nPort].acPadBuff, (unsigned short)(sNumControllers-nController), nController, nPort);
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
unsigned short ProcessPort (unsigned char *portData, unsigned short sControllers, unsigned short sStart, unsigned short sPort)
{
	unsigned short	sData;

	sData = gbl_KeyBDIn;
	PlaceButtonData(sData, sStart);
	return(1);

}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void PlaceButtonData(unsigned short sData, short sIndex)
{
	Joy[sIndex].u32LastData = Joy[sIndex].u32CurData;
	Joy[sIndex].u32CurData = sData;

	Joy[sIndex].u32DebData = (Joy[sIndex].u32CurData ^ Joy[sIndex].u32LastData) & Joy[sIndex].u32CurData;
	Joy[sIndex].u32OpenData = (Joy[sIndex].u32CurData ^ Joy[sIndex].u32LastData) & Joy[sIndex].u32LastData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void PlaceAnalogData(unsigned char *pData, short sIndex)
{
	// Fill in the digital directional values ... for now anyway (could make a flag)
	if (pData[5] < (ANALOG_CENTER - ANALOG_THRESH))
	{
		Joy[sIndex].u32CurData |= BTN_DSTK_U;
	}
	if (pData[5] > (ANALOG_CENTER + ANALOG_THRESH))
	{
		Joy[sIndex].u32CurData |= BTN_DSTK_D;
	}
	if (pData[4] < (ANALOG_CENTER - ANALOG_THRESH))
	{
		Joy[sIndex].u32CurData |= BTN_DSTK_L;
	}
	if (pData[4] > (ANALOG_CENTER + ANALOG_THRESH))
	{
		Joy[sIndex].u32CurData |= BTN_DSTK_R;
	}

	Joy[sIndex].AnalogStks[0].XThrow = pData[4] - ANALOG_CENTER;
	Joy[sIndex].AnalogStks[0].YThrow = -(pData[5] - ANALOG_CENTER);

	Joy[sIndex].AnalogStks[1].XThrow = pData[6] - ANALOG_CENTER;
	Joy[sIndex].AnalogStks[1].YThrow = -(pData[7] - ANALOG_CENTER);
}
