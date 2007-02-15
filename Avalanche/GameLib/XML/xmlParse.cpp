#if !defined( XML_BUILD_ENABLED )
# include "Layers/Memory.h"
#endif

#include "XML/XML.h"
#include "Game/Database/DBFile.h"

struct xmlStringListNode
{
	char *string;
	xmlStringListNode *next;
	xmlStringListNode *prev;
};

// head node - records the first string
xmlStringListNode xmlHeadStringNode =
{
	NULL,
	&xmlHeadStringNode,
	&xmlHeadStringNode,
};

// add the string to the string list
#define MAKENSTRING_TEMP_BUFFER_SIZE ( 512 )
static char *makenstring_temp_buffer = NULL;

static bool xml_owns_data = true;
static bool xml_found_collapsed_start_tag = false;
static ts_xmlMemory *pFirstMemoryBlock;
static ts_xmlMemory *pCurrentMemoryBlock;
//#define DEBUG_MEMORY_BLOCK
#ifdef DEBUG_MEMORY_BLOCK
static int nMemoryBlockCount;
static int nMemoryRequestCount;
static int nMemoryRequestTotal;
static int nMemoryRequestAlignedTotal;
#endif //DEBUG_MEMORY_BLOCK

static int ScanTextContent( unsigned char *buffer, unsigned int length );
static int ScanBareword( unsigned char *buffer, unsigned int length );
static int ScanQuotedText( unsigned char *buffer, unsigned int length );
static int ScanAttributeName( unsigned char *buffer, unsigned int length, char **name );
static int ScanAttributeValue( unsigned char *buffer, unsigned int length, char **value );
static int ParseAttribute( xmlAttribute **attribute, unsigned char *buffer, unsigned int length );
static int ParseAttributes( xmlElement *element, unsigned char *buffer, unsigned int length );
static int ParseMiscTag( unsigned char *buffer, unsigned int length );
static int ParseEndTag( xmlElement *element, unsigned char *buffer, unsigned int length );
static int ParseStartTag( xmlElement *element, unsigned char *buffer, unsigned int length );
static int ParseElementContent( xmlElement *element, unsigned char *buffer, unsigned int length );
static xmlElement *ParseElement( xmlElement *parent, unsigned char **buffer, unsigned int length );
static void xmlNewMemoryBlock(void);
static void *xmlGetMemory(int nSize);

static char *xmlInsertString( char *source );
static char *xmlFindString( char *key );
static char *xmlMakeNString( char *source, int size );
static char *xmlMakeString( char *source );
static char *xmlMakeStringRaw( char *source );

int isbareword( int c )
{
	if ( isalnum( c ) || ( c == '_' ) )
	{
		return( 1 );
	}
	else
	{
		return( 0 );
	}
}

int ScanTextContent( unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( *offset == '<' )
		{
			while ( isspace( *( offset - 1 ) ) )
			{	// discount any trailing white space
				--offset;
			}
			break;
		}
		++offset;
	}

	return( offset - buffer );
}

int ScanBareword( unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;

	while (
		( ( unsigned int )( offset - buffer ) < length )	&&
		isbareword( *offset )
	)
	{
		offset++;
	}

	return( ( offset - buffer ) );
}

int ScanQuotedText( unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer + 1;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( *offset == *buffer )
		{	// the quote character should be the first one in buffer
			return( ( offset - buffer ) - 1 );
		}
		offset++;
	}

	return( 0 );
}

int ScanAttributeName( unsigned char *buffer, unsigned int length, char **name )
{
	unsigned char *offset = buffer;
	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( !isbareword( *offset ) )
		{
			break;
		}

		offset++;
	}

	ASSERT( *offset == '=' );

	int size = offset - buffer;
	*name = xmlMakeNString( ( char * )buffer, size );

//	*name = (char *) xmlGetMemory(size + 1);
//	*name = new char[ size + 1 ];
//	strncpy( ( char * )( *name ), ( const char * )buffer, size );
//	(*name)[size] = '\0';

	return( offset - buffer );
}

