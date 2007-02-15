// KB  5/1/00  \Ken85\Plugin\Shaders\Anime\Evaluate.cpp

#include "stdafx.h"
#include "Attr.h"
#include "TexInfo.h"
#include "TexInEq.h"  // must be included whenever TexInfo.h is included for plugins
#include "AGPShader.h"

const char *AGPShader::GetShaderDiffuseFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_SHADERMENUFILTER);
   return (LPCTSTR)filter;
}

const char *AGPShader::GetShaderSpecularFilters()
{
   return NULL;
}

void AGPShader::GetDiffuse( Shading &shading )
{
}

BOOL AGPShader::GetSpecular( Shading &shading, Vector &lightcolor )
{
   return FALSE;
}
