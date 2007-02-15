////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#ifndef TRISOUP_H
#define TRISOUP_H

#include <container\dllist.h>
#include <container\pool.h>
#include <container\hashlist.h>
#include <collide\terraintri.h>

#define DEGENERATE_AREA_THRESHOLD 	0.01f

class TriangleSoup;

class SoupTriangle
{
public:
	// Nothing to see here
	SoupTriangle() { next = prev = NULL; }

	// Heavy-duty triangle
	Vector3									a, b, c;
	uint										ia, ib, ic;
	Vector3									normal;
	u16										color;
	u8											material;

	// Nice for setting vertices
	inline void SetVertices(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2)
		{ a = v0; b = v1; c = v2; }
	inline void SetVertices(uint a, uint b, uint c)
		{ ia = a; ib = b; ic = c; }

	// Return the area of the triangle
	static inline float Area(const Vector3 &a, const Vector3 &b, const Vector3 &c)
		{ return (Vector3::Cross(c - b, a - b).Length() * 0.5f); }

	// Return true if this triangle is degenerate
	static inline bool Degenerate(const Vector3 &a, const Vector3 &b, const Vector3 &c)
		{ return (Area(a, b, c) < DEGENERATE_AREA_THRESHOLD); }

	// Special control since we use a pool
	void *operator new(size_t size);
	void operator delete(void *me);

	// Copy
	SoupTriangle &operator=(const SoupTriangle &t);

	// we appear in a list
	SoupTriangle *prev, *next;

private:
	// We have a pool of user data structures
	static Pool<SoupTriangle>		pool;
};

class TriangleSoup
{
public:
	TriangleSoup(void);
	TriangleSoup(TriangleSoup &soup);
	~TriangleSoup(void);

	// Adds the triangle
	// Will return NULL if the triangle was degenerate and
	// not added
	SoupTriangle *Add(const Vector3 vertex[], const Vector3 vertexNormal[],
							const ts_bRGBA color[], u8 material, bool rewindTriangle);

	// Add a quad
	void AddQuad(const Vector3 vertex[], const Vector3 vertexNormal[],
		  				const ts_bRGBA color[], u8 material, bool rewindTriangle);

	// Add a triangle with new verts that has all the
	// properties inherited from like
	SoupTriangle *AddLike(const Vector3 &a, const Vector3 &b, const Vector3 &c,
									SoupTriangle *like);

	// Copy this triangle into this soup
	SoupTriangle *Copy(const SoupTriangle *copy);
	void Copy(TriangleSoup &soup);

	// Unlink this from the list
	inline void Unlink(SoupTriangle *tri)
		{ list.Unlink(tri); }

	// Add this triangle to this list
	inline void Add(SoupTriangle *tri)
		{ list.AddTail(tri); }

	// Transfer all the triangles from this soup into here
	void Transfer(TriangleSoup &soup);

	// Get at the head of the list
	inline SoupTriangle *Head()
		{ return list.Head(); }

	// How many triangles are in the list?
	inline int Triangles()
		{ return list.Items(); }

	// Get the triangle array (you call delete [] on it)
	TerrainTriangleFat *ConvertForDiskFat();

	// Get the lean triangles
	struct LeanTriangles
	{
		Vector3Packed			*vertex;
		int						vertices;
		TerrainTriangleLean	*triangle;
		u8							vertexGroup;
	};
	LeanTriangles *ConvertForDiskLean();
	static void CleanUp(LeanTriangles *lean);

private:
	// Here is the masterful list
	DoublyLinkedList<SoupTriangle>	list;

	// Here is the vertex hash
	HashList<Vector3Packed, 14>		vertexHash;

	// Adds the triangle
	SoupTriangle *Add(const Vector3 &a, const Vector3 &b, const Vector3 &c,
							const Vector3 &normalA, const Vector3 &normalB, const Vector3 &normalC,
							const ts_bRGBA &colorA, const ts_bRGBA &colorB, const ts_bRGBA &colorC,
							u8 material, bool rewindTriangle);

	// Make a "sane" normal (one that matches the direction of the vertex normals)
	Vector3 SaneNormal(const Vector3 &a, const Vector3 &b, const Vector3 &c,
								const Vector3 &normalA,	const Vector3 &normalB,	const Vector3 &normalC,
								bool &clockwise, bool allowRewind);

	u16 FaceColor(const ts_bRGBA &colorA, const ts_bRGBA &colorB, const ts_bRGBA &colorC);

	float IdentityofLagrange(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d);
};

#endif
