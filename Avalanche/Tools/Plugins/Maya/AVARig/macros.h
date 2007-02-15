#define FATAL_ERROR(STAT,MSG)					\
	if ( MS::kSuccess != STAT ) {				\
	MGlobal::displayError( MSG );			\
	return MS::kFailure;					\
	}

#define DISPLAY_ERROR(MSG)						\
	MGlobal::displayError( MSG );

#define DISPLAY_INFO(MSG)						\
	MGlobal::displayInfo( MSG );			

