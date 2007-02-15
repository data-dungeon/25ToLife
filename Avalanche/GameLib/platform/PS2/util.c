/****************************************************************************

 Module:
 util.c

 Description:
 Sample code to get data from mouse on Sony PlayStation(tm)2 
 using Logitech Mouse/Keyboard SDK
 General utility functions used by keybaord and mouse samples.

 COPYRIGHT (C) 2002 LOGITECH, INC. ALL RIGHTS RESERVED.

 This program is a trade secret of LOGITECH, and it is not to be
 reproduced, published, disclosed to others, copied, adapted,
 distributed or displayed without the prior authorization of LOGITECH.

 Licensee agrees to attach or embed this notice on all copies of the
 program, including partial copies or modified versions thereof.

****************************************************************************/

#include <eekernel.h>
#include <eeregs.h>
#include <string.h>
#include <stdio.h>

#include <liblgkbm.h>
#include "platform/PS2/util.h"

#define STR_LGKBM_ERR_INVALID_PARAMETER     "LGKBM_ERR_INVALID_PARAMETER"
#define STR_LGKBM_ERR_DEVICE_ERROR          "LGKBM_ERR_DEVICE_ERROR"
#define STR_LGKBM_ERROR                     "LGKBM_ERROR"
#define STR_LGKBM_ERR_NO_MORE_DEVICES       "LGKBM_ERR_NO_MORE_DEVICES"
#define STR_LGKBM_ERR_ALREADY_OPENED        "LGKBM_ERR_ALREADY_OPENED"
#define STR_LGKBM_ERR_DEVICE_LOST           "LGKBM_ERR_DEVICE_LOST"
#define STR_LGKBM_ERR_RPC_WAIT              "LGKBM_ERR_RPC_WAIT"
#define STR_LGKBM_ERR_ROLLOVER              "LGKBM_ERR_ROLLOVER"
#define STR_LGKBM_ERR_BUFFER_OVERFLOW       "LGKBM_ERR_BUFFER_OVERFLOW"
#define STR_LGKBM_ERR_UNKNOWN_ERROR         "LGKBM_ERR_UNKNOWN_ERROR"
#define STR_sceUsbd_CRC                     "sceUsbd_CRC"
#define STR_sceUsbd_BFV                     "sceUsbd_BFV"
#define STR_sceUsbd_DTM                     "sceUsbd_DTM"
#define STR_sceUsbd_STALL                   "sceUsbd_STALL"
#define STR_sceUsbd_NOTRESP                 "sceUsbd_NOTRESP"
#define STR_sceUsbd_PIDCF                   "STR_sceUsbd_PIDCF"
#define STR_sceUsbd_UEPID                   "STR_sceUsbd_UEPID"
#define STR_sceUsbd_DOR                     "STR_sceUsbd_DOR"
#define STR_sceUsbd_DUR                     "STR_sceUsbd_DUR"
#define STR_sceUsbd_RSVDA                   "STR_sceUsbd_RSVDA"
#define STR_sceUsbd_RSVDB                   "STR_sceUsbd_RSVDB"
#define STR_sceUsbd_BOR                     "STR_sceUsbd_BOR"
#define STR_sceUsbd_BUR                     "STR_sceUsbd_BUR"
#define STR_sceUsbd_NOTACC1                 "STR_sceUsbd_NOTACC1"
#define STR_sceUsbd_NOTACC2                 "STR_sceUsbd_NOTACC2"
#define STR_sceUsbd_UnKnownError            "STR_sceUsbd_UnKnownError"


static void reverse(char *pStr);


/*********************************************************************************
 * get_error()  : Helper function which returns the error string from the        *
 *                numeric error code value                                       *
 * ----------------------------------------------------------------------------- *
 * Input        : err_code      : error code                                     *
 * Output       : Nothing       :                                                *
 * Returns      : char *ErrStr  : pointer to the error string corresponding to   *
 *              :               : error code.                                    *
 *********************************************************************************/
const char *get_error(int err_code)
{
  static char buf_unknown_error[100];

  switch(err_code) {

    case LGKBM_ERR_sceUsbd_NOTRESP:
         return STR_sceUsbd_NOTRESP;

    case LGKBM_ERR_INVALID_PARAMETER:
         return STR_LGKBM_ERR_INVALID_PARAMETER;

   case LGKBM_ERR_DEVICE_ERROR:
         return STR_LGKBM_ERR_DEVICE_ERROR;

    case LGKBM_ERROR:
         return STR_LGKBM_ERROR;

    case LGKBM_ERR_NO_MORE_DEVICES:
         return STR_LGKBM_ERR_NO_MORE_DEVICES;

    case LGKBM_ERR_ALREADY_OPENED:
         return STR_LGKBM_ERR_ALREADY_OPENED;

    case LGKBM_ERR_DEVICE_LOST:
         return STR_LGKBM_ERR_DEVICE_LOST;

    case LGKBM_ERR_RPC_WAIT:
         return STR_LGKBM_ERR_RPC_WAIT;;

    case LGKBM_ERR_ROLLOVER:
         return STR_LGKBM_ERR_ROLLOVER;

    case LGKBM_ERR_BUFFER_OVERFLOW:
         return STR_LGKBM_ERR_BUFFER_OVERFLOW;

    default:
	sprintf(buf_unknown_error, "Unknown Error(0x%x)", err_code);
        return buf_unknown_error;
  }
}


