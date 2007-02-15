#include "mffTools.h"

#pragma warning (disable : 4786)

#define BL_POLYMODEL "[POLYMODEL]"
#define EL_POLYMODEL "[END_POLYMODEL]"

#define BL_REFERENCES "[REFERENCES]"
#define EL_REFERENCES "[END_REFERENCES]"

#define BL_SCENE "[SCENE]"
#define EL_SCENE "[END_SCENE]"

#define BL_POLYMODELINSTANCE "[POLYMODELINSTANCE]"
#define EL_POLYMODELINSTANCE "[END_POLYMODELINSTANCE]"

#define BL_VERTEXTABLE "[VERTEXTABLE]"
#define EL_VERTEXTABLE "[END_VERTEXTABLE]"

#define BL_NORMALTABLE "[NORMALTABLE]"
#define EL_NORMALTABLE "[END_NORMALTABLE]"

#define BL_UVTABLE "[UVTABLE]"
#define EL_UVTABLE "[END_UVTABLE]"

#define BL_SHADERTABLE "[SHADERTABLE]"
#define EL_SHADERTABLE "[END_SHADERTABLE]"

#define BL_MAPTABLE "[MAPTABLE]"
#define EL_MAPTABLE "[END_MAPTABLE]"

#define BL_SHADER "[SHADER]"
#define EL_SHADER "[END_SHADER]"

#define BL_COLORCHANNEL "[COLORCHANNEL]"
#define EL_COLORCHANNEL "[END_COLORCHANNEL]"

#define BL_REFLECTIVITYCHANNEL "[REFLECTIVITYCHANNEL]"
#define EL_REFLECTIVITYCHANNEL "[END_REFLECTIVITYCHANNEL]"

#define BL_STANDARDMAP "[STANDARDMAP]"
#define EL_STANDARDMAP "[END_STANDARDMAP]"

#define BL_BLENDERMAP "[BLENDERMAP]"
#define EL_BLENDERMAP "[END_BLENDERMAP]"

#define BL_TEXTUREMAP "[MAP]"
#define EL_TEXTUREMAP "[END_MAP]"

#define BL_LAYEREDTEXTUREMAP "[LAYEREDTEXTUREMAP]"
#define EL_LAYEREDTEXTUREMAP "[END_LAYEREDTEXTUREMAP]"

#define BL_TRANSFORM "[TRANSFORM]"
#define EL_TRANSFORM "[END_TRANSFORM]"

#define BL_GROUPTRANSFORM "[GROUPTRANSFORM]"
#define EL_GROUPTRANSFORM "[END_GROUPTRANSFORM]"
#define BL_GROUPTRANSFORMINSTANCE "[GROUPTRANSFORMINSTANCE]"
#define EL_GROUPTRANSFORMINSTANCE "[END_GROUPTRANSFORMINSTANCE]"

#define BL_ATTRIBUTES "[ATTRIBUTES]"
#define EL_ATTRIBUTES "[END_ATTRIBUTES]"

#define BL_VERTEXCOLORS "[VERTEXCOLORS]"
#define EL_VERTEXCOLORS "[END_VERTEXCOLORS]"

#define BL_LIGHT "[LIGHT]"
#define EL_LIGHT "[END_LIGHT]"
#define BL_LIGHTINSTANCE "[LIGHTINSTANCE]"
#define EL_LIGHTINSTANCE "[END_LIGHTINSTANCE]"

#define BL_CURVEMODEL "[CURVEMODEL]"
#define EL_CURVEMODEL "[END_CURVEMODEL]"
#define BL_CURVEMODELINSTANCE "[CURVEMODELINSTANCE]"
#define EL_CURVEMODELINSTANCE "[END_CURVEMODELINSTANCE]"

#define BL_CVTABLE "[CVTABLE]"
#define EL_CVTABLE "[END_CVTABLE]"

#define BL_KNOTTABLE "[KNOTTABLE]"
#define EL_KNOTTABLE "[END_KNOTTABLE]"

#define BL_CVCONNECTS "[CVCONNECTS]"
#define EL_CVCONNECTS "[END_CVCONNECTS]"

#define BL_KNOTCONNECTS "[KNOTCONNECTS]"
#define EL_KNOTCONNECTS "[END_KNOTCONNECTS]"

#define BL_CVCOUNTS "[CVCOUNTS]"
#define EL_CVCOUNTS "[END_CVCOUNTS]"

#define BL_KNOTCOUNTS "[KNOTCOUNTS]"
#define EL_KNOTCOUNTS "[END_KNOTCOUNTS]"

#define BL_KNOTPARAMS "[KNOTPARAMS]"
#define EL_KNOTPARAMS "[END_KNOTPARAMS]"

#define BL_UVMAPTABLE "[UVMAPTABLE]"
#define EL_UVMAPTABLE "[END_UVMAPTABLE]"

#define BL_UVMAP "[UVMAP]"
#define EL_UVMAP "[END_UVMAP]"

#define BL_VERTEXCOUNTS "[VERTEXCOUNTS]"
#define EL_VERTEXCOUNTS "[END_VERTEXCOUNTS]"

#define BL_POLYVERTEXINDICIES "[POLYVERTEXINDICIES]"
#define EL_POLYVERTEXINDICIES "[END_POLYVERTEXINDICIES]"

#define BL_VERTEXNORMALINDICIES "[VERTEXNORMALINDICIES]"
#define EL_VERTEXNORMALINDICIES "[END_VERTEXNORMALINDICIES]"

#define BL_SHADERINDICIES "[SHADERINDICIES]"
#define EL_SHADERINDICIES "[END_SHADERINDICIES]"

#define BL_NULL "[NULL]"
#define EL_NULL "[END_NULL]"

#define BL_NULLINSTANCE "[NULLINSTANCE]"
#define EL_NULLINSTANCE "[END_NULLINSTANCE]"

#define BL_BONE "[BONE]"
#define EL_BONE "[END_BONE]"

#define BL_BONEINSTANCE "[BONEINSTANCE]"
#define EL_BONEINSTANCE "[END_BONEINSTANCE]"

#define BL_DISPLAYLAYERTABLE "[DISPLAYLAYERTABLE]"
#define EL_DISPLAYLAYERTABLE "[END_DISPLAYLAYERTABLE]"

mffTools::mffTools() {}

mffTools::~mffTools()
{
	for (unsigned i = 0; i < m_references.size(); i++)
	{
		mffNode *node = (mffNode*)m_references[i];

		if (node->GetNodeType() == kMODEL)
		{
			mffModel *model = (mffModel*)node;

			for (unsigned j = 0; j < model->m_shaders.size(); j++)
			{
				delete model->m_shaders[j].m_colorMap;
			}
		}
		delete m_references[i];
	}
	
	for (i = 0; i < m_instances.size(); i++)
	{
		delete m_instances[i];
	}

	for (i = 0; i < m_attributes.size(); i++)
	{
		delete m_attributes[i];
	}
	m_references.clear();
	m_instances.clear();
	m_attributes.clear();
}

float mffTools::GetVersion()
{
	char line[MAX_LINE_SIZE];
	float version;

	fscanf(m_fp,"%s",line);

	if (strcmp(line,"[MFF_VERSION")!=0)
		return -1;

	fscanf(m_fp,"%f]\n",&version);

	return version;
}

int mffTools::ImportFile(const char *filename)
{
	m_fp = fopen(filename,"rb");

	if (m_fp == NULL)
		return 0;

	float version = GetVersion();

	if (version != VERSION)
		return -1;

	char line[MAX_LINE_SIZE];

	fscanf(m_fp,"%s\n",line);

	while (strcmp(line,EL_ATTRIBUTES)!=0)
	{
		if (strcmp(line,BL_ATTRIBUTES)==0)
		{
			ImportAttributes();
		}
		fscanf(m_fp,"%s\n",line);
	}

	fscanf(m_fp,"%s",line);

	while (strcmp(line,EL_REFERENCES)!=0)
	{
		if (strcmp(line,BL_REFERENCES)==0)
		{
			ImportReferences();
		}
		fscanf(m_fp,"%s\n",line);
	}

	fscanf(m_fp,"%s",line);

	while (strcmp(line,EL_SCENE)!=0)
	{
		if (strcmp(line,BL_SCENE)==0)
		{
			ImportInstances();
		}
		fscanf(m_fp,"%s\n",line);
	}
	fclose(m_fp);

	return 1;
}

