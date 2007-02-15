// KB  8/8/02  \Ken100\Plugin\HXT\KFM\Hxt.h
//

#include <afxwin.h>
#include "resource.h"		// main symbols
#ifndef __STRCLASS_H
#include "StrClass.h"
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

public:
	int m_exportframerange;
	int m_beginframe,m_endframe;

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
};


/////////////////////////////////////////////////////////////////////////////
