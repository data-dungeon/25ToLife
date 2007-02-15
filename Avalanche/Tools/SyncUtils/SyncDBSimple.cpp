////////////////////////////////////////////////////////////////////////////
//
// SyncDBSimple
//
// Simple class for sync databases
//
////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncDBSimple.h"
#include "TreeFactory.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"
#include "TreeIterator.h"
#include "LoadDBFeedback.h"
#include "SyncFeedback.h"
#include "GetCRC.h"
#include "LogFile.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define FAKE_SYNC

static int CompareTimes(FILETIME leftTime,FILETIME rightTime)
{
	__int64 leftValue = *((__int64*)&leftTime);
	__int64 rightValue = *((__int64*)&rightTime);
	__int64 diff = leftValue-rightValue;
	__int64 threshold = 2*10000000; // 2 seconds * 10 million (100 nanoseconds periods)
	if (diff<-threshold)
	{
		return -1; // left is older than right
	}
	else if (diff>threshold)
	{
		return 1; // right is older then left
	}
	else
	{
		return 0; // left and right are within the threshold
	}
}

SyncDBSimple::Feedback::Feedback(LoadDBFeedback &loadDBFeedback) : m_loadDBFeedback(loadDBFeedback)
{
	m_startTime = CTime::GetCurrentTime();
	m_stats.m_totalFilesScanned = 0;
	m_stats.m_fileName.Empty();
	m_stats.m_errorMessage.Empty();
	m_errorCode = NULL;
}

bool SyncDBSimple::Feedback::Update(const char *pFileName,const char *pErrorMessage)
{
	// update display only so often
	__time64_t elapsedTime = CTime::GetCurrentTime().GetTime() - m_startTime.GetTime();
	if (!m_stats.m_totalFilesScanned || elapsedTime>0 || pErrorMessage)
	{
		if (pFileName)
		{
			m_stats.m_fileName = pFileName;
		}

		if (pErrorMessage)
		{
			m_stats.m_errorMessage = pErrorMessage;
		}
		else
		{
			m_stats.m_errorMessage.Empty();
		}

		m_startTime = CTime::GetCurrentTime();
		m_loadDBFeedback.Update(m_stats);
	}
	m_stats.m_totalFilesScanned++;
	return !m_loadDBFeedback.GetAbortFlag();
}

bool SyncDBSimple::Feedback::GetAbortFlag(void) const
{
	return m_loadDBFeedback.GetAbortFlag();
}

DWORD SyncDBSimple::Feedback::GetErrorCode(void) const
{
	return m_errorCode;
}

void SyncDBSimple::Feedback::SetErrorCode(DWORD errorCode)
{
	m_errorCode = errorCode;
}

const char *SyncDBSimple::Feedback::GetLastFileName(void) const
{
	return m_stats.m_fileName;
}


SyncDBSimple::SyncDBSimple(const char *pFilePath,LoadDBFeedback &loadDBFeedback,LogFile &logFile) : 
	SyncDBBase(pFilePath),
	m_pTreeRoot(NULL),
	m_logFile(logFile)
{
	SetupDatabase(loadDBFeedback);
	m_pGetCRC = new GetCRC;
	m_pGetCRC->Init();
}

SyncDBSimple::~SyncDBSimple()
{
	delete m_pTreeRoot;
	delete m_pGetCRC;
}

bool SyncDBSimple::IsRemoteDB(void) const
{
	return m_bRemoteDB;
}

TreeNodeFolder *SyncDBSimple::GetTreeRoot(void) const
{
	return m_pTreeRoot;
}

void SyncDBSimple::SetupDatabase(LoadDBFeedback &loadDBFeedback)
{
	delete m_pTreeRoot;
	Feedback feedBack(loadDBFeedback);

	m_bRemoteDB = RemotePath(m_filePath);

	int retryCount = 5;
	while(1)
	{
		m_pTreeRoot = m_treeFactory.CreateTreeNodeFolder();
		m_pTreeRoot->SetFileName(m_filePath);

		bool result = GetNodeChildren(m_pTreeRoot,m_filePath,feedBack);
		if (!result)
		{
			delete m_pTreeRoot;
			m_pTreeRoot = NULL;

			// if the user aborted
			if (feedBack.GetAbortFlag())
			{
				feedBack.Update("","User canceled scan");
				return;
			}
			// if an error occured
			else
			{
				char buf[80];
				FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM, NULL, feedBack.GetErrorCode(), LANG_USER_DEFAULT, buf, sizeof (buf) - 1, NULL );
				TupString errorString;
				errorString.Format("Error scanning %s (%s)",feedBack.GetLastFileName(),buf);
				errorString.Remove('\n');
				errorString.Remove('\r');
				feedBack.Update("",(const char *)errorString);
			}
			retryCount--;
			if (retryCount)
			{
				feedBack.Update("","Attempting to retry scan");
			}
			else
			{
				feedBack.Update(m_filePath,"Aborting after trying to scan 5 times");
				return;
			}
		}
		else
		{
			// get out of loop since all is well
			break;
		}
	}
	SortChildren(m_pTreeRoot,true);
}

