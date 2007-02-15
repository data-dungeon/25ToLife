//============================================================================
//=
//= PatchDisp_DX.cpp - DirectX specific functions for rendering
//=
//============================================================================

#include "Display/DisplayPCH.h"

#include "Display/DirectX/DXPublished.h"
#include "Display/DirectX/PatchHelper.h"
#include "Layers/Timing.h"


#ifdef MASSIVE_AD
#include "Massive\MassiveClientDriver.h"
#include "Massive\MassiveAdObject.h"
#include "camera\Director.h"
#endif

#ifdef MASSIVE_DEBUG
//These are needed for console debug output
//for Massive's QA
#include "Game/GamePCH.h"
#include "cmdcon/cmdcon.h"
#endif

extern D3DXMATRIX _MatrixStack_Identity;

void PatchDisp__Cleanup( )
{

}
void PatchSOM__RenderDX( ts_PatchSOM** pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToWorld, ts_DisplayListRecord *pDisplayListRecord, ts_PatchSOM__RenderFlags ucFlags, ts_Body *pBody )
{
	ts_PatchSOM* pPatchSOM = pPatchSOMList[ 0 ];

//	if( pPatchSOM->dh.u16Flags & SOMM_HAS_BONES )
//		return;

#ifdef STICK_FIGURES
	/* draw boned models as stick figures for AttachMgr analysis */
	if( RenderMgr__BoneRender( )  &&  (pPatchSOM->dh.u16Flags & SOMM_HAS_BONES) )
	{
		if (PatchSOM__DrawStickFigure( pPatchSOM, pBody) )	// returns true if stick figure was drawn
			return;
	}
#endif //STICK_FIGURES

	/* rendering shadows? currently, we only support tri strips */
	if( RenderMgr__RenderingProjectedTextures( ))
	{
//		if (pPatchSOM->pTriStripBuffer)
//			PatchSOM__RenderTriStripsShadows(pPatchSOM, pmLocalToCamera, pBody);
		return;
	}

	if( !pDisplayListRecord->pDisplayList )
	{
		pDisplayListRecord = &pPatchSOM->DisplayListRecord;
		ASSERT( pDisplayListRecord->u16QuadWordCount == 0 ); // ya.. its a hack.. ick...0 means that it's been fixed up by _FixDMASOM
	}

	DXMeshInterface * pMeshInterface = (DXMeshInterface *) pDisplayListRecord->pDisplayList;

	/* built-in profiling */
	Profiler__Start( PROFILE_RENDER_IMMEDIATE );

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Push( );

	/* lighting? */
	if ( pRenderInst->DynamicallyLit() && ( pPatchSOM->dh.u16Flags & SOMM_TRI_STRIP_NORMALS ) && RenderMgr__LightingOn() )
	{
		/* lights have already been transformed into camera space, so load identity matrix so they don't get */
		/* transformed any further */
		MatrixStack__Identity( );
		RenderState__Request( RS_LIGHTING, RS_TRUE );
		RenderState__Request( RS_SEND_LIGHTS, RS_TRUE );
		RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 1.0f ) );
	}
	else																
	{
		/* disable lighting */
		RenderState__Request( RS_LIGHTING, RS_FALSE );
		RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 1.0f ) );
	}

	if( pPatchSOM->dh.u16Flags & SOMM_HAS_BONES ) //motion data?
	{
		ASSERT( pBody->usBones <= MAX_OBJECT_BONES );

		for( unsigned int i = 0; i < pBody->usBones; i++)
		{
//			Matrix4x4 * pmProj = MatrixStack__Get( MS_PROJECTION );
//			Matrix4x4 Final = LocalToView * (*pmProj);

			ts_Bone * pBone = &pBody->pBones[i];
//			BoneLocal2CamMatrix[i].FastMultiply( pBone->Matrix, Final );
			memcpy( BoneLocal2CamMatrix + i, & pBone->Matrix, sizeof( Matrix4x4 ) );
		}
	}

	DirectX_Device->SetTransform( D3DTS_WORLD, (D3DMATRIX *) pmLocalToWorld );
	MatrixStack__Load( & g4WorldToCamera );

	/* render Mesh here */
	RenderState__Begin( );

	for ( int meshIndex = 0 ; meshIndex < numSOMs ; meshIndex++ )
	{
		DXMeshInterface* pMeshInterface = (DXMeshInterface *) pDisplayListRecord[ meshIndex ].pDisplayList;

		if( pPatchSOM->dh.u16Flags & SOMM_HAS_BONES )
			pMeshInterface->setMatrixArray( (D3DXMATRIX *) BoneLocal2CamMatrix, pBody->usBones );

			pMeshInterface->renderDefault( );
	}

	DirectX_Device->SetTransform( D3DTS_WORLD, & _MatrixStack_Identity );

	/* done with render */
	RenderState__End( );
	MatrixStack__Pop( );

	/* stats */
	if( pBody )
		SceneMgr::GetStats().IncBonesRendered( pBody->usBones );

	/* built-in profiling */
	Profiler__Stop( PROFILE_RENDER_IMMEDIATE);


