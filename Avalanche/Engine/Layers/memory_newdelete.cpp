/// File: memory_newdelete.cpp
///
/// This file's main purpose is the override new and delete to use our
/// memory manager (whichever manager we might be using at the time.)

/// @{
/// override new and delete

#ifdef MEM_DEBUG_BLOCKS

#define MEM_RESET_NEW_NAME()			{ strcpy(acNewName, "new"); }

/// the name used for alloc name for calls from overridden new. can be one-shot set to something other than "new"
/// via MEM_SET_NEW_NAME macro
char acNewName[ALLOC_NAME_LENGTH] = "new";

#else

#define MEM_RESET_NEW_NAME()			((void)0)

#endif

/// value for setting alignment for calls to new. it's a one-shot, and reverts to MEM_ADDR_ALIGN after use
static int sg_newAlign = MEM_ADDR_ALIGN;

#if !defined( TOOLS )
#define OVERRIDE_NEW	// this should always be defined; we need the buffers malloc'd by new to be aligned, so we have to override it
#endif

#if defined(GCN) && defined(CW)
#define NEW_TYPE_SIZE _CSTD::size_t
#else
#define NEW_TYPE_SIZE unsigned int
#endif

//============================================================================

#if defined(OVERRIDE_NEW)

//============================================================================

void* operator new( NEW_TYPE_SIZE blockSize )
{
	MEM_SET_RETURN_ADDRESS();

	#ifdef MALLOC_STATS
		NEW_STATS_INC( blockSize );
		if ( Layers__HeapCreated() )
			MALLOC_STATS_DEC( blockSize );	// will call memAlloc, i want to track new and memAlloc separately
	#endif //MALLOC_STATS

	#ifdef MEM_USE_WIN32_MANAGER

		// if using windows memory manager, just malloc
		#if MMX_OK // alignment only necessary if using simd instructions
			void *pBlock = _aligned_malloc(blockSize, sg_newAlign);
		#else
			void *pBlock = malloc(blockSize);
		#endif
		sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator
		return(pBlock);

	#else //MEM_USE_WIN32_MANAGER

		void *pBlock;
		if ( !Layers__HeapCreated() )
			Layers__CreateHeap();

		MEM_PUSH_ALLOC_NAME(acNewName);
		MEM_RESET_NEW_NAME();

		pBlock = memAlloc( blockSize, sg_newAlign);
		sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator

		MEM_POP_ALLOC_NAME( );

		return( pBlock);

	#endif //MEM_USE_WIN32_MANAGER
}

//============================================================================

void * operator new[]( NEW_TYPE_SIZE blockSize )
{
	MEM_SET_RETURN_ADDRESS();

	#ifdef MALLOC_STATS
		NEW_STATS_INC( blockSize );
		if ( Layers__HeapCreated() )
			MALLOC_STATS_DEC( blockSize );	// will call memAlloc, i want to track new and memAlloc separately
	#endif //MALLOC_STATS

	#ifdef MEM_USE_WIN32_MANAGER

		// if using windows memory manager, just malloc
		#if MMX_OK						// alignment only necessary if using simd instructions
		void *pBlock = _aligned_malloc(blockSize, sg_newAlign);
		#else
		void *pBlock = malloc(blockSize);
		#endif
		sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator
		return pBlock;

	#else //MEM_USE_WIN32_MANAGER

		if ( !Layers__HeapCreated() )
			Layers__CreateHeap();

		MEM_PUSH_ALLOC_NAME(acNewName);
		MEM_RESET_NEW_NAME();

		void *pBlock = memAlloc( blockSize, sg_newAlign);
		sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator

		MEM_POP_ALLOC_NAME( );

		return pBlock;

	#endif //MEM_USE_WIN32_MANAGER
}

//============================================================================

