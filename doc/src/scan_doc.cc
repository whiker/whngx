#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void scan_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	// 获取参数
	int doc_id, from_noteid = 0, note_num = 0;
	
	if ( !(doc.HasMember("docid") && doc["docid"].IsInt()) )
		SEND_JMSG_RETURN(400, "parameter docid error")
	else
		doc_id = doc["docid"].GetInt();
	
	if ( doc.HasMember("from_noteid") )
	{
		if ( ! doc["from_noteid"].IsInt() )
			SEND_JMSG_RETURN(400, "parameter from_noteid error")
		from_noteid = doc["from_noteid"].GetInt();
		if (from_noteid < 0)
			from_noteid = 0;
	}
	
	if ( doc.HasMember("note_num") )
	{
		if ( ! doc["note_num"].IsInt() )
			SEND_JMSG_RETURN(400, "parameter note_num error")
		note_num = doc["note_num"].GetInt();
		if (note_num <= 0)
			note_num = 10000;
	}
	
	// 获取结果
	SQL("select * from doc_%s_%d where noteid>=%d limit %d",
		uid, doc_id, from_noteid, note_num);
	if ( MYS_QUERY )
		SEND_JMSG_RETURN(400, "doc not exist")
	MYSQL_RES *res = MYS_RESULT;
	
	// 返回
	Document doc_out;
	doc_out.SetObject();
	Document::AllocatorType& doc_alloc = doc_out.GetAllocator();
	Value key, val;
	MYSQL_ROW row;
	
	MYS_FOREACH_ROW(res, row)
	{
		key = StringRef(row[0]);
		val = StringRef(row[1]);
		doc_out.AddMember(key, val, doc_alloc);
	}
	
	StringBuffer jbuf;
	Writer<StringBuffer> jw(jbuf);
	doc.Accept(jw);
	MYS_FREE(res);
	
	send_jbuf(req, 200, jbuf);
}


} // namespace