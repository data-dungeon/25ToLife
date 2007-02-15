/****************************************************************************/
// A "dimensional" map.  This template allows you to lay-out a n x m grid
// of buckets for other objects
/****************************************************************************/
#include "Math/MathClass.h"

/****************************************************************************/
/****************************************************************************/
template <class T>
DimensionalMap<T>::DimensionalMap(
float minX,
float minY,
float maxX,
float maxY,
int grids)
{
	if (minX > maxX)
		Swap(minX, maxX);
	if (minY > maxY)
		Swap(minY, maxY);

	ASSERT(grids > 0);
	xBuckets = grids;
	yBuckets = grids;
	int count = xBuckets * yBuckets;
	bucket = new DynamicArray<T*>*[count];
	ASSERT(bucket);
	for (int i = 0; i < count; i++)
		bucket[i] = NULL;

	xOffset = -minX;
	yOffset = -minY;

	float xGridSize = (maxX - minX) / (float)grids;
	invGridX = 1.0f / xGridSize;
	float yGridSize = (maxY - minY) / (float)grids;
	invGridY = 1.0f / yGridSize;

	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
}

/****************************************************************************/
/****************************************************************************/
template <class T>
DimensionalMap<T>::~DimensionalMap(void)
{
	int count = xBuckets * yBuckets;
	for (int i = 0; i < count; i++)
	{
		if (bucket[i])
			delete bucket[i];
	}

	delete [] bucket;
}

/****************************************************************************/
// Add an "point" object (will fail if off the map)
/****************************************************************************/
template <class T>
bool DimensionalMap<T>::Add(
float x,
float y,
T *obj)
{
	ASSERT(obj);

	// Get the right bucket
	int gridX, gridY;
	if (!ToGrid(x, y, gridX, gridY))
		return(NULL);

	// Is there an array allocated yet?
	int i = ToIndex(gridX, gridY);
	if (!bucket[i])
	{
		bucket[i] = new DynamicArray<T *>;
		ASSERT(bucket[i]);
	}

	return(bucket[i] && bucket[i]->Add(obj));
}

/****************************************************************************/
// Add an "line" object (will fail if off the map)
/****************************************************************************/
template <class T>
bool DimensionalMap<T>::Add(
float xStart,
float yStart,
float xEnd,
float yEnd,
T *obj)
{
	ASSERT(obj);


	// Make sure these are kosher
	if (xStart > xEnd)
		Swap(xStart, xEnd);
	if (yStart > yEnd)
		Swap(yStart, yEnd);

	// Compute the AABB that bounds this line
	int xa, ya;
	ToGrid(xStart, yStart, xa, ya);
	if (xa > 0)
		--xa;
	if (ya > 0)
		--ya;

	int xb, yb;
	ToGrid(xEnd, yEnd, xb, yb);
	if (xb < xBuckets - 1)
		++xb;
	if (yb < yBuckets - 1)
		++yb;

	// This is horrible.
	int hits = 0;
	for (int y = ya; y <= yb; y++)
	{
		for (int x = xa; x <= xb; x++)
		{
			ASSERT(x >= 0 && x < xBuckets && y >= 0 && y < yBuckets);
			if (x >= 0 && x < xBuckets && y >= 0 && y < yBuckets)
			{	
				int i = ToIndex(x, y);
				if (!bucket[i])
				{
					bucket[i] = new DynamicArray<T *>;
					ASSERT(bucket[i]);
				}
				if (bucket[i])
					bucket[i]->Add(obj);	
				++hits;
			}	
		}
	}

	return(hits > 0);
}

/****************************************************************************/
// Get at bucket of objects
/****************************************************************************/
template <class T>
T **DimensionalMap<T>::Get(
float x,
float y,
int &count)
{
	// Get the right bucket
	int gridX, gridY;
	if (!ToGrid(x, y, gridX, gridY))
		return(NULL);

	int i = ToIndex(gridX, gridY);
	if (!bucket[i])
		return(NULL);
	count = bucket[i]->Count();
	return(bucket[i]->GetPtr());
}

/****************************************************************************/
/****************************************************************************/
template <class T>
inline bool DimensionalMap<T>::ToGrid(
float x, 
float y, 
int &gridX, 
int &gridY)
{
	ASSERT(x >= minX && x <= maxX);
	ASSERT(y >= minY && y <= maxY);

	gridX = (int)((x + xOffset) * invGridX);
	if (gridX == xBuckets)
		gridX = xBuckets - 1;
	gridY = (int)((y + yOffset) * invGridY);
	if (gridY == yBuckets)
		gridY = yBuckets - 1;

	return(gridX >= 0 && gridX < xBuckets && gridY >= 0 && gridY < yBuckets);
}

/****************************************************************************/
/****************************************************************************/
template <class T>
inline int DimensionalMap<T>::ToIndex(
int gridX,
int gridY)
{
	ASSERT(gridX >= 0 && gridX < xBuckets && gridY >= 0 && gridY < yBuckets);
	return(gridX + (gridY * xBuckets));
}

/****************************************************************************/
/****************************************************************************/
template <class T>
inline void DimensionalMap<T>::Swap(
float &a,
float &b)
{
	float temp = a;
	a = b;
	b = temp;
}