bool SyncDBSimple::GetNodeChildren(TreeNodeFolder *pTreeNodeFolder,const char *pFilePathString,Feedback &feedback)
{
	CFileFind finder;
	TupString fileName(pFilePathString);
	fileName += "\\*.*";
	BOOL bWorking = finder.FindFile(fileName);
	// if there is an error here stop
	if (!bWorking)
	{
		feedback.SetErrorCode(GetLastError());
		return false;
	}
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		// did FindNextFile returned 0 and getlasterror says there is an error
		// we have a problem...
		if (!bWorking && GetLastError() != ERROR_NO_MORE_FILES)
		{
			feedback.SetErrorCode(GetLastError());
			return false;
		}
		if (!finder.IsDots())
		{
			TreeNodeBase *pNodeBase;
			// directory
			if (finder.IsDirectory())
			{
				TreeNodeFolder *pNewFolder = m_treeFactory.CreateTreeNodeFolder();
				bool result = GetNodeChildren(pNewFolder,finder.GetFilePath(),feedback);
				if (!result)
				{
					delete pNewFolder;
					return false;
				}
				pNodeBase = pNewFolder;
			}
			// file
			else
			{
				TupString childFilename = finder.GetFileName();
				// if this is the log file then skip
				if (childFilename.CompareNoCase(m_logFile.GetLogFileName())==0)
				{
					continue;
				}

				TreeNodeFile *pNewFile = m_treeFactory.CreateTreeNodeFile();
				pNewFile->SetFileLength((DWORD)finder.GetLength());
				pNodeBase = pNewFile;
			}
			unsigned long attributes = 0;
			if (finder.IsNormal())
				attributes |= TreeNodeBase::ATTRIBUTE_NORMAL;
			if (finder.IsHidden())
				attributes |= TreeNodeBase::ATTRIBUTE_HIDDEN;
			if (finder.IsSystem())
				attributes |= TreeNodeBase::ATTRIBUTE_SYSTEM;
			if (finder.IsReadOnly())
				attributes |= TreeNodeBase::ATTRIBUTE_READ_ONLY;
			pNodeBase->SetAttributes(attributes);

			pTreeNodeFolder->AddChild(pNodeBase);
			pNodeBase->SetFileName(finder.GetFileName());

			bool result = feedback.Update(pNodeBase->GetFileName());
			if (!result)
				return false;

			FILETIME tempFileTime;
			finder.GetLastWriteTime(&tempFileTime);
			pNodeBase->SetModifiedTime(tempFileTime);
		}
	}
	return true;
}

void SyncDBSimple::SortChildren(TreeNodeFolder *pTreeNodeFolder,bool bSortChildFolders)
{
	// first sort the children of the folder
	TreeNodeBase *pTreeNodeChild = pTreeNodeFolder->GetFirstChildNode();
	while (pTreeNodeChild)
	{
		TreeNodeBase *pTreeNodePrevChild = pTreeNodeChild->GetPrev();
		while(pTreeNodePrevChild && (CompareNodes(pTreeNodeChild,pTreeNodePrevChild)<0))
		{
			// swap with my prev sibling
			pTreeNodeChild->Detatch();
			pTreeNodeFolder->InsertChild(pTreeNodeChild,pTreeNodePrevChild,false); // insert prior to prev
			pTreeNodePrevChild = pTreeNodeChild->GetPrev(); // ready to try the next prev
		}
		pTreeNodeChild = pTreeNodeChild->GetNext(); // try text child
	}
	if (bSortChildFolders)
	{
		// next call the sort on all my child folders
		TreeIterator first(pTreeNodeFolder);
		TreeIterator last(pTreeNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			TreeNodeFolder *pChildFolder = dynamic_cast<TreeNodeFolder *>(*first);
			if (pChildFolder)
			{
				SortChildren(pChildFolder,true);
			}
		}
	}
}

// folders are less than files
// returns -1 if node1<node2
// returns 0 if node1==node2
// return 1 if node1>node2
int SyncDBSimple::CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2)
{
	TreeNodeFolder *pNodeFolder1 = dynamic_cast<TreeNodeFolder *>(pNode1);
	TreeNodeFolder *pNodeFolder2 = dynamic_cast<TreeNodeFolder *>(pNode2);

	// if both files or folders
	if ((pNodeFolder1 && pNodeFolder2)||(!pNodeFolder1 && !pNodeFolder2))
	{
		return stricmp(pNode1->GetFileName(),pNode2->GetFileName());
	}
	// if node1 is a folder, node2 is a file
	else if (pNodeFolder1)
	{
		return -1;
	}
	// if node2 is a folder, node1 is a file
	else 
	{
		return 1;
	}
}


