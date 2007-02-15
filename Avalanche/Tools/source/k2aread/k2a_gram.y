%{

//#define dbgYacc(str)	puts(str)
#define dbgYacc(str)
#include "K2AtoSOM.h"
#include <stdio.h>
%}

%union
{
	int					iInteger;
	float					fFloat;
	unsigned char		*pString;
}

%token <fFloat>	TOK_FLOAT
%token <pString> 	TOK_STRING
%token <pString> 	TOK_QSTRING
%token <iInteger> TOK_INTEGER
%token LP
%token RP

%token LBL_BITMAP
%token LBL_BITMAP_FILTER
%token LBL_COMMENT
%token LBL_GEOM_FORMAT
%token LBL_INHERIT_POS
%token LBL_INHERIT_ROT
%token LBL_INHERIT_SCL
%token LBL_K2_ASCIIEXPORT
%token LBL_K2_FILETYPE
%token LBL_MAP_AMOUNT
%token LBL_MAP_CLASS
%token LBL_MAP_DIFFUSE
%token LBL_MAP_NAME
%token LBL_MAP_SUBNO
%token LBL_MAP_TYPE
%token LBL_MATERIAL
%token LBL_MATERIAL_AMBIENT
%token LBL_MATERIAL_CLASS
%token LBL_MATERIAL_COUNT
%token LBL_MATERIAL_DIFFUSE
%token LBL_MATERIAL_FALLOFF
%token LBL_MATERIAL_LIST
%token LBL_MATERIAL_NAME
%token LBL_MATERIAL_REF
%token LBL_MATERIAL_SELFILLUM
%token LBL_MATERIAL_SHADING
%token LBL_MATERIAL_SHINE
%token LBL_MATERIAL_SHINESTRENGTH
%token LBL_MATERIAL_SPECULAR
%token LBL_MATERIAL_TEXTURE_COUNT
%token LBL_MATERIAL_TRANSPARENCY
%token LBL_MATERIAL_WIRESIZE
%token LBL_MATERIAL_XP_FALLOFF
%token LBL_MATERIAL_XP_TYPE
%token LBL_MESH
%token LBL_MESH_CFACE
%token LBL_MESH_CFACELIST
%token LBL_MESH_CVERTLIST
%token LBL_MESH_FACE
%token LBL_MESH_FACE_LIST
%token LBL_MESH_FORMAT
%token LBL_MESH_NUMCFACES
%token LBL_MESH_NUMCVERTEX
%token LBL_MESH_NUMCVFACES
%token LBL_MESH_NUMFACES
%token LBL_MESH_NUMTVERTEX
%token LBL_MESH_NUMTFACES
%token LBL_MESH_NUMVERTEX
%token LBL_MESH_TFACE
%token LBL_MESH_TFACELIST
%token LBL_MESH_TVERT
%token LBL_MESH_TVERTLIST
%token LBL_MESH_VERTCOL
%token LBL_MESH_VERTEX
%token LBL_MESH_VERTEX_LIST
%token LBL_NODE_HAS_K2_DATA
%token LBL_NODE_NAME
%token LBL_NODE_PARENT
%token LBL_NODE_TM
%token LBL_OBJECT
%token LBL_GEOM
%token LBL_LIGHT
%token LBL_PROP_CASTSHADOW
%token LBL_PROP_MOTIONBLUR
%token LBL_PROP_RECVSHADOW
%token LBL_SCENE
%token LBL_SCENE_AMBIENT_STATIC
%token LBL_SCENE_BACKGROUND_STATIC
%token LBL_SCENE_FILENAME
%token LBL_SCENE_FIRSTFRAME
%token LBL_SCENE_FRAMESPEED
%token LBL_SCENE_LASTFRAME
%token LBL_SCENE_TICKSPERFRAME
%token LBL_TIMEVALUE
%token LBL_TM_POS
%token LBL_TM_ROTANGLE
%token LBL_TM_ROTAXIS
%token LBL_TM_ROW0
%token LBL_TM_ROW1
%token LBL_TM_ROW2
%token LBL_TM_ROW3
%token LBL_TM_SCALE
%token LBL_TM_SCALEAXIS
%token LBL_TM_SCALEAXISANG
%token LBL_UVW_ANGLE
%token LBL_UVW_BLUR
%token LBL_UVW_BLUR_OFFSET
%token LBL_UVW_NOISE_LEVEL
%token LBL_UVW_NOISE_PHASE
%token LBL_UVW_NOISE_SIZE
%token LBL_UVW_NOUSE_AMT
%token LBL_UVW_U_OFFSET
%token LBL_UVW_U_TILING
%token LBL_UVW_V_OFFSET
%token LBL_UVW_V_TILING
%token LBL_K2_ARTICULATED_BLOCK
%token LBL_K2_ARTICULATED_PIVOT
%token LBL_HELPER
%token LBL_HELPER_CLASS
%token LBL_BOUNDING_BOX_MIN
%token LBL_BOUNDING_BOX_MAX

