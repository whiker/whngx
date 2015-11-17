#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void restore_doc(ngx_http_request_t *req, Document &doc)
{
	CHECK_ID_TO_GET_UID(uid)
	
	if ( !(doc.HasMember("docname") && doc["docname"].IsString()) )
		SEND_JMSG_RETURN(400, "parameter docname error")
	
	const char *docname = doc["docname"].GetString();
	
	// 获取 docid
	SQL("select docid from docd_%s where docname='%s'",
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
		SEND_JMSG_RETURN(400, "can't restore")
	
	// 从 docd_UID 移到 docl_UID
	SQL("delete from docd_%s where docname='%s'",
		uid, docname);
	MYS_QUERY;
	
	SQL("insert into docl_%s values('%s_%s', '%s')",
		uid, docname, docid, docid);
	MYS_QUERY;
	
	send_jmsg(req, 200, "restore doc done");
}


} // namespace