
// various ansi codes used by SN's debugger

#ifdef PS2

// ansi reset
#define TTY_RESET "\033[0m"

// ansi colors
#define TTY_WHITE	"\033[30m"
#define TTY_RED		"\033[31m"
#define TTY_GREEN	"\033[32m"
#define TTY_YELLOW	"\033[33m"
#define TTY_BLUE	"\033[34m"
#define TTY_PURPLE	"\033[35m"
#define TTY_CYAN	"\033[36m"

#else

#define TTY_RESET ""
#define TTY_WHITE ""
#define TTY_RED ""
#define TTY_GREEN	""
#define TTY_YELLOW	""
#define TTY_BLUE	""
#define TTY_PURPLE	""
#define TTY_CYAN	""

#endif
