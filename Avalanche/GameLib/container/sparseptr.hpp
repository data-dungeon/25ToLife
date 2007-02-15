/***************************************************************************/
// A template for doing a sparse pointer matrix.
//
// This is intended to be as fast as possible...while being very compact
// in memory...
/***************************************************************************/

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#ifdef REALSPARSEMATRIX

#define DATABLOCK_THRESHOLD 2

/***************************************************************************/
// This is how you can create the matrix
/***************************************************************************/
template <class T>
inline SparsePointerMatrix<T>::SparsePointerMatrix(
T **ptrMatrix,
u16 totalColumns,
u16 totalRows)
{
	// Save the inital crap
	rows = totalRows;
	columns = totalColumns;

	// Make the row header array
	rowPtr = new u32*[rows];

	// Make the rleImage (worse case for now)
	rleDataBlock = new u32[rows * columns * 2];

	// Start the compression
	u32 *rleData = rleDataBlock;
	uint matrixIndex = 0;
	for (uint r = 0; r < rows; r++)
	{
		// setup the row pointer
		rowPtr[r] = rleData;

		// We are at the first column in the uncompressed matrix
		uint endOfColumnIndex = matrixIndex + columns;

		// Build the rle encoding
		while (matrixIndex < endOfColumnIndex)
		{
			// Do we have data here?
			uint dataCount = 0;
			while (ptrMatrix[matrixIndex + dataCount] != 0 && (matrixIndex + dataCount) < endOfColumnIndex)
				dataCount++;

			// Was there data?
			if (dataCount > 0 && dataCount <= DATABLOCK_THRESHOLD)
			{
				while (dataCount-- > 0)
					*rleData++ = (u32)ptrMatrix[matrixIndex++]; // just stick in pure data
			}
			else if (dataCount > DATABLOCK_THRESHOLD)
			{
				// Put in a header that gives the data count
				*rleData++ = (0xc0000000 | dataCount);

				// do the data
				while (dataCount-- > 0)
					*rleData++ = (u32)ptrMatrix[matrixIndex++];
			}

			// Count up the skip
			uint skipCount = 0;
			while (ptrMatrix[matrixIndex] == 0 && matrixIndex < endOfColumnIndex)
			{
				skipCount++;
				matrixIndex++;
			}

			// Did we have some stuff we skipped?
			if (skipCount)
				*rleData++ = (0x80000000 | skipCount);
		}
	}
}

/***************************************************************************/
// We have been "cast" from a memory image of a disk file
/***************************************************************************/
template <class T>
inline void SparsePointerMatrix<T>::DiskFixup(
u32 pointerBase)
{

#ifdef GCN
	ByteSwap( rows);
	ByteSwap( columns);
	ByteSwap( rowPtr);
	ByteSwap( rleDataBlock);
#endif

	// Fixup the offsets into pointers
	rowPtr = (u32 **)((uint)this + (uint)rowPtr);
	rleDataBlock = (u32 *)((uint)this + (uint)rleDataBlock);

	// Fixup the row offsets
	for (uint r = 0; r < rows; r++)
	{
#ifdef GCN
		ByteSwap( rowPtr[r]);
#endif
		rowPtr[r] = (u32 *)((u32)rowPtr[r] + (u32)rleDataBlock);
	}

	// Okay, what is the real size of the datablock?
	u32 *rleData = rleDataBlock;
	u32 rc = 0;
	u32 rowcolumns = rows * columns;
	while (rc < rowcolumns)
	{
#ifdef GCN
		ByteSwap( *rleData);
#endif
		// Pick off the rle command
		u32 command = (*rleData & 0xc0000000);

		// Is this a skip command?
		if (command == 0x80000000)
		{
			// This is a skip
			rc += (u32)(*rleData & 0x7fffffff);
			rleData++;
		}
		else if (command == 0xc0000000)
		{
			// This is a data block command
			u32 dataCount = (u32)(*rleData & 0x3fffffff);
			rleData++;

			while (dataCount > 0)
			{
#ifdef GCN
				ByteSwap( *rleData);
#endif
				*rleData += pointerBase;
#ifdef GCN
				//remove the address high 2 bits so not to be confused with command word
				*rleData &= 0x3FFFFFFF;
#endif
				rleData++;
				rc++;
				dataCount--;
			}
		}
		else // This is not a command
		{
			*rleData += pointerBase;
#ifdef GCN
			//remove the address high 2 bits so not to be confused with command word
			*rleData &= 0x3FFFFFFF;
#endif
			rleData++;
			rc++;
		}
	}
}