TreeNodeFolder *SyncDBSimple::CreateDirectory(SyncDBBase *pOtherDB,TreeNodeFolder *pOtherFolder,TreeNodeFolder *pParentFolder,SyncFeedback &feedback,bool bRight)
{
	BOOL bResult;

	// if aborting then kick out
	if (feedback.GetAbortFlag())
		return NULL;

	TupString folderPath;
	pParentFolder->GetPath(folderPath);
	folderPath += "\\";
	folderPath += pOtherFolder->GetFileName();

	TupString logString;

	if (feedback.m_bIncludePath)
	{
		if (bRight)
			logString.Format("Creating Directory -> %s",(const char *)folderPath);
		else
			logString.Format("Creating Directory %s <-",(const char *)folderPath);
	}
	else
	{
		if (bRight)
			logString.Format("Creating Directory -> %s",pOtherFolder->GetFileName());
		else
			logString.Format("Creating Directory %s <-",pOtherFolder->GetFileName());
	}

	feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

	if (feedback.m_bPreviewMode)
	{
		feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");

		TreeNodeFolder *pNewFolder = m_treeFactory.CreateTreeNodeFolder();
		pNewFolder->SetFileName(pOtherFolder->GetFileName());
		pNewFolder->SetModifiedTime(pOtherFolder->GetModifiedTime());
		AddChildNodeSorted(pParentFolder,pNewFolder);
		return pNewFolder;
	}
	else
	{
		bResult = ::CreateDirectory(folderPath,NULL);
		if (bResult)
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
			// add to log file
			TupString sourcePath;
			pOtherFolder->GetPath(sourcePath);
			if (!m_logFile.AddFolderCreate(pOtherFolder->GetFileName(),folderPath,sourcePath))
			{
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
			}

			TreeNodeFolder *pNewFolder = m_treeFactory.CreateTreeNodeFolder();
			pNewFolder->SetFileName(pOtherFolder->GetFileName());
			pNewFolder->SetModifiedTime(pOtherFolder->GetModifiedTime());
			AddChildNodeSorted(pParentFolder,pNewFolder);
			return pNewFolder;
		}
		else
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Creating Directory\n%s\n",folderPath);
			return NULL;
		}
	}
}

static SyncFeedback *pFeedback;
static DWORD CALLBACK CopyProgressRoutine(
											  LARGE_INTEGER TotalFileSize,          // file size
											  LARGE_INTEGER TotalBytesTransferred,  // bytes transferred
											  LARGE_INTEGER StreamSize,             // bytes in stream
											  LARGE_INTEGER StreamBytesTransferred, // bytes transferred for stream
											  DWORD dwStreamNumber,                 // current stream
											  DWORD dwCallbackReason,               // callback reason
											  HANDLE hSourceFile,                   // handle to source file
											  HANDLE hDestinationFile,              // handle to destination file
											  LPVOID lpData                         // from CopyFileEx
											  )
{
	if (dwStreamNumber==1)
	{
		pFeedback->m_totalFileBytes = StreamSize.QuadPart;
		pFeedback->m_totalFileBytesCopied = StreamBytesTransferred.QuadPart;
		pFeedback->UpdateStats();
	}

	if (pFeedback->GetAbortFlag())
	{
		return PROGRESS_CANCEL;
	}
	else
	{
		return PROGRESS_CONTINUE;
	}
}

