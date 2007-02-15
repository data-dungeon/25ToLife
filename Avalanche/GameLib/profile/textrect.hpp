//*************************************************************************
// A really simple rect class for text
//*************************************************************************

// We need these here so we don't need any include files
#ifndef min
#define min(a, b)    ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b)    ((a) > (b) ? (a) : (b))
#endif

//*************************************************************************
// Find the union
//*************************************************************************
inline TextRect &TextRect::operator+=(
const TextRect &r)
{
	left = min(left, r.left);
	top = min(top, r.top);
	right = max(right, r.right);
	bottom = max(bottom, r.bottom);

	return(*this);
}

//*************************************************************************
// Find the union
//*************************************************************************
inline TextRect TextRect::operator+(
const TextRect &r)
{
	TextRect temp = *this;

	temp += r;
	return(temp);
}

//*************************************************************************
// Find the intersection
//*************************************************************************
inline TextRect &TextRect::operator-=(
const TextRect &r)
{
	// Do the rects even intersect?
	if (right < r.left || left > r.right || top > r.bottom || bottom < r.top)
		Clear();
	else
	{
		// Do the other work
		left = max(left, r.left);
		top = max(top, r.top);
		right = min(right, r.right);
		bottom = min(bottom, r.bottom);
	}

	return(*this);
}

//*************************************************************************
// Find the intersection
//*************************************************************************
inline TextRect TextRect::operator-(
const TextRect &r)
{
	TextRect temp = *this;
	temp -= r;
	return(temp);
}

//*************************************************************************
// Operators - mainly to clean up the header file...
//*************************************************************************
inline int TextRect::operator==(const TextRect &r)
{
	return(r.left == left && r.right == right &&
				r.top == top && r.bottom == bottom);
}

//*************************************************************************
//*************************************************************************
inline int TextRect::operator!=(const TextRect &r)
{
	return(r.left != left || r.right != right ||
				r.top != top || r.bottom != bottom);
}

//*************************************************************************
//*************************************************************************
inline TextRect &TextRect::operator=(const TextRect &r)
{
	Set(r.left, r.top, r.right, r.bottom);
	return(*this);
}

//*************************************************************************
// Make sure left < right, top < bottom
//*************************************************************************
inline void TextRect::Normalize(void)
{
	if (left > right)
		Swap(left, right);
	if (top > bottom)
		Swap(top, bottom);
}

//*************************************************************************
// Inflate deflate the rect
//*************************************************************************
inline void TextRect::Inflate(
int x,
int y)
{
	left -= x;
	right += x;
	top -= y;
	bottom += y;
}

//*************************************************************************
// The set
//*************************************************************************
inline void TextRect::Set(
int newLeft,
int newTop,
int newRight,
int newBottom)
{
	left = newLeft;
	top = newTop;
	right = newRight;
	bottom = newBottom;
}

