#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void loginreq(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(loginreq);
	
	if ( is_username_error(doc["username"]) )
	{
		send_jmsg(req, 400, "username format error");
		return;
	}
}


} // namespace