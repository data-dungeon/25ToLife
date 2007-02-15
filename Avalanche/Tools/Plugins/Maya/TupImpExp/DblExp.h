////////////////////////////////////////////////////////////////////////////
//
// TupAnimExport
//
// Tupperware animation exporter class
//
////////////////////////////////////////////////////////////////////////////

#ifndef DBL_EXPORT
#define DBL_EXPORT

class TupExport;

class DBLExport : MPxFileTranslator
{
public:
   MString m_filterchainpath;
   MString m_processfile;

public:
   DBLExport () {};
   virtual         ~DBLExport () {};

   static void*    creator();

   MStatus         writer ( const MFileObject& file,
      const MString& optionsString,
      FileAccessMode mode );

   bool            haveReadMethod () const { return false; }
   bool            haveWriteMethod () const { return true; }
   MString         defaultExtension () const { return "dbl"; }
   MFileKind       identifyFile ( const MFileObject& fileName,
      const char* buffer,
      short size) const;

   void ExportDBL(TupperwareAggregate *pSceneAgg,const char *filepathname);
};

#endif