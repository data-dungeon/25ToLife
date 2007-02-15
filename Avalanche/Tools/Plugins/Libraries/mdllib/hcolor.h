
#ifndef __HCOLOR_H
#define __HCOLOR_H

//supports rgb 0-1

class HColor
{
public:
		double		r;
		double		g;
		double		b;
		double		a;

		HColor ();
		HColor (double r, double g, double b);
		HColor (double r, double g, double b, double a);

		HColor(double value);

		~HColor ();

		HColor				&operator = ( const HColor &other );
		HColor				&operator = ( double value );
		bool				operator == ( const HColor &other ) const;
		bool				operator != ( const HColor &other ) const;

		friend std::ostream		&operator << (std::ostream &os,HColor &other);

};

#endif
