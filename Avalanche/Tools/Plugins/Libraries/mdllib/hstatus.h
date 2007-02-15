#ifndef __HSTATUS_H
#define __HSTATUS_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif

class HStatus
{
public:
	enum HStatusCode { kNoStatus, kFailure, kSuccess };

	HStatus();
	HStatus(HStatusCode other);
	HStatus(const HStatus &other);
	HStatus::HStatus(HStatusCode other,const char *errstring);

	virtual		~HStatus();

	const char			*errorString();
	void				setErrorString(UString errstring);
	void				setErrorString(const char *errstring);
	void				setStatusCode(HStatusCode code);
	bool				isErrorString();

	HStatus				&operator = ( const HStatus &other );
	bool				operator == ( const HStatus::HStatusCode other) const;
	bool				operator != ( const HStatus::HStatusCode other) const;

private:
	HStatusCode statusCode;
	UString *error_string;
};

#endif