#if defined(GCN) && defined(CW)
void * operator new( _CSTD::size_t blockSize, const _STD::nothrow_t&)
{
	#ifdef MALLOC_STATS
		NEW_STATS_INC( blockSize );
		if ( Layers__HeapCreated() )
			MALLOC_STATS_DEC( blockSize );	// will call memAlloc, i want to track new and memAlloc separately
	#endif //MALLOC_STATS

	if ( !Layers__HeapCreated() )
		Layers__CreateHeap();

	MEM_PUSH_ALLOC_NAME(acNewName);
	MEM_RESET_NEW_NAME();

	void* pBlock = memAlloc( blockSize, sg_newAlign);
	sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator

	MEM_POP_ALLOC_NAME( );

	return pBlock;
}
#endif //defined(GCN) && defined(CW)

//============================================================================

#if defined(GCN) && defined(CW)
void * operator new[]( _CSTD::size_t blockSize, const _STD::nothrow_t&)
{
	#ifdef MALLOC_STATS
		NEW_STATS_INC(blockSize);
		if ( Layers__HeapCreated() )
			MALLOC_STATS_DEC(blockSize);	// will call memAlloc, i want to track new and memAlloc separately
	#endif //MALLOC_STATS

	if ( !Layers__HeapCreated() )
		Layers__CreateHeap();

	MEM_PUSH_ALLOC_NAME(acNewName);
	MEM_RESET_NEW_NAME();

	void* pBlock = memAlloc( blockSize, sg_newAlign);
	sg_newAlign = MEM_ADDR_ALIGN;	// sg_newAlign allows one-shot setting of alignment for new operator

	return pBlock;
}
#endif //defined(GCN) && defined(CW)

//============================================================================

void operator delete( void* pBlock )
{
	#if defined( MEM_USE_WIN32_MANAGER )

		// if using windows memory manager, just free
		#if MMX_OK						// alignment only necessary if using simd instructions
		_aligned_free(pBlock);
		#else
		free(pBlock);
		#endif

	#elif defined( MEM_USE_DEBUG_PARANOID_HEAP )

		memFree( pBlock );

	#else

		ASSERT( Layers__HeapCreated() );

		ts_memRegion* pRegion = _memFindMemRegion( pUsedRegionList, pBlock );
		if ( pRegion != NULL )
		{
			memFree( pBlock);
		}

	#endif //MEM_USE_WIN32_MANAGER
}

//============================================================================

void operator delete[]( void* pBlock )
{
	#if defined( MEM_USE_WIN32_MANAGER )

		// if using windows memory manager, just free
		#if MMX_OK						// alignment only necessary if using simd instructions
		_aligned_free(pBlock);
		#else
		free(pBlock);
		#endif

	#elif defined( MEM_USE_DEBUG_PARANOID_HEAP )

		memFree( pBlock );

	#else //MEM_USE_WIN32_MANAGER

		ASSERT( Layers__HeapCreated() );

		ts_memRegion* pRegion = _memFindMemRegion( pUsedRegionList, pBlock );
		if ( pRegion != NULL)
		{
			memFree( pBlock );
		}

	#endif //MEM_USE_WIN32_MANAGER
}

//============================================================================

#endif //OVERRIDE_NEW

//============================================================================

// one-shot setting of alignment for next call to new operator. only valid if overriding new
void memSetOneShotNewAlign( u32 u32Align)
{
	if( u32Align > MEM_ADDR_ALIGN)
	{
		sg_newAlign = u32Align;
	}
}

//============================================================================

// set memory block name to be used next time new is called
#ifdef MEM_DEBUG_BLOCKS
void memSetOneShotNewName(char *pcName)
{
	if (pcName)
	{
		strncpy(acNewName, pcName, ALLOC_NAME_LENGTH);
		acNewName[ALLOC_NAME_LENGTH - 1] = 0;	//make sure it's null-terminated
	}
	else
	{
		acNewName[0] = 0;
	}
}
#endif

//============================================================================

/// @}
