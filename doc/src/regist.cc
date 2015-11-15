#include <ctype.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


static bool is_password_error(const string &password);


// ACTION
void regist(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(regist)
	
	if ( is_username_error(doc["username"]) )
	{
		send_jmsg(req, 400, "username format error");
		return;
	}
	if ( is_password_error(doc["password"]) )
	{
		send_jmsg(req, 400, "password format error");
		return;
	}
}


bool is_password_error(const string &password)
{
	int len = password.length();
	if (len != 6)
		return true;
	
	const char *p = password.c_str();
	while ( *p && ( !(*p<'0' || *p>'9') ) )
		p++;
	return *p != '\0';
}


} // namespace