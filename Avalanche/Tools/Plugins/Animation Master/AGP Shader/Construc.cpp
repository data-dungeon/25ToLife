// KB  5/8/00  \Ken85\Plugin\Shaders\Gradient\Construc.cpp

#include "stdafx.h"
#include "Shader.h"
#include "AGPShader.h"
#include <math.h>
#include <stdio.h>

extern CShaderApp theApp;

extern "C" __declspec(dllexport) Shader *CreateShader()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return new AGPShader;
}

extern "C" __declspec(dllexport) int GetDLLVersion()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return SHADERDLLVERSION;
}

AGPShader::AGPShader()
{
	m_collision = 0;
	m_project = 0;
	m_terrain = 0;
	m_invisible = 0;
	m_tessadj = 0;
	m_pretess=0;
	m_layer=0;
	m_tessadj=0;
	strcpy(m_collisiontext,"<NONE>");
	strcpy(m_terraintext,"<NONE>");
	strcpy(m_environmappath,"<NONE>");
	strcpy(m_projecttext,"<NONE>");
	m_environmentmappercent=100;
	m_flat = FALSE;
	m_dnclip = FALSE;
	m_dnfog = FALSE;
	m_twosided = FALSE;
	m_zbias = FALSE;
	m_lod = FALSE;
	m_environmap = FALSE;
	m_deletepatch = FALSE;
	m_private = FALSE;
	m_alpha=0;
	m_32bitvc=0;
	m_coloroverride=0;
}

AGPShader::~AGPShader()
{
}


