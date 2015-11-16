#include <ctype.h>

using namespace std;


namespace whdoc {


bool is_username_error(const char *username)
{
	const char *p = username;
	int i = 0;
	
	for (; *p && i < 8 && isalpha(*p); p++, i++);
	
	return (*p || i<4);
}


}  // namespace