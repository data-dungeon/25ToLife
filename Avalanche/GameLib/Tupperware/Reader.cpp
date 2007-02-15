////////////////////////////////////////////////////////////////////////////
//
// TupperwareReader
//
// A reader of tupperware files.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Reader.h"

#include "Aggregate.h"
#include "Codec.h"
#include "List.h"
#include "Log.h"
#include "Scalar.h"

////////////////////////////////////////////////////////////////////////////

TupperwareReader::TupperwareReader() 
{
}


TupperwareReader::~TupperwareReader() 
{
}

TupperwareAggregate* TupperwareReader::Read( const char* buffer, int bufferLength) 
{
	if (bufferLength==0)
	{
		return NULL;
	}

	TupperwareParser parser( buffer , bufferLength,NULL,false);
	TupperwareAggregate* root = parser.ParseAggregateBody( true );

	return root;
}


TupperwareAggregate* TupperwareReader::Read( const char* file_name,TupArray<FileBuffer> *pFileBuffers) 
{
	FILE *f = fopen( file_name, "rb" );

	if (f == NULL) 
	{
		TupperwareLog::Write("Can't open \"%s\" for reading\n", file_name);
		return NULL;
	}

	// get length of file
	fseek(f, 0, SEEK_END);
	int fileLength = ftell(f); 
	fseek(f, 0, SEEK_SET);

	if (fileLength==0)
	{
		fclose( f );
		return NULL;
	}

	// allocate a buffer to hold the entire file
	char *buffer = new char[fileLength];
	// read it all in
	int bytesRead = fread(buffer,1,fileLength,f);
	fclose( f );

	if (pFileBuffers)
	{
		TupperwareParser parser(buffer,fileLength,file_name,true);
		TupperwareAggregate* pRoot = parser.ParseAggregateBody( true );
		pFileBuffers->SetSize(parser.m_bufferDataArray.GetSize());
		for (int bufferIndex=0;bufferIndex<pFileBuffers->GetSize();bufferIndex++)
		{
			(*pFileBuffers)[bufferIndex].m_filename = parser.m_bufferDataArray[bufferIndex].m_filename;
			(*pFileBuffers)[bufferIndex].m_pBuffer = parser.m_bufferDataArray[bufferIndex].m_pBuffer;
			(*pFileBuffers)[bufferIndex].m_nBufferLength = parser.m_bufferDataArray[bufferIndex].m_nBufferLength;
			(*pFileBuffers)[bufferIndex].m_nParentBufferIndex = parser.m_bufferDataArray[bufferIndex].m_nParentBufferIndex;
		}
		// don't free the buffers, let the caller do that...
		return pRoot;
	}
	else
	{
		TupperwareParser parser(buffer,fileLength,file_name,false);
		TupperwareAggregate* pRoot = parser.ParseAggregateBody( true );
		delete buffer;
		return pRoot;
	}
}

// TupperwareToken /////////////////////////////////////////////////////////

TupperwareToken::TupperwareToken(TupperwareTokenType type) 
{
	m_Type = type; 
	m_String = NULL;
	m_Ref = NULL;
}


TupperwareToken::TupperwareToken(TupperwareTokenType type, char *string) 
{
//ned says: i played with having this go through the tupperware memory manager. in loading mountain top, it is called
// roughly 20000 times! total memory is 450k! but adam says that these mallocs are temporary-- used for a bit then free'd.
// as opposed to the tupperware config tree. so not worth going through tupperware memory manager
	m_Type = type; 
	m_String = new char[strlen(string) + 1];
	m_Ref = new int;
	strcpy(m_String, string);
	*m_Ref = 1;
}


TupperwareToken::TupperwareToken(TupperwareTokenType type, int value) 
{
	m_Type = type; 
	m_Int = value;
	m_Ref = NULL;
}


