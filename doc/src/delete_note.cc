#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void delete_note(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docid") && doc["docid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter docid error")
	if ( !(doc.HasMember("noteid") && doc["noteid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter noteid error")
	
	int doc_id  = doc["docid"].GetInt();
	int note_id = doc["noteid"].GetInt();
	
	SQL("delete from doc_%s_%d where noteid='%d'",
		uid, doc_id, note_id);
	MYS_QUERY;
	
	send_jmsg(req, 200, "delete note done");
}


} // namespace