/***************************************************************************/
// Get a ptr back for the given row and column (unclipped)
/***************************************************************************/
template <class T>
inline T *SparsePointerMatrix<T>::Get(
int column,
int row)
{
	ASSERT(row >= 0 && row < rows);
	ASSERT(column >= 0 && column < columns);

	// Pick the row to traverse
	u32 *rleData = rowPtr[row];
	u32 c = 0;

	// Scan for it
	while ((u32)column >= c)
	{
		// Pick off the rle command
		u32 command = (*rleData & 0xc0000000);

		// Is this a skip command?
		if (command == 0x80000000)
		{
			// This is a skip
			c += (u32)(*rleData & 0x7fffffff);
			rleData++;
		}
		else if (command == 0xc0000000)
		{
			// This is a data block command
			u32 dataCount = (u32)(*rleData & 0x3fffffff);
			rleData++;

			// Is the data we are looking for here?
			if ((u32)column < c + dataCount)
#ifndef GCN
				return( (T *) rleData[column - c]);
#else
				//add back our address bit
				return( (T *)( rleData[column - c] | 0x80000000));
#endif

			// Otherwise, big skip
			c += dataCount;
			rleData += dataCount;
		}
		else // This is not a command
		{
			// Have we found it?
			if ((u32)column == c)
#ifndef GCN
				return( (T *)( *rleData));
#else
				//add back our address bit
				return( (T *)( *rleData | 0x80000000));
#endif

			// next
			c++;
			rleData++;
		}
	}

	// No luck
	return(NULL);
}

/***************************************************************************/
// Get a ptr back for the given row and column
// returns null if it is off an edge
/***************************************************************************/
template <class T>
inline T *SparsePointerMatrix<T>::GetClipped(
int column,
int row)
{
	// Do the initial clip
	if (row < 0 || row >= rows || column < 0 || column >= columns)
		return(NULL);

	// Now it is safe
	return(Get(column, row));
}

/***************************************************************************/
// Write it to disk
/***************************************************************************/
template <class T>
inline bool SparsePointerMatrix<T>::Write(
FILE *fh)
{
	// Okay, what is the real size of the datablock?
	u32 *rleEnd = rowPtr[rows - 1];
	u32 c = 0;
	while (c < columns)
	{
		// Is this a skip command?
		if ((*rleEnd & 0xc0000000) == 0x80000000)
		{
			c += (*rleEnd & 0x7fffffff);
			rleEnd++;
		}
		else if ((*rleEnd & 0xc0000000) == 0xc0000000)
		{
			u32 dataCount = (*rleEnd & 0x3fffffff);
			c += dataCount;
			rleEnd += (dataCount + 1);
		}
		else
		{
			// Just a single
			c++;
			rleEnd++;
		}
	}

	u32 rleSize = (u32)rleEnd - (u32)rleDataBlock;

	// save off our innards
	u32 **realRowPtr = rowPtr;
	u32 *realrleDataBlock = rleDataBlock;

	// "fix" them up
	rowPtr = (u32 **)(sizeof(SparsePointerMatrix<T>));
	rleDataBlock = (u32 *)(sizeof(SparsePointerMatrix<T>) + rows * sizeof(u32 *));

	// write out me
	if (fwrite(this, sizeof(SparsePointerMatrix<T>), 1, fh) != 1)
		return(false);

	// Write out the fixed up row ptr's
	u32 **fixupRow = new u32*[rows];
	for (uint r = 0; r < rows; r++)
		fixupRow[r] = (u32 *)((u32)realRowPtr[r] - (u32)realrleDataBlock);
	bool ret = (fwrite(fixupRow, sizeof(u32 *), rows, fh) == rows);
	delete [] fixupRow;
	if (!ret)
		return(false);

	// Write out the rle block
	if (fwrite(realrleDataBlock, 1, rleSize, fh) != rleSize)
		return(false);

	// fix us up
	rowPtr = realRowPtr;
	rleDataBlock = realrleDataBlock;

	// success!
	return(true);
}

