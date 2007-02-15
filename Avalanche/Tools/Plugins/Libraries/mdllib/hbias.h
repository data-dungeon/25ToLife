//////////////////////////////////////////////////////////////////////////////////////////
//
// BIAS CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HBIAS_H
#define __HBIAS_H

class HBias
{
public:
	HBias();
	HBias(const HBias &other);
	~HBias();

	void		clear();

	double		alpha;
	double		gamma;
	double		magnitude;

	void		readHBias(std::ifstream &fin);

	HBias		&operator = ( const HBias &other );
};

#endif