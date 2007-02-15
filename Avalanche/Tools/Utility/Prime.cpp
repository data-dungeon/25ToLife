// Dan  8/18/2004  \DanLP110\FileIO\Files.cpp

#include "stdafx.h"
#include "AMacros.h"
#include <cmath>

UINT GetPrimeLargerThan( UINT nStart )
{
   if (nStart < 3)
      return 3;

   UINT n;
   UINT j,g;
   double k;

   if ((nStart/2)==((float)nStart/2.0)) 
      n = nStart-1;
   else 
      n = nStart-2;

   do {
      n += 2;
      g = (UINT)sqrt((double)n);
      j = 3;
      k = (double)n / (double)j;      
      while (k != (double)(long)k && j<=g) 
      {
         j += 2;
         k = (double)n / (double)j;
      }
      if (j>g)
         return n; 
   } while (TRUE);
}

