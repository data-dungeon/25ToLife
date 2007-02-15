////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"

Pool<SoupTriangle> SoupTriangle::pool(32 * 1024, 32 * 1024);

//***************************************************************************
// Sleezy
//***************************************************************************
uint HashKey(Vector3Packed &v)
{
	return((uint)((Math::Abs(v.x()) + Math::Abs(v.y()) + Math::Abs(v.z())) * 100.0f));
}

/***************************************************************************/
/***************************************************************************/
void *SoupTriangle::operator new(size_t size)
{
	// Sanity check
	assert(size == sizeof(SoupTriangle));

	// Chug one off the pool (should never fail)
	void *buffer = pool.NewPtr();
	assert(buffer);

	// Hand it back to the system
	return buffer;
}

/***************************************************************************/
/***************************************************************************/
void SoupTriangle::operator delete(void *me)
{
	// Do we have to check this?
	if (me)
	{
		// Alias it back into ourselves
		SoupTriangle *tri = (SoupTriangle *)me;

		// Put it back in the pool
	 	pool.Delete(tri);
	}
}

////////////////////////////////////////////////////////////////////////////
// Copy
////////////////////////////////////////////////////////////////////////////
SoupTriangle &SoupTriangle::operator=(
const SoupTriangle &t)
{
	a = t.a;
	b = t.b;
	c = t.c;

	ia = t.ia;
	ib = t.ib;
	ic = t.ic;

	normal = t.normal;
	color = t.color;
	material = t.material;

	return *this;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
TriangleSoup::TriangleSoup(void) :
list(true)
{
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
TriangleSoup::TriangleSoup(
TriangleSoup &soup) :
list(true)
{
	Copy(soup);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
TriangleSoup::~TriangleSoup(void)
{
}

////////////////////////////////////////////////////////////////////////////
// Adds the triangle
// Will return NULL if the triangle was degenerate and
// not added
////////////////////////////////////////////////////////////////////////////
SoupTriangle *TriangleSoup::Add(
const Vector3 vertex[],
const Vector3 vertexNormal[],
const ts_bRGBA color[],
u8 material,
bool rewindTriangle)
{
	return Add(vertex[0], vertex[1], vertex[2],
					vertexNormal[0], vertexNormal[1], vertexNormal[2],
					color[0], color[1], color[2],
					material, rewindTriangle);
}

//***************************************************************************
// Process a quad, making it into the two best triangles
//***************************************************************************
void TriangleSoup::AddQuad(
const Vector3 vertex[],
const Vector3 vertexNormal[],
const ts_bRGBA color[],
u8 material,
bool rewindTriangle)
{
	// Setup some aliases so the code is easier to look at
	const Vector3 &a = vertex[0];
	const Vector3 &b = vertex[1];
	const Vector3 &c = vertex[2];
	const Vector3 &d = vertex[3];
	const Vector3 &normalA = vertexNormal[0];
	const Vector3 &normalB = vertexNormal[1];
	const Vector3 &normalC = vertexNormal[2];
	const Vector3 &normalD = vertexNormal[3];
	const ts_bRGBA &colorA = color[0];
	const ts_bRGBA &colorB = color[1];
	const ts_bRGBA &colorC = color[2];
	const ts_bRGBA &colorD = color[3];

	// Which is the better split?
	bool clockwiseBDA, clockwiseBCD;
	Vector3 normalBDA = SaneNormal(b, d, a, normalB, normalD, normalA, clockwiseBDA, rewindTriangle);
	Vector3 normalBCD = SaneNormal(b, c, d, normalB, normalC, normalD, clockwiseBCD, rewindTriangle);

	float splitBD = IdentityofLagrange(normalA, normalC, normalBDA, normalBCD);

	bool clockwiseACD, clockwiseABC;
	Vector3 normalACD = SaneNormal(a, c, d, normalA, normalC, normalD, clockwiseACD, rewindTriangle);
	Vector3 normalABC = SaneNormal(a, b, c, normalA, normalB, normalC, clockwiseABC, rewindTriangle);

	float splitAC = IdentityofLagrange(normalB, normalD, normalABC, normalACD);

	// Add the two triangles
	if (splitBD > splitAC)
	{
		Add(b, d, a, normalB, normalD, normalA, colorB, colorD, colorA, material, rewindTriangle);
		Add(b, c, d, normalB, normalC, normalD, colorB, colorC, colorD, material, rewindTriangle);
	}
	else
	{
		Add(a, c, d, normalA, normalC, normalD, colorA, colorC, colorD, material, rewindTriangle);
		Add(a, b, c, normalA, normalB, normalC, colorA, colorB, colorC, material, rewindTriangle);
	}
}

////////////////////////////////////////////////////////////////////////////
// Add a triangle that is totally ready
////////////////////////////////////////////////////////////////////////////
SoupTriangle *TriangleSoup::AddLike(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
SoupTriangle *like)
{
	// Create lookups for the vertices
	uint ia = vertexHash.Add(Vector3Packed(a));
	uint ib = vertexHash.Add(Vector3Packed(b));
	uint ic = vertexHash.Add(Vector3Packed(c));

	// Degenerate?
	if (ia == ib || ib == ic || ia == ic || SoupTriangle::Degenerate(a, b, c))
		return NULL;

	// Allocate a new one
	SoupTriangle *tri = new SoupTriangle;
	if (!tri)
		return NULL;

	// Make sure our winding is correct
	Vector3 faceNormal;
	faceNormal = Vector3::Cross(c - b, a - b);
	faceNormal.SafeNormalize();
	if (Vector3::Dot(faceNormal, like->normal) > 0.0f)
	{
		tri->SetVertices(a, b, c);
		tri->SetVertices(ia, ib, ic);
	}
	else
	{
		tri->SetVertices(a, c, b);
		tri->SetVertices(ia, ic, ib);
	}

	// Copy the other stuff from the "like" triangle
	tri->normal = like->normal;
	tri->color = like->color;
	tri->material = like->material;

	// Add it to the list
	list.AddTail(tri);

	// Give it back to them
	return tri;
}

////////////////////////////////////////////////////////////////////////////
// Copy this triangle into this soup
////////////////////////////////////////////////////////////////////////////
SoupTriangle *TriangleSoup::Copy(
const SoupTriangle *copy)
{
	// Okay to call this with null
	if (!copy)
		return NULL;

	// Allocate a new one
	SoupTriangle *tri = new SoupTriangle;
	if (!tri)
		return NULL;

	// Copy it!
	*tri = *copy;

	// Add it and give it back
	list.AddTail(tri);
	return tri;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void TriangleSoup::Copy(
TriangleSoup &soup)
{
	SoupTriangle *tri = soup.Head();
	while (tri)
	{
		Copy(tri);
		tri = tri->next;
	}
}

////////////////////////////////////////////////////////////////////////////
// Transfer all the triangles from this soup into here
////////////////////////////////////////////////////////////////////////////
void TriangleSoup::Transfer(
TriangleSoup &soup)
{
	SoupTriangle *tri = soup.Head();
	while (tri)
	{
		SoupTriangle *next = tri->next;

		soup.list.Unlink(tri);
		list.AddTail(tri);

		tri = next;
	}
}

//***************************************************************************
// Get the triangle array (you call delete [] on it)
//***************************************************************************
TerrainTriangleFat *TriangleSoup::ConvertForDiskFat()
{
	// Don't give them back the pointer we have nothing
	if (Triangles() == 0)
		return NULL;

	// Create the buffer
	void *buffer = new char[Triangles() * sizeof(TerrainTriangleFat)];
	assert(buffer);

	// Fill in the triangles
	TerrainTriangleFat *terrainTri = (TerrainTriangleFat *)buffer;
	SoupTriangle *tri = Head();
	while (tri)
	{
		terrainTri->SetVertices(tri->a, tri->b, tri->c);
		terrainTri->SetFaceNormal(tri->normal);

		terrainTri->ResetTestID();

		terrainTri->SetColor(tri->color);
		terrainTri->SetMaterial(tri->material);

		// Always compute axis, rather than plane
		terrainTri->ComputeMajorAxis(tri->normal);

		tri = tri->next;
		terrainTri++;
	}

	// Give them back the data!
	return (TerrainTriangleFat *)buffer;
}

//***************************************************************************
//***************************************************************************
TriangleSoup::LeanTriangles *TriangleSoup::ConvertForDiskLean()
{
	// Make the return structure
	LeanTriangles *lean = new LeanTriangles;
	assert(lean);
	if (!lean)
		return NULL;

	// Create the array of triangles
	lean->triangle = new TerrainTriangleLean[Triangles()];
	assert(lean->triangle);

	// Fill in the triangles
	SoupTriangle *tri = Head();
	TerrainTriangleLean *terrainTri = lean->triangle;
	while (tri)
	{
		terrainTri->SetVertices(tri->ia, tri->ib, tri->ic);

		terrainTri->SetFaceNormal(tri->normal);

		terrainTri->ResetTestID();

		terrainTri->SetColor(tri->color);
		terrainTri->SetMaterial(tri->material);

		// Always compute axis, rather than plane
		terrainTri->ComputeMajorAxis(tri->normal);

		tri = tri->next;
		terrainTri++;
	}

	// What was our load?
 	uint load = vertexHash.Load();

	// Create the vertex array
	lean->vertices = vertexHash.Count();
	lean->vertex = new Vector3Packed[lean->vertices];

	// Now save off the vertex list
	Vector3Packed *hashVertex = vertexHash.Array();
	for (int v = 0; v < lean->vertices; v++)
		lean->vertex[v] = hashVertex[v];

	// Now add the vertex group
	lean->vertexGroup = TerrainTriangleLean::AddVertexGroup(lean->vertex);
	for (int t = 0; t < Triangles(); t++)
		lean->triangle[t].SetVertexGroup(lean->vertexGroup);

	return lean;
}

//***************************************************************************
//***************************************************************************
void TriangleSoup::CleanUp(
LeanTriangles *lean)
{
	if (lean)
	{
		// Clean up the arrays
		delete [] lean->vertex;
		delete [] lean->triangle;

		// We created a vertex group here
		TerrainTriangleLean::RemoveVertexGroup(lean->vertexGroup);

		// Done with the structure
		delete lean;
	}
}

//***************************************************************************
// Adds the triangle
//***************************************************************************
SoupTriangle *TriangleSoup::Add(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &normalA,
const Vector3 &normalB,
const Vector3 &normalC,
const ts_bRGBA &colorA,
const ts_bRGBA &colorB,
const ts_bRGBA &colorC,
u8 material,
bool rewindTriangle)
{
	// first see if it is really small
	if (SoupTriangle::Degenerate(a, b, c))
		return NULL;

	// Create lookups for the vertices
	uint ia = vertexHash.Add(Vector3Packed(a));
	uint ib = vertexHash.Add(Vector3Packed(b));
	uint ic = vertexHash.Add(Vector3Packed(c));

	// Degenerate? (any two vertices equal)
	if (ia == ib || ib == ic || ia == ic)
		return NULL;

	// Allocate a new one
	SoupTriangle *tri = new SoupTriangle;
	if (!tri)
		return NULL;

	// Start out with the normal
	Vector3 faceNormal;
	bool clockwise;
	faceNormal = SaneNormal(a, b, c, normalA, normalB, normalC, clockwise, rewindTriangle);

	// Save the normal
	tri->normal = faceNormal;

	// Do the vertices in the right order
	if (clockwise)
	{
		tri->SetVertices(a, c, b);
		tri->SetVertices(ia, ic, ib);
	}
	else
	{
		tri->SetVertices(a, b, c);
		tri->SetVertices(ia, ib, ic);
	}

	// Figure out the color
	tri->color = FaceColor(colorA, colorB, colorC);

	// Save off the material
	tri->material = material;

	// Add it to the list
	list.AddTail(tri);

	// Give it back to them
	return tri;
}

//***************************************************************************
// Make a "sane" normal (one that matches the direction of the vertex normals)
//***************************************************************************
inline Vector3 TriangleSoup::SaneNormal(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &normalA,
const Vector3 &normalB,
const Vector3 &normalC,
bool &clockwise,
bool allowRewind)
{
	Vector3 faceNormal;

	// Are we allowed to rewind this triangle?
	if (!allowRewind)
	{
		faceNormal = Vector3::Cross(b - a, c - a);
		faceNormal.SafeNormalize();
		clockwise = false;
	}
	else
	{
		// What is the average normal?
		Vector3 averageNormal = (normalA + normalB + normalC).Normalized();

		// what is the computed normal
		faceNormal = Vector3::Cross(b - c, a - c);
		faceNormal.SafeNormalize();

		// Sanity check!
		if (Vector3::Dot(averageNormal, faceNormal) >= 0.0f)
			clockwise = true;
		else
		{
			faceNormal *= -1.0f;
			clockwise = false;
		}
	}

	return faceNormal;
}

//***************************************************************************
//***************************************************************************
u16 TriangleSoup::FaceColor(
const ts_bRGBA &colorA,
const ts_bRGBA &colorB,
const ts_bRGBA &colorC)
{
	// calculate average color for triangle, convert to 5-6-5
	u16 color = 0;
	u16 component = (colorA.R + colorB.R + colorC.R) / 3;
	color |= (((component >> 3) & 0x1f) << 0);

	component = (colorA.G + colorB.G + colorC.G) / 3;
	color |= (((component >> 2) & 0x3f) << 5);

	component = (colorA.B + colorB.B + colorC.B) / 3;
	color |= (((component >> 3) & 0x1f) << 11);

	return color;
}

//***************************************************************************
// The "Identity of Lagrange" states:
// (a x b) * (c x d) = (a * c)(b * d) - (a * d)(b * c)
//***************************************************************************
float TriangleSoup::IdentityofLagrange(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &d)
{
	return(Vector3::Dot(a, c) * Vector3::Dot(b, d) -
				Vector3::Dot(a, d) * Vector3::Dot(b, c));
}

