#ifndef __AGPShader_H
#define __AGPShader_H

typedef struct bflags {
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

typedef struct {
	BitFlag	bfFlags;
	float siPretess;
	unsigned short siEnvironMapPercent;
	char strEnvironMapPath[_MAX_PATH];
} AGPShaderExport;

typedef struct {
	char strProjectName[64];
	char strTerrainType[64];
} AGPShaderExport_2;

typedef struct {
	AGPShaderExport	Shader1;
	AGPShaderExport_2 Shader2;
} FullAGPShaderExport;

class AGPShader
{
public:
	AGPShader()
	{
		memset(&AGPStruct.Shader1.bfFlags,0,sizeof(bflags));
		AGPStruct.Shader1.siPretess = 0;
		AGPStruct.Shader1.siEnvironMapPercent = 0;
		strcpy(AGPStruct.Shader1.strEnvironMapPath,"\0");
	
		strcpy(AGPStruct.Shader2.strProjectName,"\0");
		strcpy(AGPStruct.Shader2.strTerrainType,"Default\0");
	}
	~AGPShader(){}

	void		SetCollision(unsigned short value) { AGPStruct.Shader1.bfFlags.collision = value; };
	void		SetTerrain(unsigned short value) { AGPStruct.Shader1.bfFlags.terrain = 0; };//this is no longer needed
	void		SetLayer(unsigned short value) { AGPStruct.Shader1.bfFlags.layer = value; };
	void		SetTessAdjustment(unsigned short value) { AGPStruct.Shader1.bfFlags.tessadj = value; };
	void		SetFlatShadedOn(bool flag) { AGPStruct.Shader1.bfFlags.flat = flag; };
	void		SetNoClippingOn(bool flag) { AGPStruct.Shader1.bfFlags.dnclip = flag; };
	void		SetNoFogOn(bool flag) { AGPStruct.Shader1.bfFlags.dnfog = flag; };
	void		SetTwoSidedOn(const bool flag) { AGPStruct.Shader1.bfFlags.twosided = flag; };
	void		SetLODVisInternalsOn(bool flag) { AGPStruct.Shader1.bfFlags.lod = flag; };
	void		SetEnvironMapOn(bool flag) { AGPStruct.Shader1.bfFlags.environmap = flag; };
	void		SetDeletePatchOn(bool flag) { AGPStruct.Shader1.bfFlags.deletepatch = flag; };
	void		SetInvisibleOn(bool flag) { AGPStruct.Shader1.bfFlags.invisible = flag; };
	void		SetMapPrivateOn(bool flag) { AGPStruct.Shader1.bfFlags.mapprivate = flag; };
	void		SetAlphaOn(bool flag) { AGPStruct.Shader1.bfFlags.alpha = flag; };
	void		SetVC32bitOn(bool flag) { AGPStruct.Shader1.bfFlags.vc32bit = flag; };
	void		SetOverrideColorOn(bool flag) { AGPStruct.Shader1.bfFlags.overridecolor = flag; };
	void		SetPreTess(float value) { AGPStruct.Shader1.siPretess = value; };
	void		SetEnvironMapPercent(unsigned short value) { AGPStruct.Shader1.siEnvironMapPercent = value; };
	void		SetProjectName(const char *name) { strcpy(AGPStruct.Shader2.strProjectName,name); };
	void		SetTerrainType(const char *type) { strcpy(AGPStruct.Shader2.strTerrainType,type); };
	void		SetEnvironmentalPath(const char *environmappath) { strcpy(AGPStruct.Shader1.strEnvironMapPath,environmappath); };
	void		SetZBias(bool flag) { AGPStruct.Shader1.bfFlags.zbias = flag; };

	unsigned short	GetCollision() { return AGPStruct.Shader1.bfFlags.collision; };
	unsigned short	GetTerrain() { return AGPStruct.Shader1.bfFlags.terrain; };//this is no longer needed
	unsigned short	GetLayer() { return AGPStruct.Shader1.bfFlags.layer; };
	unsigned short	GetTessAdjustment() { return AGPStruct.Shader1.bfFlags.tessadj; };
	bool			GetFlatShaded() { return AGPStruct.Shader1.bfFlags.flat; };
	bool			GetNoClipping() { return AGPStruct.Shader1.bfFlags.dnclip; };
	bool			GetNoFog() { return AGPStruct.Shader1.bfFlags.dnfog; };
	bool			GetTwoSided() { return AGPStruct.Shader1.bfFlags.twosided; };
	bool			GetLODVisInternals() { return AGPStruct.Shader1.bfFlags.lod; };
	bool			GetEnvironMap() { return AGPStruct.Shader1.bfFlags.environmap; };
	bool			GetDeletePatch() { return AGPStruct.Shader1.bfFlags.deletepatch; };
	bool			GetInvisible() { return AGPStruct.Shader1.bfFlags.invisible; };
	bool			GetMapPrivate() { return AGPStruct.Shader1.bfFlags.mapprivate; };
	bool			GetAlpha() { return AGPStruct.Shader1.bfFlags.alpha; };
	bool			GetVC32bit() { return AGPStruct.Shader1.bfFlags.vc32bit; };
	bool			GetOverrideColor() { return AGPStruct.Shader1.bfFlags.overridecolor; };
	float			GetPreTess() { return AGPStruct.Shader1.siPretess; };
	unsigned short	GetEnvironMapPercent() { return AGPStruct.Shader1.siEnvironMapPercent; };
	const char		*GetProjectName() { return AGPStruct.Shader2.strProjectName; };
	const char		*GetTerrainType() { return AGPStruct.Shader2.strTerrainType; };
	const char		*GetEnvironmentalPath() { return AGPStruct.Shader1.strEnvironMapPath; };
	bool			GetZBias() { return AGPStruct.Shader1.bfFlags.zbias; };


	bool operator == ( const AGPShader &other ) const
	{
		if (memcmp(&AGPStruct,&other.AGPStruct,sizeof(FullAGPShaderExport)) == 0)
			return true;
		return false;	
	};

	bool operator != ( const AGPShader &other ) const
	{
			return !(*this == other);
	};
	
public:
	FullAGPShaderExport AGPStruct;
};

#endif