#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void add_note(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docid") && doc["docid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter docid error")
	if ( !(doc.HasMember("content") && doc["content"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter content error")
	
	int doc_id = doc["docid"].GetInt();
	const char *content = doc["content"].GetString();
	
	// 插入新note, 并获取新noteid
	MYS_SQLS;
	SQL("insert into doc_%s_%d(content) values('%s'); \
		 select last_insert_id()",
		uid, doc_id, content);
	if ( MYS_QUERY )
	{
		MYS_UN_SQLS;
		send_jmsg(req, 400, "doc not exist");
		return;
	}
	MYSQL_RES *res = MYS_RESULT;
	MYSQL_ROW row = MYS_NEXT_ROW(res);
	strcpy(noteid, row[0]);
	MYS_FREE(res);
	MYS_UN_SQLS;
	
	// 返回新noteid
	Document doc_out;
	doc_out.SetObject();
	Value val;
	val = StringRef(noteid);
	doc_out.AddMember("noteid", val, doc_out.GetAllocator());
	
	send_json(req, 200, doc_out);
}


} // namespace