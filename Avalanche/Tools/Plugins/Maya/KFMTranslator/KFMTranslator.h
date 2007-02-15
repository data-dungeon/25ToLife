
#define REQUIRE_IOSTREAM

#include <maya/MPxFileTranslator.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDGModifier.h>
#include <maya/MSelectionlist.h>
#include "../../libraries/MTools/IntHash.h"

#include <vector>

#include "kfm.h"

class MQuaternion;
class MVector;
class MSelectionlist;
class MObject;

class AnimChannelMObjects
{
public:
	AnimChannelMObjects()
	{
		// translation
		transcurveobjectx = MObject::kNullObj;
		transcurveobjecty = MObject::kNullObj;
		transcurveobjectz = MObject::kNullObj;

		//rotation
		rotcurveobjectx = MObject::kNullObj;
		rotcurveobjecty = MObject::kNullObj;
		rotcurveobjectz = MObject::kNullObj;

		// scale
		scalcurveobjectx = MObject::kNullObj;
		scalcurveobjecty = MObject::kNullObj;
		scalcurveobjectz = MObject::kNullObj;
	}

	
	MObject transcurveobjectx,transcurveobjecty,transcurveobjectz;
	MObject rotcurveobjectx,rotcurveobjecty,rotcurveobjectz;
	MObject scalcurveobjectx,scalcurveobjecty,scalcurveobjectz;

	AnimChannelMObjects &operator = (const AnimChannelMObjects &other)
	{
		// translation
		transcurveobjectx = transcurveobjectx;
		transcurveobjecty = transcurveobjecty;
		transcurveobjectz = transcurveobjectz;

		//rotation
		rotcurveobjectx = rotcurveobjectx;
		rotcurveobjecty = rotcurveobjecty;
		rotcurveobjectz = rotcurveobjectz;

		// scale
		scalcurveobjectx = scalcurveobjectx;
		scalcurveobjecty = scalcurveobjecty;
		scalcurveobjectz = scalcurveobjectz;
		return *this;
	}	
	
};

class KFMTranslator : public MPxFileTranslator
{
public:
                    KFMTranslator () {};
    virtual         ~KFMTranslator () {};


	static void*    creator();

    MStatus         reader ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode);

    MStatus         writer ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode );

    bool            haveReadMethod () const;
    bool            haveWriteMethod () const;
    MString         defaultExtension () const;
    MFileKind       identifyFile ( const MFileObject& fileName,
                                   const char* buffer,
                                   short size) const;

    MStatus         ExportSelected();
    MStatus         ExportAll();

	
	int				ReadKFM(const char *filename);
	int				ReadVector(FILE *fp,Vector &vect);
	int				ReadVector2(FILE *fp,Vector2 &vect);
	int				ReadString(FILE *fp,char *str,int len);

	int				WriteKFM(const char *filename);
	void				WriteVector(FILE *fp,Vector vect);
	void				WriteVector2(FILE *fp,Vector2 vect);
	void				WriteString(FILE *fp,char *str);

	int				BuildKFM();

protected:
	MString			HashBoneNameToMayaBoneName(const char *bonename);
	MString			MayaBoneNameToHashBoneName(const char *bonename);
	
   // import
	int				BuildAnimCurves(Channel &channel,MDagPath dagpath);
   MStatus        HookUpSourceClip(MObject object,const MSelectionList &bones,MObject &characterobject,MString sourceclipname,MObject &sourceclipobject);
   void           HookUpAnimCurvesToSourceClip(MObject characterobject,MObject sourceclipobject);

   // export
	int				BuildChannel(MDagPath bonedagpath);
	int				GetChannelCurves(MDagPath bonedagpath,Channel &channel);
	void			   FillInEmptyChannelItem(std::vector<Keyframe> &keyframes,double value,bool isScale = false);
	
public:
	Time m_length;
	std::vector<Channel> m_channellist;

	IntHash m_bonemap;
	std::vector<AnimChannelMObjects> m_animchannels;
	bool m_forcenewcharacter;
	
	bool m_applytobones;
};

extern const char *const KFMOptionScript;
extern const char *const KFMDefaultOptions;

// MACROS

#define IF_ERROR(ERR)			\
   if (!ERR)						\
{										\
   fclose(fp);					   \
   return 0;						\
}

#define EOF_ERROR()				\
   if (feof(fp))					\
{										\
   fclose(fp);						\
   return 0;						\
}

#define RETURN_ERROR()			\
{										\
   m_animchannels.clear();		\
   return MS::kFailure;			\
}


