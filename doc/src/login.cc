#include "whngx_util.h"
#include "global.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void login(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(login);
}


} // namespace