#ifdef MASSIVE_AD

	// Handle Impressions here ts_PatchSOM contrains the bb and vertex list, so all you need to bring in is the eye point
	for ( int meshIndex = 0 ; meshIndex < numSOMs ; meshIndex++ )
	{
		DXMeshInterface* pMeshInterface = (DXMeshInterface *) pDisplayListRecord[ meshIndex ].pDisplayList;
		
		if(pMeshInterface->m_MassiveIE)
		{
			/////////////
			//Angle
			/////////////
		
			//Get Objects Direction in the World
			//TODO:Need to go over with artist to decide if positive Z or X will be facing
			//right now it is set for positive Z
			Vector3 normal;
			normal = pmLocalToWorld->GetRow2();
			
			//Get Object's Position in World
			Vector3 vWorldPosition = pmLocalToWorld->GetTranslation();
			
			//Get the Camera Position in World
			Vector3 vCameraPosition = SceneMgr::GetCurrentScene()->d_cameraWorldPos;

			//Get the ray from the camera to the object and normalize it
			Vector3 rayToIE = vCameraPosition - vWorldPosition;
			rayToIE = rayToIE.Normalized();

			//Get the angle(dot product) between the ray and the objects direction
			float angle = rayToIE.Dot(normal);

			//////////////
			//Size
			/////////////
			Matrix4x4 localToWorld = *pmLocalToWorld;
			CScene *currentScene = SceneMgr::GetCurrentScene();

			//Get the Screen Height and Width
			unsigned int screenWidth = (unsigned int)currentScene->d_sceneView.GetViewport()->fWidth;
			unsigned int screenHeight = (unsigned int)currentScene->d_sceneView.GetViewport()->fHeight;

			//Get the Onscreen Coords for the BB
			Vector2 maxScreen; //Place holder for max value of the BB in screen coord
			Vector2 minScreen; //Place holder for min value of the BB in screen coord
			currentScene->GetScreenCoords (pPatchSOM->BoundingBox.Max.Transform3(localToWorld),maxScreen);
			currentScene->GetScreenCoords (pPatchSOM->BoundingBox.Min.Transform3(localToWorld),minScreen);

			//Get the Diagonal size of the BB on screen
			Vector2 diagVector = maxScreen - minScreen;
			unsigned int size = (unsigned int)diagVector.Length();
			

			//Report the Angle and Size
			MassiveObjectReportImpressions(pMeshInterface->m_MassiveIE, size, angle, screenHeight, screenWidth);
		}
	}
#endif 

}

