////////////////////////////////////////////////////////////////////////////
//
// TupImpExp
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImpExp.h $
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:29p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * motion path exporting
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * animation globals
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 10:23a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add units
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 10:35a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export characters and clips
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first phase of animation export
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 3:53p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * layered texture output
 * uv sets output
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:09p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex alpha added to apply vc modifier
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 12:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * face attributes
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 12/12/02   Time: 8:55a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix mesh instance matrix to use delta matrix from root bone
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 12/10/02   Time: 2:18p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add bones that are not part of joint clusters
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 12/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone matrix output correct
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 4:17p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix bone name
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 11/15/02   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex color modifiers
 * able to disable object consolidation
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 3:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bezier shape
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * attribute fixes
 * consolidate based on name (ignore numbers on end)
 * save material from mesh to use on other instances
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/29/02   Time: 8:58a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * starting point for tup exporter
 */

#ifndef TUP_IMP_EXP
#define TUP_IMP_EXP

class SkinManager;
class MeshManager;
class TupTranslatorAddShaders;

class TupImpExp : public MPxFileTranslator {
public:
                    TupImpExp () {};
    virtual         ~TupImpExp () {};


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
};

#endif