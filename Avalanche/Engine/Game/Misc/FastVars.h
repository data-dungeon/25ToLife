/*************************************/
/*                                   */
/*   g_FastVars.h                    */
/*   new engine  06/20/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   speed-critical variables        */
/*                                   */
/*************************************/

#ifndef __G_FASTVARS_H
#define __G_FASTVARS_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"			// low-level include
#include "Game/Misc/Bounds.h"				// for ts_BoundingBox declaration of GeneralBoundingBox

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

#define WORK_VECTOR_COUNT 8	// currently, 8 needed by BoxInView

/* size of pre-built viewport dma packet, in quad words */

#define VIEWPORT_PACKET_QWC							4 //yuch2

/* size of pre-built light (light definitions) dma packet, in quad words */

#define LIGHT_PACKET_QWC								16

/* sizes of pre-built matrix/light state dma packets, in quad words */

#define DIRECTIONAL_LIGHT_MATRIX_PACKET_QWC		(7 + 4)	// "+ 4" is for matrix contained in each packet
#define AMBIENT_LIGHT_MATRIX_PACKET_QWC			(4 + 4)
#define FULL_AMBIENT_LIGHT_MATRIX_PACKET_QWC		(4 + 4)
#define NO_LIGHT_MATRIX_PACKET_QWC					(4 + 4)
#define NO_LIGHT_PLUS_ENV_MAP_MATRIX_PACKET_QWC	(7 + 4)	// includes a normal transform matrix for environment mapping
#define FLUSHE_PACKET_QWC								(1)

/* size of pre-built vertex weighting dma packet, in quad words */

//#define VERTEX_WEIGHTING_PACKET_SIZE				(2 + (3 * 4) + (3 * 3))	// 2 quad-word DMA header plus space for 3 vertex and 3 lighting matrices
//yuch-- for now, just 4 vertex transforms
#define VERTEX_WEIGHTING_PACKET_QWC					(2 + (4 * 4))	// 2 quad-word DMA header plus space for 4 vertex matrices

/* size of pre-build override color packet, in quad words */

#define OVERRIDE_COLOR_PACKET_QWC					(2 + 1)			// 2 quad-word DMA header plus 1 quad word for color

/* size of pre-built vif interrupt packet, in quad words */

#define VIF_INTERRUPT_PACKET_QWC						1

/* size of pre-built send-texture-via-path2 packet, in quad words */

#define TEXTURE_PATH2_PACKET_QWC						3

/* size of pre-built flushA packet in quadwords */

#define FLUSHA_PACKET_QWC		  						1

/* size of pre-built send-texture-via-path3 packet, in quad words */

#define TEXTURE_PATH3_PACKET_QWC						8

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

#if defined (WIN32) || defined(GCN)

// generic style scratchpad declaration (no scratchpad).
#define SCRATCHPAD(type, variable)	type variable

#elif ( defined(PS2) && defined(SN) )

// sn systems style scratchpad declaration.
#define SCRATCHPAD(type, variable)	type variable	__attribute__((section(".spad")))

#else

// codewarrior style scratchpad declaration.
#define SCRATCHPAD(type, variable)	type __declspec(scratchpad) variable

#endif

extern SCRATCHPAD(Graphics4x4,			g4Identity);
extern SCRATCHPAD(Graphics4x4,			g4Temp);
extern SCRATCHPAD(Graphics4x4,			g4WorldToCamera);
extern SCRATCHPAD(Graphics4x4,			g4CameraToWorld);
extern SCRATCHPAD(Graphics4x4,			g4LocalToCamera);
extern SCRATCHPAD(Graphics4x4,			g4BoneTransform);
extern DirCos3x3	g3YRotate90;	// HEY! DirCos3x3 is NOT aligned correctly, so it can't go in the scratchpad where alignment is important.
extern SCRATCHPAD(Vector3,					WorkVector[WORK_VECTOR_COUNT]);
extern SCRATCHPAD(Vector3,					vZero);
extern SCRATCHPAD(Vector3,					vUp);
extern SCRATCHPAD(ts_BoundingBox,		GeneralBoundingBox);
extern SCRATCHPAD(ts_BoundingSphere,	GeneralBoundingSphere);

#ifdef PS2

/* pre-built DMA packets */

extern SCRATCHPAD(u32,	u32ViewportPacket[]);
extern SCRATCHPAD(u32,	u32LightPacket[]);
extern SCRATCHPAD(u32,	u32DirectionalLightMatrixPacket[]);
extern SCRATCHPAD(u32,	u32AmbientLightMatrixPacket[]);
extern SCRATCHPAD(u32,	u32FullAmbientLightMatrixPacket[]);
extern SCRATCHPAD(u32,	u32NoLightMatrixPacket[]);
extern SCRATCHPAD(u32,	u32NoLightPlusEnvMapMatrixPacket[]);
extern SCRATCHPAD(u32,	u32VertexWeightingPacket[]);
extern SCRATCHPAD(u32,	u32OverrideColorPacket[]);
extern SCRATCHPAD(u32,	u32VIFInterruptPacket[]);
extern SCRATCHPAD(u32,	u32GSState1Packet[]);
extern SCRATCHPAD(u32,	u32GSState2Packet[]);
extern SCRATCHPAD(u8,	u8TexturePath2Packet[]);
extern SCRATCHPAD(u8,	u8FlushAPacket[]);
extern SCRATCHPAD(u8,	u8ParticleInitPacket[]);
extern SCRATCHPAD(u8,	u8ParticleDataPacket[]);
extern SCRATCHPAD(u8,	u8SpriteInitPacket[]);
extern SCRATCHPAD(u8,	u8SpriteDataPacket[]);
extern SCRATCHPAD(u8,	u8FlushePacket[]);
extern SCRATCHPAD(u8,	u8TexturePath3Packet[]);

#endif // PS2

#ifdef GCN
extern u32 gxLightIDs;
#endif

/******************* global prototypes *******************/

extern void FastVars__Init(void);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __G_FASTVARS_H
