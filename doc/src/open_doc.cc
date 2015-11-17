#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void open_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docname") && doc["docname"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter docname error")
	
	const char *docname = doc["docname"].GetString();
	
	// 获取 docid
	SQL("select docid from docl_%s where docname='%s'",
		uid, docname);
	MYS_QUERY;
	
	MYSQL_RES *res = MYS_RESULT;
	int res_cnt = res->row_count;
	if (res_cnt > 0)
	{
		MYSQL_ROW row = MYS_NEXT_ROW(res);
		strcpy(docid, row[0]);
	}
	MYS_FREE(res);
	if (res_cnt == 0)
		SEND_JMSG_RETURN(400, "doc not exist")
	
	// 返回 docid
	Document doc_out;
	doc_out.SetObject();
	Value val;
	val = StringRef(docid);
	doc_out.AddMember("docid", val, doc_out.GetAllocator());
	
	send_json(req, 200, doc_out);
}


} // namespace