StandardTCMaterial ** PatchSOM__CreateMeshMaterials( DXMeshInterface & MeshInterface, ts_RecordList *pTextureContextList, ts_RecordList * pWorldTextureContextList, unsigned int & numMaterials )
{
	unsigned int numShaderRecords = 0;

	const ShaderRecord *	pShaderRecords = MeshInterface.getShaderRecords( numShaderRecords );
	ASSERT( numShaderRecords );

	StandardTCMaterial ** pNewMaterials = new StandardTCMaterial * [numShaderRecords];

	for( unsigned int i = 0; i < numShaderRecords; i++ )
	{
		ts_TextureContext *			pTC;

		if( pShaderRecords[i].d_TextureContextID & 0xff000000 )
			pTC = (ts_TextureContext *) GetRecordAddress( pWorldTextureContextList, pShaderRecords[i].d_TextureContextID );
		else
			pTC = (ts_TextureContext *) GetRecordAddress( pTextureContextList, pShaderRecords[i].d_TextureContextID );

		pNewMaterials[i] = new StandardTCMaterial( MeshInterface, pShaderRecords[i], pTC );
	}

	numMaterials = numShaderRecords;

	return pNewMaterials;
}

void * PatchSOM__BuildMeshData( void * pMeshDataBlock, ts_RecordList * pTextureContextList, ts_RecordList * pWorldTextureContextList, unsigned int & numMeshes )
{

	numMeshes = *((unsigned int *) pMeshDataBlock);
	ASSERT( numMeshes > 0 );

	unsigned int * pMeshOffsets = ((unsigned int *) pMeshDataBlock) + 1;

	DXMeshInterface ** ppMeshes = (DXMeshInterface **) pMeshOffsets;

	for( unsigned int idx = 0; idx < numMeshes; idx++ )
	{
		unsigned int currMeshOffset = pMeshOffsets[idx];
		unsigned int numMaterials = 0;

		void * pMeshData = (void *) ((unsigned int) pMeshOffsets + currMeshOffset);

		ppMeshes[idx] = DXPublished::createMeshInterface( pMeshData );
		ASSERT( ppMeshes[idx] );

		StandardTCMaterial ** ppMaterials = PatchSOM__CreateMeshMaterials( * ppMeshes[idx], pTextureContextList, pWorldTextureContextList, numMaterials );
		ASSERT( ppMaterials );


#ifdef MASSIVE_AD

		// Since this seems to be where we materials and mated to their meshes this is where we'll filter textures for Massive Name Tags 
		// and if found create Massive Inventory Elements
		for( unsigned int jdx = 0; jdx < numMaterials; jdx++)
		{
			ts_TextureContext *p_TextureContext = ppMaterials[jdx]->getTextureContext();
	
			if(p_TextureContext->TextureLayer[0].TextureName)
			{
				if(!strncmp("massive_ie_",p_TextureContext->TextureLayer[0].TextureName,11))
				{
					ppMeshes[idx]->m_MassiveIE = MassiveObjectInit(p_TextureContext->TextureLayer[0].TextureName,jdx,eTEXTURE_IE,p_TextureContext);
				}
			}
		}
#endif

		ppMeshes[idx]->setDefaultMaterials( (DXMaterialBase **) ppMaterials, numMaterials );

		
	}

	return ppMeshes;
}

void PatchSOM__ReleaseMeshData( void * pMeshDataBlock, unsigned int numMeshes)
{
	ASSERT_PTR( pMeshDataBlock );

	DXMeshInterface ** ppMeshes = (DXMeshInterface ** ) pMeshDataBlock;

	for( unsigned int i = 0; i < numMeshes; i++ )
	{
		ASSERT_PTR( ppMeshes[i] );

		unsigned int numMaterials = 0;
		StandardTCMaterial ** pDefaultMaterials = (StandardTCMaterial **) ppMeshes[i]->getDefaultMaterials( numMaterials );

		if( pDefaultMaterials )
		{
			for( unsigned int m = 0; m < numMaterials; m++ )
				if( pDefaultMaterials[m] )
				{
					delete pDefaultMaterials[m];
					pDefaultMaterials[m] = 0;
				}

			delete [] pDefaultMaterials;
		}

		delete ppMeshes[i];
		ppMeshes[i] = 0;
	}

}