%type <fFloat> float_t
%type <pString> node_name

%%

program: /* NULL */
	| ascii_export file_type program_statement_list
	;

program_statement_list:
	program_statement
	| program_statement_list program_statement
	;

program_statement:
	comment
	| scene_block							{ dbgYacc( "Scene Block"); }
	| material_list_block				{ dbgYacc( "Material List Block"); }
	| geom_object_list_block			{ dbgYacc( "Object Geom Block"); }
	| helper_object_list_block			{ dbgYacc( "Object Helper Block"); }
	| error
	;

file_type:
	LBL_K2_FILETYPE TOK_INTEGER  		{ dbgYacc( "File Type"); }
	;
ascii_export:
	LBL_K2_ASCIIEXPORT TOK_INTEGER 	{ dbgYacc( "ASCII Export");  }
	;

comment:
	LBL_COMMENT TOK_QSTRING				{ dbgYacc( "Comment"); }
	;

scene_block:
	LBL_SCENE LP scene_block_line_list RP
	;

scene_block_line_list:
	scene_block_line
	| scene_block_line_list scene_block_line
	;

scene_block_line:
	LBL_SCENE_FILENAME					TOK_QSTRING
	| LBL_SCENE_FIRSTFRAME 				TOK_INTEGER
	| LBL_SCENE_LASTFRAME		  		TOK_INTEGER
	| LBL_SCENE_FRAMESPEED				TOK_INTEGER
	| LBL_SCENE_TICKSPERFRAME			TOK_INTEGER
	| LBL_SCENE_BACKGROUND_STATIC		TOK_FLOAT TOK_FLOAT TOK_FLOAT
	| LBL_SCENE_AMBIENT_STATIC			TOK_FLOAT TOK_FLOAT TOK_FLOAT
	;

material_list_block:
	LBL_MATERIAL_LIST LP material_count material_block_list RP
	;

material_count:
	LBL_MATERIAL_COUNT TOK_INTEGER	{ProcessMaterialCount ($2);}

	;

material_block_list:
	material_block
	| material_block_list material_block
	;

material_block:
	LBL_MATERIAL TOK_INTEGER LP material_block_line_list RP
	;

material_block_line_list:
	material_block_line
	| material_block_line_list material_block_line
	;
material_block_line:
	LBL_MATERIAL_NAME						TOK_QSTRING		{ProcessMaterialName ($2);}
	| LBL_MATERIAL_CLASS					TOK_QSTRING
	| LBL_MATERIAL_AMBIENT				TOK_FLOAT TOK_FLOAT TOK_FLOAT
	| LBL_MATERIAL_DIFFUSE				TOK_FLOAT TOK_FLOAT TOK_FLOAT
	| LBL_MATERIAL_SPECULAR				TOK_FLOAT TOK_FLOAT TOK_FLOAT
	| LBL_MATERIAL_SHINE					TOK_FLOAT
	| LBL_MATERIAL_SHINESTRENGTH		TOK_FLOAT
	| LBL_MATERIAL_TRANSPARENCY		TOK_FLOAT
	| LBL_MATERIAL_WIRESIZE				TOK_FLOAT
	| LBL_MATERIAL_SHADING				TOK_STRING
	| LBL_MATERIAL_XP_FALLOFF			TOK_FLOAT
	| LBL_MATERIAL_SELFILLUM			TOK_FLOAT
	| LBL_MATERIAL_FALLOFF				TOK_STRING
	| LBL_MATERIAL_XP_TYPE				TOK_STRING
	| LBL_MATERIAL_TEXTURE_COUNT		TOK_INTEGER
	| map_diffuse_block
	;

map_diffuse_block:
	LBL_MAP_DIFFUSE LP map_diffuse_block_line_list RP
	;

map_diffuse_block_line_list:
	map_diffuse_block_line
	| map_diffuse_block_line_list map_diffuse_block_line
	;

