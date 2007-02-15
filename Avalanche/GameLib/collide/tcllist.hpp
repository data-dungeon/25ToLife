/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline CTerrainLayerAABBIterator::CTerrainLayerAABBIterator(
CCollisionStack *i_stack)
{
	d_stack = i_stack;
}

/***************************************************************************/
/***************************************************************************/
inline AABB &CTerrainLayerAABBIterator::Bounds()
{
	return d_iterator.Bounds();
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainLayerAABBIterator::Initialize(
TerrainCollideLayerList &i_list)
{
	d_list = &i_list;

	for (d_layerIndex = 0; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		// Alias the layer
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack))
			return true;
	}

	return false;
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle *CTerrainLayerAABBIterator::GetNextTriangle()
{
	TerrainTriangle *triangle = d_iterator.GetNextTriangle(d_list->m_layer[d_layerIndex].CurrentTestID());

	if (triangle != NULL)
		return triangle;

	for (d_layerIndex++; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack) &&
				((triangle = d_iterator.GetNextTriangle(layer.CurrentTestID())) != NULL))
			return triangle;
	}

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CTerrainLayerOBBIterator::CTerrainLayerOBBIterator(
CCollisionStack *i_stack)
{
	d_stack = i_stack;
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainLayerOBBIterator::Initialize(
TerrainCollideLayerList &i_list)
{
	// Build the bounding information
	const float skinnyFactor = 3.0f; 
	d_obb.ComputeWorldExtents(d_aabb);
	if (skinnyFactor * d_obb.Volume() >= d_aabb.Volume())
		d_iterator.Setup(d_aabb);
	else
	{
		//d_obb.Transform(d_corner);
		d_iterator.Setup(d_aabb, d_obb, d_corner);
	}

	d_list = &i_list;

	for (d_layerIndex = 0; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		// Alias the layer
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack))
			return true;
	}

	return false;
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle *CTerrainLayerOBBIterator::GetNextTriangle()
{
	TerrainTriangle *triangle = d_iterator.GetNextTriangle(d_list->m_layer[d_layerIndex].CurrentTestID());

	if (triangle != NULL)
		return triangle;

	for (d_layerIndex++; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack) &&
				((triangle = d_iterator.GetNextTriangle(layer.CurrentTestID())) != NULL))
			return triangle;
	}

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CTerrainLayerLineIterator::CTerrainLayerLineIterator(
CCollisionStack *i_stack)
{
	d_stack = i_stack;
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainLayerLineIterator::Setup(
const Vector3 &i_start,
const Vector3 &i_end)
{
	d_iterator.Setup(i_start, i_end);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainLayerLineIterator::Initialize(
TerrainCollideLayerList &i_list)
{
	d_list = &i_list;

	for (d_layerIndex = 0; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack))
			return true;
	}

	return false;
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainLayerLineIterator::FoundEnd(const Vector3 &i_intersection)
{
	d_iterator.FoundEnd(i_intersection);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle *CTerrainLayerLineIterator::GetNextTriangle()
{
	TerrainTriangle *triangle = d_iterator.GetNextTriangle(d_list->m_layer[d_layerIndex].CurrentTestID());

	if (triangle != NULL)
		return triangle;

	for (d_layerIndex++; d_layerIndex < d_list->m_layers; d_layerIndex++)
	{
		TerrainCollideLayer &layer = d_list->m_layer[d_layerIndex];

		if (layer.MatchMask(d_list->m_layerMask) && d_iterator.Initialize(layer, d_stack) &&
				((triangle = d_iterator.GetNextTriangle(layer.CurrentTestID())) != NULL))
			return triangle;
	}

	return NULL;
}