void * PatchSOM__GetIndexedMesh( void * pMeshInterfaceBlock, unsigned int idx )
{
	return ((DXMeshInterface **) pMeshInterfaceBlock)[idx];
}

void PatchSOM__ReleaseIndexedMesh( void * pMeshInterface )
{
	
}

#ifdef _XBOX
#include "Display/DirectX/XBResourceManager.h"
#include "Display/DirectX/DXMeshBase_Xbox.h"
XBResourceManager	m_Resources;

const char * PatchSOM__GetXGDFilename( const char * pDBLFilename )
{
static char					XGDFilename[1024];

	assert( strlen( pDBLFilename) < 1020 );

	strcpy( XGDFilename, "d:\\data_xbox\\" );
	strcat( XGDFilename, pDBLFilename );

	for( unsigned int i = 0; i < strlen( XGDFilename ); i++ )
		if( XGDFilename[i] == '/' )
			XGDFilename[i] = '\\';

	char * pXGDFileEnd = XGDFilename + strlen( XGDFilename );

	while( (pXGDFileEnd != XGDFilename)  &&  *pXGDFileEnd != '.' )
		pXGDFileEnd--;

	if( pXGDFileEnd == XGDFilename )
		pXGDFileEnd = XGDFilename + strlen( XGDFilename );

	if( stricmp( pXGDFileEnd, ".dbl" ) != 0 )
		return 0;

	strcpy( pXGDFileEnd, ".xvd" );

	return XGDFilename;
}

void PatchSOM__CheckHWDataDXD( CDXD_Data* Dx )
{
	if (Dx == NULL)
		return;

	const char * pXGDFile = PatchSOM__GetXGDFilename( Dx->GetDBLName() );
	
	if( pXGDFile == 0 )
		return;

	CXBPackedResource * pResources = m_Resources.addResourceFileDXD( Dx );

	DXMeshBase::setActiveResources( pResources );

}

void PatchSOM__ReleaseHWDataDXD( CDXD_Data* Dx )
{
	if (Dx == NULL)
		return;

	const char * pXGDFile = PatchSOM__GetXGDFilename( Dx->GetDBLName() );
	
	if( pXGDFile == 0 )
		return;

	m_Resources.destroyResourceFileDXD( Dx );
}


void PatchSOM__ReleaseAllHWData( void )
{
	m_Resources.destroyResourceFiles( );
}
#endif


#ifdef MASSIVE_AD

DoublyLinkedList<MassiveIE>	m_MassiveIE;

/******************************************************************************
-- MassiveObjectInit(Clump)
-- Sets Massive Inventory Element Up with relevant game info
******************************************************************************/

MassiveIE *MassiveObjectInit(const char *IEName,unsigned int textureNum,MAO_IETYPE ieType,ts_TextureContext *p_TextureContext)
{
	MassiveIE *mao = (MassiveIE *)memAlloc(sizeof(MassiveIE));


	mao->m_sIEName = (char *)memAlloc(strlen(IEName)-11);
	strcpy(mao->m_sIEName,IEName+11);
	mao->textureNum = textureNum;
	mao->ieType = ieType;
	mao->m_eStatus = ESTAT_NO_IE;		
	mao->m_hInvElem = 0;
	mao->m_hCrEx = 0;
	mao->p_TextureContext = p_TextureContext;

	m_MassiveIE.AddTail(mao);
	//Attach mao to Link List so we can update downloading for higher up

	return mao;
}

/******************************************************************************
-- MassiveObjectReportImpressions()
-- Records Impression data from player viewing of IE's in Game World
******************************************************************************/