map_diffuse_block_line:									
	LBL_MAP_NAME							TOK_QSTRING
	LBL_MAP_CLASS							TOK_QSTRING
	LBL_MAP_SUBNO							TOK_INTEGER
	LBL_MAP_AMOUNT							TOK_FLOAT
	LBL_BITMAP								TOK_QSTRING				
	LBL_MAP_TYPE							TOK_STRING
	LBL_UVW_U_OFFSET						TOK_FLOAT				
	LBL_UVW_V_OFFSET						TOK_FLOAT				
	LBL_UVW_U_TILING						TOK_FLOAT				
	LBL_UVW_V_TILING						TOK_FLOAT				
	LBL_UVW_ANGLE							TOK_FLOAT
	LBL_UVW_BLUR							TOK_FLOAT
	LBL_UVW_BLUR_OFFSET					TOK_FLOAT
	LBL_UVW_NOUSE_AMT						TOK_FLOAT
	LBL_UVW_NOISE_SIZE 					TOK_FLOAT
	LBL_UVW_NOISE_LEVEL					TOK_INTEGER
	LBL_UVW_NOISE_PHASE					TOK_FLOAT
	LBL_BITMAP_FILTER						TOK_STRING		{ProcessMaterialBlock ($10, $14, $16, $18, $20);}
	;

geom_object_list_block:
	LBL_OBJECT LBL_GEOM LP object_block_list RP
	;

helper_object_list_block:
	LBL_OBJECT LBL_HELPER LP object_block_list 			
	LBL_BOUNDING_BOX_MIN		TOK_FLOAT TOK_FLOAT TOK_FLOAT
	LBL_BOUNDING_BOX_MAX		TOK_FLOAT TOK_FLOAT TOK_FLOAT
	RP																				{ProcessHelper ();}
	;

object_block_list:
	object_block
		{ dbgYacc( "Block"); }
	| object_block_list object_block
	;

object_block:
	object_node_block
		{ dbgYacc( "Node"); }
	| object_geom_format
	| object_properties
	| object_material_ref
	;

object_properties:
	LBL_PROP_MOTIONBLUR					TOK_INTEGER
	| LBL_PROP_CASTSHADOW				TOK_INTEGER
	| LBL_PROP_RECVSHADOW				TOK_INTEGER
	;

object_material_ref:
	LBL_MATERIAL_REF						TOK_INTEGER		{ProcessMaterialRef ($2);}
	;

object_node_block:
	node_name 								
	LBL_NODE_PARENT						TOK_QSTRING		{ProcessNodeParentName ($1,$3);}
	LBL_NODE_HAS_K2_DATA  				TOK_STRING
	opt_articulated_block
	opt_helper_class
	node_trans_mat
	;

opt_articulated_block:
	| articulated_block
	;		

articulated_block:
	LBL_K2_ARTICULATED_BLOCK			
	LBL_K2_ARTICULATED_PIVOT			TOK_FLOAT TOK_FLOAT TOK_FLOAT	{ ProcessArticulatedPivot ($3, $4, $5);}
	;

opt_helper_class:
	| LBL_HELPER_CLASS 					TOK_QSTRING
	;		

node_trans_mat:
	LBL_NODE_TM LP node_trans_mat_block RP
	;
node_trans_mat_block:
	node_trans_mat_line
	| node_trans_mat_block node_trans_mat_line
	;

node_trans_mat_line:
	node_name
	{ dbgYacc( "MatName"); }
	 LBL_INHERIT_POS						float_t float_t float_t
	 LBL_INHERIT_ROT						float_t float_t float_t
	 LBL_INHERIT_SCL						float_t float_t float_t
	 matrix
	 LBL_TM_POS							float_t float_t float_t
	 LBL_TM_ROTAXIS						float_t float_t float_t
	 LBL_TM_ROTANGLE						float_t
	 LBL_TM_SCALE							float_t float_t float_t
	 LBL_TM_SCALEAXIS					float_t float_t float_t
	 LBL_TM_SCALEAXISANG				float_t
	;

node_name:
	LBL_NODE_NAME TOK_QSTRING		{$$ = $2;}
	;

matrix:
	LBL_TM_ROW0 float_t float_t float_t
	LBL_TM_ROW1 float_t float_t float_t
	LBL_TM_ROW2 float_t float_t float_t
	LBL_TM_ROW3 float_t float_t float_t
	;


object_geom_format:
	LBL_GEOM_FORMAT LBL_MESH_FORMAT
	{ dbgYacc( "Geom Format"); }
	| geom_mesh
	;

geom_mesh:
	LBL_MESH LP mesh_block RP
	;

mesh_block:
	LBL_TIMEVALUE TOK_INTEGER
	mesh_faces_block_list
	;

mesh_faces_block_list:
	mesh_faces_block
	{ dbgYacc( "Promote"); }
	| mesh_faces_block_list mesh_faces_block
	;

mesh_faces_block:
	mesh_vertex_count opt_mesh_face_block
	;

opt_mesh_face_block:
	| mesh_face_block
		{ dbgYacc( "Face Block"); }
	| mesh_cface_block
		{ dbgYacc( "CFace Block"); }
	;

mesh_face_block:
	mesh_face_count
	mesh_vertex_list_block
	mesh_face_list_block
	;