TupperwareToken::TupperwareToken(TupperwareTokenType type, float value) 
{
	m_Type = type; 
	m_Float = value;
	m_Ref = NULL;
}


TupperwareToken::TupperwareToken(const TupperwareToken& token) 
{
	m_Type = token.m_Type;
	m_Int = token.m_Int;

	if (m_Type == STRING || m_Type == KEY || m_Type == BINARY_DATA) 
	{
		m_String = token.m_String;
		m_Ref = token.m_Ref;
		++*m_Ref;
	}
}


TupperwareToken& TupperwareToken::operator=(const TupperwareToken& token) 
{
	if (m_Type == STRING || m_Type == KEY || m_Type == BINARY_DATA) 
	{
		if(!--*m_Ref) 
		{
			delete m_Ref;
			delete[] m_String;
		}
		m_Ref = NULL;
		m_String = NULL;
	}

	m_Type = token.m_Type;
	m_Int = token.m_Int;

	if (m_Type == STRING || m_Type == KEY || m_Type == BINARY_DATA) 
	{
		m_String = token.m_String;
		m_Ref = token.m_Ref;
		++*m_Ref;
	}

	return *this;
}


TupperwareToken::~TupperwareToken() 
{
	if (m_Type == STRING || m_Type == KEY || m_Type == BINARY_DATA) 
	{
		if(!--*m_Ref) 
		{
			delete m_Ref;
			delete[] m_String;
		}
		m_Ref = NULL;
		m_String = NULL;
	}
	m_Type = UNKNOWN;
}

// TupperwareParser ////////////////////////////////////////////////////////

TupperwareParser::TupperwareParser(const char *pBuffer,int nBufferLength,const char *pFilename,bool bKeepBufferData) :
	m_bKeepBufferData(bKeepBufferData)
{
	BufferData newBufferData;
	if (pFilename)
	{
		newBufferData.m_filename = pFilename;
	}
	else
	{
		// make string have no size
		newBufferData.m_filename.Empty();
	}

	m_pBuffer = newBufferData.m_pBuffer = pBuffer;
	m_nBufferLength = newBufferData.m_nBufferLength = nBufferLength;
	m_nBufferPos = newBufferData.m_nBufferPos = 0;
	newBufferData.m_nParentBufferIndex = -1; // we are the parent
#ifdef TRACK_LINE_NUMBERS
	m_nBufferLineNum = newBufferData.m_nBufferLineNum = 0;
	m_nBufferCharPos = newBufferData.m_nBufferCharPos = 0;
#endif // TRACK_LINE_NUMBERS
	m_bufferDataArray.Add(newBufferData);
	m_nCurrentBufferIndex = 0; // start with the first one

	m_pTokenStr = new char[256];
	m_pTokenStr[0] = 0;
	m_nTokenStrLen = 256;
	m_nTokenStrPos = 0;
}


TupperwareParser::~TupperwareParser() 
{
	delete[] m_pTokenStr;
}