/*********************************************************************************
 * get_event_type()     : Helper function which returns the string name of the   *
 *                      : event type.                                            *
 * ----------------------------------------------------------------------------- *
 * Input        : event_type    : numeric value of the event type.               *
 * Output       : Nothing       :                                                *
 * Returns      : char *        : Pointer to the event type string name          *
 *********************************************************************************/
const char *get_event_type(u_short event_type)
{
	static const char *EventType[] = {
				 "Invalid",         // 0
				 "Down   ",         // 1
				 "Up     ",         // 2
				 "Repeat ",         // 3
 				};


	switch(event_type) {
		case LGKBM_KEY_DOWN:
		case LGKBM_KEY_UP:
		case LGKBM_KEY_REPEAT:
			return EventType[event_type];
		default:
			return EventType[0];
	}
}


/******************************************************************************
 * len_strcpy() : This function copes the source string into the destination  *
 *          string and returns the lenght of the string. This is used in place*
 *          of strcpy when we want to concatinate multiple strings. Returung  *
 *          length of the string, allows the second string to be copied       *
 *          directly at the end of the first string rather than first find the*
 *          end of the first string (which is what strcat does)               *
 *          Use strcpy and strcat in place of this function if strcpy and     *
 *          strcat are implemented as movsb and scansb instrcutions.          *
 *--------------:-------------------:-----------------------------------------*
 *  Input       : psrcstr           : source string        .                  *
 *              : pdststr           : destination string                      *
 *              : ------------------: --------------------------------------- *
 * Output       : pStr              : copy of source string                   *
 * Returns      : i                 : lenght of the destination string.       *
 ******************************************************************************/
int len_strcpy(char *pdststr, const char *psrcstr)
{
    int i = 0;
    while ((*pdststr++ = *psrcstr++))
        ++i;

    return i;
}


/******************************************************************************
 * reverse() : This function reverse the charectors in a string               *
 *          Example if the input string is "STRING", the output string is     *
 *          "GNIRTS"                                                          *
 *--------------:-------------------:-----------------------------------------*
 *  Input       : pStr              : String to be reversed.                  *
 *              :                   :                                         *
 * Output       : pStr              : reversed string.                        *
 * Returns      : Nothing           :                                         *
 ******************************************************************************/
static void reverse(char *pStr)
{
    int i,j;
    char c;
    for(i=0,j=strlen(pStr)-1;i<j;i++,j--) {
       c = pStr[i];
       pStr[i] = pStr[j];
       pStr[j] = c;
    }
}


/******************************************************************************
 * itoa() : This function converts the singed integer into a base 10 string   *
 *          (decimal digit)                                                   *
 *--------------:-------------------:-----------------------------------------*
 *  Input       : iVal              : integer to convert into ascii string    *
 *              : pStr              : buffer to hold the ascii string.        *
 *              :                   :                                         *
 * Output       : pStr              : contains the sting representation of the*
 *              :                   : integer.                                *
 * Returns      : pStr              : ptr to the ascii string                 *
 ******************************************************************************/
char *myitoa(int iVal, char *pStr)
{
    int i = 0;
    int fSign = 0;

    if(iVal < 0 ) {
       fSign = 1;
       iVal = -iVal;
    }
    do {                  /* generate digits in the reverse order    */
       pStr[i++] = (char )(iVal % 10 + '0') ;  /* get next digit        */
    } while ( (iVal /= 10) > 0) ;          /* delete it           */

    if(fSign) {
       pStr[i++] = '-';
    }
    pStr[i] = '\0' ;
    reverse(pStr);
    return(pStr);
}



/******************************************************************************
 * GetDeviceLocationString() : This function returns the device location      *
 *                           : string in the following form                   *
 *                           (i, j, k, .. ) upto 7 level which represents the *
 *                           USB port numbers of the successive hub through   *
 *                           which the device is connected on to the PS2 usb  *
 *                           port.                                            *
 *--------------:-------------------:-----------------------------------------*
 *  Input       : pLocationString   : pointer to the char buffer to receive   *
 *              :                   : device location string.                 *
 *              : ptopology         : buffer giving the device loactions in   *
 *              :                   : port numbers as returned by USBD.       *
 *              :                   :                                         *
 * Output       : pLocationString   : contains the device location string in  *
 *              :                   : following format.                       *
 *              :                   : ((i, j, k, ..) where                    *
 *              :                   : i : gives the root hub port number on   *
 *              :                   :     which the device is connected       *
 *              :                   : j, k.. : sucessive hub port numbers thru*
 *              :                   :          which the device is connected. *
 * Returns      : nothing           :                                         *
 ******************************************************************************/
void GetDeviceLocationString(char *pLocationString, const char *ptopology)
{
    int  len = 0, i = 0;
    char temp_buf[10];

    len_strcpy(pLocationString, "(");
    while(ptopology[i]) {
        ++len;
        myitoa(ptopology[i], temp_buf);
        len += len_strcpy(pLocationString+len, temp_buf);
        pLocationString[len] = ',';
        ++i;
    }
    len += len_strcpy(pLocationString+len, ")");
    return;
}