/***************************************************************************/
// How big would this thing be on disk?
/***************************************************************************/
template <class T>
inline uint SparsePointerMatrix<T>::DiskSize(void)
{
	uint size = sizeof(SparsePointerMatrix<T>) + rows * sizeof(u32 *);

	// Okay, what is the real size of the datablock?
	u32 *rleEnd = rowPtr[rows - 1];
	u32 c = 0;
	while (c < columns)
	{
		// Is this a skip command?
		if ((*rleEnd & 0xc0000000) == 0x80000000)
		{
			c += (*rleEnd & 0x7fffffff);
			rleEnd++;
		}
		else if ((*rleEnd & 0xc0000000) == 0xc0000000)
		{
			u32 dataCount = (*rleEnd & 0x3fffffff);
			c += dataCount;
			rleEnd += (dataCount + 1);
		}
		else
		{
			// Just a single
			c++;
			rleEnd++;
		}
	}

	// figure the size of the block
	size += ((uint)rleEnd - (uint)rleDataBlock);

	// return the full size
	return(size);
}

/***************************************************************************/
// Cleanup (if not read from disk)
/***************************************************************************/
template <class T>
inline void SparsePointerMatrix<T>::NonDiskCleanup(void)
{
	delete [] rowPtr;
	delete [] rleDataBlock;
	rowPtr = NULL;
	rleDataBlock = NULL;
}

#else
// The cheesy non-sparse implementation

/***************************************************************************/
// This is how you can create the matrix
/***************************************************************************/
template <class T>
inline SparsePointerMatrix<T>::SparsePointerMatrix(
T **ptrMatrix,
u16 totalColumns,
u16 totalRows)
{
	// Save the inital crap
	rows = totalRows;
	columns = totalColumns;

	// Make the array
	u32 total = rows * columns
	ptr = new T*[total];

	// copy it in
	for (u32 i = 0; i < total; i++)
		ptr[i] = ptrMatrix[i];
}

/***************************************************************************/
// We have been "cast" from a memory image of a disk file
/***************************************************************************/
template <class T>
inline void SparsePointerMatrix<T>::DiskFixup(
u32 pointerBase)
{
#ifdef GCN
	ByteSwap( rows);
	ByteSwap( columns);
	ByteSwap( ptr);
#endif
	// fixup the pointer
	ptr = (T **)((u32)this + (u32)ptr);

	// Fixup the row offsets
	u32 total = rows * columns
	for (u32 i = 0; i < total; i++)
	{
		ByteSwap( ptr[i]);
		if (ptr[i] != 0)
			ptr[i] = (T *)((u32)ptr[i] + pointerBase);
	}
}

/***************************************************************************/
// Get a ptr back for the given row and column (unclipped)
/***************************************************************************/
template <class T>
inline T *SparsePointerMatrix<T>::Get(
int column,
int row)
{
	ASSERT(row >= 0 && row < rows);
	ASSERT(column >= 0 && column < columns);

	return(ptr[column + row * columns]);
}

/***************************************************************************/
// Get a ptr back for the given row and column
// returns null if it is off an edge
/***************************************************************************/
template <class T>
inline T *SparsePointerMatrix<T>::GetClipped(
int column,
int row)
{
	// Do the initial clip
	if (row < 0 || row >= rows || column < 0 || column >= columns)
		return(NULL);

	// Now it is safe
	return(ptr[column + row * columns]);
}

/***************************************************************************/
// Write it to disk
/***************************************************************************/
template <class T>
inline bool SparsePointerMatrix<T>::Write(
FILE *fh)
{
	// save off our innards
	T **realPtr = ptr;

	// "fix" them up
	ptr = (T **)(sizeof(SparsePointerMatrix<T>));

	// write out me
	if (fwrite(this, sizeof(SparsePointerMatrix<T>), 1, fh) != 1)
		return(false);

	// Write out the ptr block
	ptr = realPtr;
	u32 total = rows * columns;
	if (fwrite(ptr, sizeof(T *), total, fh) != total)
		return(false);

	// success!
	return(true);
}

/***************************************************************************/
// How big would this thing be on disk?
/***************************************************************************/
template <class T>
inline uint SparsePointerMatrix<T>::DiskSize(void)
{
	return(sizeof(SparsePointerMatrix<T>) + rows * columns * sizeof(T *));
}

/***************************************************************************/
// Cleanup (if not read from disk)
/***************************************************************************/
template <class T>
inline void SparsePointerMatrix<T>::NonDiskCleanup(void)
{
	delete [] ptr;
}

#endif
