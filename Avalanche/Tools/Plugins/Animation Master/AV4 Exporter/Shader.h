#define MAX_NAME 64
/*	The first 4 bits in ulFlags represents the collision type.
	0000 = None
	0001 = Geometry
	0010 = OBB #1
	0011 = OBB #2
	0100 = OBB #3
	0101 = OBB #4
	0110 = OBB #5
	0111 = OBB #6
	1000 = OBB #7
	1001 = OBB #8
*/
#define		AGP_COLLISION_TERRAIN		0
#define		AGP_COLLISION_OBB				1
#define		AGP_COLLISION_INVISIBLE		2
#define		AGP_COLLISION_NONE			3
/* Bits 5-12 represent the terrain type. (Added bits to align with space set aside in engine)
	00000000 = asphalt
	00000001 = glasswall
	00000010 = rockwall
	00000011 = guardrail
	00000100 = snowwall
	00000101 = snow
	00000110 = groomed snow
	00000111 = ice
	00001000 = chainlink
	00001001 = puddle
	00001010 = deep water
	00001011 = sand
	00001100 = boardwalk
	00001101 = dirt
	00001110 = grass
	00001111 = wood
	00010000 = flesh
	00010001 = death
	00010010 = camerawall
	00010011 = nocollide */
	
#define		AGP_TERRAIN_ASPHALT			0
#define		AGP_TERRAIN_GLASSWALL		1
#define		AGP_TERRAIN_ROCKWALL			2
#define		AGP_TERRAIN_GUARDRAIL		3
#define		AGP_TERRAIN_SNOWWALL			4
#define		AGP_TERRAIN_SNOW				5
#define		AGP_TERRAIN_GROOMEDSNOW		6
#define		AGP_TERRAIN_ICE				7
#define		AGP_TERRAIN_CHAINLINK		8
#define		AGP_TERRAIN_PUDDLE			9
#define		AGP_TERRAIN_DEEPWATER		10
#define		AGP_TERRAIN_SAND				11
#define		AGP_TERRAIN_BOARDWALK		12
#define		AGP_TERRAIN_DIRT				13
#define		AGP_TERRAIN_GRASS				14
#define		AGP_TERRAIN_WOOD				15
#define		AGP_TERRAIN_FLESH				16
#define		AGP_TERRAIN_DEATH				17
#define		AGP_TERRAIN_CAMERA_WALL		18
#define		AGP_TERRAIN_NO_COLLIDE		19

/* Bits 13-15 represent the display layer.  This number is between 0 and 7, with
	0 being the terrain and 7 being the sky. (000b - 111b)*/
	
/*	Bits 16-17 represents the Tesselation Adjustment type (00b - 10b) (11b is unused)
	00 = None
	01 = Absolute
	10 = Multiplier */

#define		AGP_TESSADJ_NONE				0
#define		AGP_TESSADJ_ABSOLUTE			1
#define		AGP_TESSADJ_MULTIPLIER		2
/*
   Bit 18 is a boolean representing whether or not this patch is flat shaded. (0 = False, 1=True)
   Bit 19 is a boolean representing if this path clips. (0 = Clips, 1=Does not clip)
   Bit 20 is a boolean representing if fog affects this patch. (0 = Fog affects, 1=Fog does not affect)
   Bit 21 is a boolean representing whether or not this patch is two sided. (0 = False, 1=True)
   Bit 22 is a boolean representing whether or not this patch is zbiased. (0 = False, 1=True)
   Bit 23 is a boolean representing whether or not LOD Crossfading (visible internals) is desired. (0 = False, 1=True)
   Bit 24 is a boolean representing whether or not this group uses an environment map. (0 = False, 1=True)
   Bit 25 is a boolean representing whether or not this patch should be deleted. (0 = False, 1=True) 
   Bit 26 is a boolean representing whether or not this patch is invisible (glass wall) (0 = False, 1=True) 
   Bit 27 is a boolean representing whether or not this patch's environment map is private(0 = False, 1=True) */

/*  The above flags are used to fill out AGPShaderExport.ulFlags. */
typedef struct {
	unsigned short collision	:	4;
	unsigned short terrain		:	8;
	unsigned short layer		:	3;
	unsigned short tessadj		:	2;
	unsigned short flat			:	1;
	unsigned short dnclip		:	1;
	unsigned short dnfog		:	1;
	unsigned short twosided		:	1;
	unsigned short zbias		:	1;
	unsigned short lod			:	1;
	unsigned short environmap	:	1;
	unsigned short deletepatch	:	1;
	unsigned short invisible	:	1;
	unsigned short mapprivate	:	1;
	unsigned short alpha		:	1;				// enable alpha channel of environment map
	unsigned short vc32bit		:	1;			// use 32-bit vertex colors
	unsigned short overridecolor:	1;			// override base color with instance color (for changing car colors, etc)
} BitFlag;


/*
		In the AV4 files, the following structure is stored after the materials, but before the texture maps.
		First, the total number of shaders is stored as an int.  Then, the structure below is saved "totalshaders"
		times.
		In the patch data, the indicies into the shaders are stored for each patch.  After the material ids, but before
		the flags.  First the total number of ids are saved as an integer.  Then, each index is saved as a long.  There
		should be a number of longs equal to the value of totalids.
	
  */
typedef struct {
	BitFlag	bfFlags;
	float siPretess;
	unsigned short siEnvironMapPercent;
	char strEnvironMapPath[_MAX_PATH];
} AGPShaderExport;

//In version 4 files, the following structure will be saved out to disk immediately after the AGPShaderExport structure
typedef struct {
	char strProjectName[MAX_NAME];
	char strTerrainType[MAX_NAME];
} AGPShaderExport_2;

// a combined version
typedef struct {

	AGPShaderExport	Shader1;
	AGPShaderExport_2	Shader2;
} FullAGPShaderExport;


