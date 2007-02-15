////////////////////////////////////////////////////////////////////////////
//
// TupperwareAggregate
//
// An aggregate entity (stores a collection of other atoms).
//
////////////////////////////////////////////////////////////////////////////

inline int TupperwareAggregate::strnullcmp(const char *str1, const char *str2)
{
	if (str1 == NULL && str1 == NULL)
		return 0;

	if (str1 == NULL)
		return -1;

	if (str2 == NULL)
		return 1;

	return strcmp(str1, str2);
}
