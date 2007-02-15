// KB  5/24/00  \Ken85\Plugin\Shaders\Gradient\Serializ.cpp

#include "stdafx.h"
#include "AGPShader.h"

BOOL AGPShader::Save(char *&label, char *&value, int count)
{
   static char text[256];
	switch (count) {
	case 0:
		label = "[AGP]Project";
		sprintf( text, "%d, %s", m_project, m_projecttext);
		value = text;
		return TRUE;
	case 1:
		label = "[AGP]TerrainText";
		sprintf( text, "%s", m_terraintext);
		value = text;
		return TRUE;
	case 2:
		label = "[AGP]Collision";
		sprintf( text, "%d", m_collision);
		value = text;
		return TRUE;
	case 3:
		label = "[AGP]Terrain";
		sprintf( text, "%d", m_terrain);
		value = text;
		return TRUE;
	case 4:
		label = "[AGP]Invisible";
		sprintf( text, "%s", m_invisible?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 5:
		label = "[AGP]EnvironMapPercent";
		sprintf( text, "%d", m_environmentmappercent);
		value = text;
		return TRUE;
	case 6:
		label = "[AGP]EnvironMapPath";
		sprintf( text, "%s", m_environmappath);
		value = text;
		return TRUE;
	case 7:
		label = "[AGP]Pretess";
		sprintf( text, "%f", m_pretess);
		value = text;
		return TRUE;
	case 8:
		label = "[AGP]Layer";
		sprintf( text, "%d", m_layer);
		value = text;
		return TRUE;
	case 9:
		label = "[AGP]Flat";
		sprintf( text, "%s", m_flat?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 10:
		label = "[AGP]DNClip";
      sprintf( text, "%s", m_dnclip?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 11:
		label = "[AGP]DNFog";
      sprintf( text, "%s", m_dnfog?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 12:
		label = "[AGP]Twosided";
      sprintf( text, "%s", m_twosided?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 13:
		label = "[AGP]ZBias";
      sprintf( text, "%s", m_zbias?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 14:
		label = "[AGP]LOD";
      sprintf( text, "%s", m_lod?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 15:
		label = "[AGP]EnvironMap";
      sprintf( text, "%s", m_environmap?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 16:
		label = "[AGP]DeletePatch";
      sprintf( text, "%s", m_deletepatch?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 17:
		label = "[AGP]EnvironMapPrivate";
		sprintf( text, "%s", m_private?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 18:
		label = "[AGP]TessAdj";
		sprintf( text, "%d", m_tessadj);
		value = text;
		return TRUE;
	case 19:
		label = "[AGP]EnvironMapAlpha";
		sprintf( text, "%s", m_alpha?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 20:
		label = "[AGP]OverrideColor";
		sprintf( text, "%s", m_coloroverride?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	case 21:
		label = "[AGP]32BitVertexColor";
		sprintf( text, "%s", m_32bitvc?"TRUE":"FALSE" );
		value = text;
		return TRUE;
	}
   return FALSE;
}

BOOL AGPShader::ParseArg( const char *label, const char *value )
{
	static char text[256];
	bool			tfound=false;
	bool			pfound=false;
	HKEY			hKey;
	char			path[_MAX_PATH];
	unsigned long	iSize = _MAX_PATH;
	long			ret;
	unsigned long	dword;
	FILE			*fp;

	if (strcmp(label, "[AGP]Project")==0)
	{
		sscanf( value, "%d, %s", &m_project, m_projecttext);
//		strcpy(m_projecttext,value);
		return TRUE;
	}
	if (strcmp(label, "[AGP]Collision")==0)
	{
		sscanf( value, "%d", &m_collision);
		return TRUE;
	}
	if (strcmp(label, "[AGP]TerrainText")==0)
	{
		if (value==NULL)
			return TRUE;
		sscanf( value, "%s", &m_terraintext);
		return TRUE;
	}
	if (strcmp(label, "[AGP]Terrain")==0)
	{
		ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Hash, Inc.\\Animation Master 2000 V8.5",0,KEY_READ,&hKey);

		sscanf( value, "%d", &m_terrain);

		if (ret==ERROR_SUCCESS)
		{
			ret=RegQueryValueEx(hKey,"InstallationDir",NULL,&dword,(unsigned char *)path,&iSize);
			if (ret==ERROR_SUCCESS)
			{
				strcat(path,"\\shaders\\terraintypes.txt");
				if((fp=fopen(path,"rb"))!=NULL)
				{
					int iIndex=0;
					do{
						char buf[250];
						fgets(buf,250,fp);
						//Strip off CR/LF
						for (unsigned int i=0; i<strlen(buf); i++)
							if (buf[i]<16)
								buf[i]=0;
	
						if (buf[0]=='[')
						{
							if (strcmp(m_projecttext,"<NONE>")==0)
							{
								strcpy(m_projecttext,buf);
								pfound=true;
							} else
							{
								if (strcmp(m_projecttext,buf)==0)
								{
									pfound=true;
								} else
									pfound=false;
							}
						} else
						{
							if (pfound)
							{
								if (iIndex==m_terrain)
								{
									if (strcmp(m_terraintext,"<NONE>")==0)
										strcpy(m_terraintext,buf);
								}
								iIndex++;
							}
						}
					} while(!feof(fp));
					//If we've gone through the entire list, and the project wasn't found, default to the first
					//project in the list
					if (!pfound)
					{
						fseek(fp,0,SEEK_SET);
						int iIndex=0;
						do{
							char buf[250];
							fgets(buf,250,fp);
							//Strip off CR/LF
							for (unsigned int i=0; i<strlen(buf); i++)
								if (buf[i]<16)
									buf[i]=0;
	
							if (buf[0]=='[')
							{
								if (!pfound)
								{
									strcpy(m_projecttext,buf);
									pfound=true;
								}
							} else
							{
								if (pfound)
								{
									if (iIndex==m_terrain)
									{
										if (strcmp(m_terraintext,"<NONE>")==0)
											strcpy(m_terraintext,buf);
									}
									iIndex++;
								}
							}
						} while(!feof(fp));
					}
					fclose(fp);
				}
			}
		}
		return TRUE;
	}
	if (strcmp(label, "[AGP]Invisible")==0)
	{
      if (strcmp(value, "TRUE") == 0)
         m_invisible = TRUE;
      else
         m_invisible = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]EnvironMapPercent")==0)
	{
		sscanf( value, "%d", &m_environmentmappercent);
		return TRUE;
	}
	if (strcmp(label, "[AGP]Pretess")==0)
	{
		sscanf( value, "%f", &m_pretess);
		return TRUE;
	}
	if (strcmp(label, "[AGP]EnvironMapPath")==0)
	{
		strcpy(m_environmappath,value);
		return TRUE;
	}
	if (strcmp(label, "[AGP]Layer")==0)
	{
		sscanf( value, "%d", &m_layer);
		return TRUE;
	}
	if (strcmp(label, "[AGP]Flat") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_flat = TRUE;
      else
         m_flat = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]DNClip") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_dnclip = TRUE;
      else
         m_dnclip = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]DNFog") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_dnfog = TRUE;
      else
         m_dnfog = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]Twosided") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_twosided = TRUE;
      else
         m_twosided = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]ZBias") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_zbias = TRUE;
      else
         m_zbias = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]LOD") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_lod = TRUE;
      else
         m_lod = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]EnvironMap") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_environmap = TRUE;
      else
         m_environmap = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]DeletePatch") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_deletepatch = TRUE;
      else
         m_deletepatch = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]EnvironMapPrivate") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_private = TRUE;
      else
         m_private = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]TessAdj")==0)
	{
		sscanf( value, "%d", &m_tessadj);
		return TRUE;
	}
	if (strcmp(label, "[AGP]EnvironMapAlpha") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_alpha = TRUE;
      else
         m_alpha = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]OverrideColor") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_coloroverride = TRUE;
      else
         m_coloroverride = FALSE;
	   return TRUE;
	}
	if (strcmp(label, "[AGP]32BitVertexColor") == 0) {
      if (strcmp(value, "TRUE") == 0)
         m_32bitvc = TRUE;
      else
         m_32bitvc = FALSE;
	   return TRUE;
	}
	return FALSE;
}

