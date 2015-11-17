#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void create_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docname") && doc["docname"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter docname error")
	
	const char *docname = doc["docname"].GetString();
	
	SQL("insert into docl_%s(docname) values('%s')",
		uid, docname);
	if ( MYS_QUERY )
		SEND_JMSG_RETURN(400, "doc already exists")
	else
		SEND_JMSG_RETURN(200, "create doc ok")
}


} // namespace