int ScanAttributeValue( unsigned char *buffer, unsigned int length, char **value )
{
	unsigned char *offset = buffer;

	int quoteLength = ScanQuotedText( offset, length - ( offset - buffer ) );

	offset += quoteLength + 2;

	*value = xmlMakeNString( ( char * )( buffer + 1 ), quoteLength );

//	*value = (char *) xmlGetMemory(quoteLength + 1);
//	*value = new char[ quoteLength + 1 ];
//	strncpy( ( char * )( *value ), ( const char * )( buffer + 1 ), quoteLength );
//	(*value)[quoteLength] = '\0';

	return( offset - buffer );
}

int ParseAttribute( xmlAttribute **attribute, unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( isspace( *offset ) )
		{
			offset++;
			continue;
		}
		else
		{
			*attribute = (xmlAttribute *) xmlGetMemory(sizeof(xmlAttribute));
//			*attribute = new xmlAttribute( );
			memset( *attribute, 0, sizeof( xmlAttribute ) );

			int delta = ScanAttributeName( offset, length - ( ( int )offset - ( int )buffer ), &( (*attribute)->name ) );
			offset += delta;

			// skip the '=' sign
			ASSERT( *offset == '=' );
			offset++;

			delta = ScanAttributeValue( offset, length - ( ( int )offset - ( int )buffer ), &( (*attribute)->value ) );
			offset += delta;

			return( offset - buffer );

		}
	}

	*attribute = NULL;
	return( offset - buffer );
}

int ParseAttributes( xmlElement *element, unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;
	unsigned char *name = NULL;
	unsigned char *value = NULL;

	while ( ( ( unsigned int )( offset - buffer ) < length ) && ( *offset != '>' ) && ( ( *offset != '/' ) && ( *( offset + 1 ) != '>' ) ) )
	{
		if ( isspace( *offset ) )
		{
			offset++;
			continue;
		}
		else if ( !isspace( *offset ) )
		{
			xmlAttribute *attribute = NULL;
			int delta = ParseAttribute( &attribute, offset, length - ( ( int )offset - ( int )buffer ) );

			ASSERT( delta > 0 );

			offset += delta;

			if ( attribute != NULL )
			{	// insert the attribute into the attribute list for the current element
				xmlAttribute *curr, *prev;

				curr = element->p_Attributes;
				prev = NULL;
				while( curr != NULL )
				{
					prev = curr;
					curr = curr->p_NextAttribute;
				}

				if ( prev != NULL )
				{	// insert at tail of list
					prev->p_NextAttribute = attribute;
					attribute->p_NextAttribute = NULL;
				}
				else
				{	// insert at head of list
					attribute->p_NextAttribute = element->p_Attributes;
					element->p_Attributes = attribute;
				}
			}
		}
	}

	return( offset - buffer );
}

int ParseMiscTag( unsigned char *buffer, unsigned int length )
{
	bool bFoundBang = ( *( buffer + 1 ) == '!' );
	unsigned char *offset = buffer;
	if ( bFoundBang )
	{
		// <!--
		offset += 4;
	}
	else
	{
		// <?
		offset += 2;
	}

	while( ( unsigned int )( offset - buffer ) < length )
	{
		if ( !bFoundBang && ( *offset == '?' ) && ( *( offset + 1 ) == '>' ) )
		{	// found the end part of the tag

			// eat the ending "?>"
			offset += 2;

			return( offset - buffer );
		}
		else if ( bFoundBang && ( *offset == '-' ) && ( *( offset + 1 ) == '-' ) && ( *( offset + 2 ) == '>' ) )
		{	// found the end part of the tag

			// eat the ending "-->"
			offset += 3;

			return( offset - buffer );
		}
		offset++;
	}

	ASSERT( 0 );

	return( 0 );
}

int ParseEndTag( xmlElement *element, unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( isspace( *offset ) )
		{
			offset++;
			continue;
		}
		else if ( ( *offset == '<' ) && ( *( offset + 1 ) == '/' ) )
		{
			offset += 2;
			continue;
		}
		else if ( isalnum( *offset ) )
		{
			int size = strlen( ( const char * )element->name );

			if ( strncmp( ( const char * )element->name, ( const char * )offset, size ) != 0 )
			{
				return( -1 );
			}
			else
			{
				offset += size;
			}
			continue;
		}
		else if ( *offset == '>' )
		{
			// eat the closing '>'
			offset++;

			return( offset - buffer );
		}
	}

	ASSERT( 0 );

	return( -1 );
}

