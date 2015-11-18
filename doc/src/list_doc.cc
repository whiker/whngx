#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void list_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	Document doc_out;
	doc_out.SetObject();
	Value arr(kArrayType), val;
	Document::AllocatorType& doc_alloc = doc_out.GetAllocator();
	
	SQL("select docname from docl_%s", uid);
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	MYSQL_ROW row;
	MYS_FOREACH_ROW(res,row)
	{
		val = StringRef(row[0]);
		arr.PushBack(val, doc_alloc);
	}
	doc_out.AddMember("docs", arr, doc_alloc);
	
	StringBuffer jbuf;
	Writer<StringBuffer> jw(jbuf);
	doc.Accept(jw);
	MYS_FREE(res);
	
	send_jbuf(req, 200, jbuf);
}


} // namespace