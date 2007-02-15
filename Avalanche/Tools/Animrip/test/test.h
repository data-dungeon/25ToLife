//this is a comment
#define BLAH 1
#define BLAH2 0xff
#define BLAH3 3
#define FOO	0xdead	//this is a comment

#define TestProc( a ) \
  PUSH a;\
  PUSH 0.0f;\
  CALLC testProcCall;\

/* here is a test
of the comment stripping routines
*/