TupperwareToken TupperwareParser::NextToken() 
{
	char c = NextChar();

	int eating_whitespace = 1;
	while (eating_whitespace) 
	{
		while(isspace(c)) 
		{
			c = NextChar();
		}
	
		if (c == '#') 
		{
			while(c != '\n' && c != '\r' && c != 0) 
			{
				c = NextChar();
			}
		}

		if (!isspace(c)) 
		{
			eating_whitespace = 0;
		}
	}

	if (isalpha(c) || c == '_') 
	{	/*  Key  */
		ResetTokenStr();

		do
		{
			AddToTokenStr(c);

			c = NextChar();
		}
		// these are the valid characters after the first one
		while(isalnum(c) || c == '_' || c=='!' || c=='%' || c=='^' || c==':' || c=='*' || c=='@' || c=='&' || c=='#' || c=='?');

		AdjustBlockBack(1);

		return TupperwareToken(TupperwareToken::KEY, m_pTokenStr);
	}
	else if (c == '!')  // include in the form of ! "filename"
	{
		c = NextChar();
		eating_whitespace = 1;
		while (eating_whitespace) 
		{
			while(isspace(c)) 
			{
				c = NextChar();
			}
			if (!isspace(c)) 
			{
				eating_whitespace = 0;
			}
		}
		if (c == '"')
		{
			ResetTokenStr();

			int in_string = 1;
			while (in_string) 
			{
				c = NextChar();
				while (c != '"') 
				{
					AddToTokenStr(c);
					c = NextChar();
				}

				c = NextChar();
				if (c == '"') 
				{
					AddToTokenStr(c);
				} 
				else 
				{
					AdjustBlockBack(1);
					in_string = 0;
				}
			}
			AddIncludeFile(m_pTokenStr);
			return NextToken();
		}
		return NextToken();
	}
	else if (isdigit(c) || c == '-' || c == '+') 
	{  /*  Float, Int */
		bool is_float = false;

		ResetTokenStr();

		do 
		{
			if (c == '.' || c == 'e' || c == 'E') 
			{
				is_float = true;
			}

			AddToTokenStr(c);

			c = NextChar();
		}
		while(isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-' || c == '+');

		AdjustBlockBack(1);

		if (is_float) 
		{
			return TupperwareToken(TupperwareToken::FLOAT, (float)atof(m_pTokenStr));
		} else 
		{
			return TupperwareToken(TupperwareToken::INT, atoi(m_pTokenStr));
		}
	} 
	else if (c == '"') 
	{  /*  String  */
		ResetTokenStr();

		int in_string = 1;
		while (in_string) 
		{
			c = NextChar();
			while (c != '"') 
			{
				AddToTokenStr(c);

				c = NextChar();
			}

			c = NextChar();
			if (c == '"') 
			{
				AddToTokenStr(c);
			} 
			else 
			{
				AdjustBlockBack(1);
				in_string = 0;
			}
		}

		return TupperwareToken(TupperwareToken::STRING, m_pTokenStr);
	}
	else if (c == '$') 
	{  /*  Binary Data  */
		ResetTokenStr();

		int scan_loop = 0;
		int in_string = 1;
		while (in_string) 
		{
			char block[4097];
			int len;

			int scanning = 1;
			int i;
			while (scanning) 
			{
				char *blk = block;
				len = NextBlock(blk, 4096);
				blk[len] = '$';

				i = 0;
				if (len == 0) 
					break;

				int in_block = 1;
				while (in_block) 
				{
					char *scan = blk;

					while (*(scan++) != '$');
					scan--;
					i = scan - blk;

					if (i + 1 < len && blk[i + 1] == '$') 
					{
						AddBlockToTokenStr(blk, i + 1);
						len = len - i - 2;
						blk = &blk[i + 2];
						i = 0;
					}
					else
					{
						in_block = 0;
					}
				}

				if (i == len) 
				{
					AddBlockToTokenStr(blk, len);
				} 
				else
				{
					AddBlockToTokenStr(blk, i);
					AdjustBlockBack(len - i - 1); 
					scanning = 0;
				}
			}
		
			c = NextChar();
			if (c == '$') {
				AddToTokenStr(c);
			} 
			else
			{
				AdjustBlockBack(1);
				in_string = 0;
			}
		}
		return TupperwareToken(TupperwareToken::BINARY_DATA, m_pTokenStr);
	}
	else if (c == '=')
	{  /*  Equal  */
		return TupperwareToken(TupperwareToken::EQUAL);
	}
	else if (c == '{') 
	{  /*  Begin Aggregate  */
		return TupperwareToken(TupperwareToken::BEGIN_AGGREGATE);
	}
	else if (c == '}') 
	{  /*  End Aggregate  */
		return TupperwareToken(TupperwareToken::END_AGGREGATE);
	}
	else if (c == '[') 
	{  /*  Begin List  */
		return TupperwareToken(TupperwareToken::BEGIN_LIST);
	}
	else if (c == ']') 
	{  /*  End List */
		return TupperwareToken(TupperwareToken::END_LIST);
	}
	else if (c == ',') 
	{  /*  Comma, list separator  */
		return TupperwareToken(TupperwareToken::LIST_SEPARATOR);
	}
	else if (c == 0) 
	{ /* End-of-file */
		return TupperwareToken(TupperwareToken::END_OF_FILE);
	}
	else 
	{
		assert(0);
		return TupperwareToken(TupperwareToken::UNKNOWN);
	}
}


TupperwareList *TupperwareParser::ParseListBody() 
{
	TupperwareList *list = new TupperwareList();
	TupperwareToken token(NextToken());
	TupperwareAtom::DataType dataType = TupperwareAtom::DATA_UNKNOWN;
	void *pListData=NULL;
	int nListCapacity = 0;
	int nListLength = 0;

	while (token.m_Type != TupperwareToken::END_LIST) 
	{
		if (dataType == TupperwareAtom::DATA_UNKNOWN) 
		{
			if (token.m_Type == TupperwareToken::FLOAT) 
			{
				dataType = TupperwareAtom::DATA_FLOAT;

				pListData = new float[16];
				nListCapacity = 16;
			} 
			else if(token.m_Type == TupperwareToken::INT) 
			{
				dataType = TupperwareAtom::DATA_INT;

				pListData = new int[16];
				nListCapacity = 16;
			} 
			else if(token.m_Type == TupperwareToken::STRING) 
			{
				dataType = TupperwareAtom::DATA_STRING;

				pListData = new char *[16];
				nListCapacity = 16;
			} 
			else if(token.m_Type == TupperwareToken::BINARY_DATA) 
			{
				TupperwareLog::Write( "TupperwareParser(): BINARY LIST not supported!\n" );
				assert(0);
			} 
			else 
			{
				assert(0);
			}
		}

		if (dataType == TupperwareAtom::DATA_FLOAT) 
		{
			assert(token.m_Type == TupperwareToken::FLOAT);
			((float *)pListData)[nListLength++] = token.m_Float;

			if (nListLength >= nListCapacity) 
			{
				void *pNewListData = new float[nListCapacity * 4];
				memcpy(pNewListData, pListData, sizeof(float) * nListCapacity);
				nListCapacity *= 4;
				delete[] (float*)pListData;
				pListData = pNewListData;
			}
		} 
		else if (dataType == TupperwareAtom::DATA_INT) 
		{
			assert(token.m_Type == TupperwareToken::INT);
			((int *)pListData)[nListLength++] = token.m_Int;

			if (nListLength >= nListCapacity) 
			{
				void *pNewListData = new int[nListCapacity * 4];
				memcpy(pNewListData, pListData, sizeof(float) * nListCapacity);
				nListCapacity *= 4;
				delete[] (int*)pListData;
				pListData = pNewListData;
			}
		} 
		else if (dataType == TupperwareAtom::DATA_STRING) 
		{
			assert(token.m_Type == TupperwareToken::STRING);

			((char **)pListData)[nListLength] = new char[strlen(token.m_String) + 1];
			strcpy(((char **)pListData)[nListLength], token.m_String);

			nListLength++;

			if (nListLength >= nListCapacity) 
			{
				void *pNewListData = new char *[nListCapacity * 4];
				memcpy(pNewListData, pListData, sizeof(char *) * nListCapacity);
				nListCapacity *= 4;
				delete[] (char**)pListData;
				pListData = pNewListData;
			}
		}

		token = NextToken();

		if (token.m_Type == TupperwareToken::LIST_SEPARATOR) 
		{
			token = NextToken();
		}
	}

	if (dataType == TupperwareAtom::DATA_FLOAT) 
	{
		list->SetAsFloat((float *)pListData,nListLength);
		delete[] (float*)pListData; // this is the temp one
	} 
	else if (dataType == TupperwareAtom::DATA_INT) 
	{
		list->SetAsInt((int *)pListData,nListLength);
		delete[] (int*)pListData; // this is the temp one
	} 
	else if (dataType == TupperwareAtom::DATA_STRING) 
	{
		list->SetAsString((const char **)pListData,nListLength);
		// delete the strings
		for (int stringIndex=0;stringIndex<nListLength;stringIndex++)
		{
			delete ((char **)pListData)[stringIndex];
		}
		delete[] (char**)pListData; // this is the temp one
	}
	return list;
}

TupperwareAggregate *TupperwareParser::ParseAggregateBody(bool root_aggregate) 
{
	TupperwareAggregate *aggregate = new TupperwareAggregate();
	TupperwareAtom *lastChild = NULL;

	TupperwareToken token = NextToken();

	while (token.m_Type == TupperwareToken::KEY) 
	{
		TupperwareToken keyToken = token;
		TupperwareToken nameToken = NextToken();


		if (nameToken.m_Type == TupperwareToken::STRING) 
		{
			token = NextToken();
		}
		else
			token = nameToken;

		assert(token.m_Type == TupperwareToken::EQUAL);

		token = NextToken();
		if (token.m_Type == TupperwareToken::BEGIN_AGGREGATE) 
		{
			TupperwareAggregate *child = ParseAggregateBody(false);
			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );
			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		} 
		else if (token.m_Type == TupperwareToken::BEGIN_LIST) 
		{
			TupperwareList *child = ParseListBody();

			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );

			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		} 
		else if (token.m_Type == TupperwareToken::FLOAT) 
		{
			TupperwareScalar *child = new TupperwareScalar();

			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );
			child->SetAsFloat( token.m_Float );

			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		} 
		else if (token.m_Type == TupperwareToken::INT) 
		{
			TupperwareScalar *child = new TupperwareScalar();

			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );
			child->SetAsInt( token.m_Int );

			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		} 
		else if (token.m_Type == TupperwareToken::STRING) 
		{
			TupperwareScalar *child = new TupperwareScalar();

			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );
			child->SetAsString( token.m_String );

			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		}
		else if (token.m_Type == TupperwareToken::BINARY_DATA)
		{
			TupperwareScalar *child = new TupperwareScalar();

			child->SetKey( keyToken.m_String );
			child->SetName( nameToken.m_String );

			int length;
			void* decoded = TupperwareCodec::AsciiToBinary( token.m_String, &length );
			child->SetAsBinary(decoded,length);
			// we dont really know what type this is here, but to resolve warnings
			// we call it a char[]
			delete[] (char*)decoded;

			aggregate->InsertChild(child,lastChild,true);
			lastChild = child;
		} 
		else 
		{
			assert(0);
		}

		token = NextToken();
	}

	if (root_aggregate) 
	{
		assert(token.m_Type == TupperwareToken::END_OF_FILE);
	} 
	else 
	{
		assert(token.m_Type == TupperwareToken::END_AGGREGATE);
	}

	return aggregate;
}