TreeNodeFile *SyncDBSimple::CopyFileToDB(SyncDBBase *pOtherDB,TreeNodeFile *pOtherFile,TreeNodeFolder *pParentFolder,TreeNodeFile *pFile,SyncFeedback &feedback,int flags,bool bRight)
{
	BOOL bResult;

	// if aborting then kick out
	if (feedback.GetAbortFlag())
		return NULL;

	// if file exists already in the database
	if (pFile)
	{
		TupString destFilePath;
		pFile->GetPath(destFilePath);
		TupString sourceFilePath;
		pOtherDB->GetPathToFile(pOtherFile,sourceFilePath);

		if (CheckFileInSync(pFile))
		{
			if (bRight)
				feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_LEFT_TO_RIGHT;
			else
				feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_RIGHT_TO_LEFT;

			feedback.m_sourceFile = sourceFilePath;
			feedback.m_destFile = destFilePath;

			TupString timeString = (flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME) ? "+" : "";
			TupString logString;

			if (feedback.m_bIncludePath)
			{
				if (bRight)
					logString.Format("Copying File -> %s%s",(const char *)destFilePath,(const char *)timeString);
				else
					logString.Format("Copying File %s%s <-",(const char *)destFilePath,(const char *)timeString);
			}
			else
			{
				if (bRight)
					logString.Format("Copying File -> %s%s",pFile->GetFileName(),(const char *)timeString);
				else
					logString.Format("Copying File %s%s <-",pFile->GetFileName(),(const char *)timeString);
			}
			feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

			if (feedback.m_bPreviewMode)
			{
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");

				FILETIME sourceFileTime = pOtherFile->GetModifiedTime();
				FILETIME destFileTime = pFile->GetModifiedTime();

				if (!(flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME))
				{
					// if source < dest
					if (CompareTimes(sourceFileTime,destFileTime)<0)
					{
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_ORANGE,"\nUpdated source file time to current time due to copying over newer file\n");
					}
				}

				feedback.m_totalBytesCopied += pOtherFile->GetFileLength();
				feedback.UpdateStats();

				pFile->SetFileLength(pOtherFile->GetFileLength());
				pFile->SetModifiedTime(pOtherFile->GetModifiedTime());
				return pFile;
			}
			else
			{
				DWORD fileAttributes = ::GetFileAttributes(destFilePath);
				bool bSetDestAttributes = false;
				if (fileAttributes == INVALID_FILE_ATTRIBUTES)
				{
					feedback.m_errorCount++;
					feedback.UpdateError();
					TupString errorString;
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Accessing\n%s\n",
						destFilePath);
					return NULL;
				}
				else if (fileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					// make file not read only
					fileAttributes &=~FILE_ATTRIBUTE_READONLY;
					bSetDestAttributes = true;
				}
				if (bSetDestAttributes)
				{
					bResult = ::SetFileAttributes(destFilePath,fileAttributes);
					if (!bResult)
					{
						feedback.m_errorCount++;
						feedback.UpdateError();
						TupString errorString;
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Setting Attributes\n%s\n",
							destFilePath);
						return NULL;
					}
				}

				// if we are not setting the destination file to the current time when it is copied
				// we still need to make both files the current time when copying over an older file
				if (!(flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME))
				{
					if (CheckAndTouch(pOtherDB,pOtherFile,pFile))
					{
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_ORANGE,"\nUpdated source file time to current time due to copying over newer file\n");
						// add to log file
						if (!m_logFile.AddFileTimeUpdate(pFile->GetFileName(),sourceFilePath))
						{
							feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
						}
					}
				}
				// grant access to the copy progress routine...
				pFeedback = &feedback;
				bResult = ::CopyFileEx(sourceFilePath,destFilePath,CopyProgressRoutine,this,NULL,0);
				if (bResult)
				{
					feedback.m_totalBytesCopied += pOtherFile->GetFileLength();
					feedback.UpdateStats();
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
					// add to log file
					if (!m_logFile.AddFileCopy(pFile->GetFileName(),destFilePath,sourceFilePath))
					{
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
					}
					pFile->SetFileLength(pOtherFile->GetFileLength());

					// in this situation we only make the destination file have the current time
					if (flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME)
					{
						TouchFile(pFile);
					}
					else
					{
						pFile->SetModifiedTime(pOtherFile->GetModifiedTime());
					}
					if (flags & FLAGS_VERIFY_COPIED_FILE)
					{
						if (feedback.m_bIncludePath)
						{
							if (bRight)
								logString.Format("Verifying File -> %s",(const char *)destFilePath);
							else
								logString.Format("Verifying File %s <-",(const char *)destFilePath);
						}
						else
						{
							if (bRight)
								logString.Format("Verifying File -> %s",pFile->GetFileName());
							else
								logString.Format("Verifying File %s <-",pFile->GetFileName());
						}
						feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

						DWORD crcValue,otherCRCValue;
						GetCRCFromFile(pFile,crcValue);
						pOtherDB->GetCRCFromFile(pOtherFile,otherCRCValue);
						if (crcValue==otherCRCValue)
						{
							feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
						}
						else
						{
							feedback.m_errorCount++;
							feedback.UpdateError();
							TupString errorString;
							feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nFile Verify Error\n%s\n",
								destFilePath);
						}
					}
				}
				else
				{
					if (feedback.GetAbortFlag()==false)
					{
						DWORD lastError = ::GetLastError();
						feedback.m_errorCount++;
						feedback.UpdateError();
						TupString errorString;
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Copying File from\n%s to\n%s\n",
							sourceFilePath,destFilePath);
					}
					return NULL;
				}
				// check and reset read only flag on dest file
				fileAttributes = ::GetFileAttributes(destFilePath);
				bSetDestAttributes = false;
				if (fileAttributes == INVALID_FILE_ATTRIBUTES)
				{
					feedback.m_errorCount++;
					feedback.UpdateError();
					TupString errorString;
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Accessing\n%s\n",
						destFilePath);
					return NULL;
				}
				else if (fileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					// make file not read only
					fileAttributes &=~FILE_ATTRIBUTE_READONLY;
					bSetDestAttributes = true;
				}
				if (bSetDestAttributes)
				{
					bResult = ::SetFileAttributes(destFilePath,fileAttributes);
					if (!bResult)
					{
						feedback.m_errorCount++;
						feedback.UpdateError();
						TupString errorString;
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Setting Attributes\n%s\n",
							destFilePath);
						return NULL;
					}
				}
				return pFile;
			}
		}
		else
		{
			if (bRight)
				feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_LEFT_TO_RIGHT;
			else
				feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_RIGHT_TO_LEFT;
			
			feedback.m_sourceFile = sourceFilePath;
			feedback.m_destFile = destFilePath;
			feedback.m_totalFileBytes = pOtherFile->GetFileLength();
			feedback.m_totalFileBytesCopied = 0;
			
			feedback.UpdateStats();

			TupString timeString = (flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME) ? "+" : "";
			TupString logString;

			if (feedback.m_bIncludePath)
			{
				if (bRight)
					logString.Format("Copying File -> %s%s",(const char *)destFilePath,(const char *)timeString);
				else
					logString.Format("Copying File %s%s <-",(const char *)destFilePath,(const char *)timeString);
			}
			else
			{
				if (bRight)
					logString.Format("Copying File -> %s%s",pFile->GetFileName(),(const char *)timeString);
				else
					logString.Format("Copying File %s%s <-",pFile->GetFileName(),(const char *)timeString);
			}
			feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);
			feedback.m_errorCount++;
			feedback.UpdateError();
			TupString errorString;
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError - File  %s has been updated following refresh.\n",destFilePath);
			return NULL;
		}
	}
	else
	{
		TupString destFilePath;
		pParentFolder->GetPath(destFilePath);
		destFilePath += "\\";
		destFilePath += pOtherFile->GetFileName();
		TreeNodeFile *pNewFile = m_treeFactory.CreateTreeNodeFile();
		pNewFile->SetFileName(pOtherFile->GetFileName());
		pNewFile->SetFileLength(pOtherFile->GetFileLength());
		TupString sourceFilePath;
		pOtherDB->GetPathToFile(pOtherFile,sourceFilePath);

		if (bRight)
			feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_LEFT_TO_RIGHT;
		else
			feedback.m_syncMode = SyncFeedback::SYNC_MODE_COPY_RIGHT_TO_LEFT;

		feedback.m_sourceFile = sourceFilePath;
		feedback.m_destFile = destFilePath;

		TupString logString;
		TupString timeString = (flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME) ? "+" : "";

		if (feedback.m_bIncludePath)
		{
			if (bRight)
				logString.Format("Copying File -> %s%s",(const char *)destFilePath,(const char *)timeString);
			else
				logString.Format("Copying File %s%s <-",(const char *)destFilePath,(const char *)timeString);
		}
		else
		{
			if (bRight)
				logString.Format("Copying File -> %s%s",pOtherFile->GetFileName(),(const char *)timeString);
			else
				logString.Format("Copying File %s%s <-",pOtherFile->GetFileName(),(const char *)timeString);
		}
		feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);
		// grant access to the copy progress routine...
		pFeedback = &feedback;
		if (feedback.m_bPreviewMode)
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
			AddChildNodeSorted(pParentFolder,pNewFile);
			return pNewFile;
		}
		else
		{
			bResult = ::CopyFileEx(sourceFilePath,destFilePath,CopyProgressRoutine,this,NULL,0);
			if (bResult)
			{
				AddChildNodeSorted(pParentFolder,pNewFile);

				feedback.m_totalBytesCopied += pOtherFile->GetFileLength();
				feedback.UpdateStats();
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
				// add to log file
				if (!m_logFile.AddFileCreate(pNewFile->GetFileName(),destFilePath,sourceFilePath))
				{
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
				}
				// in this situation we only make the destination file have the current time
				if (flags & FLAGS_UPDATE_DEST_TO_CURRENT_TIME)
				{
					TouchFile(pNewFile);
				}
				else
				{
					pNewFile->SetModifiedTime(pOtherFile->GetModifiedTime());
				}
				if (flags & FLAGS_VERIFY_COPIED_FILE)
				{
					if (feedback.m_bIncludePath)
					{
						if (bRight)
							logString.Format("Verifying File -> %s",(const char *)destFilePath);
						else
							logString.Format("Verifying File %s <-",(const char *)destFilePath);
					}
					else
					{
						if (bRight)
							logString.Format("Verifying File -> %s",pNewFile->GetFileName());
						else
							logString.Format("Verifying File %s <-",pNewFile->GetFileName());
					}
					feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

					DWORD crcValue,otherCRCValue;
					GetCRCFromFile(pNewFile,crcValue);
					pOtherDB->GetCRCFromFile(pOtherFile,otherCRCValue);
					if (crcValue==otherCRCValue)
					{
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
					}
					else
					{
						feedback.m_errorCount++;
						feedback.UpdateError();
						TupString errorString;
						feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nFile Verify Error\n%s\n",
							destFilePath);
					}
				}
			}
			else
			{
				if (feedback.GetAbortFlag()==false)
				{
					DWORD lastError = ::GetLastError();
					feedback.m_errorCount++;
					feedback.UpdateError();
					TupString errorString;
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Copying File from\n%s to\n%s\n",
						sourceFilePath,destFilePath);
				}
				delete pNewFile;
				return NULL;
			}
			DWORD fileAttributes = ::GetFileAttributes(destFilePath);
			bool bSetDestAttributes = false;
			if (fileAttributes == INVALID_FILE_ATTRIBUTES)
			{
				feedback.m_errorCount++;
				feedback.UpdateError();
				TupString errorString;
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Getting Attributes\n%s\n",
					destFilePath);
			}
			else if (fileAttributes & FILE_ATTRIBUTE_READONLY)
			{
				// make file not read only
				fileAttributes &=~FILE_ATTRIBUTE_READONLY;
				bSetDestAttributes = true;
			}
			if (bSetDestAttributes)
			{
				bResult = ::SetFileAttributes(destFilePath,fileAttributes);
				if (!bResult)
				{
					feedback.m_errorCount++;
					feedback.UpdateError();
					TupString errorString;
					feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Setting Attributes\n%s\n",
						destFilePath);
					delete pNewFile;
					return NULL;
				}
			}
			return pNewFile;
		}
	}
}

