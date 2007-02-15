/////////////////////////////////////////////////////////////////////////////
// Display height-field stuff
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "GameHelper/GameHelperPCH.h"


/////////////////////////////////////////////////////////////////////////////
// We don't do stuff until the initialize
/////////////////////////////////////////////////////////////////////////////
DisplayHeightField::DisplayHeightField(void) :
triStripList(true)
{
	node = NULL;
	instance = NULL;
	som = NULL;

	textureContext = NULL;
	textureLayers = 0;

	wantEngineLighting = false;
	needNormals = false;

	// init the maps
	for (int m = 0; m < DISPHF_MAXTEXTURES; m++)
	{
		uWrap[m] = 1.0f;
		vWrap[m] = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DisplayHeightField::~DisplayHeightField(void)
{
	// Make sure we get out of the renderer
	if (instance)
		SceneMgr::RemoveInstance(instance);

	// Clear the node list
	if (node)
		delete [] node;

	// Clear all the allocated thingies
	if (som)
		memFree(som);
	if (instance)
		delete instance;
	if (textureContext)
		memFree(textureContext);
}

/////////////////////////////////////////////////////////////////////////////
// Build defaults for a texture layer
/////////////////////////////////////////////////////////////////////////////
void DisplayHeightField::DefaultTextureLayerInit(
TextureLayerInit &init,
int textureIndex)
{
	memset(&init, '\0', sizeof(TextureLayerInit));
	init.textureIndex = textureIndex;
	init.method = TS_METHOD_MODULATE;
	init.blend = RS_BLENDFUNC_DEFAULT;
	init.filter = TS_FILTER_BILINEAR;
	init.uWrap = 1.0f;
	init.vWrap = 1.0f;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the texturing for the height field
// baseColor provides the base color for the material
// if a texture file is provided, the layers are read from it
// if layers is > 0 layerInit must have an array of TextureLayerInit's
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::SetMaterial(
const ts_bRGBA &baseColor,
char *textureFile,
int layers,
TextureLayerInit *layerInit)
{
	// Allocate all the data
	MEM_SET_ALLOC_NAME("DispHF texture");
	textureContext = (ts_FullTextureContext *)memAlloc(sizeof(ts_FullTextureContext));
	ASSERT(textureContext);

	// Clear the context
	memset(textureContext, '\0', sizeof(ts_FullTextureContext));
	textureLayers = 0;

	// Set a base color, even with no texture
	textureContext->BaseColor = baseColor;

	// Did the specify a texture
	if (textureFile)
	{
		// Load the textures
		t_Handle hTexture = dblLoad(textureFile);
		if (hTexture == INVALID_HANDLE)
			return(false);

		// Load the thingie using the thingie
		TextureDatabase* tdb = (TextureDatabase*)dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, hTexture);
		if (!tdb) 
			return false;

		// Set the bind number!!!!!!
		textureContext->TextureCount = layers;
		textureLayers = layers;

		for (int i = 0; i < layers; i++)
		{
			// All of your base are belong to us
			textureContext->TextureLayer[i].textureDatabase = tdb;
			textureContext->TextureLayer[i].s16TextureIndex = layerInit[i].textureIndex;

			// Hack up a bunch of this shizit
			textureContext->TextureLayer[i].u8Method = layerInit[i].method;
			textureContext->TextureLayer[i].u8Blend = layerInit[i].blend;
			textureContext->TextureLayer[i].u8WrapU = TS_WRAP_REPEAT;
			textureContext->TextureLayer[i].u8WrapV = TS_WRAP_REPEAT;
			textureContext->TextureLayer[i].u8Filter = layerInit[i].filter;
			textureContext->TextureLayer[i].u8Flags = layerInit[i].flags;

			// Save the wrap values
			uWrap[i] = layerInit[i].uWrap;
			vWrap[i] = layerInit[i].vWrap;
		}
	}

	// We are out of here!
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the initial height-field geometry (use the orientation matrix if you don't want
// this in the xy plane)
// if flags is non-null it is an xPoints * yPoints array of flags
// no checks are made to see if this causes invalid tri-strips.
// returns false on failure
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::BuildGeometry(
int pointsX,
int pointsY,
float sizeX,
float sizeY,
bool lighting,
bool wantVertexColors,
ts_bRGBA *vertexColor)
{
	// THEY MUST HAVE THE TEXTURE SET UP!!!
	ASSERT(textureContext);
	if (!textureContext)
		return(false);

	// Duh, must define some triangles
	if (pointsX < 2 || pointsY < 2)
		return(false);

	// Engine based lighting is evil
	wantEngineLighting = lighting;
	if (wantEngineLighting)
		needNormals = true;

	// Setup the node array
	xSize = pointsX;
	ySize = pointsY;
	xGrid = sizeX / (float)pointsX;
	yGrid = sizeY / (float)pointsY;
	invXGrid = 1.0f / xGrid;
	invYGrid = 1.0f / yGrid;
	node = new DHFNode[xSize * ySize];
	if (!node)
		return(false);

	// Build tri strips based on columns
	bool buildOK = true;
	for (int x = 0; x < xSize - 1 && buildOK; x++)
	{
		if (!BuildColumnTriStrip(x, x + 1))
			buildOK = false;
	}

	// Did the build work
	if (!buildOK)
	{
		delete [] node;
		node = NULL;
		return(false);
	}

	// Do we have any tristrips? (degenerate)
	if (!triStripList.Head())
	{
		// There are no triangles to display
		delete [] node;
		node = NULL;
		return(false);
	}

	// setup the instance stuff
	if (!InitializeModel())
		return(false);

	// Do they want vertex colors?
	ts_bRGBA defaultColor = { 255, 255, 255, 255 };
	if (!vertexColor)
		vertexColor = &defaultColor;
	if (wantVertexColors)
		BuildVertexColors(*vertexColor);

	// Setup the collisions
	collisions = true;
	clipper.SetExtents(xGrid * 0.5f, yGrid * 0.5f, xGrid * ((float)xSize - 0.5f), yGrid * ((float)ySize - 0.5f));
	aabb = (AABB *)&instance->BoundingBox();

	// Somehow all this code actually worked
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// nodeInfo is a pointsX by points Y (in X row order) that modifies the geometry
// Call this after BuildGeometry and SetupPhysics
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::ModifyNodes(
NodeInfo *info)
{
	ASSERT(info);

	// Setup the nodes
	for (int y = 0; y < ySize; y++)
		for (int x = 0; x < xSize; x++)
		{
			DHFNode *n = Node(x, y);
			ModifyNode(x, y, *info);
			++info;
		}

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Override a single node
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::ModifyNode(
int x,
int y,
NodeInfo &info)
{
	// Duh
	if (x < 0 || x >= xSize || y < 0 || y >= ySize)
		return(false);

	// Alias the node
	DHFNode *n = Node(x, y);

	// Are the overiding the position?
	if (info.override & OVERRIDE_POSITION)
		n->SetPoint(info.position);

	// Color?
	if (info.override & OVERRIDE_COLOR)
		n->SetColor(info.color);

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// If you modify a lot of nodes positions, call this!!
/////////////////////////////////////////////////////////////////////////////
void DisplayHeightField::RecomputeBounds(void)
{
	Vector3 min = *node[0].GetPoint();
	Vector3 max = *node[0].GetPoint();

	for (int i = 1; i < xSize * ySize; i++)
	{
		max = Vector3::Max(max, *node[i].GetPoint());
		min = Vector3::Min(min, *node[i].GetPoint());
	}

	// setup the shizit
	float minLength = min.Length();
	float maxLength = max.Length();
	som->BoundingSphere.Radius = max(minLength, maxLength);
	som->BoundingBox.Min = min;
	som->BoundingBox.Max = max;
	instance->UpdateBoundingVolumesAccurate();
}

/////////////////////////////////////////////////////////////////////////////
// Position the object in the world
/////////////////////////////////////////////////////////////////////////////
void DisplayHeightField::Place(
const Graphics4x4 &m)
{
	if (instance)
	{
		// Default matrix
		instance->Matrix() = m;

		// Compute an inverse matrix for applying displacents
		m.Invert(worldToLocal);

		// setup the bounding data
		instance->UpdateBoundingVolumes();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Update the animation
/////////////////////////////////////////////////////////////////////////////
void DisplayHeightField::Update(
Graphics4x4 &camera)
{
	// Any work to do?
	if (!node)
		return;

	// Compute the normals now that we are done
	if (needNormals)
	{
		for (int iy = 0; iy < ySize; iy++)
			for (int ix = 0; ix < xSize; ix++)
			{
				// Get our node pointer
				DHFNode *node = Node(ix, iy);
		 		Vector3 normal = ComputeNormal(ix, iy);
		 		node->SetNormal(normal);
			}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 DisplayHeightField::PointInWorld(
int x,
int y)
{
	// Off the map?
	if (x < 0 || x >= xSize || y < 0 || y >= ySize)
	{
		Vector3 poo;
		poo.Clear();
		return(poo);
	}

	// Translate it into the work
	DHFNode *node = Node(x, y);
	return(*node->GetPoint() * instance->Matrix());
}

/////////////////////////////////////////////////////////////////////////////
// Collide a segment with all the height fields
// Returns false if no collision
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::Test(
Segment &seg,
DisplayHeightField::Collision &collision)
{
	// Put the segment into our local space
	Segment local;
	local.start = seg.start * worldToLocal;
	local.end = seg.end * worldToLocal;

	// Is it in the bounding box?
	if (!aabb->SegmentCrosses(local.start, local.end))
		return(false);

#define swap(a, b) {	float t = a(); a(b()); b(t); }

	// Clipper works in xz plane
	swap(local.start.y, local.start.z);
	swap(local.end.y, local.end.z);

	// Setup a clipper & clip the incoming segment
	Segment clipped;
	if (!clipper.Clip(local.start, local.end, clipped.start, clipped.end))
		return(false);

	// Clipper works in xz plane
	swap(clipped.start.y, clipped.start.z);
	swap(clipped.end.y, clipped.end.z);

	// Compute the extents
	if (clipped.start.x() > clipped.end.x())
		swap(clipped.start.x, clipped.end.x);
	if (clipped.start.z() > clipped.end.z())
		swap(clipped.start.y, clipped.end.y);

	int startx = (int)(clipped.start.x() * invXGrid);
	int endx = (int)(clipped.end.x() * invXGrid);
	int starty = (int)(clipped.start.y() * invYGrid);
	int endy = (int)(clipped.end.y() * invYGrid);

	// Init the collision
	collision.d = 10000.0f;
	collision.hf = NULL;

	// Test triangles
	for (int y = starty; y <= endy; y++)
		for (int x = startx; x <= endx; x++)
		{
			// Should we terminate on first hit?
			if (TestQuad(clipped, x, y, collision))
				collision.hf = this;
		}

	// Did we get a hit?
	if (!collision.hf)
		return(false);

	// Convert quantities back into world space
	collision.intersection = collision.intersection * instance->Matrix();
	collision.normal = collision.normal.BasisTransform(instance->Matrix());

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::TestQuad(
Segment &seg,
int x,
int y,
Collision &collision)
{
	// some asserts for range
	ASSERT(x >= 0);
	ASSERT(x < xSize - 1);
	ASSERT(y >= 0);
	ASSERT(y < ySize - 1);

	// Get the points
	Vector3 *v[4];
	v[0] = Node(x, y)->GetPoint();
	v[1] = Node(x + 1, y)->GetPoint();
	v[2] = Node(x , y + 1)->GetPoint();
	v[3] = Node(x + 1, y + 1)->GetPoint();

	return(TestTriangle(seg, *v[0], *v[1], *v[2], collision) ||
				TestTriangle(seg, *v[1], *v[2], *v[3], collision));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::TestTriangle(
Segment &seg,
Vector3 &a,
Vector3 &b,
Vector3 &c,
Collision &collision)
{
	// dammit, we have to compute normals...
	Vector3 n = TriangleTest::Normal(a, b, c);

	// Compute the plane intersections
	Vector3 i;
	float t;
	if (seg.TrueIntersectWithPlane(a, n, i, t) &&
			t < collision.d &&
			TriangleTest::PointInsideXY(i, a, b, c))
	{
		collision.intersection = i;
		collision.normal = n;
		collision.d = t;
		return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////////////////////////////
// Return the node pointer for an x,y
/////////////////////////////////////////////////////////////////////////////
inline DHFNode *DisplayHeightField::Node(
int x,
int y)
{
	return(&node[x + y * xSize]);
}

/////////////////////////////////////////////////////////////////////////////
// Return the position of a node
/////////////////////////////////////////////////////////////////////////////
inline Vector4 DisplayHeightField::NodePos(
int x,
int y)
{
	Vector4 p;

	p.Set((float)x * xGrid,
			(float)y * yGrid,
			0.0f,
			0.0f);

	return(p);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline Vector3 DisplayHeightField::SafeNormal(
int x,
int y)
{
	static Vector3 nil( 0.0f, 0.0f, 0.0f );
	if (x < 0 || x >= xSize || y < 0 || y >= ySize)
		return(nil);
	return(*node[x + y * xSize].GetNormal());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline Vector3 *DisplayHeightField::SafePoint(
int x,
int y)
{
	// of edge
	if (x < 0 || x >= xSize || y < 0 || y >= ySize)
		return(NULL);
	return(node[x + y * xSize].GetPoint());
}

/////////////////////////////////////////////////////////////////////////////
// Build a tri-strip out of a column of the HF, there may be hidden nodes
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::BuildColumnTriStrip(
int x0,
int x1)
{
	// How many vertices are we going to have?
	int vertices = ySize * 2;

	// Create our initial tri-strip
	int flags = CTRISTRIP_TWOSIDED | CTRISTRIP_ZEROFILLHDR;
	if (wantEngineLighting)
		flags |= CTRISTRIP_HASNORMALS;
	TriStripListElement *triStripE = new TriStripListElement(vertices, textureLayers, flags);
	ASSERT(triStripE);
	if (!triStripE)
		return(false);

	// Add it into the list of tristrips
	triStripList.AddHead(triStripE);

	// Get an alias to the tri strip
	CTriStrip *triStrip = triStripE->Get();

	// Get the vertex pointer
	Vector4 *triStripV = triStrip->Vertex();
	Vector3 *triStripN = triStrip->Normal();
	Vector3 up( 0.0f, 0.0f, 1.0f );

	// Build it in column order
	int index = 0;
	for (int row = 0; row < ySize; row++)
	{
		// Get the node pointers
		DHFNode *node0 = Node(x0, row);
		DHFNode *node1 = Node(x1, row);

		// Generate the vertices
		// Stick them in the tri strip
		Vector4 point = NodePos(x0, row);
		triStripV[index] = point;
		node0->AssociatePoint((Vector3 *)&triStripV[index]);

		point = NodePos(x1, row);
		triStripV[index + 1] = point;
		node1->AssociatePoint((Vector3 *)&triStripV[index + 1]);

		// Stick in the normal
		if (wantEngineLighting)
		{
			triStripN[index] = up;
			node0->AssociateNormal((Vector3*)&triStripN[index]);

			triStripN[index+1] = up;
			node1->AssociateNormal((Vector3*)&triStripN[index + 1]);
		}

		// Get the uv's
		for (int t = 0; t < textureLayers; t++)
		{
			triStrip->uv[t][index] = GetUV(t, x0, row);
			node0->AssociateUV(t, &triStrip->uv[t][index]);

			triStrip->uv[t][index + 1] = GetUV(t, x1, row);
			node1->AssociateUV(t, &triStrip->uv[t][index + 1]);
		}

		// Next set of indices
		index += 2;
	}

	// Set our vertex number
	triStrip->Vertices() = vertices;

	// We are done, create the model
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Initialize all the instance shizit
/////////////////////////////////////////////////////////////////////////////
bool DisplayHeightField::InitializeModel(void)
{
	// Allocate all the data
	instance = new CInstance;
	ASSERT(instance);

	MEM_SET_ALLOC_NAME("DispHF SOM");
	som = (ts_PatchSOM *)memAlloc(sizeof(ts_PatchSOM));
	ASSERT(som);

	// Initialize the som
	memset(som, '\0', sizeof(ts_PatchSOM));
	strcpy(som->Name, "DispHF");
	som->dh.u8DataType = DATA_SOM;

	// mark this as dynamic geometry so that it gets a new display list built every frame
	som->dh.u16Flags |= SOMM_DYNAMIC_GEOMETRY;
	if (wantEngineLighting)
		som->dh.u16Flags |= SOMM_TRI_STRIP_NORMALS;

	// setup the bounding data
	som->BoundingBox.Min.Set(0.0f,
									 0.0f,
									 -1.0f);
	som->BoundingBox.Max.Set((float)xSize * xGrid,
									 (float)ySize * yGrid,
									 1.0f);
	som->BoundingSphere.Center = BoundingBox__GetCenter(&som->BoundingBox);
	float radius = Math::Max((float)xSize * xGrid, (float)ySize * yGrid);
	som->BoundingSphere.Radius = radius;

	// Get the model and create the instance
	ts_Geometry geometry;
	geometry.pSOM = som;
	geometry.pLOD = NULL;

	// Make the instance
	instance->InitFromGeometry( geometry );
	instance->SetAlphaSort( true );
	instance->SetVertexColor32Bit( true );

	// Make sure to disable lighting
	instance->SetDynamicallyLit( wantEngineLighting );

	// calculate initial bounding data
	instance->UpdateBoundingVolumesAccurate();

	// Insert the tristrips into the som
	TriStripListElement *triStrip = triStripList.Head();
	while (triStrip)
	{
		// tack it into the som
		ts_TriStripHeader *hdr = triStrip->Get()->Header();
		ts_TriStripHeader *next = som->pTriStripBuffer;
		som->pTriStripBuffer = hdr;

		// Set the texture context
		hdr->pTextureContext = (ts_TextureContext *)textureContext;

		// Add in the remaining ones
		hdr->pNext = next;

		// Increment the count
		som->u32TriStripCount++;

		// next!
		triStrip = triStrip->next;
	}

	// Add it to the scene
	SceneMgr::AddInstance(instance);

	// We're out of here!
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Build the vertex colors
/////////////////////////////////////////////////////////////////////////////
void DisplayHeightField::BuildVertexColors(
ts_bRGBA &c)
{
	// Compute how large this will be
	int stripSize = Align_RoundTo16((2 * ySize) * sizeof(ts_bRGBA));
	int totalSize = stripSize * xSize;

	// Allocate the array
	instance->SetVertexColorPtr( memAlloc( totalSize ) );
	if ( !instance->VertexColorPtr() )
		return;

	// Init to default colors
	for (int y = 0; y < ySize; y++)
	{
		for (int x = 0; x < xSize; x++)
		{
			DHFNode *node = Node(x, y);
			SetupColor(x, y, *node);
			node->SetColor(c);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Set a color in the color data
/////////////////////////////////////////////////////////////////////////////
inline void DisplayHeightField::SetupColor(
int x,
int y,
DHFNode &node)
{
	// How big is each strip?
	int stripSize = Align_RoundTo16((2 * ySize) * sizeof(ts_bRGBA));

	// Figure out the location of the first color
	int strip = x - 1;
	if (strip >= 0)
	{
		// Alias the color table
		ts_bRGBA *colorTable = (ts_bRGBA *)((char *)instance->VertexColorPtr() + strip * stripSize);
		node.AssociateColor(&colorTable[y * 2 + 1]);
	}

	// Figure out the location of the second color
	++strip;
	if (strip < xSize)
	{
		// Alias the color table
		ts_bRGBA *colorTable = (ts_bRGBA *)((char *)instance->VertexColorPtr() + strip * stripSize);
		node.AssociateColor(&colorTable[y * 2]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Compute a normal for a node
/////////////////////////////////////////////////////////////////////////////
Vector3 DisplayHeightField::ComputeNormal(
int x,
int y)
{
	// The origin
	Vector3 *o = Node(x, y)->GetPoint();

	// Total sum
	Vector3 sum( 0.0f, 0.0f, 0.0f );

	// The neighbors
#define PHATTY_NORMAL
#ifdef PHATTY_NORMAL
	Vector3 *e[8];
	e[0] = SafePoint(x + 1, y);
	e[1] = SafePoint(x + 1, y + 1);
	e[2] = SafePoint(x, y + 1);
	e[3] = SafePoint(x - 1, y + 1);
	e[4] = SafePoint(x - 1, y);
	e[5] = SafePoint(x - 1, y - 1);
	e[6] = SafePoint(x, y - 1);
	e[7] = SafePoint(x + 1, y - 1);

	// Point to the neighbors
	Vector3 en[8];
	if (e[0]) en[0] = *e[0] - *o;
	if (e[1]) en[1] = *e[1] - *o;
	if (e[2]) en[2] = *e[2] - *o;
	if (e[3]) en[3] = *e[3] - *o;
	if (e[4]) en[4] = *e[4] - *o;
	if (e[5]) en[5] = *e[5] - *o;
	if (e[6]) en[6] = *e[6] - *o;
	if (e[7]) en[7] = *e[7] - *o;

	// Sum cross products
	if (e[0] && e[1]) sum += Vector3::Cross(en[0], en[1]);
	if (e[1] && e[2]) sum += Vector3::Cross(en[1], en[2]);
	if (e[2] && e[3]) sum += Vector3::Cross(en[2], en[3]);
	if (e[3] && e[4]) sum += Vector3::Cross(en[3], en[4]);
	if (e[4] && e[5]) sum += Vector3::Cross(en[4], en[5]);
	if (e[5] && e[6]) sum += Vector3::Cross(en[5], en[6]);
	if (e[6] && e[7]) sum += Vector3::Cross(en[6], en[7]);
	if (e[7] && e[0]) sum += Vector3::Cross(en[7], en[0]);

#else
	Vector3 *e[4];
	e[0] = SafePoint(x + 1, y);
	e[1] = SafePoint(x, y + 1);
	e[2] = SafePoint(x - 1, y);
	e[3] = SafePoint(x, y - 1);

	// Point to the neighbors
	Vector3 en[4];
	if (e[0]) en[0] = *e[0] - *o;
	if (e[1]) en[1] = *e[1] - *o;
	if (e[2]) en[2] = *e[2] - *o;
	if (e[3]) en[3] = *e[3] - *o;

	// Sum cross products
	if (e[0] && e[1]) sum += Vector3::Cross(en[0], en[1]);
	if (e[1] && e[2]) sum += Vector3::Cross(en[1], en[2]);
	if (e[2] && e[3]) sum += Vector3::Cross(en[2], en[3]);
	if (e[3] && e[0]) sum += Vector3::Cross(en[3], en[0]);
#endif

	// Normalize and set it
	sum.Normalize();
	return(sum);
}