void mffTools::ImportReferences()
{
	char line[MAX_LINE_SIZE];

	unsigned refcount = 0;
	fscanf(m_fp,"%s %i\n",line,&refcount);

	for (unsigned i = 0; i < refcount; i++)
	{
		fscanf(m_fp,"%s\n",line);// BEGIN LABEL

		if (strcmp(line,BL_POLYMODEL)==0)
		{
			mffPolyModel *model = new mffPolyModel;
			ImportPolygonModel(*model);
			m_references.push_back(model);
		}
		else if (strcmp(line,BL_GROUPTRANSFORM) == 0)
		{
			mffGroupTransform *group = new mffGroupTransform;
			ImportGroupTransform(*group);
			m_references.push_back(group);
		}
		else if (strcmp(line,BL_VERTEXCOLORS) == 0)
		{
			mffVertexColorNode *vcnode = new mffVertexColorNode;
			ImportVertexColors(*vcnode);
			m_references.push_back(vcnode);
		}
		else if (strcmp(line,BL_LIGHT) == 0)
		{
			mffLight *light = new mffLight;
			ImportLight(*light);
			m_references.push_back(light);
		}
		else if (strcmp(line,BL_CURVEMODEL) == 0)
		{
			mffCurveModel *curve = new mffCurveModel;
			ImportCurveModel(*curve);
			m_references.push_back(curve);
		}
		else if (strcmp(line,BL_NULL) == 0)
		{
			mffNULL *Null = new mffNULL;
			ImportNULL(*Null);
			m_references.push_back(Null);
		}
		else if (strcmp(line,BL_BONE) == 0)
		{
			mffBone *bone = new mffBone;
			ImportBone(*bone);
			m_references.push_back(bone);
		}
		else if (strcmp(line,BL_SHADERTABLE) == 0)
		{
			mffShaderTableNode *shadertable = new mffShaderTableNode;
			ImportShaderTable(shadertable->m_shaders);
			m_references.push_back(shadertable);
		}
		else if (strcmp(line,BL_MAPTABLE) == 0)
		{
			mffMapTableNode *maptable = new mffMapTableNode;
			ImportMapTable(maptable->m_maps);
			m_references.push_back(maptable);
		}
		else if (strcmp(line,BL_DISPLAYLAYERTABLE) == 0)
		{
			mffDisplayLayerTableNode *displaylayertable = new mffDisplayLayerTableNode;
			ImportDisplayLayers(*displaylayertable);
			m_references.push_back(displaylayertable);			
		}
	}
}

void mffTools::ImportInstances()
{
	char line[MAX_LINE_SIZE];

	unsigned instancecount = 0;
	fscanf(m_fp,"%s %i\n",line,&instancecount);

	for (unsigned i = 0; i < instancecount; i++)
	{
		fscanf(m_fp,"%s\n",line);//INSTANCE OR GROUP TRANSFORM LABEL
		
		if (strcmp(line,BL_POLYMODELINSTANCE)==0)
		{
			mffPolyModelInstance *polymodelinstance = new mffPolyModelInstance;

			ImportPolygonInstance(*polymodelinstance);

			m_instances.push_back(polymodelinstance);
			
		}
		else if (strcmp(line,BL_GROUPTRANSFORMINSTANCE) == 0)
		{
			mffGroupTransformInstance *groupinstance = new mffGroupTransformInstance;

			ImportGroupTransformInstance(*groupinstance);

			m_instances.push_back(groupinstance);
		}
		else if (strcmp(line,BL_LIGHTINSTANCE) == 0)
		{
			mffLightInstance *lightinstance = new mffLightInstance;

			ImportLightInstance(*lightinstance);

			m_instances.push_back(lightinstance);
		}
		else if (strcmp(line,BL_CURVEMODELINSTANCE) == 0)
		{
			mffCurveModelInstance *curveinstance = new mffCurveModelInstance;

			ImportCurveModelInstance(*curveinstance);

			m_instances.push_back(curveinstance);
		}
		else if (strcmp(line,BL_NULLINSTANCE) == 0)
		{
			mffNULLInstance *nullinstance = new mffNULLInstance;

			ImportNULLInstance(*nullinstance);

			m_instances.push_back(nullinstance);
		}
	else if (strcmp(line,BL_BONEINSTANCE) == 0)
		{
			mffBoneInstance *boneinstance = new mffBoneInstance;

			ImportBoneInstance(*boneinstance);

			m_instances.push_back(boneinstance);
		}
	}
}

void mffTools::ImportInstanceNode(mffNode *node)
{
	node->m_name.import(m_fp);
	fscanf(m_fp,"index %i\n",&node->m_index);
	fscanf(m_fp,"refidx %i\n",&node->m_refidx);
	fscanf(m_fp,"layidx %i\n",&node->m_layerindex);
	fscanf(m_fp,"parent %i\n",&node->m_parentindex);

	// import child indicies
	fscanf(m_fp, "children ");
	node->m_childindicies.import(m_fp);

	// attribute indicies
	fscanf(m_fp,"attributes ");
	node->m_attrindicies.import(m_fp);
}

void mffTools::ImportModelInstance(mffModelInstance *modelinstance)
{
	char line[MAX_LINE_SIZE];
	fscanf(m_fp,"%s %i\n",line,&modelinstance->m_vcid);
}

void mffTools::ImportPolygonInstance(mffPolyModelInstance &polymodelinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&polymodelinstance);
	ImportModelInstance((mffModelInstance *)&polymodelinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

void mffTools::ImportCurveModelInstance(mffCurveModelInstance &curvemodelinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&curvemodelinstance);
	ImportModelInstance((mffModelInstance *)&curvemodelinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

void mffTools::ImportGroupTransform(mffGroupTransform &group)
{
	char line[MAX_LINE_SIZE];

	ImportReferenceNode((mffNode*)&group);

	ImportTransform(group.m_transform);
	fscanf(m_fp,"%s\n",line);//GROUP END LABEL
}

void mffTools::ImportLight(mffLight &light)
{
	char line[MAX_LINE_SIZE];

	ImportReferenceNode((mffNode*)&light);
	
	GetLine(m_fp,MAX_LINE_SIZE,line);
	if (strcmp(line,"type omni") == 0)
		light.m_type = mffLight::omni;
	else if (strcmp(line,"type spot") == 0)
		light.m_type = mffLight::spot;
	else if (strcmp(line,"type directional")==0)
		light.m_type = mffLight::directional;
	fscanf(m_fp,"%s %f\n",line,&light.m_intensity);
	light.m_color.import(m_fp);
	fscanf(m_fp,"%s\n",line);//LIGHT END LABEL
}

void mffTools::ImportLightInstance(mffLightInstance &lightinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&lightinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

void mffTools::ImportVertexColors(mffVertexColorNode &vertexcolornode)
{
	char line[MAX_LINE_SIZE];
	fscanf(m_fp,"index %i\n",&vertexcolornode.m_index);
	vertexcolornode.m_VertexColors.import(m_fp);
	fscanf(m_fp,"%s\n",line);//VERTEXCOLOR END LABEL
}

void mffTools::ImportGroupTransformInstance(mffGroupTransformInstance &groupinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&groupinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

void mffTools::ImportPolygonModel(mffPolyModel &model)
{
	char line[MAX_LINE_SIZE];

	ImportReferenceNode((mffNode*)&model);

	fscanf(m_fp,"%s\n",line);

	while (strcmp(line,EL_POLYMODEL) != 0)
	{
		if (strcmp(line,BL_VERTEXTABLE)==0)
		{
			model.m_VertexArray.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_UVMAPTABLE) == 0)
		{
			ImportUVMaps(model.m_UVMaps);
		}
		else if (strcmp(line,BL_NORMALTABLE)==0)
		{
			model.m_VertexNormalArray.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_VERTEXNORMALINDICIES)==0)
		{
			model.m_VertexNormalIndicies.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_VERTEXCOUNTS)==0)
		{
			model.m_VertexCounts.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_POLYVERTEXINDICIES)==0)
		{
			model.m_PolyVertexIndicies.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_SHADERINDICIES)==0)
		{
			model.m_ShaderIndicies.import(m_fp);
			fscanf(m_fp,"%s\n",line);// END LABEL
		}
		else if (strcmp(line,BL_SHADERTABLE)==0)
		{
			ImportShaderTable(model.m_shaders);
		}
		fscanf(m_fp,"%s\n",line);
	}
}