void SyncDBSimple::GetPathToFile(const TreeNodeFile *pFile,TupString &filePath) const
{
	pFile->GetPath(filePath);
}

void SyncDBSimple::AddChildNodeSorted(TreeNodeFolder *pParentFolder,TreeNodeBase *pNewChildNode)
{
	TreeNodeBase *pTreeNodeChild = pParentFolder->GetFirstChildNode();
	bool bAddedChild = false;
	while (pTreeNodeChild)
	{
		if (CompareNodes(pNewChildNode,pTreeNodeChild)>0)
		{
			pParentFolder->InsertChild(pNewChildNode,pTreeNodeChild,false);
			bAddedChild = true;
			break;
		}
		pTreeNodeChild = pTreeNodeChild->GetNext(); // try text child
	}
	// if no children were found to insert before then add to end
	if (!bAddedChild)
	{
		pParentFolder->AddChild(pNewChildNode);
	}
}

bool SyncDBSimple::DeleteFileFromDB(TreeNodeFile *pFile,SyncFeedback &feedback,bool bRight)
{
	BOOL bResult;
	// if file exists already in the database
	ASSERT(pFile);

	// if aborting then kick out
	if (feedback.GetAbortFlag())
		return false;

	TupString destFilePath;
	pFile->GetPath(destFilePath);

	if (bRight)
		feedback.m_syncMode = SyncFeedback::SYNC_MODE_DELETING_RIGHT;
	else
		feedback.m_syncMode = SyncFeedback::SYNC_MODE_DELETING_LEFT;

	feedback.m_sourceFile = destFilePath;
	feedback.m_totalFileBytes = 0;
	feedback.m_totalFileBytesCopied = 0;
	feedback.UpdateStats();

	TupString logString;
	if (feedback.m_bIncludePath)
	{
		if (bRight)
			logString.Format("Deleting File -> %s",(const char *)destFilePath);
		else
			logString.Format("Deleting File %s <-",(const char *)destFilePath);
	}
	else
	{
		if (bRight)
			logString.Format("Deleting File -> %s",pFile->GetFileName());
		else
			logString.Format("Deleting File %s <-",pFile->GetFileName());
	}
	feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

	if (feedback.m_bPreviewMode)
	{
		feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
		delete pFile;
		return true;
	}
	else
	{
		DWORD fileAttributes = ::GetFileAttributes(destFilePath);
		bool bSetDestAttributes = false;
		if (fileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			feedback.m_errorCount++;
			feedback.UpdateError();
			TupString errorString;
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Accessing\n%s\n",
				destFilePath);
			return NULL;
		}
		else if (fileAttributes & FILE_ATTRIBUTE_READONLY)
		{
			// make file not read only
			fileAttributes &=~FILE_ATTRIBUTE_READONLY;
			bSetDestAttributes = true;
		}
		if (bSetDestAttributes)
		{
			bResult = ::SetFileAttributes(destFilePath,fileAttributes);
			if (!bResult)
			{
				feedback.m_errorCount++;
				feedback.UpdateError();
				TupString errorString;
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Setting Attributes\n%s\n",
					destFilePath);
				return NULL;
			}
		}

		bResult = ::DeleteFile(destFilePath);

		if (bResult)
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
			// add to log file
			if (!m_logFile.AddFileDelete(pFile->GetFileName(),destFilePath))
			{
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
			}
			delete pFile;
			return true;
		}
		else
		{
			feedback.m_errorCount++;
			feedback.UpdateError();
			TupString errorString;
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Deleting File \n%s\n",
				destFilePath);
			return false;
		}
	}
}