char TupperwareParser::NextBufferNextChar(void)
{
	if (m_nCurrentBufferIndex!=0)
	{
		// delete buffer (const_cast since it is a const char *)
		if (!m_bKeepBufferData)
		{
			delete const_cast<char *>(m_bufferDataArray[m_nCurrentBufferIndex].m_pBuffer);
			m_bufferDataArray[m_nCurrentBufferIndex].m_pBuffer = NULL;
		}
		// point to parent
		m_nCurrentBufferIndex = m_bufferDataArray[m_nCurrentBufferIndex].m_nParentBufferIndex;
		// valid buffer
		m_pBuffer = m_bufferDataArray[m_nCurrentBufferIndex].m_pBuffer;
		m_nBufferLength = m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferLength;
		m_nBufferPos = m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferPos;
#ifdef TRACK_LINE_NUMBERS
		m_nBufferLineNum = m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferLineNum;
		m_nBufferCharPos = m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferCharPos;
#endif // TRACK_LINE_NUMBERS
		return NextChar();
	}
	else // last one
	{
		m_pBuffer = NULL;
		m_nBufferLength = 0;
		m_nBufferPos = 0;
#ifdef TRACK_LINE_NUMBERS
		m_nBufferLineNum = 0;
		m_nBufferCharPos = 0;
#endif // TRACK_LINE_NUMBERS
		return 0;
	}
}