int ParseStartTag( xmlElement *element, unsigned char *buffer, unsigned int length )
{
	ASSERT( *buffer == '<' );
	ASSERT( element != NULL );

	// skip the opening '<'
	unsigned char *offset = buffer + 1;

	int size = ScanBareword( offset, length - ( offset - buffer ) );

	element->name = xmlMakeNString( ( char * )( buffer + 1 ), size );

//	element->name = (char *) xmlGetMemory(size + 1);
//	element->name = new char[ size + 1 ];
//	strncpy( ( char * )element->name, ( const char * )( buffer + 1 ), size );
//	element->name[ size ] = '\0';

	offset += size;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
    if ( *offset == '/' && *(offset+1) == '>' )
    { // we found a collapsed element w/ no text
      xml_found_collapsed_start_tag = true;

      offset += 2;

      return( offset - buffer );
    }
		if ( *offset == '>' )
		{	// we're done with the start tag, return the delta

			// eat the closing '>'
			offset++;

			return( offset - buffer );
		}
		else if ( isspace( *offset ) )
		{	// there are attributes
			int delta = ParseAttributes( element, offset, length - ( ( int )offset - ( int )buffer ) );
			
			ASSERT( delta > 0 );

			offset += delta;

			continue;
		}
	}

	return( 0 );
}

int ParseElementContent( xmlElement *element, unsigned char *buffer, unsigned int length )
{
	unsigned char *offset = buffer;

	while ( ( unsigned int )( offset - buffer ) < length )
	{
		if ( isspace( *offset ) )
		{	// eat whitespace
			offset++;
			continue;
		}
		else if ( ( *offset == '<' ) && ( *( offset + 1 ) == '/' ) )
		{	// end tag encountered, done with content
			return( offset - buffer );
		}
    else if ( ( *offset == '<' ) && ( ( *( offset + 1 ) == '!' ) || ( *( offset + 1 ) == '?' ) ) )
    {
			int delta = ParseMiscTag( offset, length - ( ( int )offset - ( int )buffer ) );

			offset += delta;
			continue;
    }
		else if ( *offset == '<' )
		{	// add the new element
			// offset should be updated to the new position in the buffer
			// the child is already added to the parent's p_Children list, too
			xmlElement *child = ParseElement( element, &offset, length );

			element->element_content |= XML_CONTENT_ELEMENT;

			continue;
		}
		else
		{	// add the text content - only once!!!
			ASSERT( !( element->element_content & XML_CONTENT_SIMPLE ) );

			// Scan text to '<'
			int size = ScanTextContent( offset, length );

			element->text = xmlMakeNString( ( char * )offset, size );

//			element->text = (char *) xmlGetMemory(size + 1);
//			element->text = new char[ size + 1 ];
//			strncpy( ( char * )( element->text ), ( const char * )offset, size );
//			element->text[ size ] = '\0';

			element->element_content |= XML_CONTENT_SIMPLE;

			offset += size;

			continue;
		}
	}

	return( 0 );
}

