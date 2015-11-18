#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void modify_note(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docid") && doc["docid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter docid error")
	
	// int doc_id = doc["docid"].GetInt();
}


} // namespace