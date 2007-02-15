#include "XML/XML.h"

#if defined( XML_BUILD_ENABLED )

static void WriteElementContents( xmlElement *element, ostream &out );
static void WriteEndTag( xmlElement *element, ostream &out  );
static void WriteTagAttributes( xmlElement *element, ostream &out );
static void WriteStartTag( xmlElement *element, ostream &out );
static void xml_tab_out( xmlElement *element, ostream &out );

static const char _tab = 0x09;

void xml_tab_out( xmlElement *element, ostream &out )
{
	xmlElement *curr;

	curr = element->p_Parent;
	while( curr != NULL )
	{
		out << _tab;
//		fwrite( "\t", 1, 1, file );
		curr = curr->p_Parent;
	}
}

void WriteElementContents( xmlElement *element, ostream &out )
{
	if ( element->element_content & XML_CONTENT_SIMPLE )
	{
		out << element->text;
//		fwrite( element->text, strlen( element->text ), 1, file );
	}

	if ( element->element_content & XML_CONTENT_ELEMENT )
	{
		xmlElement *child = element->p_Children;

		while ( child != NULL )
		{
			out << "\r\n";
//			fwrite( "\n", 1, 1, file );
			xmlWriteElement( child, out );

			child = child->p_NextElement;
		}
	}
}

void WriteEndTag( xmlElement *element, ostream &out )
{
	// pretty-print the end tag
	if ( element->element_content & XML_CONTENT_ELEMENT )
	{	// only indent the end tag if there are child elements
		out << "\r\n";
//		fwrite( "\n", 1, 1, file );
		xml_tab_out( element, out );
	}

	out << "</" << element->name << '>';
//	fwrite( "</", 2, 1, file );
//	fwrite( element->name, strlen( element->name ), 1, file );
//	fwrite( ">", 1, 1, file );
}

void WriteTagAttributes( xmlElement *element, ostream &out )
{
	xmlAttribute *attribute = element->p_Attributes;
	while ( attribute != NULL )
	{
		out << ' ' << attribute->name << "=\"" << attribute->value << '"';
//		fwrite( " ", 1, 1, file );
//		fwrite( attribute->name, strlen( attribute->name ), 1, file );
//		fwrite( "=\"", 2, 1, file );
//		fwrite( attribute->value, strlen( attribute->value ), 1, file );
//		fwrite( "\"", 1, 1, file );

		attribute = attribute->p_NextAttribute;
	}
}

void WriteStartTag( xmlElement *element, ostream &out )
{
	xml_tab_out( element, out );

	out << '<' << element->name;
//	fwrite( "<", 1, 1, file );
//	fwrite( element->name, strlen( element->name ), 1, file );

	WriteTagAttributes( element, out );

	out << '>';
//	fwrite( ">", 1, 1, file );
}

void xmlWriteElement( xmlElement *element, ostream &out )
{
	WriteStartTag( element, out );

	if ( element->element_content != XML_CONTENT_EMPTY )
	{
		WriteElementContents( element, out );
	}

	WriteEndTag( element, out );
}

#endif // XML_BUILD_ENABLED