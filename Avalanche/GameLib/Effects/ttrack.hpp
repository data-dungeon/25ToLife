/***************************************************************************/
// Tire track instance implementations
/***************************************************************************/

/***************************************************************************/
// Add vertex data
/***************************************************************************/
inline void TireTrack::AddVertexData(
Vector3 &a,
Vector3 &b,
float segmentLength)
{
	// build the vertex.
	SetVertexData(pairs++ * 2, a, b, segmentLength);
}

/***************************************************************************/
// Set vertex data
/***************************************************************************/
inline void TireTrack::SetVertexData(
uint vertBase,
Vector3 &a,
Vector3 &b,
float segmentLength)
{
	ASSERT(Locked());

	vbuf.Lock();

	// vertex color
	vbuf.Diffuse(vertBase) = VtxBuffer::cvtRGBA(baseColor);
	vbuf.Diffuse(vertBase+1) = VtxBuffer::cvtRGBA(baseColor);
	
	// vertex positions...
	vbuf.Vertex(vertBase) = a;
	vbuf.Vertex(vertBase+1) = b;

	// stretch texture once across verts...
	Vector2 uv1, uv2;
	uv1.x(1.0f);
	uv2.x(0.0f);

	// other tex coord based on how far we moved...
	uv1.y(vSeg[vertBase>>1] + segmentLength * invSegmentLength * invSegments);
	uv2.y(uv1.x());

	// assign tex coords
	vbuf.TexCoordv2(0, vertBase) = uv1;
	vbuf.TexCoordv2(0, vertBase+1) = uv2;

	vbuf.UnLock();
}

