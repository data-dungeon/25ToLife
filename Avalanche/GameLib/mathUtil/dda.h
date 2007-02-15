/***************************************************************************/
// A class implementation of the standard digital difference algoritm
// (the classic midpoint line)
//
// Note: Does no error checking, do not enumerate until endpoints are set.
/***************************************************************************/
#ifndef DDA_H
#define DDA_H

class DDA
{
public:
   // Constructors/destructor
   DDA(void) {}
   DDA(int xstart, int ystart, int xend, int yend)
      { SetEndpoints(xstart, ystart, xend, yend); }

   // A set end points if you didn't use the constructor
   inline void SetEndpoints(int xstart, int ystart, int xend, int yend);

   // Enumerators
	// Give the next point in the line.  Returns false if there is no more points
   inline bool NextPoint(int &nextX, int &nextY);
   inline bool NextPointFat(int &nextX, int &nextY, bool &isFat, int &fatX, int &fatY);

	// A user callback for "Trace" returns true for more stuff, false for done
	typedef bool (*POINTCALLBACK)(void *userData, int nextX, int nextY);

	// Returns the result of POINTCALLBACK
   inline bool Trace(POINTCALLBACK pointCB, void *userData);

   // Reset the enumeration (so you can do it again)
   inline void Reset(void);

private:
   // Stuff to keep track of the line
   int 	x0, y0;
   int 	x1, y1;
   int 	dx, dy;
   int 	ax, ay;
   int 	sx, sy;
   int 	d;
   int 	x, y;
   int 	point;
	bool 	done;
};

// Include the implementation
#include "mathUtil/dda.hpp"

#endif
