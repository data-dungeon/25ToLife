#ifndef __HATTR_H
#define __HATTR_H

#ifndef __HCOLOR_H
#include "hcolor.h"
#endif

class HAttr
{
public:
	HAttr();
	HAttr(const HAttr &other);
	~HAttr();

	void				clear(void);

	void				readHAttr(std::ifstream &fin);

	HAttr				&operator = ( const HAttr &other );
	bool				operator == ( const HAttr &other ) const;
	bool				operator != ( const HAttr &other ) const;

	HColor				diffusecolor;
	float				diffusefalloff;
	float				ambiance;
	float				roughness;
	float				roughnesscale;
	float				reflectivity;
	HColor				transparency;
	float				refraction;
	HColor				specularcolor;
	float				specularsize;
	float				specularintensity;
	float				density;
	float				translucency;
	float				radiosity;
	float				reflectivityfalloff;
};

class HAttrArray
{
public:

	HAttrArray ();
	~HAttrArray ();

	void			clear(void);
	unsigned		length(void) const;
	void			append(HAttr &attr);

	void				readHAttrArray(std::ifstream &fin);

	HAttrArray &operator = ( const HAttrArray &other );

	HAttr			operator[]( unsigned index ) const;
 	HAttr&	 		operator[]( unsigned index );

	std::vector<HAttr> classList;
};

#endif