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
	
	// 向 docl_%s 插入新行
	MYS_SQLS;
	SQL("insert into docl_%s(docname) values('%s'); \
		 select last_insert_id()",
		uid, docname);
	if ( MYS_QUERY )
	{
		MYS_UN_SQLS;
		send_jmsg(req, 400, "doc already exists");
		return;
	}
	// 获取新docid
	res = MYS_RESULT;
	row = MYS_NEXT_ROW(res);
	strcpy(docid, row[0]);
	MYS_FREE(res);
	MYS_UN_SQLS;
	
	// 创建文档表
	SQL("create table doc_%s_%s ( \
			noteid int auto_increment, \
			note varchar() not null, \
			primary key (noteid) \
		) default charset=utf8",
		uid, docid);
	MYS_QUERY;
	
	send_jmsg(req, 200, "create doc done");
}


} // namespace