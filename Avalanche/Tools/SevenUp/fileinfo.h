#ifndef _FILEINFO_H_
#define _FILEINFO_H_

class FileNameEx
{
	public:
		FileNameEx( void );
		FileNameEx( const char *pFileName );

		void SetFileName( const char *pFileName );

	public:

		char* Path( void ) { return m_FilePath.get(); }
		char* Name( void ) { return m_FileName.get(); }
		char* Title( void ) { return m_FileTitle.get(); }
		char* Ext( void ) { return m_FileExt.get(); }
		char* FullName( void ) { return m_FullPath.get(); }

		filename m_FilePath;
		filename m_FileName;
		filename m_FileTitle;
		filename m_FileExt;
		filename m_FullPath;

	protected:
		void Parse( const char *pFileName );
};

#endif // _FILEINFO_H_