void TupperwareParser::AddIncludeFile(const char *pFilename)
{
	// check if we loaded the main tup from a file
	// ignore this if it is from a buffer
#if defined (WIN32) && !defined (_XBOX)
	if (strlen(m_bufferDataArray[m_nCurrentBufferIndex].m_filename))
	{
		char mainFilePath[_MAX_PATH];
		// get the full path to the main file
		_fullpath(mainFilePath,m_bufferDataArray[m_nCurrentBufferIndex].m_filename,_MAX_PATH);

		char fdrive[_MAX_DRIVE];
		char fdir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char fext[_MAX_EXT];

		// strip off the filename and just have the path
		_splitpath(mainFilePath,fdrive,fdir,fname,fext);
		_makepath(mainFilePath,fdrive,fdir,NULL,NULL);

		char savePath[_MAX_PATH];
		_getcwd(savePath,_MAX_PATH); // get current path

		_chdir(mainFilePath); // set current path to main filename path

		char includeFilePath[_MAX_PATH];

		_fullpath(includeFilePath,pFilename,_MAX_PATH);
		_chdir(savePath); // restore current path

		FILE *f = fopen( includeFilePath, "rb" );
		if (f == NULL) 
		{
			TupperwareLog::Write("Can't open \"%s\" for reading\n", includeFilePath);
			return;
		}

		// get length of file
		fseek(f, 0, SEEK_END);
		int fileLength = ftell(f); 
		fseek(f, 0, SEEK_SET);

		if (fileLength==0)
		{
			fclose( f );
			return;
		}
		// allocate a buffer to hold the entire file
		char *buffer = new char[fileLength];
		// read it all in
		int bytesRead = fread(buffer,1,fileLength,f);
		fclose( f );

		// save buffer information into array
		m_bufferDataArray[m_nCurrentBufferIndex].m_pBuffer = m_pBuffer;
		m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferLength = m_nBufferLength;
		m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferPos = m_nBufferPos;
#ifdef TRACK_LINE_NUMBERS
		m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferLineNum = m_nBufferLineNum;
		m_bufferDataArray[m_nCurrentBufferIndex].m_nBufferCharPos = m_nBufferCharPos;
#endif // TRACK_LINE_NUMBERS

		// create new buffer entry and set the main variables as well
		// this is now a tree - each block points to it's parent instead of working like a stack
		BufferData newBufferData;
		newBufferData.m_filename = includeFilePath;
		m_pBuffer = newBufferData.m_pBuffer = buffer;
		m_nBufferLength = newBufferData.m_nBufferLength = fileLength;
		m_nBufferPos = newBufferData.m_nBufferPos = 0;
#ifdef TRACK_LINE_NUMBERS
		m_nBufferLineNum = newBufferData.m_nBufferLineNum = 0;
		m_nBufferCharPos = newBufferData.m_nBufferCharPos = 0;
#endif // TRACK_LINE_NUMBERS
		newBufferData.m_nParentBufferIndex = m_nCurrentBufferIndex;
		m_bufferDataArray.Add(newBufferData);
		m_nCurrentBufferIndex = m_bufferDataArray.GetSize()-1; // point to last one
		// now read from this block
	}
#endif
}


////////////////////////////////////////////////////////////////////////////