xmlElement *ParseElement( xmlElement *parent, unsigned char **buffer, unsigned int length )
{
	unsigned char	*offset = *buffer;
	xmlElement		*element = NULL;

	while ( ( unsigned int )( offset - *buffer ) < length )
	{
		if ( isspace( *offset ) )
		{	// eat leading whitespace
			offset++;
			continue;
		}
		else if ( ( *offset == '<' ) && ( ( *( offset + 1 ) == '!' ) || ( *( offset + 1 ) == '?' ) ) )
		{
			int delta = ParseMiscTag( offset, length - ( ( int )offset - ( int )buffer ) );

			offset += delta;
			continue;
		}
		else if ( ( element != NULL ) && ( *offset == '<' ) && ( *( offset + 1 ) == '/' ) )
		{	// end tag
			int delta = ParseEndTag( element, offset, length - ( offset - *buffer ) );
			ASSERT( delta > 0 );

			offset += delta;
  
      // update the buffer to the new offset, and return the newly created element
			*buffer = offset;
			return ( element );
		}
		else if ( ( element == NULL ) && ( *offset == '<' ) && ( *( offset + 1 ) != '/' ) )
		{	// start tag
			element = (xmlElement *) xmlGetMemory(sizeof(xmlElement));
//			element = new xmlElement( );
			memset( element, 0, sizeof( xmlElement ) );
			element->p_Parent = parent;
			if ( xml_owns_data )
			{
				element->element_content |= XML_CONTENT_OWNS_DATA;
			}
			
			if ( parent != NULL )
			{	// add this element to it's parent's list - if there's a parent element
				xmlElement *curr, *prev;

				curr = element->p_Children;
				prev = NULL;
				while( curr != NULL )
				{
					prev = curr;
					curr = curr->p_NextElement;
				}
				
				if ( prev != NULL )
				{
					prev->p_NextElement = element;
					element->p_NextElement = NULL;
				}
				else
				{	// add the element to the TAIL of the parent's child list
					xmlElement *pCurrChild, *pPrevChild;
					pCurrChild = parent->p_Children;
					pPrevChild = NULL;
					while ( pCurrChild != NULL )
					{
						pPrevChild = pCurrChild;
						pCurrChild = pCurrChild->p_NextElement;
					}

					if ( pPrevChild != NULL )
					{	// there's at least one child already
						element->p_NextElement = NULL;
						pPrevChild->p_NextElement = element;
					}
					else
					{	// no children, add to the head
						element->p_NextElement = NULL;
						parent->p_Children = element;
					}
				}
			}

			xml_found_collapsed_start_tag = false;

			// get the tag name, and attributes
			int delta = ParseStartTag( element, offset, length - ( offset - *buffer ) );
			ASSERT( delta > 0 );

			offset += delta;

			if ( !xml_found_collapsed_start_tag )
			{ // get the content of this element
				delta = ParseElementContent( element, offset, length - ( offset - *buffer ) );
			}
			else
			{ // signal that there was no more content (text/children) - no need to parse closing tag
				delta = -1;
			}

			if ( delta < 0 )
			{	// the element may be empty
				element->element_content |= XML_CONTENT_EMPTY;
				delta = 0;
			}

			if ( xml_found_collapsed_start_tag )
			{ // EARLY OUT: this is a collapsed element (no text/children); update the buffer to the new offset, and return the newly created element
				xml_found_collapsed_start_tag = false;

				*buffer = offset;
				return ( element );
			}

			offset += delta;

			continue;
		}
		else
		{	// mysterious characters . . . try skipping?
			offset++;
		}

		//This will only hit if the tags wrap a comment, or the comment is the last part of the definition
		//ASSERT( 0 );
	}

	// don't update the buffer
	return( NULL );
}

xmlElement *xmlParseBuffer( unsigned char *buffer, unsigned int length, bool owns_data )
{
	unsigned char *myBuff = buffer;

	xmlHeadStringNode.next = &xmlHeadStringNode;
	xmlHeadStringNode.prev = &xmlHeadStringNode;
	xmlHeadStringNode.string = NULL;

	xml_owns_data = owns_data;

#ifdef DEBUG_MEMORY_BLOCK
	nMemoryBlockCount = 0;
	nMemoryRequestCount = 0;
	nMemoryRequestTotal = 0;
	nMemoryRequestAlignedTotal = 0;
#endif //DEBUG_MEMORY_BLOCK

	/* malloc first memory block */

	pCurrentMemoryBlock = NULL;
	xmlNewMemoryBlock();
	ASSERT(pCurrentMemoryBlock);
	if (pCurrentMemoryBlock == NULL)
		return(NULL);
	pFirstMemoryBlock = pCurrentMemoryBlock;

	/* setup a parse buffer */
	makenstring_temp_buffer = ( char * )xmlGetMemory( MAKENSTRING_TEMP_BUFFER_SIZE );

	/* recursively parse the buffer */

	xmlElement *xmlDatabaseHead = ParseElement( NULL, &myBuff, length );

#ifdef DEBUG_MEMORY_BLOCK
	float fBytesPerRequest = (float) nMemoryRequestTotal / (float) nMemoryRequestCount;
	int nWastedBytes = nMemoryRequestAlignedTotal - nMemoryRequestTotal;
#endif //DEBUG_MEMORY_BLOCK

	/* mark database head, point to first memory block */

	if (xmlDatabaseHead)
	{
		xmlDatabaseHead->element_content |= XML_CONTENT_DATABASE_HEAD;
		ASSERT(xmlDatabaseHead->p_Parent == NULL);	// database head should never have a parent
		xmlDatabaseHead->p_Parent = (xmlElement *) pFirstMemoryBlock;
	}
	else
	{
		/* parsing failed. free memory */

		ASSERT(false);
		xmlFreeMemoryBlocks(pFirstMemoryBlock);
	}

	return(xmlDatabaseHead);
}

