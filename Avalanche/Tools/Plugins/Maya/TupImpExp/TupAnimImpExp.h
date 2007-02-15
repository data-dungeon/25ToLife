////////////////////////////////////////////////////////////////////////////
//
// TupImpExp
//
// Tupperware animation importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUP_ANIM_IMP_EXP
#define TUP_ANIM_IMP_EXP

class TupAnimImpExp : public MPxFileTranslator {
public:
   TupAnimImpExp () {};
   virtual         ~TupAnimImpExp () {};


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