bool SyncDBSimple::DeleteDirectory(TreeNodeFolder *pFolder,SyncFeedback &feedback,bool bRight)
{
	// get rid of all my kids...
	while (pFolder->GetFirstChildNode())
	{
		// if aborting then kick out
		if (feedback.GetAbortFlag())
			return false;

		TreeNodeBase *pChild = pFolder->GetFirstChildNode();
		TreeNodeFolder *pChildFolder = dynamic_cast<TreeNodeFolder *>(pChild);
		if (pChildFolder)
		{
			bool bResult = DeleteDirectory(pChildFolder,feedback,bRight);
			if (!bResult)
			{
				return false;
			}
		}
		else
		{
			TreeNodeFile *pChildFile = dynamic_cast<TreeNodeFile *>(pChild);
			if (pChildFile)
			{
				bool bResult = DeleteFileFromDB(pChildFile,feedback,bRight);
				if (!bResult)
				{
					return false;
				}
			}
		}
	}

	BOOL bResult;
	TupString folderPath;
	pFolder->GetPath(folderPath);
	TupString logString;
	if (feedback.m_bIncludePath)
	{
		if (bRight)
			logString.Format("Deleting Directory -> %s",(const char *)folderPath);
		else
			logString.Format("Deleting Directory %s <-",(const char *)folderPath);
	}
	else
	{
		if (bRight)
			logString.Format("Deleting Directory -> %s",pFolder->GetFileName());
		else
			logString.Format("Deleting Directory %s <-",pFolder->GetFileName());
	}
	feedback.AddToLog(LogFlags::FONT_COLOR_BLACK,"%s",(const char *)logString);

	CheckAndRemoveLog(folderPath);

	if (feedback.m_bPreviewMode)
	{
		feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
		delete pFolder;
		return true;
	}
	else
	{
		bResult = ::RemoveDirectory(folderPath);

		if (bResult)
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_GREEN," Ok.\n");
			// add to log file
			if (!m_logFile.AddFolderDelete(pFolder->GetFileName(),folderPath))
			{
				feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Error Accessing log file.\n");
			}

			delete pFolder;
			return true;
		}
		else
		{
			feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"\nError Deleting Directory\n%s\n",folderPath);
			return false;
		}
	}
}