#if defined( XML_BUILD_ENABLED )
xmlElement *xmlParseFile( FILE *file, bool owns_data )
{
	xml_owns_data = owns_data;

	fseek( file, 0, SEEK_END );

	int size = ftell( file );

	fseek( file, 0, SEEK_SET );

	unsigned char *buffer = new unsigned char[ size ];

	fread( buffer, size, 1, file );

	xmlElement *result = xmlParseBuffer( buffer, size, owns_data );

	delete [] buffer;

	return( result );
}
#else
xmlElement *xmlParseFile( FileStream *file, bool owns_data )
{
	xml_owns_data = owns_data;

	int size = file->Size( );

	unsigned char *buffer = new unsigned char[ size ];

	file->Read( buffer, size );

	xmlElement *result = xmlParseBuffer( buffer, size, owns_data );

	delete [] buffer;

	return( result );
}
#endif

/* parse xml out of a file. may have been pre-loaded (and pre-parsed) out of uber file */

xmlElement *xmlParseFile(char *pFileName)
{
	/* create uber file version of file name (translate xml to dbx) */

	char FileName[200];
	strcpy(FileName, pFileName);
	for (uint i = 0; i < strlen(FileName); i++)
		FileName[i] = toupper(FileName[i]);
	char *pExt = strstr(FileName, ".XML");
	ASSERT(pExt);	// input file name should end in ".xml"
	if (pExt)
		*pExt = 0;
	strcat(FileName, ".dbx");	// .dbx is "translated" extension for .xml (see FileStream::m_fsExtTrans)

	/* look for uber file version */

	t_Handle Handle = dblLoad(FileName);
	if (Handle != INVALID_HANDLE)
	{
		xmlElement *pDatabase = (xmlElement *) dblGetChunkData(DB_XML_SOURCE, (const char *)DEFAULT_CHUNK_KEY_STRING, Handle);

		/* caller now has responsibilty for cleaning up the xml tree */

		if (pDatabase)
			dblRemoveData(Handle, pDatabase);

		/* return result */

		return(pDatabase);
	}

	/* not in uber file-- load from disk */

	FileStream *pFile = FileStream::NewAndOpen(pFileName, FileStream::READ);
	if (pFile == NULL)
		return(NULL);	// file not found
	xmlElement *pDatabase = xmlParseFile(pFile);
	delete pFile;	// release text file
	return(pDatabase);
}

/* malloc a new memory block when no memory is available in existing blocks */

static void xmlNewMemoryBlock(void)
{
	/* malloc a new block. memory record will be first thing in block */
#if defined( XML_BUILD_ENABLED )
	ts_xmlMemory *pMemoryBlock = (ts_xmlMemory *) malloc(XML_MEMORY_BLOCK_SIZE);
#else
	ts_xmlMemory *pMemoryBlock = (ts_xmlMemory *) memAlloc(XML_MEMORY_BLOCK_SIZE);
#endif // XML_BUILD_ENABLED

	/* fill in memory record */

	pMemoryBlock->nSize = XML_MEMORY_BLOCK_SIZE;
	pMemoryBlock->nCurrentOffset = sizeof(ts_xmlMemory);	// first available memory is after memory block record
	ALIGN_MEMORY_BLOCK(pMemoryBlock->nCurrentOffset);
	pMemoryBlock->pNext = NULL;

	/* hook in to end of chain */

	if (pCurrentMemoryBlock)
		pCurrentMemoryBlock->pNext = pMemoryBlock;
	pCurrentMemoryBlock = pMemoryBlock;

	/* for debugging only */

#ifdef DEBUG_MEMORY_BLOCK
	nMemoryBlockCount++;
#endif //DEBUG_MEMORY_BLOCK
}

