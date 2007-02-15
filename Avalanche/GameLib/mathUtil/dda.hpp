/***************************************************************************/
// A class implementation of the standard digital difference algoritm
/***************************************************************************/

/***************************************************************************/
// Set the endpoints of the line segment
/***************************************************************************/
inline void DDA::SetEndpoints(
int xstart,
int ystart,
int xend,
int yend)
{
	x0 = xstart;
	y0 = ystart;
	x1 = xend;
	y1 = yend;

	dx = x1 - x0;
	dy = y1 - y0;

	ax = 2 * abs(dx);
	ay = 2 * abs(dy);

	if (dx >= 0)
		sx = 1;
	else
		sx = -1;

	if (dy >= 0)
		sy = 1;
	else
		sy = -1;

	Reset();
}

/***************************************************************************/
// Give the next point in the line.  Returns false if there is no more points
/***************************************************************************/
inline bool DDA::NextPoint(
int &nextX,
int &nextY)
{
	if (point == 0)
		point++;
	else
	{
		if (ax > ay)
		{
			if (x == x1)
				return(false);

			if (d >= 0)
			{
				y += sy;
				d -= ax;
			}

			x += sx;
			d += ay;
		}
		else
		{
			if (y == y1)
				return(false);

			if (d >= 0)
			{
				x += sx;
				d -= ay;
			}

			y += sy;
			d += ax;
		}
	}

	nextX = x;
	nextY = y;

	return(true);
}

/***************************************************************************/
// Fat bresenham's line
/***************************************************************************/
inline bool DDA::NextPointFat(
int &nextX,
int &nextY,
bool &isFat,
int &fatX,
int &fatY)
{
	// Are we done?
	if (done)
		return(false);

	isFat = false;
	if (point == 0)
		point++;
	else
	{
		if (ax > ay)
		{
			if (x == x1)
			{
				nextX = x1;
				nextY = y1;
				done = true;
				return(true);
			}

			if (d >= 0)
			{
				isFat = true;
				fatY = y;
				y += sy;
				d -= ax;
			}

			x += sx;
			fatX = x;
			d += ay;
		}
		else
		{
			if (y == y1)
			{
				nextX = x1;
				nextY = y1;
				done = true;
				return(true);
			}

			if (d >= 0)
			{
				isFat = true;
				fatX = x;
				x += sx;
				d -= ay;
			}

			y += sy;
			fatY = y;
			d += ax;
		}
	}

	nextX = x;
	nextY = y;

	return(true);
}

/***************************************************************************/
// Trace the entire line
/***************************************************************************/
inline bool DDA::Trace(
POINTCALLBACK pointCB,
void *userData)
{
	int nextX, nextY;
	bool ret;

	// Go through the whole line
	while (NextPoint(nextX, nextY) && (ret = pointCB(userData, nextX, nextY)));

	return(ret);
}

/***************************************************************************/
// Reset the enumeration
/***************************************************************************/
inline void DDA::Reset(void)
{
   x = x0;
   y = y0;

   if (ax > ay)
      d = ay - ax / 2;
   else
      d = ax - ay / 2;

   point = 0;

	done = false;
}

