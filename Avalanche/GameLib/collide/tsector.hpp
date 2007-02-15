/***************************************************************************/
// The inline utilities
/***************************************************************************/

/***************************************************************************/
// Initialize (we are loaded from disk, so we have to fixup pointers)
/***************************************************************************/
void TerrainSector::DiskFixup(void)
{
	uint baseAddr = (uint)this;

#ifdef GCN
	EndianSwap( );
#endif

	// None of these should be NULL (they all point to the end of the structure
	// if there is no data
	ASSERT(triangleIndex != NULL);
#ifdef WANTTERRAINEDGES
	ASSERT(edgeIndex != NULL);
#endif

	triangleIndex = (u24 *)(baseAddr + (uint)triangleIndex);
#ifdef WANTTERRAINEDGES
	edgeIndex = (u24 *)(baseAddr + (uint)edgeIndex);
#endif
}