void mffTools::ImportShaderTable(std::vector<mffShader> &Shaders)
{
	unsigned shadercount;
	char label[MAX_LINE_SIZE];

	enum shaderchannel { COLOR, REFLECT };
	int channel;

	fscanf(m_fp,"%s %i",label,&shadercount);

	for (unsigned i = 0; i < shadercount; i++)
	{
		mffShader Shader;

		fscanf(m_fp,"%s\n",label);
		GetLine(m_fp,MAX_LINE_SIZE,label);
		strcpy(Shader.m_name,label);
		int index;
		fscanf(m_fp,"index %i\n",&index);

		bool finished = false;

		while (strcmp(label,EL_SHADER)!=0)
		{
			fscanf(m_fp,"%s\n",label);

			if (strcmp(label,BL_COLORCHANNEL) == 0)
			{
				channel = COLOR;
				Shader.m_color.import(m_fp);
			}
			else if (strcmp(label,BL_REFLECTIVITYCHANNEL) == 0)
			{
				channel = REFLECT;
				fscanf(m_fp,"reflectivity %f\n",&Shader.m_reflect);
			}
			else if (strcmp(label,EL_SHADER) == 0)
			{
				Shaders.push_back(Shader);
				continue;	
			}
			else if (strcmp(label,BL_STANDARDMAP)==0)
			{
				mffStd *standard = new mffStd;
				standard->m_mapindicies.import(m_fp);
				if (channel == COLOR)
					Shader.m_colorMap = (mffMapObject *)standard;
				if (channel == REFLECT)
					Shader.m_reflectMap = (mffMapObject *)standard;
				fscanf(m_fp,"%s",label);//GET STANDARD END LABEL
			}
			else if (strcmp(label,BL_BLENDERMAP)==0)
			{
				mffBlender *blender = new mffBlender;
				blender->m_mapindicies.import(m_fp);
				Shader.m_colorMap = (mffMapObject*)blender;
				fscanf(m_fp,"%s",label);//GET BLENDER MAP END LABEL
			}
			else if (strcmp(label,BL_LAYEREDTEXTUREMAP)==0)
			{
				mffLayeredTexture *layeredtexture = new mffLayeredTexture;
				layeredtexture->m_mapindicies.import(m_fp);
				layeredtexture->m_types.import(m_fp);
//				layeredtexture->m_alphavalues.import(m_fp);
				layeredtexture->m_mapsconnected.import(m_fp);
//				layeredtexture->m_alphasconnected.import(m_fp);

				Shader.m_colorMap = (mffMapObject*)layeredtexture;
				fscanf(m_fp,"%s",label);//GET LAYEREDTEXTURE END LABEL
			}
		}
	}
	fscanf(m_fp,"%s\n",label);// GET SHADER TABLE END LABEL
}

void mffTools::ImportDisplayLayers(mffDisplayLayerTableNode &displaylayertable)
{
	char line[MAX_LINE_SIZE];
	int index;
	fscanf(m_fp,"index %i\n",&index);
	displaylayertable.m_layernames.import(m_fp);
	displaylayertable.m_layerflags.import(m_fp);
	displaylayertable.m_layercolors.import(m_fp);
	fscanf(m_fp,"%s\n",line);// GET DISPLAY LAYER TABLE END LABEL
}

void mffTools::ExportDisplayLayers(mffDisplayLayerTableNode &displaylayertable)
{
	fprintf(m_fp,"%s\n",BL_DISPLAYLAYERTABLE);
	fprintf(m_fp,"index %i\n",displaylayertable.m_index);
	displaylayertable.m_layernames.export(m_fp);
	displaylayertable.m_layerflags.export(m_fp);
	displaylayertable.m_layercolors.export(m_fp);
	fprintf(m_fp,"%s\n",EL_DISPLAYLAYERTABLE);
}

void mffTools::ImportNULL(mffNULL &Null)
{
	char line[MAX_LINE_SIZE];

	ImportReferenceNode((mffNode *)&Null);

	fscanf(m_fp,"%s\n",line);//END LABEL
}