bool MassiveObjectReportImpressions(MassiveIE *mao, unsigned int size, float angle, unsigned int screenHeight, unsigned int screenWidth)
{

	
	MMT_ERROR err = MMT_GENERROR;
	MMT_Impression imp_data;
	MMT_STATUS stat = MMT_STATUS_INVALID;

	//Create Impression Data
	imp_data.inView = 1; //If this function gets hit its being rendered, so its in view
	imp_data.imageImpression.angle = angle;
	imp_data.imageImpression.imageSize = size;
	imp_data.imageImpression.screenHeight = screenHeight;
	imp_data.imageImpression.screenWidth = screenWidth;

	err = MMT_UpdateInventoryElement(mao->m_hInvElem, mao->m_hCrEx, &imp_data);

#ifdef MASSIVE_DEBUG
	//By enabling this, you are going to get a message EVERY FRAME
	g_console.Echo("Angle:%0.2f Size:%d \n",angle,size);
#endif

	// Only print out a change when the return erro changes
	if (err != mao->m_eLastError) 
	{
		mao->m_eLastError = err;
		if (MMT_SUCCESS != mao->m_eLastError) 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_UpdateTarget: ERROR. MMT_UpdateTarget execution failing...\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
			return false;
		}
	}

	//Get the status of the IE
	MMT_GetStatus(mao->m_hInvElem,&stat);

	// The return status indicates it's time to rotate the ad in the Inventory Element.
	if (MMT_STATUS_ROTATE_INVENTORY_ELEMENT == stat) 
	{
		#ifdef MASSIVE_DEBUG
			// Set the status so that the next Creative Execution will be requested.
			g_console.Echo("MMT_UpdateTarget: Rotate ad.\n");
			g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
		#endif
		mao->m_eStatus = ESTAT_IE_RETRIEVED;
	}
	return true;
}

/******************************************************************************
-- MassiveObjectRequestIEInfo()
-- Query Server if Inventory Element Exists
******************************************************************************/

bool MassiveObjectRequestIEInfo(MassiveIE *mao)
{
	MassiveMediaHandle InvElem;
	MMT_ERROR err = MMT_GENERROR;
	MMT_STATUS stat = MMT_STATUS_INVALID;
	if (ESTAT_NO_IE == mao->m_eStatus)
	{
		err = MMT_GetInventoryElementHandle(mao->m_sIEName, &InvElem);  //Temp name for Sample Game
		if (MMT_SUCCESS != err) 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetTargetHandle: ERROR. Request for Inventory Element information failed.\n");
				g_console.Echo("[IE Name = %s]\n", mao->m_sIEName);
			#endif
			mao->m_eStatus = ESTAT_IE_FAILED;
			mao->m_hInvElem = MMT_INVALID_HANDLE;
		}
		else
		{
			mao->m_eStatus = ESTAT_RETRIEVE_IE;
			mao->m_hInvElem = InvElem;
			err = MMT_GetStatus(mao->m_hInvElem ,&stat);
			return true;
		}

	}
	return false;
}

/******************************************************************************
-- MassiveObjectProcessIEInfoRequest()
-- Get Results of RpMassiveObjectRequestIEInfo() for Inventory Element I.E. Make sure its valid on the Server Side so we can dish it data
******************************************************************************/

bool MassiveObjectProcessIEInfoRequest(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;
	MMT_STATUS stat = MMT_STATUS_INVALID;


	if (ESTAT_RETRIEVE_IE == mao->m_eStatus) 
	{
		err = MMT_GetStatus(mao->m_hInvElem, &stat);
		if (MMT_SUCCESS == err) 
		{
			if (MMT_STATUS_OBJECT_READY == stat) 
			{
				mao->m_eStatus = ESTAT_IE_RETRIEVED;
				return true;
			}
			else if (MMT_STATUS_OBJECT_PENDING != stat) 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("MMT_GetTargetHandle: STATUS. Request for Inventory Element did not return valid data.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem);
				#endif
				mao->m_eStatus = ESTAT_IE_FAILED;
				mao->m_hInvElem = MMT_INVALID_HANDLE;
			}
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetStatus: ERROR. Failed while getting status of a Inventory Element handle.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem);
			#endif
			mao->m_eStatus = ESTAT_IE_FAILED;
			mao->m_hInvElem = MMT_INVALID_HANDLE;
		}
	}
	return false;
}