void SyncDBSimple::CheckAndRemoveLog(const char *pPathString)
{
	CFileFind finder;
	TupString fileName(pPathString);
	fileName += "\\*.*";
	int validChildCount = 0;
	bool bLogFileFound = false;

	BOOL bWorking = finder.FindFile(fileName);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots())
		{
			// directory
			if (finder.IsDirectory())
			{
				validChildCount++;
			}
			// file
			else
			{
				TupString childFilename = finder.GetFileName();
				if (childFilename.CompareNoCase(m_logFile.GetLogFileName())==0)
					bLogFileFound = true;
				else
					validChildCount++;
			}
		}
	}
	if (validChildCount==0 && bLogFileFound)
	{
		TupString logFilePath(pPathString);
		logFilePath += "\\";
		logFilePath += m_logFile.GetLogFileName();
		BOOL bResult;

		int numRetries = 10;
		for (int attempt=0;attempt<numRetries;attempt++)
		{
			bResult = ::DeleteFile(logFilePath);
			if (bResult)
				break;
			Sleep(100); // try again later
		}
	}
}

int SyncDBSimple::RenameFolder(TreeNodeFolder *pFolder,const char *pNewName)
{
	TupString oldName = pFolder->GetFileName();
	TupString oldPath;
	pFolder->GetPath(oldPath);
	pFolder->SetFileName(pNewName);
	TupString newPath;
	pFolder->GetPath(newPath);
	pFolder->SetFileName(oldName);
	int result = rename(oldPath,newPath);
	if (result==0)
	{
		m_logFile.AddFolderRename(pNewName,newPath,oldPath);
	}
	return result;
}

int SyncDBSimple::RenameFile(TreeNodeFile *pFile,const char *pNewName)
{
	TupString oldName = pFile->GetFileName();
	TupString oldPath;
	pFile->GetPath(oldPath);
	pFile->SetFileName(pNewName);
	TupString newPath;
	pFile->GetPath(newPath);
	pFile->SetFileName(oldName);
	int result = rename(oldPath,newPath);
	m_logFile.AddFileRename(pNewName,newPath,oldPath);
	return result;
}

// this is to check if the destination file that we are copying over is the same date/time of when
// we did refreshed.  If it is not return false
bool SyncDBSimple::CheckFileInSync(TreeNodeFile *pFile)
{
	TupString path;
	pFile->GetPath(path);
	HANDLE hFile = ::CreateFile(path,  // open path
		0,										// access only
		FILE_SHARE_READ,					// share for reading 
		NULL,									// no security 
		OPEN_EXISTING,						// existing file only 
		FILE_ATTRIBUTE_NORMAL,			// normal file 
		NULL);								// no attr. template 
	
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return true; // if we cannot open the file so don't worry
	}

	bool bTimesMatch = true;
	FILETIME currentFileTime;
	if (::GetFileTime(hFile,(LPFILETIME) NULL,(LPFILETIME) NULL,&currentFileTime))
	{
		FILETIME previousFileTime = pFile->GetModifiedTime();
		if (CompareTimes(previousFileTime,currentFileTime)!=0)
			bTimesMatch = false;
	}
	CloseHandle(hFile); 

	return bTimesMatch;
}

