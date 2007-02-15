//////////////////////////////////////////////////////////////////////////////////////////
//
// HSHADER CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HSHADER_H
#define __HSHADER_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HCOLOR_H
#include "hcolor.h"
#endif

class HShaderArray;

class HShader
{
public:
	struct sflags {
		unsigned long	kIsAGP					: 1;
	} flags;

	HShader();
	HShader(const HShader &other);
	~HShader();

	void				clear();
	bool				hasMaps();

	HShader				*next();
	HShader				*previous();
	HShader				*begin();
	HShader				*end();

	HShader				&operator = ( const HShader &other );
	bool				operator == ( const HShader &other ) const;
	bool				operator != ( const HShader &other ) const;

	friend std::ostream		&operator << (std::ostream &os,HShader &other);

	UString				name;
	HColor				diffusecolor;
	float				diffusefalloff;
	HColor				specularcolor;
	float				specularsize;
	float				specularintensity;
	HColor				transparency;
	float				refraction;
	float				roughness;
	float				roughnesscale;
	float				reflectivity;
	float				reflectivityfalloff;
	float				ambiance;
	float				translucency;
	float				density;
	float				radiosity;
	
	std::vector<int>	mapIndicies;

	HShaderArray		*shaderArray;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// HSHADER ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

class HShaderArray
{
public:

	HShaderArray ();
	~HShaderArray ();

	void				clear(void);
	unsigned			length(void) const;
	unsigned			add(HShader &shader);
	unsigned			append(HShader &shader);

	HShader				*get(unsigned index);
	HShader				*begin();
	HShader				*end();

	HShaderArray		&operator = ( const HShaderArray &other );
	HShader				operator[]( unsigned index ) const;
 	HShader&	 		operator[]( unsigned index );

	friend std::ostream		&operator << (std::ostream &os,HShaderArray &other);

	std::vector<HShader> classList;
};

#endif