/****************************************************************************/
// A "dimensional" map.  This template allows you to lay-out a n x m grid
// of buckets for other objects
/****************************************************************************/
#ifndef DIMMAP
#define DIMMAP

template <class T> class DimensionalMap
{
public:
	inline DimensionalMap(float minX, float minY, float maxX, float maxY, int grids);
	inline ~DimensionalMap(void);

	// Add an "point" object (will fail if off the map)
	inline bool Add(float x, float y, T *obj);

	// Add an "line" object (will fail if off the map)
	inline bool Add(float xStart, float yStart, float xEnd, float yEnd, T *obj);

	// Get at bucket of objects
	inline T **Get(float x, float y, int &count);

private:
	float					invGridX, invGridY;
	int 					xBuckets, yBuckets;
	DynamicArray<T*>	**bucket;
	float					xOffset, yOffset;
	float					minX, minY, maxX, maxY;
	
	inline bool ToGrid(float x, float y, int &gridX, int &gridY);
	inline int ToIndex(int gridX, int gridY);

	inline void Swap(float &a, float &b);
};

// The all important implementation
#include "container/dimmap.hpp"

#endif