/******************************************************************************
-- MassiveObjectRequestCrExInfo()
-- Request next CrEx for Ad Object
******************************************************************************/

bool MassiveObjectRequestCrExInfo(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;
	MassiveMediaHandle m_hCrEx = MMT_INVALID_HANDLE;



	if (ESTAT_IE_RETRIEVED == mao->m_eStatus)
	{
		// The first request for a crex is fulfilled with MMT_GetCurrentTargetMedia, all subsequent calls use MMT_GetNextTargetMedia
		if (MMT_INVALID_HANDLE == mao->m_hCrEx)
		{
			err = MMT_GetCurrentCrexForInventoryElement(mao->m_hInvElem, &m_hCrEx);
			mao->m_hCrEx = m_hCrEx;
		}
		else
		{
			err = MMT_GetNextCrexForInventoryElement(mao->m_hInvElem, &m_hCrEx);
			mao->m_hCrEx = m_hCrEx;
		}

		if (MMT_SUCCESS == err) 
		{
			mao->m_eStatus = ESTAT_RETRIEVE_CREX;
			return true;
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetTargetMedia: ERROR. Request for Creative Execution information failed.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem);
			#endif
			mao->m_eStatus = ESTAT_CREX_FAILED;
			mao->m_hCrEx = MMT_INVALID_HANDLE;
		}
	}
	return false;
}

/******************************************************************************
-- RpMassiveObjectProcessCrExInfoRequest()
-- Process results of RpMassiveObjectRequestCrExInfo() see if CrEx is valid and action needs to take place
******************************************************************************/

bool MassiveObjectProcessCrExInfoRequest(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;
	MMT_STATUS stat = MMT_STATUS_INVALID;

	if (ESTAT_RETRIEVE_CREX == mao->m_eStatus) 
	{
		err = MMT_GetStatus(mao->m_hCrEx, &stat);
		if (MMT_SUCCESS == err) 
		{
			if (MMT_STATUS_OBJECT_READY == stat) 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("MMT_GetTargetMedia: Request for Creative Execution successfully retrieved data.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
				#endif
				mao->m_eStatus = ESTAT_CREX_RETREIVED;
				return true;
			}
			else if (MMT_STATUS_OBJECT_PENDING != stat) 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("MMT_GetTargetHeaders: STATUS. Request for Creative Execution did not return valid data.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
				#endif
				mao->m_eStatus = ESTAT_CREX_FAILED;
				mao->m_hCrEx = MMT_INVALID_HANDLE;
			}
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetStatus: ERROR. Failed while getting status of a Creative Execution handle.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
			mao->m_eStatus = ESTAT_CREX_FAILED;
			mao->m_hInvElem = MMT_INVALID_HANDLE;
		}
	}
	return false;
}

/******************************************************************************
-- MassiveObjectRequestMediaData()
-- Request bin data for next Ad Media element
******************************************************************************/

bool MassiveObjectRequestMediaData(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;

	if (ESTAT_CREX_RETREIVED == mao->m_eStatus)
	{
		// The first request for a crex is fulfilled with MMT_GetCurrentTargetMedia, all subsequent calls use MMT_GetNextTargetMedia
		err = MMT_GetCrex(mao->m_hCrEx);

		if (MMT_SUCCESS == err) 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_RequestMediaData: Request for Ad Media information succeeded.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
			mao->m_eStatus = ESTAT_DOWNLOAD_MEDIA;

		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_RequestMediaData: ERROR. Request for Ad Media information failed.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
			mao->m_eStatus = ESTAT_MEDIA_FAILED;
			mao->m_hCrEx = MMT_INVALID_HANDLE;
		}
	}
	return false;
}