// returns true if file time is updated
bool SyncDBSimple::CheckAndTouch(SyncDBBase *pOtherDB,TreeNodeFile *pOtherFile,TreeNodeFile *pFile)
{
	FILETIME sourceFileTime = pOtherFile->GetModifiedTime();
	FILETIME destFileTime = pFile->GetModifiedTime();

	bool bTouchedFile = false;

	// if source < dest
	if (m_bRemoteDB && CompareTimes(sourceFileTime,destFileTime)<0)
	{
		FILETIME currentFileTime;
		SYSTEMTIME currentSystemTime;

		::GetSystemTime(&currentSystemTime);		// gets current time
		::SystemTimeToFileTime(&currentSystemTime, &currentFileTime);	// converts to file time format

		TupString otherPath;
		pOtherFile->GetPath(otherPath);

		HANDLE hFile = ::CreateFile(otherPath,  // open path
			GENERIC_WRITE,						// write access
			FILE_SHARE_READ,					// share for reading 
			NULL,									// no security 
			OPEN_EXISTING,						// existing file only 
			FILE_ATTRIBUTE_NORMAL,			// normal file 
			NULL);								// no attr. template 

		if (::SetFileTime(hFile,(LPFILETIME) NULL, (LPFILETIME) NULL, &currentFileTime))
		{
			pOtherFile->SetModifiedTime(currentFileTime);
			bTouchedFile = true;
		}
		CloseHandle(hFile);
	}
	return bTouchedFile;
}

// returns true if file time is updated
bool SyncDBSimple::TouchFile(TreeNodeFile *pFile)
{
	FILETIME currentFileTime;
	SYSTEMTIME currentSystemTime;

	::GetSystemTime(&currentSystemTime);		// gets current time
	::SystemTimeToFileTime(&currentSystemTime, &currentFileTime);	// converts to file time format

	TupString path;
	pFile->GetPath(path);

	HANDLE hFile = ::CreateFile(path,  // open path
		GENERIC_WRITE,						// write access
		FILE_SHARE_READ,					// share for reading 
		NULL,									// no security 
		OPEN_EXISTING,						// existing file only 
		FILE_ATTRIBUTE_NORMAL,			// normal file 
		NULL);								// no attr. template 

	if (::SetFileTime(hFile,(LPFILETIME) NULL, (LPFILETIME) NULL, &currentFileTime))
	{
		pFile->SetModifiedTime(currentFileTime);
	}
	CloseHandle(hFile);
	return true;
}


bool SyncDBSimple::GetCRCFromFile(TreeNodeFile *pFile,DWORD &crcValue) const
{
	if (pFile->ValidCRC())
	{
		crcValue = pFile->GetCRC();
		return true;
	}
	else
	{
		TupString path;
		pFile->GetPath(path);
		if (m_pGetCRC->GetCRCFromFile(path,crcValue))
		{
			pFile->SetCRC(crcValue);
			pFile->SetValidCRC(true);
			return true;
		}
	}
	return false;
}

bool SyncDBSimple::RemotePath(const char *pPath)
{
	TupString remotePath = pPath;

	if (remotePath[1] == ':')
	{
		char buffer[1000];
		TupString testPath = remotePath.Left(2);
		int numChars = QueryDosDevice(testPath,buffer,1000);
		if (numChars)
		{
			TupString prefix = buffer;
			if (prefix.Find("\\Device\\LanmanRedirector\\")==0)
			{
				int startPos = prefix.Find("\\",26);
				TupString newPath = "\\\\";
				newPath += prefix.Mid(startPos+1);
				newPath += remotePath.Mid(2);
				remotePath = newPath;
			}
			else if (prefix.Find("\\??\\UNC\\")==0)
			{
				TupString newPath = "\\\\";
				newPath += prefix.Mid(8);
				newPath += remotePath.Mid(2);
				remotePath = newPath;
			}
			else if (prefix.Find("\\??\\")==0)
			{
				TupString newPath = prefix.Mid(4);
				newPath += remotePath.Mid(2);
				remotePath = newPath;
			}
		}
	}

	TupString serverName;

	// if unc name (get server name from string)
	if (remotePath.GetLength()>=2 && remotePath[0] == '\\' && remotePath[1] == '\\')
	{
		int foundEndOfServerIndex = remotePath.Find("\\",2);
		// no backslash at end (just use the characters right of "\\"
		if (foundEndOfServerIndex==-1)
		{
			serverName = remotePath.Mid(2);
			remotePath = "";
		}
		// get computer name from string
		else
		{
			serverName = remotePath.Mid(2,foundEndOfServerIndex-2);
			remotePath = remotePath.Mid(foundEndOfServerIndex);
		}
		// compare against computer name
		char nameBuffer[512];
		DWORD maxSize = 512;
		GetComputerName(nameBuffer,&maxSize);
		return stricmp(nameBuffer,serverName) ? true : false;
	}
	return false;
}
