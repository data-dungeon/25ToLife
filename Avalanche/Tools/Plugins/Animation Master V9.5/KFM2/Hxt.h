// IE.h : main header file for the IE DLL
//

#include <afxwin.h>
#include "resource.h"		// main symbols
#ifndef __STRCLASS_H
#include "StrClass.h"
#endif

#ifndef __CollectData_H
#include "..\avalancheexporter\collectdata.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
//
#define KFMVERSION  0x12345678+1
#define NUMCHANNELITEMS 10

class FileStream;
class HActionCache;
class HActionCacheContainer;
class HModelCache;
class HAnimObjectShortcut;
class HQuaternionRotateDriver;
class HChannelDriver;
class HModel;
class HDriverShortcut;
class HBone;

class CHxtApp : public CWinApp
{
public:
	CHxtApp();

// Export
   BOOL OnExportActionCache(HActionCache *hac);
   BOOL OnExportActionCacheContainer(HActionCacheContainer *hacc);
   void ReportExportError(const String &filename);

   BOOL ExportKFM(String &filename, float errortol, HActionCache *hac, HModelCache *hmc);
   void SaveBoneTransformDriver(FileStream &fs, HBone *bone, HAnimObjectShortcut *boneshortcut, float errortolerance, float scalefactor);
   BOOL SaveKeyFrames(FileStream &fs, HQuaternionRotateDriver *rotatedriver, HChannelDriver *translatedriver[], HChannelDriver *scaledriver[], float scalefactor);
   void SaveChannelDriver(FileStream &fs, HChannelDriver *channeldriver, float unit);

// Import
   BOOL OnImportActionCache(HActionCache *hac);
   BOOL OnImportActionCacheContainer(HActionCacheContainer *hacc);
   void ReportImportError(const String &filename);
   BOOL ImportKFM(const String &filename, HActionCache *hac);

   // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHxtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHxtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	void SaveAnimatedVertices(FileStream &fs, HModel *hm,HActionCache *action,HModelCache *hmc, float scalefactor);
	bool SaveVertexBone(FileStream &fs,HCP *cp,HModel *hm,HActionCache *action, float scalefactor);
	HDriverShortcut *GetCPTranslateDriver(const char *cpname,HActionCache *action);
	int CountVertexBones(HModel *hm,HActionCache *action);
	bool isValidCP(HCP *cp,HActionCache *action);

public:
	CollectData m_collectdata;	
	int m_vertexbonesexported;
	int m_vertexbonecount;

};


/////////////////////////////////////////////////////////////////////////////
