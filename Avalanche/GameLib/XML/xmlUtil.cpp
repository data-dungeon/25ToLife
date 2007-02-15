#include <stdlib.h>
#include "Layers/Memory.h"
#include "XML/XML.h"
#include "Math/Quatern.h"

/* xml tree now lives in a series of largish malloc'd buffers, instead of thousands of small new's. if this is the
database head, then we should delete the buffers, otherwise there is nothing to do */

void xmlFreeElement(xmlElement *element)
{
	if ((element) && (element->element_content & XML_CONTENT_DATABASE_HEAD))
	{
		ASSERT(element->p_Parent);	// should have been set by xmlParseBuffer
		xmlFreeMemoryBlocks((ts_xmlMemory *) element->p_Parent);
	}
}

#if 0
xmlElement *xmlFreeElement( xmlElement *element )
{
	if ( element == NULL )
	{
		return( NULL );
	}

	bool owns_data = ( element->element_content & XML_CONTENT_OWNS_DATA ) != 0;

	// delete attributes
	xmlAttribute *curr_attr = element->p_Attributes;

	while( curr_attr != NULL )
	{
		xmlAttribute *next_attr = NULL;
		if ( owns_data )
		{
			if ( curr_attr->name != NULL )
			{
				delete [] curr_attr->name;
			}

			if ( curr_attr->value != NULL )
			{
				delete [] curr_attr->value;
			}
		}

		curr_attr->name = NULL;
		curr_attr->value = NULL;

		next_attr = curr_attr->p_NextAttribute;

		delete curr_attr;

		curr_attr = next_attr;
	}

	element->p_Attributes = NULL;

	// delete children
	xmlElement *curr_elem = element->p_Children;

	while( curr_elem != NULL )
	{
		xmlElement *next_elem = NULL;

		next_elem = xmlFreeElement( curr_elem );

		curr_elem = next_elem;
	}

	element->p_Children = NULL;

	// delete text
	if ( owns_data )
	{
		if ( element->text != NULL )
		{
			delete [] element->text;
		}

		// delete the element name
		if ( element->name != NULL )
		{
			delete [] element->name;
		}
	}

	element->text = NULL;
	element->name = NULL;

	curr_elem = element->p_NextElement;

	delete element;

	return( curr_elem );
}
#endif //0

// search the given element's child list for a child named *name
xmlElement		*xmlChildByName( xmlElement *element, const char *name )
{
	if(element->p_Children)
	{
		xmlElement *curr = element->p_Children;

		while( curr != NULL )
		{
			if ( strcmp( ( const char * )curr->name, name ) == 0 )
			{
				return( curr );
			}

			curr = curr->p_NextElement;
		}
	}

	return( NULL );
}

// get the indexTH child from the element, or NULL
xmlElement *xmlChildByIndex( xmlElement *element, unsigned int index )
{
	xmlElement *curr = element->p_Children;
	unsigned int curr_index = 0;

	while( ( curr != NULL ) && ( curr_index < index ) )
	{
		curr = curr->p_NextElement;
		curr_index++;
	}

	return( curr );
}

// get the first child that has the given attribute and value pair
xmlElement *xmlChildByAttributeValue( xmlElement *element, const char *attributeKey, const char *attributeValue )
{
	if ( ( element == NULL ) || ( attributeKey == NULL ) || ( attributeValue == NULL ) )
	{	// sanity checking
		return( NULL );
	}
	else
	{
		return( xmlNextByAttributeValue( element->p_Children, attributeKey, attributeValue ) );
	}
}
xmlElement *xmlChildPrevious( xmlElement *element )
{
	xmlElement *pCurr = element->p_Parent->p_Children;
	while ( ( pCurr != NULL ) && ( pCurr->p_NextElement != element ) )
	{
		pCurr = pCurr->p_NextElement;
	}

	return( pCurr );
}

// get the next sibling (starting with the given element) that has the given attribute and value pair
xmlElement *xmlNextByAttributeValue( xmlElement *element, const char *attributeKey, const char *attributeValue )
{
	if ( ( element == NULL ) || ( attributeKey == NULL ) || ( attributeValue == NULL ) )
	{	// sanity checking
		return( NULL );
	}
	else
	{
		xmlElement *pCurr = element;
		while ( pCurr != NULL )
		{
			xmlAttribute *pAttr = xmlAttributeByName( pCurr, attributeKey );
			if ( ( pAttr != NULL ) && ( strcmp( attributeValue, pAttr->value ) == 0 ) )
			{	// found a matching key/value pair
				return( pCurr );
			}

			pCurr = pCurr->p_NextElement;
		}

		// failed to find a matching next
		return( NULL );
	}
}

// search the given element's attribute list for an attribute named *name
xmlAttribute	*xmlAttributeByName( xmlElement *element, const char *name )
{
	xmlAttribute *curr = element->p_Attributes;
	while( curr != NULL )
	{
		if ( strcmp( ( const char * )curr->name, name ) == 0 )
		{
			return( curr );
		}

		curr = curr->p_NextAttribute;
	}

	return( NULL );
}

/* return a float value from the passed child's text field */

float xmlGetFloatFromChildByName(xmlElement *element, const char *name)
{
	element = xmlChildByName(element, name);
	ASSERT(element);
	if (element)
		return((float) atof(element->text));
	return(0.0f);
}