void mffTools::ImportNULLInstance(mffNULLInstance &nullinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&nullinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

void mffTools::ImportBone(mffBone &bone)
{
	char line[MAX_LINE_SIZE];

	ImportReferenceNode((mffNode*)&bone);

	fscanf(m_fp,"%s\n",line);//LIGHT END LABEL
}

void mffTools::ImportBoneInstance(mffBoneInstance &boneinstance)
{
	char line[MAX_LINE_SIZE];
	ImportInstanceNode((mffNode*)&boneinstance);
	fscanf(m_fp,"%s\n",line);//INSTANCE END LABEL
}

int mffTools::ExportFile(const char *filename)
{
	m_fp = fopen(filename,"w");

	if (m_fp == NULL)
		return 0;

	fprintf(m_fp,"[MFF_VERSION %.1f]\n",VERSION);

	ExportAttributes();

	fprintf(m_fp,"%s\n",BL_REFERENCES);
	fprintf(m_fp,"count %i\n",GetNumReferences());

	mffPolyModel *polymodel;

	for (unsigned i = 0; i < GetNumReferences(); i++)
	{
		mffNode *node = (mffNode*)m_references[i];

		if (node->GetNodeType() == kMODEL)
		{
			mffModel *model = (mffModel*)node;
			if (model->GetModelType() == kPOLYGONMODEL)
			{
				polymodel = (mffPolyModel *)model;
				ExportPolygonModel(*polymodel);
			}
			else if (model->GetModelType() == kCURVEMODEL)
			{
				mffCurveModel *curve = (mffCurveModel *)node;
				ExportCurveModel(*curve);
			}
		}
		else if (node->GetNodeType() == kGROUPTRANSFORM)
		{
			mffGroupTransform *group = (mffGroupTransform *)node;
			ExportGroupTransform(*group);
		}
		else if (node->GetNodeType() == kVERTEXCOLORNODE)
		{
			mffVertexColorNode *vcnode = (mffVertexColorNode *)node;
			ExportVertexColors(*vcnode);
		}
		else if (node->GetNodeType() == kLIGHT)
		{
			mffLight *light = (mffLight *)node;
			ExportLight(*light);
		}
		else if (node->GetNodeType() == kNULL)
		{
			mffNULL *Null = (mffNULL *)node;
			ExportNULL(*Null);
		}
		else if (node->GetNodeType() == kBONE)
		{
			mffBone *bone = (mffBone *)node;
			ExportBone(*bone);
		}
		else if (node->GetNodeType() == kSHADERTABLENODE)
		{
			mffShaderTableNode *shadertable = (mffShaderTableNode*)node;
			ExportShaderTable(shadertable->m_shaders);
		}
		else if (node->GetNodeType() == kMAPTABLENODE)
		{
			mffMapTableNode *maptable = (mffMapTableNode*)node;
			ExportMapTable(maptable->m_maps);
		}
		else if (node->GetNodeType() == kDISPLAYLAYERTABLENODE)
		{
			mffDisplayLayerTableNode *displaylayertable = (mffDisplayLayerTableNode*)node;
			ExportDisplayLayers(*displaylayertable);
		}		
	}

	fprintf(m_fp,"%s\n",EL_REFERENCES);

	fprintf(m_fp,"%s\n",BL_SCENE);
	fprintf(m_fp,"count %i\n",GetNumInstances());
	for (i = 0; i < GetNumInstances(); i++)
	{		
		mffNode *node = (mffNode*)m_instances[i];

		if (node->GetNodeType() == kMODELINSTANCE)
		{
			mffModelInstance *model = (mffModelInstance*)node;

			if (model->GetModelType() == kPOLYGONMODELINSTANCE)
			{
				mffPolyModelInstance *polymodelinstance = (mffPolyModelInstance *)model;
				ExportPolygonModelInstance(*polymodelinstance);
			}
			else if (model->GetModelType() == kCURVEMODELINSTANCE)
			{
				mffCurveModelInstance *curveinstance = (mffCurveModelInstance *)node;
				ExportCurveModelInstance(*curveinstance);
			}
		}
		else if (node->GetNodeType() == kGROUPTRANSFORMINSTANCE)
		{
			mffGroupTransformInstance *groupinstance = (mffGroupTransformInstance *)node;
			ExportGroupTransformInstance(*groupinstance);
		}
		else if (node->GetNodeType() == kLIGHTINSTANCE)
		{
			mffLightInstance *lightinstance = (mffLightInstance *)node;
			ExportLightInstance(*lightinstance);
		}
		else if (node->GetNodeType() == kNULLINSTANCE)
		{
			mffNULLInstance *nullinstance = (mffNULLInstance *)node;
			ExportNULLInstance(*nullinstance);
		}
		else if (node->GetNodeType() == kBONEINSTANCE)
		{
			mffBoneInstance *boneinstance = (mffBoneInstance *)node;
			ExportBoneInstance(*boneinstance);
		}
	}
	fprintf(m_fp,"%s\n",EL_SCENE);

	fclose(m_fp);

	return 1;
}

void mffTools::ExportShaderTable(std::vector<mffShader> &shaders)
{
//	Export Shader Table
	fprintf(m_fp,"%s\n",BL_SHADERTABLE);
	fprintf(m_fp,"count %i\n",shaders.size());
	for (unsigned i = 0; i < shaders.size(); i++)
	{
		fprintf(m_fp,"%s\n",BL_SHADER);
		fprintf(m_fp,"%s\n",shaders[i].m_name);
		fprintf(m_fp,"index %i\n",i);

		if (!shaders[i].m_colorMap)
		{
			fprintf(m_fp,"%s\n",BL_COLORCHANNEL);
			mffColor *color = &shaders[i].m_color;
			color->export(m_fp);
			fprintf(m_fp,"%s\n",EL_COLORCHANNEL);
		}
		else
		{
			if (shaders[i].m_colorMap->GetType() == mffMapObject::kSTANDARD)
			{
				fprintf(m_fp,"%s\n",BL_COLORCHANNEL);

				mffStd *mapobj = (mffStd *)shaders[i].m_colorMap;

				mffColor color(1.0f,1.0f,1.0f,1.0f);
				color.export(m_fp);

				fprintf(m_fp,"%s\n",BL_STANDARDMAP);
				mapobj->m_mapindicies.export(m_fp);
				fprintf(m_fp,"%s\n",EL_STANDARDMAP);
				
				fprintf(m_fp,"%s\n",EL_COLORCHANNEL);
			}
			else if (shaders[i].m_colorMap->GetType() == mffMapObject::kBLENDER)
			{
				fprintf(m_fp,"%s\n",BL_COLORCHANNEL);

				mffBlender *mapobj = (mffBlender *)shaders[i].m_colorMap;

				mffColor color(1.0f,1.0f,1.0f,1.0f);
				color.export(m_fp);
				
				fprintf(m_fp,"%s\n",BL_BLENDERMAP);
				mapobj->m_mapindicies.export(m_fp);
				fprintf(m_fp,"%s\n",EL_BLENDERMAP);

				fprintf(m_fp,"%s\n",EL_COLORCHANNEL);
			}
			else if (shaders[i].m_colorMap->GetType() == mffMapObject::kLAYEREDTEXTURE)
			{
				fprintf(m_fp,"%s\n",BL_COLORCHANNEL);

				mffLayeredTexture *mapobj = (mffLayeredTexture *)shaders[i].m_colorMap;

				mffColor color(1.0f,1.0f,1.0f,1.0f);
				color.export(m_fp);
				
				fprintf(m_fp,"%s\n",BL_LAYEREDTEXTUREMAP);
				
				mapobj->m_mapindicies.export(m_fp);
				mapobj->m_types.export(m_fp);
//				mapobj->m_alphavalues.export(m_fp);
				mapobj->m_mapsconnected.export(m_fp);
//				mapobj->m_alphasconnected.export(m_fp);

				fprintf(m_fp,"%s\n",EL_LAYEREDTEXTUREMAP);

				fprintf(m_fp,"%s\n",EL_COLORCHANNEL);
			}
			
		}

		if (!shaders[i].m_reflectMap)
		{
			fprintf(m_fp,"%s\n",BL_REFLECTIVITYCHANNEL);
			fprintf(m_fp,"reflectivity %f\n",shaders[i].m_reflect);
			fprintf(m_fp,"%s\n",EL_REFLECTIVITYCHANNEL);
		}
		else
		{
			if (shaders[i].m_reflectMap->GetType() == mffMapObject::kSTANDARD)
			{
				fprintf(m_fp,"%s\n",BL_REFLECTIVITYCHANNEL);

				mffStd *mapobj = (mffStd *)shaders[i].m_reflectMap;

				fprintf(m_fp,"reflectivity %f\n",shaders[i].m_reflect);

				fprintf(m_fp,"%s\n",BL_STANDARDMAP);
				mapobj->m_mapindicies.export(m_fp);
				fprintf(m_fp,"%s\n",EL_STANDARDMAP);
				
				fprintf(m_fp,"%s\n",EL_REFLECTIVITYCHANNEL);
			}
		}
		fprintf(m_fp,"%s\n",EL_SHADER);
	}
	fprintf(m_fp,"%s\n",EL_SHADERTABLE);
}

void mffTools::ExportMapTable(std::vector<mffMap> &maps)
{
//	Export Map Table
	fprintf(m_fp,"%s\n",BL_MAPTABLE);
	fprintf(m_fp,"count %i\n",maps.size());
	for (unsigned i = 0; i < maps.size(); i++)
	{
		ExportMap(maps[i],i);
	}
	fprintf(m_fp,"%s\n",EL_MAPTABLE);
}

void mffTools::ImportMapTable(std::vector<mffMap> &maps)
{
	maps.clear();
	char line[MAX_LINE_SIZE];
//	Export Map Table
	int mapcount;
	fscanf(m_fp,"count %i",&mapcount);

	for (unsigned i = 0; i < mapcount; i++)
	{
		mffMap mffmap;
		ImportMap(mffmap);
		maps.push_back(mffmap);
	}
	fscanf(m_fp,"%s\n",line);// GET MAP TABLE END LABEL
}

void mffTools::ImportReferenceNode(mffNode *node)
{
	node->m_name.import(m_fp);
	fscanf(m_fp,"index %i\n",&node->m_index);
	fscanf(m_fp,"isexternalfilereference %i\n",&node->m_isexternalreference);
	if (node->m_isexternalreference)
		node->m_filename.import(m_fp);
}
void mffTools::ExportReferenceNode(mffNode *node)
{
	node->m_name.export("name",m_fp);
	fprintf(m_fp,"index %i\n",node->m_index);
	fprintf(m_fp,"isexternalfilereference %i\n",node->m_isexternalreference);
	if (node->m_isexternalreference)
		node->m_filename.export("externalfilename",m_fp);
}

void mffTools::ExportPolygonModel(mffPolyModel &polymodel)
{
	fprintf(m_fp,"%s\n",BL_POLYMODEL);

	ExportReferenceNode((mffNode*)&polymodel);

//	Export Shader Table
	ExportShaderTable(polymodel.m_shaders);

//	Export Vertex Table
	fprintf(m_fp,"%s\n",BL_VERTEXTABLE);
	polymodel.m_VertexArray.export(m_fp);
	fprintf(m_fp,"%s\n",EL_VERTEXTABLE);

//	Export UV Maps
	ExportUVMaps(polymodel.m_UVMaps);

//	Export Normal Table
	fprintf(m_fp,"%s\n",BL_NORMALTABLE);
	polymodel.m_VertexNormalArray.export(m_fp);
	fprintf(m_fp,"%s\n",EL_NORMALTABLE);

//	VertexNormal Indicies
	fprintf(m_fp,"%s\n",BL_VERTEXNORMALINDICIES);
	polymodel.m_VertexNormalIndicies.export(m_fp);
	fprintf(m_fp,"%s\n",EL_VERTEXNORMALINDICIES);
	
//	Vertex Counts
	fprintf(m_fp,"%s\n",BL_VERTEXCOUNTS);
	polymodel.m_VertexCounts.export(m_fp);
	fprintf(m_fp,"%s\n",EL_VERTEXCOUNTS);

//	PolyVertex Indicies	
	fprintf(m_fp,"%s\n",BL_POLYVERTEXINDICIES);
	polymodel.m_PolyVertexIndicies.export(m_fp);
	fprintf(m_fp,"%s\n",EL_POLYVERTEXINDICIES);

//	Shader Indicies
	fprintf(m_fp,"%s\n",BL_SHADERINDICIES);
	polymodel.m_ShaderIndicies.export(m_fp);
	fprintf(m_fp,"%s\n",EL_SHADERINDICIES);

	fprintf(m_fp,"%s\n",EL_POLYMODEL);
}

void mffTools::ExportUVMaps(std::vector<mffUVMap> &uvmaps)
{
	fprintf(m_fp,"%s\n",BL_UVMAPTABLE);
	int count = uvmaps.size();
	fprintf(m_fp,"count %i\n",count);

	for (int i = 0; i < count; i++)
		ExportUVMap(uvmaps[i]);

	fprintf(m_fp,"%s\n",EL_UVMAPTABLE);
}

void mffTools::ExportUVMap(mffUVMap &uvmap)
{
	fprintf(m_fp,"%s\n",BL_UVMAP);
	uvmap.m_name.export("name",m_fp);
	uvmap.m_UVArray.export(m_fp);
	uvmap.m_UVCounts.export(m_fp);
	uvmap.m_PolyUVIndicies.export(m_fp);
	fprintf(m_fp,"%s\n",EL_UVMAP);
}

void mffTools::ImportUVMaps(std::vector<mffUVMap> &uvmaps)
{
	int count;
	char line[MAX_LINE_SIZE];

	fscanf(m_fp,"%s %i\n",line,&count);
	uvmaps.resize(count);
	for (int i = 0; i < count; i++)
		ImportUVMap(uvmaps[i]);
	fscanf(m_fp,"%s\n",line);// END LABEL
}

void mffTools::ImportUVMap(mffUVMap &uvmap)
{
	char line[MAX_LINE_SIZE];

	fscanf(m_fp,"%s\n",line);// BEGIN LABEL

	uvmap.m_name.import(m_fp);

	uvmap.m_UVArray.import(m_fp);
	uvmap.m_UVCounts.import(m_fp);
	uvmap.m_PolyUVIndicies.import(m_fp);

	fscanf(m_fp,"%s\n",line);// END LABEL
}

void mffTools::ImportCurveModel(mffCurveModel &curvemodel)
{
	char line[MAX_LINE_SIZE];
	mffVector vect;

	ImportReferenceNode((mffNode*)&curvemodel);

	fscanf(m_fp,"%s %i\n",line,&curvemodel.m_degree);
	fscanf(m_fp,"%s %i\n",line,&curvemodel.m_spans);
	fscanf(m_fp,"%s %i\n",line,&curvemodel.m_form);

	fscanf(m_fp,"%s\n",line);// CVCOUNTS BEGIN LABEL
	curvemodel.m_CVCounts.import(m_fp);
	fscanf(m_fp,"%s\n",line);// CVCOUNTS END LABEL

	fscanf(m_fp,"%s\n",line);// KNOTCOUNTS BEGIN LABEL
	curvemodel.m_KnotCounts.import(m_fp);
	fscanf(m_fp,"%s\n",line);// KNOTCOUNTS END LABEL

	fscanf(m_fp,"%s\n",line);// CVCONNECTS BEGIN LABEL
	curvemodel.m_CVConnects.import(m_fp);
	fscanf(m_fp,"%s\n",line);// CVCONNECTS END LABEL

	fscanf(m_fp,"%s\n",line);// KNOTCONNECTS BEGIN LABEL
	curvemodel.m_KnotConnects.import(m_fp);
	fscanf(m_fp,"%s\n",line);// KNOTCONNECTS END LABEL
	
	fscanf(m_fp,"%s\n",line);// CVTABLE BEGIN LABEL
	curvemodel.m_CVArray.import(m_fp);
	fscanf(m_fp,"%s\n",line);// CVTABLE END LABEL

	fscanf(m_fp,"%s\n",line);// KNOTTABLE BEGIN LABEL
	curvemodel.m_KnotArray.import(m_fp);
	fscanf(m_fp,"%s\n",line);// KNOTTABLE END LABEL

	fscanf(m_fp,"%s\n",line);// KNOTPARAMS BEGIN LABEL
	curvemodel.m_KnotParams.import(m_fp);
	fscanf(m_fp,"%s\n",line);// KNOTPARAMS END LABEL

	fscanf(m_fp,"%s\n",line);//CURVE MODEL END LABEL
}

void mffTools::ExportCurveModel(mffCurveModel &curvemodel)
{
	fprintf(m_fp,"%s\n",BL_CURVEMODEL);

	ExportReferenceNode((mffNode *)&curvemodel);

	fprintf(m_fp,"degree %i\n",curvemodel.m_degree);
	fprintf(m_fp,"spans %i\n",curvemodel.m_spans);
	fprintf(m_fp,"form %i\n",curvemodel.m_form);

	fprintf(m_fp,"%s\n",BL_CVCOUNTS);
	curvemodel.m_CVCounts.export(m_fp);
	fprintf(m_fp,"%s\n",EL_CVCOUNTS);
	
	fprintf(m_fp,"%s\n",BL_KNOTCOUNTS);
	curvemodel.m_KnotCounts.export(m_fp);	
	fprintf(m_fp,"%s\n",EL_KNOTCOUNTS);

	fprintf(m_fp,"%s\n",BL_CVCONNECTS);
	curvemodel.m_CVConnects.export(m_fp);		
	fprintf(m_fp,"%s\n",EL_CVCONNECTS);
	
	fprintf(m_fp,"%s\n",BL_KNOTCONNECTS);
	curvemodel.m_KnotConnects.export(m_fp);			
	fprintf(m_fp,"%s\n",EL_KNOTCONNECTS);

	fprintf(m_fp,"%s\n",BL_CVTABLE);
	curvemodel.m_CVArray.export(m_fp);			
	fprintf(m_fp,"%s\n",EL_CVTABLE);

	fprintf(m_fp,"%s\n",BL_KNOTTABLE);
	curvemodel.m_KnotArray.export(m_fp);			
	fprintf(m_fp,"%s\n",EL_KNOTTABLE);

	fprintf(m_fp,"%s\n",BL_KNOTPARAMS);
	curvemodel.m_KnotParams.export(m_fp);			
	fprintf(m_fp,"%s\n",EL_KNOTPARAMS);

	fprintf(m_fp,"%s\n",EL_CURVEMODEL);
}

void mffTools::ExportInstanceNode(mffNode *node)
{
	node->m_name.export("name",m_fp);
	fprintf(m_fp,"index %i\n",node->m_index);
	fprintf(m_fp,"refidx %i\n",node->m_refidx);
	fprintf(m_fp,"layidx %i\n",node->m_layerindex);
	fprintf(m_fp,"parent %i\n",node->m_parentindex);

	// export child indicies
	fprintf(m_fp, "children ");
	node->m_childindicies.export(m_fp);

	// export attributes
	fprintf(m_fp,"attributes ");
	node->m_attrindicies.export(m_fp);
}

void mffTools::ExportModelInstance(mffModelInstance *modelinstance)
{
	fprintf(m_fp,"vcid %i\n",modelinstance->m_vcid);
}

void mffTools::ExportPolygonModelInstance(mffPolyModelInstance &polymodelinstance)
{
	fprintf(m_fp,"%s\n",BL_POLYMODELINSTANCE);
	ExportInstanceNode((mffNode *)&polymodelinstance);
	ExportModelInstance((mffModelInstance*)&polymodelinstance);
	fprintf(m_fp,"%s\n",EL_POLYMODELINSTANCE);
}

void mffTools::ExportCurveModelInstance(mffCurveModelInstance &curvemodelinstance)
{
	fprintf(m_fp,"%s\n",BL_CURVEMODELINSTANCE);
	ExportInstanceNode((mffNode *)&curvemodelinstance);
	ExportModelInstance((mffModelInstance*)&curvemodelinstance);
	fprintf(m_fp,"%s\n",EL_CURVEMODELINSTANCE);
}

void mffTools::ExportVertexColors(mffVertexColorNode &vertexcolornode)
{
	fprintf(m_fp,"%s\n",BL_VERTEXCOLORS);
	fprintf(m_fp,"index %i\n",vertexcolornode.m_index);
	vertexcolornode.m_VertexColors.export(m_fp);
	fprintf(m_fp,"%s\n",EL_VERTEXCOLORS);
}

void mffTools::ImportTransform(mffTransform &transform)
{
	char line[MAX_LINE_SIZE];
	fscanf(m_fp,"%s\n",line);//TRANSFORM BEGIN LABEL

	fscanf(m_fp,"translate ");
	transform.translate.import(m_fp);

	fscanf(m_fp,"rotate ");
	transform.rotate.import(m_fp);

	fscanf(m_fp,"scale ");
	transform.scale.import(m_fp);
		
	fscanf(m_fp,"rotatepivot ");
	transform.rotatepivot.import(m_fp);
	
	fscanf(m_fp,"rotatepivottranslation ");
	transform.rotatepivottranslation.import(m_fp);

	fscanf(m_fp,"scalepivot ");
	transform.scalepivot.import(m_fp);

	fscanf(m_fp,"scalepivottranslation ");
	transform.scalepivottranslation.import(m_fp);

	fscanf(m_fp,"%s\n",line);//TRANSFORM END LABEL
}

void mffTools::ExportTransform(mffTransform &transform)
{
	fprintf(m_fp,"%s\n",BL_TRANSFORM);

	fprintf(m_fp,"translate ");
	transform.translate.export(m_fp);

	fprintf(m_fp,"rotate ");
	transform.rotate.export(m_fp);

	fprintf(m_fp,"scale ");
	transform.scale.export(m_fp);
		
	fprintf(m_fp,"rotatepivot ");
	transform.rotatepivot.export(m_fp);
	
	fprintf(m_fp,"rotatepivottranslation ");
	transform.rotatepivottranslation.export(m_fp);

	fprintf(m_fp,"scalepivot ");
	transform.scalepivot.export(m_fp);

	fprintf(m_fp,"scalepivottranslation ");
	transform.scalepivottranslation.export(m_fp);
	
	fprintf(m_fp,"%s\n",EL_TRANSFORM);
}

void mffTools::ImportMap(mffMap &map)
{
	char line[MAX_LINE_SIZE];

	fscanf(m_fp,"%s\n",line);//BEGIN LABEL
	GetLine(m_fp,MAX_LINE_SIZE,line);
	strcpy(map.m_filename,line);
	int index;
	fscanf(m_fp,"index %i\n",&index);

//	boolean
	fscanf(m_fp,"wrapu %i wrapv %i\n",&map.m_wrapu,&map.m_wrapv);
	fscanf(m_fp,"mirroru %i mirrorv %i\n",&map.m_mirroru,&map.m_mirrorv);
	fscanf(m_fp,"stagger %i\n",&map.m_stagger);

//	offsets
	fscanf(m_fp,"repeat ");
	map.m_repeat.import(m_fp);
	fscanf(m_fp,"translation ");
	map.m_translation.import(m_fp);
	fscanf(m_fp,"rotation %f\n",&map.m_rotation);

//	color balance
	fscanf(m_fp,"defaultcolor ");
	map.m_defaultcolor.import(m_fp);
	fscanf(m_fp,"colorgain ");
	map.m_colorgain.import(m_fp);
	fscanf(m_fp,"coloroffset ");
	map.m_coloroffset.import(m_fp);

	fscanf(m_fp,"alphagain %f\n",&map.m_alphagain);
	fscanf(m_fp,"alphaoffset %f\n",&map.m_alphaoffset);

//	texture animation
	fscanf(m_fp,"startframe %i\n",&map.m_startframe);
	fscanf(m_fp,"endframe %i\n",&map.m_stopframe);
	fscanf(m_fp,"stepframe %i\n",&map.m_stepframe);
	
//	uv map index
	fscanf(m_fp,"uvsetname ");
	GetLine(m_fp,MAX_LINE_SIZE,line);

	strcpy(map.m_uvsetname,line);

	fscanf(m_fp,"%s\n",line);//END LABEL
}

void mffTools::ExportMap(mffMap &map,int index)
{
	fprintf(m_fp,"%s\n",BL_TEXTUREMAP);

	fprintf(m_fp,"%s\n",map.m_filename);

	fprintf(m_fp,"index %i\n",index);

//	boolean
	fprintf(m_fp,"wrapu %i wrapv %i\n",map.m_wrapu,map.m_wrapv);
	fprintf(m_fp,"mirroru %i mirrorv %i\n",map.m_mirroru,map.m_mirrorv);
	fprintf(m_fp,"stagger %i\n",map.m_stagger);

//	offsets
	fprintf(m_fp,"repeat ");
	map.m_repeat.export(m_fp);
	fprintf(m_fp,"translation ");
	map.m_translation.export(m_fp);
	float rotation = map.m_rotation;
	fprintf(m_fp,"rotation %f\n",rotation);

//	color balance
	fprintf(m_fp,"defaultcolor ");
	map.m_defaultcolor.export(m_fp);
	fprintf(m_fp,"colorgain ");
	map.m_colorgain.export(m_fp);
	fprintf(m_fp,"coloroffset ");
	map.m_coloroffset.export(m_fp);
	fprintf(m_fp,"alphagain %f\n",map.m_alphagain);
	fprintf(m_fp,"alphaoffset %f\n",map.m_alphaoffset);

//	texture animation
	fprintf(m_fp,"startframe %i\n",map.m_startframe);
	fprintf(m_fp,"endframe %i\n",map.m_stopframe);
	fprintf(m_fp,"stepframe %i\n",map.m_stepframe);
	
//	uv map index
	fprintf(m_fp,"uvsetname %s\n",map.m_uvsetname);
	
	fprintf(m_fp,"%s\n",EL_TEXTUREMAP);

}

void mffTools::ExportGroupTransform(mffGroupTransform &group)
{
	fprintf(m_fp,"%s\n",BL_GROUPTRANSFORM);
	ExportReferenceNode((mffNode*)&group);
	ExportTransform(group.m_transform);
	fprintf(m_fp,"%s\n",EL_GROUPTRANSFORM);	
}

void mffTools::ExportGroupTransformInstance(mffGroupTransformInstance &groupinstance)
{
	fprintf(m_fp,"%s\n",BL_GROUPTRANSFORMINSTANCE);
	ExportInstanceNode((mffNode *)&groupinstance);

	fprintf(m_fp,"%s\n",EL_GROUPTRANSFORMINSTANCE);
}

void mffTools::ExportLight(mffLight &light)
{
	fprintf(m_fp,"%s\n",BL_LIGHT);

	ExportReferenceNode((mffNode *)&light);
	
	if (light.m_type == mffLight::omni)
		fprintf(m_fp,"type omni\n");
	else if (light.m_type == mffLight::spot)
		fprintf(m_fp,"type spot\n");
	else //(light.m_type == mffLight::directional)
		fprintf(m_fp,"type directional\n");

	fprintf(m_fp,"intensity %f\n",light.m_intensity);
	light.m_color.export(m_fp);
	fprintf(m_fp,"%s\n",EL_LIGHT);	
}

void mffTools::ExportLightInstance(mffLightInstance &lightinstance)
{
	fprintf(m_fp,"%s\n",BL_LIGHTINSTANCE);
	ExportInstanceNode((mffNode *)&lightinstance);
	fprintf(m_fp,"%s\n",EL_LIGHTINSTANCE);
}

void mffTools::ExportNULL(mffNULL &Null)
{
	fprintf(m_fp,"%s\n",BL_NULL);
	ExportReferenceNode((mffNode*)&Null);
	fprintf(m_fp,"%s\n",EL_NULL);	
}

void mffTools::ExportNULLInstance(mffNULLInstance &nullinstance)
{
	fprintf(m_fp,"%s\n",BL_NULLINSTANCE);
	ExportInstanceNode((mffNode *)&nullinstance);
	fprintf(m_fp,"%s\n",EL_NULLINSTANCE);
}

void mffTools::ExportBone(mffBone &bone)
{
	fprintf(m_fp,"%s\n",BL_BONE);
	ExportReferenceNode((mffNode*)&bone);
	fprintf(m_fp,"%s\n",EL_BONE);	
}

void mffTools::ExportBoneInstance(mffBoneInstance &boneinstance)
{
	fprintf(m_fp,"%s\n",BL_BONEINSTANCE);
	ExportInstanceNode((mffNode *)&boneinstance);
	fprintf(m_fp,"%s\n",EL_BONEINSTANCE);
}

int mffTools::ImportAttributes()
{
	char line[MAX_LINE_SIZE],attrType[MAX_LINE_SIZE],name[MAX_LINE_SIZE],shortname[32],attrType2[MAX_LINE_SIZE];
	mffAttribute::mffAttrType2 type;
	unsigned attrcount = 0;
	fscanf(m_fp,"%s %i",line,&attrcount);

	for (unsigned i = 0; i < attrcount; i++)
	{
		fscanf(m_fp,"%s %s %s\n",attrType2,name,shortname);

		if (strcmp(attrType2,"[FACE]")==0)
			type = mffAttribute::kFACE;
		else if (strcmp(attrType2,"[OBJECT]")==0)
			type = mffAttribute::kOBJECT;
		else
			return 0;

		fscanf(m_fp,"[%s ",attrType);
		if (strcmp(attrType,"bool")==0)
		{
			mffBoolean mffbool;
			strcpy(mffbool.m_name,name);
			strcpy(mffbool.m_shortname,shortname);
			int value;
			fscanf(m_fp,"%i]\n",&value);
			mffbool.m_value = value ? true : false;
			mffbool.m_type = type;
			AddBoolAttribute(mffbool);
		}
		else if (strcmp(attrType,"int")==0)
		{
			mffInt mffint;
			strcpy(mffint.m_name,name);
			strcpy(mffint.m_shortname,shortname);
			fscanf(m_fp,"%i]\n",&mffint.m_value);
			mffint.m_type = type;
			AddIntAttribute(mffint);
		}
		else if (strcmp(attrType,"float")==0)
		{
			mffFloat mfffloat;
			strcpy(mfffloat.m_name,name);
			strcpy(mfffloat.m_shortname,shortname);
			fscanf(m_fp,"%f]\n",&mfffloat.m_value);
			mfffloat.m_type = type;
			AddFloatAttribute(mfffloat);
		}
		else if (strcmp(attrType,"double")==0)
		{
			mffDouble mffdouble;
			strcpy(mffdouble.m_name,name);
			strcpy(mffdouble.m_shortname,shortname);
			float temp;
			fscanf(m_fp,"%f]\n",&temp);
			mffdouble.m_value = (double)temp;
			mffdouble.m_type = type;
			AddDoubleAttribute(mffdouble);
		}
		else if (strcmp(attrType,"string]")==0)
		{
			mffString mffstring;
			strcpy(mffstring.m_name,name);
			strcpy(mffstring.m_shortname,shortname);
			mffstring.import(m_fp);
			mffstring.m_type = type;
			AddStringAttribute(mffstring);	
		}
		else if (strcmp(attrType,"enum")==0)
		{
			mffEnum mffenum;
			strcpy(mffenum.m_name,name);
			strcpy(mffenum.m_shortname,shortname);

			char line[MAX_LINE_SIZE];

			fscanf(m_fp,"%i ",&mffenum.m_value);

			char c = fgetc(m_fp);

			int count = 0;

			while (c != ']')
			{
				if (c == ',')
				{
					line[count] = '\0';
					count = 0;
					mffString str(line);
					mffenum.m_names.push_back(str);
					c = fgetc(m_fp);
					continue;
				}
				line[count] = c;
				c = fgetc(m_fp);
				count++;
			}
			line[count] = '\0';

			mffString str(line);
			mffenum.m_names.push_back(str);
			fscanf(m_fp,"\n");
			mffenum.m_type = type;
			AddEnumAttribute(mffenum);	
		}
		else if (strcmp(attrType,"intarray")==0)
		{
			int count = 0;
			int value;
			mffIntArray mffintarray;
			strcpy(mffintarray.m_name,name);
			strcpy(mffintarray.m_shortname,shortname);
			fscanf(m_fp,"(%i)",&count);
			for (int i = 0; i < count-1; i++)
			{
				fscanf(m_fp,"%i",&value);
				mffintarray.push_back(value);
			}
			fscanf(m_fp,"%i ]\n",&value);
			mffintarray.push_back(value);
			mffintarray.m_type = type;
			AddIntArrayAttribute(mffintarray);
		}
		else if (strcmp(attrType,"dblintarray")==0)
		{
			int count = 0;
			int value,id;
			mffDblIntArray mffdblintarray;
			strcpy(mffdblintarray.m_name,name);
			strcpy(mffdblintarray.m_shortname,shortname);
			fscanf(m_fp,"id:%i (%i)",&id,&count);
			for (int i = 0; i < count; i++)
			{
				fscanf(m_fp,"%i",&value);
				mffdblintarray.m_values1.push_back(value);
			}
			fscanf(m_fp,"\n(%i)",&count);
			for (i = 0; i < count-1; i++)
			{
				fscanf(m_fp,"%i",&value);
				mffdblintarray.m_values2.push_back(value);
			}
			fscanf(m_fp,"%i ]\n",&value);
			mffdblintarray.m_values2.push_back(value);
			mffdblintarray.m_type = type;
			mffdblintarray.m_id = id;
			AddDblIntArrayAttribute(mffdblintarray);
		}
	}
	return 1;
}

void mffTools::ExportAttributes()
{
	fprintf(m_fp,"%s\n",BL_ATTRIBUTES);
	int numAttributes = GetNumAttributes();
	fprintf(m_fp,"count %i\n",numAttributes);
	
	for (int i = 0; i < numAttributes; i++)
	{
		mffAttribute *attr = m_attributes[i];
		if (attr->GetAttrType() == mffAttribute::kINVALID)
		{
			attr->exportName(m_fp);
			attr->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kBOOLEAN)
		{
			mffBoolean *mffbool = (mffBoolean*)attr;
			mffbool->exportName(m_fp);
			mffbool->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kINTEGER)
		{
			mffInt *mffint = (mffInt*)attr;
			mffint->exportName(m_fp);
			mffint->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kFLOAT)
		{
			mffFloat *mfffloat = (mffFloat*)attr;
			mfffloat->exportName(m_fp);
			mfffloat->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kDOUBLE)
		{
			mffDouble *mffdouble = (mffDouble*)attr;
			mffdouble->exportName(m_fp);
			mffdouble->export(m_fp);
		}

		else if (attr->GetAttrType() == mffAttribute::kVECTOR)
		{
			mffVector *mffvect = (mffVector*)attr;
			mffvect->exportName(m_fp);
			mffvect->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kSTRING)
		{
			mffString *mffstr = (mffString*)attr;
			mffstr->exportName(m_fp);
			mffstr->export("[string]",m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kENUM)
		{
			mffEnum *mffenum = (mffEnum*)attr;
			mffenum->exportName(m_fp);
			mffenum->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kINTARRAY)
		{
			mffIntArray *mffintarray = (mffIntArray*)attr;
			mffintarray->exportName(m_fp);
			mffintarray->export(m_fp);
		}
		else if (attr->GetAttrType() == mffAttribute::kDBLINTARRAY)
		{
			mffDblIntArray *mffdblintarray = (mffDblIntArray*)attr;
			mffdblintarray->exportName(m_fp);
			mffdblintarray->export(m_fp);
		}
	}
	fprintf(m_fp,"%s\n",EL_ATTRIBUTES);
}

mffPolyModel *mffTools::GetPolyModel(const unsigned index)
{
	mffNode *node = (mffNode*)m_references[index];

	if (node->GetNodeType() == kMODEL)
	{
		mffModel *model = (mffModel*)m_references[index];
		
		if (model->GetModelType() == kPOLYGONMODEL)
			return (mffPolyModel*)model;
	}

	return NULL;
}

mffPolyModelInstance *mffTools::GetPolyModelInstance(const unsigned index)
{
	mffNode *node = (mffNode*)m_instances[index];

	if (node->GetNodeType() == kMODELINSTANCE)
	{
		mffModelInstance *model;
		
		model = (mffModelInstance*)m_instances[index];

		if (model->GetModelType() == kPOLYGONMODELINSTANCE)
			return (mffPolyModelInstance*)model;
	}

	return NULL;
}

mffGroupTransform *mffTools::GetGroupTransform(const unsigned index)
{
	mffNode *node = (mffNode*)m_references[index];

	if (node->GetNodeType() == kGROUPTRANSFORM)
	{		
		return (mffGroupTransform*)node;
	}

	return NULL;
}

mffGroupTransformInstance *mffTools::GetGroupTransformInstance(const unsigned index)
{
	mffNode *node = (mffNode*)m_instances[index];

	if (node->GetNodeType() == kGROUPTRANSFORMINSTANCE)
	{		
		return (mffGroupTransformInstance*)node;
	}

	return NULL;
}

int mffTools::AddPolyModel(mffPolyModel &polymodel)
{
	mffPolyModel *pPolyModel = new mffPolyModel;

	*pPolyModel = polymodel;

	pPolyModel->m_index = m_references.size();

	m_references.push_back(pPolyModel);

	return m_references.size()-1;
}


int mffTools::AddPolyModelInstance(mffPolyModelInstance &polymodelinstance,mffIntArray &attrindicies)
{
	mffPolyModelInstance *pPolyModelInstance = new mffPolyModelInstance;

	*pPolyModelInstance = polymodelinstance;

	pPolyModelInstance->m_index = m_instances.size();

	m_instances.push_back(pPolyModelInstance);
	
	// copy attribute indicies
	pPolyModelInstance->m_attrindicies = attrindicies;

	return m_instances.size()-1;
}

int mffTools::AddCurveModel(mffCurveModel &curvemodel)
{
	mffCurveModel *pCurveModel = new mffCurveModel;

	*pCurveModel = curvemodel;

	pCurveModel->m_index = m_references.size();

	m_references.push_back(pCurveModel);

	return m_references.size()-1;
}

int mffTools::AddCurveModelInstance(mffCurveModelInstance &curvemodelinstance)
{
	mffCurveModelInstance *pCurveModelInstance = new mffCurveModelInstance;

	*pCurveModelInstance = curvemodelinstance;

	pCurveModelInstance->m_index = m_instances.size();

	m_instances.push_back(pCurveModelInstance);

	return m_instances.size()-1;
}

int mffTools::AddVertexColorNode(mffVertexColorNode &vcnode)
{
	mffVertexColorNode *pVCnode = new mffVertexColorNode;
	*pVCnode = vcnode;
	pVCnode->m_index = m_references.size();
	m_references.push_back(pVCnode);
	return m_references.size()-1;
}

int mffTools::AddShaderTableNode(mffShaderTableNode &shadertablenode)
{
	mffShaderTableNode *pShaderTableNode = new mffShaderTableNode;
	*pShaderTableNode = shadertablenode;
	pShaderTableNode->m_index = m_references.size();
	m_references.push_back(pShaderTableNode);
	return m_references.size()-1;
}

int mffTools::AddMapTableNode(mffMapTableNode &maptablenode)
{
	mffMapTableNode *pMapTableNode = new mffMapTableNode;
	*pMapTableNode = maptablenode;
	pMapTableNode->m_index = m_references.size();
	m_references.push_back(pMapTableNode);
	return m_references.size()-1;
}

int mffTools::AddDisplayLayerTableNode(mffDisplayLayerTableNode &displaylayertablenode)
{
	mffDisplayLayerTableNode *pDispalyLayerTableNode = new mffDisplayLayerTableNode;
	*pDispalyLayerTableNode = displaylayertablenode;
	pDispalyLayerTableNode->m_index = m_references.size();
	m_references.push_back(pDispalyLayerTableNode);
	return m_references.size()-1;
}

int mffTools::AddLight(mffLight &light)
{
	mffLight *pLight = new mffLight;
	*pLight = light;
	pLight->m_index = m_references.size();
	m_references.push_back(pLight);
	return m_references.size()-1;
}

int mffTools::AddLightInstance(int parentindex,mffLight &light)
{
	int refidx = AddLight(light);

	mffLightInstance *pLightInstance = new mffLightInstance;

	// copy group name to instance name
	pLightInstance->m_name = light.m_name;
	pLightInstance->m_index = m_instances.size();

	// assign reference index
	pLightInstance->m_refidx = refidx;

	// assign parent child relationships
	
	mffNode *parent = NULL;
	if (parentindex>=0)
		parent = GetInstanceNode(parentindex);

	pLightInstance->m_parentindex = parentindex;
	
	m_instances.push_back(pLightInstance);

	if (parent)
		parent->m_childindicies.push_back(m_instances.size()-1);

	return m_instances.size()-1;
}

int mffTools::AddGroupTransform(mffGroupTransform &group)
{
	mffGroupTransform *pGroup = new mffGroupTransform;
	*pGroup = group;
	pGroup->m_index = m_references.size();
	m_references.push_back(pGroup);
	return m_references.size()-1;
}

int mffTools::AddGroupTransformInstance(int parentindex,
										int layerindex,
										mffGroupTransform &group,
										mffIntArray &attrindicies)
{
	int refidx = AddGroupTransform(group);

	mffGroupTransformInstance *pGroupTransformInstance = new mffGroupTransformInstance;

	// copy group name to instance name
	pGroupTransformInstance->m_name = group.m_name;
	pGroupTransformInstance->m_index = m_instances.size();

	// assign reference index
	pGroupTransformInstance->m_refidx = refidx;

	// assign parent child relationships
	
	mffNode *parent = NULL;
	if (parentindex>=0)
		parent = GetInstanceNode(parentindex);

	pGroupTransformInstance->m_parentindex = parentindex;
	
	m_instances.push_back(pGroupTransformInstance);

	if (parent)
		parent->m_childindicies.push_back(m_instances.size()-1);

	// copy attribute indicies
	pGroupTransformInstance->m_attrindicies = attrindicies;

	pGroupTransformInstance->m_layerindex = layerindex;

	return m_instances.size()-1;
}

int mffTools::AddNULL(mffNULL &Null)
{
	mffNULL *pNULL = new mffNULL;
	*pNULL = Null;
	pNULL->m_index = m_references.size();
	m_references.push_back(pNULL);
	return m_references.size()-1;
}

int mffTools::AddNULLInstance(int parentindex,mffNULL &Null,mffIntArray &attrindicies)
{
	int refidx = AddNULL(Null);

	mffNULLInstance *pNULLInstance = new mffNULLInstance;

	// copy group name to instance name
	pNULLInstance->m_name = Null.m_name;
	pNULLInstance->m_index = m_instances.size();

	// assign reference index
	pNULLInstance->m_refidx = refidx;

	// assign parent child relationships
	
	mffNode *parent = NULL;
	if (parentindex>=0)
		parent = GetInstanceNode(parentindex);

	pNULLInstance->m_parentindex = parentindex;
	
	m_instances.push_back(pNULLInstance);

	if (parent)
		parent->m_childindicies.push_back(m_instances.size()-1);

	// copy attribute indicies
	pNULLInstance->m_attrindicies = attrindicies;

	return m_instances.size()-1;
}

int mffTools::AddBone(mffBone &bone)
{
	mffBone *pBone = new mffBone;
	*pBone = bone;
	pBone->m_index = m_references.size();
	m_references.push_back(pBone);
	return m_references.size()-1;
}

int mffTools::AddBoneInstance(int parentindex,mffBone &bone)
{
	int refidx = AddBone(bone);

	mffBoneInstance *pBoneInstance = new mffBoneInstance;

	// copy group name to instance name
	pBoneInstance->m_name = bone.m_name;
	pBoneInstance->m_index = m_instances.size();

	// assign reference index
	pBoneInstance->m_refidx = refidx;

	// assign parent child relationships
	
	mffNode *parent = NULL;
	if (parentindex>=0)
		parent = GetInstanceNode(parentindex);

	pBoneInstance->m_parentindex = parentindex;
	
	m_instances.push_back(pBoneInstance);

	if (parent)
		parent->m_childindicies.push_back(m_instances.size()-1);

	return m_instances.size()-1;
}

int mffTools::AddBoolAttribute(mffBoolean &attribute)
{
	mffBoolean *pAttr = new mffBoolean;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddIntAttribute(mffInt &attribute)
{
	mffInt *pAttr = new mffInt;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddFloatAttribute(mffFloat &attribute)
{
	mffFloat *pAttr = new mffFloat;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddDoubleAttribute(mffDouble &attribute)
{
	mffDouble *pAttr = new mffDouble;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddStringAttribute(mffString &attribute)
{
	mffString *pAttr = new mffString;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddEnumAttribute(mffEnum &attribute)
{
	mffEnum *pAttr = new mffEnum;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddIntArrayAttribute(mffIntArray &attribute)
{
	mffIntArray *pAttr = new mffIntArray;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::AddDblIntArrayAttribute(mffDblIntArray &attribute)
{
	mffDblIntArray *pAttr = new mffDblIntArray;
	*pAttr = attribute;
	m_attributes.push_back(pAttr);
	return m_attributes.size()-1;
}

int mffTools::GetNumInstancesOfReference(mffNode *node)
{
	int cnt = 0;
	for (int i = 0; i  < GetNumInstances(); i++)
	{
		mffNode *inode = GetInstanceNode(i);
		if (node->m_refidx == inode->m_refidx)
			cnt++;
	}
	return cnt;
}