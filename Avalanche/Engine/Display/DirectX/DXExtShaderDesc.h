#ifndef __DX_EXT_SHADER_DESC_H
#define __DX_EXT_SHADER_DESC_H

typedef struct _shaderDescEntry
{
	DWORD *		pData;
	DWORD			lenData;
} shaderDescEntry;

typedef struct _extShaderDesc
{
	shaderDescEntry	Decl;
	shaderDescEntry	Defn;
	shaderDescEntry	Code;
} extShaderDesc;

#endif
