#ifndef __HMACROS_H
#define __HMACROS_H

#define THROW_FATAL_ERROR(MSG)					\
	throw HStatus(HStatus::kFailure,MSG);

#define FATAL_ERROR(STAT,MSG)					\
		STAT.setErrorString(MSG);				\
		STAT.setStatusCode(HStatus::kFailure);	\
		return status;

#endif