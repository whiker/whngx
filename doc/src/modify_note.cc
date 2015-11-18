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
	if ( !(doc.HasMember("noteid") && doc["noteid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter noteid error")
	if ( !(doc.HasMember("content") && doc["content"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter content error")
	
	int doc_id  = doc["docid"].GetInt();
	int note_id = doc["noteid"].GetInt();
	const char *content = doc["content"].GetString();
	
	SQL("update doc_%s_%d set content='%s' where noteid='%d'",
		uid, doc_id, content, note_id);
	if ( MYS_QUERY )
	{
		SQL("insert into doc_%s_%d(noteid, content) \
			 values('%d', '%s')",
			uid, doc_id, note_id, content);
		MYS_QUERY;
	}
	
	send_jmsg(req, 200, "add note done");
}


} // namespace