/***************************************************************************/
// A template for doing a sparse pointer matrix.
//
// This is intended to be as fast as possible...while being very compact
// in memory...
/***************************************************************************/
#ifndef SPARSEPTR_H
#define SPARSEPTR_H

#include <stdio.h>
#include "platform/BaseType.h"

// If you are having speed problems undef define this
#define REALSPARSEMATRIX

template <class T> class SparsePointerMatrix
{
public:
	// This would be from something loaded from disk
	inline SparsePointerMatrix(void) {}

	// This is how you can create the matrix
	inline SparsePointerMatrix(T **ptrMatrix, u16 totalColumns, u16 totalRows);

	// Clean up stuff (this doesn't do anything)
	inline ~SparsePointerMatrix(void) {}

	// We have been "cast" from a memory image of a disk file
	inline void DiskFixup(u32 pointerBase);

	// Get a ptr back for the given row and column (unclipped)
	inline T *Get(int column, int row);

	// Get a ptr back for the given row and column
	// returns null if it is off an edge
	inline T *GetClipped(int column, int row);

	// Write it to disk
	inline bool Write(FILE *fh);

	// How big is this disk image?
	inline uint DiskSize(void);

	// Cleanup (if not read from disk)
	inline void NonDiskCleanup(void);

private:
#ifdef REALSPARSEMATRIX
	// the number of rows and colunms in the original matrix
	u16					rows, columns;

	// Each row points into the rle block start
	u32					**rowPtr;

	// The actual rle data pool (high bit signs a skip)
	u32	  				*rleDataBlock;
#else
	u16					rows, columns;
	T						**ptr;
#endif
};

// The implementation
#include "container/sparseptr.hpp"

#endif