/******************************************************************************
-- MassiveObjectProcessAdMediaDataRequest()
-- Download and prepare Ad media Bin Data requested by MassiveObjectRequestMediaData()  
******************************************************************************/

bool MassiveObjectProcessAdMediaDataRequest(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;
	MMT_STATUS stat = MMT_STATUS_INVALID;

	if (ESTAT_DOWNLOAD_MEDIA == mao->m_eStatus)
	{
		err = MMT_GetStatus(mao->m_hCrEx, &stat);
		if (MMT_SUCCESS == err) 
		{
			if (MMT_STATUS_OBJECT_READY == stat) 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("MMT_RequestMediaData: Request for Ad Media successfully retrieved data.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
				#endif
				mao->m_eStatus = ESTAT_MEDIA_DOWNLOADED;
				return true;
			}
			else if (MMT_STATUS_OBJECT_PENDING != stat) 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("MMT_RequestMediaData: STATUS. Request for Ad Media did not return valid data.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
				#endif
				mao->m_eStatus = ESTAT_MEDIA_FAILED;
				mao->m_hCrEx = MMT_INVALID_HANDLE;
			}
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetStatus: ERROR. Failed while getting status of a Creative Execution handle.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
			mao->m_eStatus = ESTAT_MEDIA_FAILED;
			mao->m_hInvElem = MMT_INVALID_HANDLE;
		}
	}
	return false;
}

/******************************************************************************
-- MassiveObjectCommitAdMediaToGame()
-- Places new Media Object into Game 
******************************************************************************/

bool MassiveObjectCommitAdMediaToGame(MassiveIE *mao)
{
	MMT_ERROR err = MMT_GENERROR;
	char *pData = NULL;
	int	nDataLen = 0;

	err = MMT_GetBinaryData(mao->m_hCrEx, (void **)&pData, &nDataLen);
	if (MMT_SUCCESS == err) 
	{
		#ifdef MASSIVE_DEBUG
			g_console.Echo("MMT_GetAdvertData:  Retrieval for Ad Media Data successful.\n");
			g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
		#endif
		mao->m_eStatus = ESTAT_REPORT_IMPRESSION;

		// Make a copy of the Ad Media Data to ad to the engine (we just write to disk)

		// Gather the components for a media file name
		char *sz_FileName = NULL;
		const char *sz_CrexName = MMT_GetName(mao->m_hCrEx);
		char *sz_FileExt = NULL;
		MMT_MEDIATYPE e_Type = MMT_GetType(mao->m_hCrEx);
		switch (e_Type) 
		{

		case MMT_MEDIA_IMAGE_JPG:
				sz_FileExt = ".jpg";
				break;
			case MMT_MEDIA_IMAGE_PNG:
				sz_FileExt = ".png";
				break;

		case MMT_MEDIA_IMAGE_BMP:
				sz_FileExt = ".bmp";
				break;

		case MMT_MEDIA_IMAGE_DXT1:
				sz_FileExt = ".dds";
				break;
			case MMT_MEDIA_IMAGE_DXT5:
				sz_FileExt = ".dds";
				break;
			case MMT_MEDIA_IMAGE_TGA:
				sz_FileExt = ".tga";
				break;
			case MMT_MEDIA_IMAGE_GIF:
				sz_FileExt = ".gif";
				break;
			default:
				sz_FileExt = ".bin";
				break;
		}
		// Writes file name to disk. //Should look at trying to stream the data chunk straight into the Texture Code-mat
		if (sz_CrexName) 
		{
			sz_FileName = (char *)malloc((strlen(sz_CrexName) + strlen(sz_FileExt) + 1) * sizeof(char));
			if (sz_FileName) 
			{
				// This is how we get our Texture into the game
				// First we create a new texture using the data we've just downloaded
				Texture *tex = new Texture(mao->p_TextureContext->TextureLayer->textureDatabase, sz_FileName);		
				Texture *originalTex = mao->p_TextureContext->TextureLayer->textureDatabase->GetTexture(mao->p_TextureContext->TextureLayer->s16TextureIndex);
				//Get the original Image so we can use its info to help create our new texture
				Image *originalImage = originalTex->GetImage();
				
				if(e_Type == MMT_MEDIA_IMAGE_TGA)
				{	
					//Image creation expects to pointed to actual image data not header, so we bump it 44 bytes down
					tex->AddPlatformImage(false, originalImage->GetFlags(),3 /*Media Type for a r8g8b8 texture*/, originalImage->GetWidth(), originalImage->GetHeight(), (void *)(pData+44));
				}
				else if(e_Type == MMT_MEDIA_IMAGE_BMP)
				{	
					//Image creation expects to pointed to actual image data not header, so we bump it 56 bytes down
					tex->AddPlatformImage(false, originalImage->GetFlags(),3 /*Media Type for a r8g8b8 texture*/, originalImage->GetWidth(), originalImage->GetHeight(), (void *)(pData+56));
				}
				else
					return false;
				//DXMeshInterface::setDefaultMaterials;
				//Need to see if we can safely delete the old texture
				//Then we append that Texture to the Object's TextureDatabase
				//and update the TextureLayer to point to the new Texture's ID
				//Need to understand why we cannot remove this texture
//				mao->p_TextureContext->TextureLayer->textureDatabase->RemoveTexture(originalTex);
				u16 TexID = mao->p_TextureContext->TextureLayer->textureDatabase->AppendTexture(tex);
				mao->p_TextureContext->TextureLayer->s16TextureIndex = TexID;

				return true;
			}
			else 
			{
				#ifdef MASSIVE_DEBUG
					g_console.Echo("ERROR: Unable allocate memory to assemble Ad Media file name.\n");
					g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
				#endif
			}
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("ERROR: Creative Execution has no name related to it.  Could not create file.\n");
				g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
			#endif
		}

	}
	else 
	{
		#ifdef MASSIVE_DEBUG
			g_console.Echo("MMT_GetAdvertData: ERROR. Retrieval for Ad Media Data failed.\n");
			g_console.Echo("[IE Name = %s; IE Handle = %d; CREX Handle = %d]\n", mao->m_sIEName, mao->m_hInvElem, mao->m_hCrEx);
		#endif
		mao->m_eStatus = ESTAT_MEDIA_FAILED;
		mao->m_hCrEx = MMT_INVALID_HANDLE;
	}
	return false;
}

