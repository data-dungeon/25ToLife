//*************************************************************************
// utility to write padding
//*************************************************************************
#ifndef WRITEPAD_H
#define WRITEPAD_H

#include <stdio.h>
#include "platform/BaseType.h"

// Write up to 15 bytes of padding (passing in 0 does nothing)
bool WritePad(FILE *fh, uint bytes);

#endif
