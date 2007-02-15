//*************************************************************************
// utility to write padding
//*************************************************************************
#include "ToolHelper/writepad.h"

#define MAX_WRITEPAD		128
static char writePad[MAX_WRITEPAD] = { 0 };

//*************************************************************************
// Write up to 15 bytes of padding (passing in 0 does nothing)
//*************************************************************************
bool WritePad(
FILE *fh,
uint bytes)
{
	ASSERT(fh);

	if (bytes == 0)
		return(true);
	else if (bytes <= MAX_WRITEPAD)
		return(fwrite(writePad, bytes, 1, fh) == 1);
	else
	{
		// I could be more clever, but I'm not
		bool ret = true;
		for (uint i = 0; i < bytes && ret; i++)
			ret = (fwrite(writePad, 1, 1, fh) == 1);
		return(ret);
	}
}