/******************************************************************************
-- MassiveObjectFrameTick()
-- Called each frame to keep Inventory Objects updated with Server 
******************************************************************************/

bool MassiveObjectFrameTick()
{
	bool err = false;
	MassiveIE *currentMao = m_MassiveIE.Head();
	while(currentMao != NULL)
	{

		if(currentMao->m_eStatus != ESTAT_REPORT_IMPRESSION)
		{
			switch (currentMao->m_eStatus) 
			{
				case ESTAT_NO_IE:
					err = MassiveObjectRequestIEInfo(currentMao);
					break;
				case ESTAT_RETRIEVE_IE:
					err = MassiveObjectProcessIEInfoRequest(currentMao);
					break;
				case ESTAT_IE_RETRIEVED:
					err = MassiveObjectRequestCrExInfo(currentMao);
					break;
				case ESTAT_RETRIEVE_CREX:
					err = MassiveObjectProcessCrExInfoRequest(currentMao);
					break;
				case ESTAT_CREX_RETREIVED:
					err = MassiveObjectRequestMediaData(currentMao);
					break;
				case ESTAT_DOWNLOAD_MEDIA:
					err = MassiveObjectProcessAdMediaDataRequest(currentMao);
					break;
				case ESTAT_MEDIA_DOWNLOADED:
					err = MassiveObjectCommitAdMediaToGame(currentMao);
					break;
				case ESTAT_IE_FAILED:
					break;

			}
		}
		currentMao = currentMao->next;
	}
	return err;
}
#endif