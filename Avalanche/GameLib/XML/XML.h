#if !defined( __XML_H )
#define __XML_H

#include "platform/BaseType.h"
#include "Math/Vector.h"

#ifdef PS2
#include <ctype.h>
#include <string.h>
#endif

#ifdef GCN
#include <ctype.h>
#include <string.h>
#include "platform/GCN/GCNMWPort.h"
#endif

#if defined( XML_CREATE_ENABLED )
#include <stdio.h>
#else
#include "EngineHelper/filestream.h"
#endif

struct xmlAttribute
{
	char				*name;
	char				*value;

	xmlAttribute	*p_NextAttribute;
};

#define XML_CONTENT_EMPTY				( 0x00 )
#define XML_CONTENT_ELEMENT			( 0x01 )
#define XML_CONTENT_SIMPLE				( 0x02 )
#define XML_CONTENT_MIXED				( XML_CONTENT_ELEMENT | XML_CONTENT_SIMPLE )
#define XML_CONTENT_OWNS_DATA			( 0x80 )
#define XML_CONTENT_DATABASE_HEAD	( 0x40 )	// record is the head of the xml tree. p_Parent points to memory block record

struct xmlElement
{
	char				*name;
	unsigned char	element_content;		// flag for which type of content

	char				*text;					// for simple content

	xmlElement		*p_Children;			// for element content
	xmlElement		*p_Parent;

	xmlAttribute	*p_Attributes;

	xmlElement		*p_NextElement;
};

/* this structure is used to record the memory blocks used to build the xml tree. the issue is that as originally implemented,
the xml parsing code did thousands of tiny new's to allocate the tree memory. for the run-time game engine, this turned out
to be expensive in memory handles, and in release time. so we will malloc a series of largish blocks, one at a time, and each 
"new" will chew a chunk out of the block. when we run out of space, we'll malloc another one. the first memory in each block
will be a ts_xmlMemory record, and the first xmlElement in the tree (the head of the entire database) will use its p_Parent
field to point to the first memory record. the memory record is always the first record in the memory block

this results in a more "static" structure for the xml database tree-- individual elements can't be free'd, but more
elements can always be added */

#define XML_MEMORY_BLOCK_SIZE		5000	// in bytes

struct ts_xmlMemory
{
	int					nSize;				// size of malloc'd block-- always XML_MEMORY_BLOCK_SIZE? let use override?
	int					nCurrentOffset;	// offset of next available memory in block
	ts_xmlMemory		*pNext;				// once we get to the first memory record, we have a linked list of memory blocks
};

/* align memory block sub-blocks to 4-byte alignment */

#define MEMORY_BLOCK_ALIGNMENT 4
#define ALIGN_MEMORY_BLOCK(n)					n = (n + MEMORY_BLOCK_ALIGNMENT - 1) & ~(MEMORY_BLOCK_ALIGNMENT - 1)
#define IS_MEMORY_BLOCK_ALIGNED(pAddr)		((int) pAddr & (MEMORY_BLOCK_ALIGNMENT - 1)) == 0

// xmlParse
xmlElement		*xmlParseBuffer( unsigned char *buffer, unsigned int length, bool owns_data = true );
#if defined( XML_BUILD_ENABLED )
xmlElement		*xmlParseFile( FILE *file, bool owns_data = true );
#else
xmlElement		*xmlParseFile( FileStream *file, bool owns_data = true );
#endif
xmlElement		*xmlParseFile(char *pFileName);

// xmlUtil
void				xmlFreeElement( xmlElement *element );											// if element is database head, frees memory blocks
xmlElement		*xmlChildByName( xmlElement *element, const char *name );				// returns the first child element "name"
xmlElement		*xmlChildByIndex( xmlElement *element, unsigned int index );			// returns the nth child of element - or NULL if no nth child
xmlElement		*xmlChildByAttributeValue( xmlElement *element, const char *attributeKey, const char *attributeValue );
xmlElement		*xmlChildPrevious( xmlElement *element );
xmlElement		*xmlNextByAttributeValue( xmlElement *element, const char *attributeKey, const char *attributeValue );
xmlAttribute	*xmlAttributeByName( xmlElement *element, const char *name );			// returns the first attribute "name"
float				xmlGetFloat(xmlElement *element);												// returns atof of current element's text field
float				xmlGetFloatFromChildByName(xmlElement *element, const char *name);	// returns atof of child's text field
int				xmlGetInt(xmlElement *element);
int				xmlGetIntFromChildByName(xmlElement *element, const char *name);		// returns atoi of child's text field
Vector3			xmlGetVct3f(xmlElement *element);		
void				xmlGetVct3fFromChildByName(xmlElement *element, const char *name, Vector3 *pVector);	// requested child should have x, y, and z children
void				xmlGetQuatFromChildByName( xmlElement *element, const char *name, ts_Quatvw *pQuat );	// requested child should have x, y, z, and w children
void				xmlFreeMemoryBlocks(ts_xmlMemory *pMemoryBlock);

#if defined( XML_BUILD_ENABLED )
#include <ostream.h>

xmlElement		*xmlNewElement( const char *name, const char *text );							// returns the new element
xmlElement		*xmlAddElement( xmlElement *element, xmlElement *child );						// returns element
xmlAttribute	*xmlAddAttribute( xmlElement *element, const char *name, const char *value );	// returns the new attribute

// xmlWrite
void			xmlWriteElement( xmlElement *element, ostream &out );
#endif	// XML_BUILD_ENABLED

#endif