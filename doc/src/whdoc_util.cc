#include <ctype.h>
#include <string>

using namespace std;


namespace whdoc {


bool is_username_error(const string &username)
{
	int len = username.length();
	if (len < 4 || len > 8)
		return true;
	
	const char *p = username.c_str();
	while ( *p && isalpha(*p) )
		p++;
	return *p != '\0';
}


}  // namespace