/* grab some memory out of current memory block */

static void *xmlGetMemory(int nSize)
{
	/* we only look for space in the current memory block. it's possible that there might be space available at */
	/* the end of a previous block, but memory isn't THAT tight, especially since this memory is typically */
	/* transient (free'd at end of initialization) */

	ASSERT(pCurrentMemoryBlock);
	if (pCurrentMemoryBlock->nSize - pCurrentMemoryBlock->nCurrentOffset < nSize)
	{
		/* out of space, time for a new memory block */

		xmlNewMemoryBlock();
		ASSERT(pCurrentMemoryBlock);
	}

	/* for debugging only */

#ifdef DEBUG_MEMORY_BLOCK
	nMemoryRequestCount++;
	nMemoryRequestTotal += nSize;
	nMemoryRequestAlignedTotal += ALIGN_MEMORY_BLOCK(nSize);
#endif //DEBUG_MEMORY_BLOCK

	/* grab memory. memory record is first thing in block, so can use it's address for start of block */

	ASSERT(pCurrentMemoryBlock->nSize - pCurrentMemoryBlock->nCurrentOffset >= nSize);	// if this hits, requesting more than XML_MEMORY_BLOCK_SIZE!
	void *pReturn = (void *) ((int) pCurrentMemoryBlock + pCurrentMemoryBlock->nCurrentOffset);
	ASSERT(IS_MEMORY_BLOCK_ALIGNED(pReturn));
	pCurrentMemoryBlock->nCurrentOffset += nSize;
	ALIGN_MEMORY_BLOCK(pCurrentMemoryBlock->nCurrentOffset);
	return(pReturn);
}

// add a string to the string list
char *xmlInsertString( char *source )
{
	// make the new node
	xmlStringListNode *node = ( xmlStringListNode * )xmlGetMemory( sizeof( xmlStringListNode ) );
	node->next = &xmlHeadStringNode;
	node->prev = xmlHeadStringNode.prev;
	node->prev->next = node;
	xmlHeadStringNode.prev = node;

	int length = strlen( source );
	node->string = ( char * )xmlGetMemory( length + 1 );
	memset( node->string, 0, length + 1 );
	strcpy( node->string, source );

	return( node->string );
}

// find a string in the string list
char *xmlFindString( char *key )
{
	// xmlHeadStringNode has no valid string associated with it
	xmlStringListNode *curr = xmlHeadStringNode.next;

	while( curr != &xmlHeadStringNode )
	{
		if ( strcmp( curr->string, key ) == 0 )
		{
			return( curr->string );
		}

		curr = curr->next;
	}

	return( NULL );
}

char *xmlMakeNString( char *source, int size )
{
	ASSERT( size < MAKENSTRING_TEMP_BUFFER_SIZE );

	strncpy( makenstring_temp_buffer, ( const char * )source, size );
	makenstring_temp_buffer[ size ] = '\0';

	return( xmlMakeString( makenstring_temp_buffer ) );
}

// add the string to the string list
char *xmlMakeString( char *source )
{
	if ( xml_owns_data )
	{	// we can do what we want with data we own, so, stuff it in a list
		// check for the string in our existing list
		char *result = xmlFindString( source );

		if ( result != NULL )
		{	// if so, return a pointer to it
			return( result );
		}
		else
		{	// if not, make a new entry for it
			return( xmlInsertString( source ) );
		}
	}
	else
	{	// we don't own the data, so we can't store it in a list
		return( xmlMakeStringRaw( source ) );
	}
}

// don't add the string to the string list
char *xmlMakeStringRaw( char *source )
{
	ASSERT( source != NULL );

	int size = strlen( source );
	char *result = (char *) xmlGetMemory(size + 1);
	strncpy( result, ( const char * )source, size );
	result[ size ] = '\0';

	return( result );
}
