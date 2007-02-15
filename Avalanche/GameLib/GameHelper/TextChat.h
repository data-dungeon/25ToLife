#define KEYBOARD_TEXT_CHAT "KeyBoardTextChat"
#define TEXT_CHAT "TextChat"
#define FILTER_TEXT_CHAT "FilterText"
#define FILTERED_TEXT_CHAT "TextFiltered"

struct STextChat
{
	u64 id;
	u32 flags;
	char *text;
};

