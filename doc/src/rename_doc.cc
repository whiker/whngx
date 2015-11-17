#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void rename_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("oldname") && doc["oldname"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter oldname error")
	if ( !(doc.HasMember("newname") && doc["newname"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter newname error")
	
	const char *oldname = doc["oldname"].GetString();
	const char *newname = doc["newname"].GetString();
	
	// 更新文档名字
	SQL("update docl_%s set docname='%s' where docname='%s'",
		uid, newname, oldname);
	if ( MYS_QUERY )
		SEND_JMSG_RETURN(400, "newname already exist")
	else
		SEND_JMSG_RETURN(200, "rename done")
}


} // namespace