mesh_cface_block:
	mesh_vertex_list_block
	mesh_face_count
	mesh_face_list_block
	;

mesh_vertex_count:
/*	mesh_vertex_ident	TOK_INTEGER	*/		
	LBL_MESH_NUMVERTEX		TOK_INTEGER		{ProcessNumVertex ($2);}
	| LBL_MESH_NUMTVERTEX	TOK_INTEGER		{ProcessNumTVertex ($2);}
	| LBL_MESH_NUMCVERTEX	TOK_INTEGER		{ProcessNumCVertex ($2);}
		{ dbgYacc( "Vertex Count"); }		
	;
/*
mesh_vertex_ident:
	{ dbgYacc( "VIdent"); }
	LBL_MESH_NUMVERTEX
	| LBL_MESH_NUMTVERTEX
	| LBL_MESH_NUMCVERTEX
	;
*/

mesh_face_count:
/*
	mesh_face_count_ident TOK_INTEGER	{ProcessNumFaces ($2);}
*/
	LBL_MESH_NUMFACES		 	TOK_INTEGER	{ProcessNumFaces ($2);}
	| LBL_MESH_NUMTFACES 	TOK_INTEGER	{ProcessNumTFaces ($2);}
	| LBL_MESH_NUMCFACES	 	TOK_INTEGER	{ProcessNumCFaces ($2);}
	;

/*
mesh_face_count_ident:
	LBL_MESH_NUMFACES
	| LBL_MESH_NUMTVFACES
	| LBL_MESH_NUMCFACES
	| LBL_MESH_NUMCVFACES
	;
*/


mesh_vertex_list_block:
	{ dbgYacc( "Vertex List Block"); }
	mesh_vertex_list_ident LP mesh_vertex_list RP
	;

mesh_vertex_list_ident:
	LBL_MESH_VERTEX_LIST
	| LBL_MESH_TVERTLIST
	| LBL_MESH_CVERTLIST
	;

mesh_vertex_list:
	mesh_vertex
	| mesh_vertex_list mesh_vertex
	;

mesh_vertex:
/*
	mesh_vertex_ident	TOK_INTEGER TOK_FLOAT TOK_FLOAT TOK_FLOAT {ProcessVertex ($2, $3, $4, $5);}
*/
	LBL_MESH_VERTEX		TOK_INTEGER TOK_FLOAT TOK_FLOAT TOK_FLOAT {ProcessVertex ($2, $3, $4, $5);}
	| LBL_MESH_TVERT		TOK_INTEGER TOK_FLOAT TOK_FLOAT TOK_FLOAT {ProcessTVertex ($2, $3, $4, $5);}
	| LBL_MESH_VERTCOL	TOK_INTEGER TOK_FLOAT TOK_FLOAT TOK_FLOAT {ProcessCVertex ($2, $3, $4, $5);}
	;
/*
mesh_vertex_ident:
	LBL_MESH_VERTEX
	| LBL_MESH_TVERT
	| LBL_MESH_VERTCOL
	;
*/
mesh_face_list_block:
	{ dbgYacc( "Face List Block"); }
	mesh_face_list_ident LP mesh_face_list RP
	;

mesh_face_list_ident:
	LBL_MESH_FACE_LIST
	| LBL_MESH_TFACELIST
	| LBL_MESH_CFACELIST
	;

mesh_face_list:
	mesh_face
	| mesh_face_list mesh_face
	;

mesh_face:
/*
	mesh_face_ident TOK_INTEGER TOK_INTEGER TOK_INTEGER TOK_INTEGER
*/
	LBL_MESH_FACE			TOK_INTEGER TOK_INTEGER TOK_INTEGER TOK_INTEGER   	{ProcessFace	($2, $3, $4, $5);}
	| LBL_MESH_TFACE		TOK_INTEGER TOK_INTEGER TOK_INTEGER TOK_INTEGER		{ProcessTFace	($2, $3, $4, $5);}
	| LBL_MESH_CFACE		TOK_INTEGER TOK_INTEGER TOK_INTEGER TOK_INTEGER		{ProcessCFace	($2, $3, $4, $5);}
	;
/*
mesh_face_ident:
	LBL_MESH_FACE
	| LBL_MESH_TFACE
	| LBL_MESH_CFACE
	;
*/
float_t:
	TOK_FLOAT		{ dbgYacc( "f float_t"); $$ = $1; }
	| TOK_INTEGER	{ dbgYacc( "i float_t"); $$ = (float) $1; }
	;

%%
int			iLineCount = 0;

int yyerror( unsigned char *pStr)
{
	printf( "Err on line %d: %s\n", iLineCount, pStr);
	return 0;
}

