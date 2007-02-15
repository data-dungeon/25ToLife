/***********************************/
/*                                 */
/* PigpenUtility.h                 */
/* Pigpen particle tool  12/20/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* utility routines                */
/*                                 */
/***********************************/

#ifndef __PIGPENUTILITY_H
#define __PIGPENUTILITY_H

/******************* includes ****************************/

/******************* defines *****************************/

/******************* forward declarations ****************/

/******************* structures **************************/

/* for handling edit boxes. handling dialog controls is probably the biggest pain about building this tool.
instead of having to maintain switch statements, i want to make processing more generic */

typedef struct
{
	int nID;							// id of dialog edit box control
	int nDataID;					// id of data that fills edit box, data item returns a pointer to it
	int nDataIndex;				// supplemental data index for things like effect channels-- array of structure
	float fMinValue;				// min value for data item
	float fMaxValue;				// max value for data item
	float fDeltaMultiplier;		// how much a single spinner click should change value
	char acFormat[20];			// format for display in edit box
	bool bIsInteger;				// whether value will be exported as an integer
} ts_EditBox;

/* same for check boxes. this structure is also used for default setting of flag bits */

typedef struct
{
	int nID;							// id of dialog check box control
	int nDataID;					// id of flag data that check box sets/clears flag in
	int nDataIndex;				// supplemental data index for things like effect channels-- array of structure
	unsigned long ulFlagBit;	// flag bit that check box affects
	bool bShowData;				// call ShowData again when check box is clicked to update button status
} ts_CheckBox;

/* and for radio buttons */

typedef struct
{
	int nID;							// id of dialog check box control
	int nDataID;					// id of flag data that check box sets/clears flag in
	unsigned long ulValue;		// value that button sets in bit field
	unsigned long ulMask;		// mask to use when setting value
	bool bShowData;				// call ShowData again when check box is clicked to update button status
} ts_RadioButton;

/******************* functions ***************************/

CString& ToCString(int nValue);
CString& ToCString(float fValue);
bool AllZeros(CString& String);
void DeleteArray(CObArray& Array);
void SwapBytes(char *pBytes, int nCount);


#endif //__PIGPENUTILITY_H