float xmlGetFloat(xmlElement *element)
{
	ASSERT(element);
	if (element)
		return((float) atof(element->text));
	return(0.0f);
}

int xmlGetInt(xmlElement *element)
{
	ASSERT(element);
	if (element)
		return(atoi(element->text));
	return(0);
}

/* return an integer value from the passed child's text field */

int xmlGetIntFromChildByName(xmlElement *element, const char *name)
{
	element = xmlChildByName(element, name);
	ASSERT(element);
	if (element)
		return(atoi(element->text));
	return(0);
}

/* fill in a Vector3 from passed element's child position node-- requested child node should in turn have x, y, and z children */

Vector3 xmlGetVct3f(xmlElement *element)
{
	Vector3 AnswerVector;
	ASSERT(element);
	/*pVector->x = xmlGetFloatFromChildByName(element, "x");
	pVector->y = xmlGetFloatFromChildByName(element, "y");
	pVector->z = xmlGetFloatFromChildByName(element, "z");*/

	AnswerVector.Set(xmlGetFloatFromChildByName(element, "x"),
						  xmlGetFloatFromChildByName(element, "y"),
						  xmlGetFloatFromChildByName(element, "z"));

	return AnswerVector;
}

void xmlGetVct3fFromChildByName(xmlElement *element, const char *name, Vector3 *pVector)
{
	xmlElement *pVectorNode = xmlChildByName(element, name);
	ASSERT(pVectorNode);
	pVector->Set(xmlGetFloatFromChildByName(pVectorNode, "x"),
					 xmlGetFloatFromChildByName(pVectorNode, "y"),
					 xmlGetFloatFromChildByName(pVectorNode, "z"));
}

/* fill in a ts_Quatvw from passed element's child position node -- child needs x, y, z and w children */
void xmlGetQuatFromChildByName( xmlElement *element, const char *name, ts_Quatvw *pQuat )
{
	xmlElement *pQuatNode = xmlChildByName( element, name );
	ASSERT( pQuatNode != NULL );
	pQuat->V.Set( xmlGetFloatFromChildByName( pQuatNode, "x" ),
					  xmlGetFloatFromChildByName( pQuatNode, "y" ),
					  xmlGetFloatFromChildByName( pQuatNode, "z" ));
	pQuat->W	= xmlGetFloatFromChildByName( pQuatNode, "w" );
}


#if defined( XML_BUILD_ENABLED )

// returns the new element
xmlElement		*xmlNewElement( const char *name, const char *text )
{
	xmlElement *result;

	ASSERT( name != NULL );

	result = new xmlElement;
	result->element_content = 0;
	result->p_Attributes = NULL;
	result->p_Children = NULL;
	result->p_NextElement = NULL;
	result->p_Parent = NULL;

	result->name = new char[ strlen( name ) + 1 ];
	strcpy( result->name, name );

	if ( text != NULL )
	{
		result->text = new char[ strlen( text ) + 1 ];
		strcpy( result->text, text );
		result->element_content |= XML_CONTENT_SIMPLE;
	}
	else
	{
		result->text = NULL;
	}

	return( result );
}

// returns element
xmlElement		*xmlAddElement( xmlElement *element, xmlElement *child )
{
	ASSERT( element != NULL );
	ASSERT( child != NULL );

	xmlElement *curr, *prev;

	curr = element->p_Children;
	prev = NULL;
	while( curr != NULL )
	{
		prev = curr;
		curr = curr->p_NextElement;
	}

	if ( prev == NULL )
	{	// this is the first child
		element->p_Children = child;
	}
	else
	{
		prev->p_NextElement = child;
	}
	child->p_NextElement = NULL;

	element->element_content |= XML_CONTENT_ELEMENT;

	return( element );
}

// returns the new attribute
xmlAttribute	*xmlAddAttribute( xmlElement *element, const char *name, const char *value )
{
	ASSERT( element != NULL );
	ASSERT( name != NULL );
	ASSERT( value != NULL );

	xmlAttribute *curr, *prev, *attr = new xmlAttribute;

	attr->name = new char[ strlen( name ) + 1 ];
	strcpy( attr->name, name );

	attr->value = new char[ strlen( value ) + 1 ];
	strcpy( attr->value, value );

	curr = element->p_Attributes;
	prev = NULL;
	while( curr != NULL )
	{
		prev = curr;
		curr = curr->p_NextAttribute;
	}

	if ( prev == NULL )
	{	// first attribute
		element->p_Attributes = attr;
	}
	else
	{
		prev->p_NextAttribute = attr;
	}
	attr->p_NextAttribute = NULL;

	return( attr );
}

#endif // XML_BUILD_ENABLED

/* follow the linked list to free memory blocks used to build xml database tree */

void xmlFreeMemoryBlocks(ts_xmlMemory *pMemoryBlock)
{
	while (pMemoryBlock)
	{
		ts_xmlMemory *pNext = pMemoryBlock->pNext;
#if defined( XML_BUILD_ENABLED )
		free(pMemoryBlock);	// the memory record is always the first thing in the malloc'd block
#else
		memFree(pMemoryBlock);	// the memory record is always the first thing in the malloc'd block
#endif
		pMemoryBlock = pNext;
	}
}
