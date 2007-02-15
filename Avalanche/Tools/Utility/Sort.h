#pragma once

void UTILITYEXPORT Sort( int *pArray, int nLow, int nHigh );
void UTILITYEXPORT Sort( int *pArray, int nSize ) { Sort(pArray, 0, nSize); }
