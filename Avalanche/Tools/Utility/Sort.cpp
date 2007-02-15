// Dan  8/18/2004  \DanLP110\FileIO\Files.cpp

#include "stdafx.h"
#include "Sort.h"

void Sort( int *pArray, int nLow, int nHigh )
{
   int nUp, nDown, nBreak, nTemp, nValueBreak;

   if  (nLow < nHigh) {
      if (nHigh - nLow == 1) {
         if (pArray[nLow] < pArray[nHigh]) {
            nTemp = pArray[nLow];
            pArray[nLow] = pArray[nHigh];
            pArray[nHigh] = nTemp;
         }
      } else {
         nBreak = (nLow+nHigh)/2;
         nValueBreak = pArray[nBreak];
         nTemp = pArray[nBreak];
         pArray[nBreak] = pArray[nHigh];
         pArray[nHigh] = nTemp;
         do {
            nUp = nLow;
            nDown = nHigh;
            while (nUp < nDown && pArray[nUp] >= nValueBreak)
               nUp++;
            while(nDown > nUp && pArray[nDown] <= nValueBreak)
               nDown--;
            if (nUp < nDown) {
               nTemp = pArray[nUp];
               pArray[nUp] = pArray[nDown];
               pArray[nDown] = nTemp;
            }
         } while (nUp < nDown);
         nTemp = pArray[nUp];
         pArray[nUp] = pArray[nHigh];
         pArray[nHigh] = nTemp;
         if (nUp - nLow < nHigh - nUp) {
            Sort( pArray, nLow, nUp - 1 );
            Sort( pArray, nUp + 1, nHigh );
         } else {
            Sort( pArray, nUp + 1, nHigh );
            Sort( pArray, nLow, nUp - 1 );
         }
      }
   }
}
