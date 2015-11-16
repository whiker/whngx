#include <string.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char uid[16], id[16];
char code[UUID_LEN+1];


bool is_uuid_error(const char *uuid);


// ACTION
void login(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(login);
	const char *username = doc["username"].GetString();
	const char *cuu      = doc["cuu"].GetString();
	const char *iuu      = doc["iuu"].GetString();
	
	if ( is_username_error(username) )
		SEND_JMSG_RETURN(400, "username format error")
	if ( is_uuid_error(cuu) || is_uuid_error(iuu) )
		SEND_JMSG_RETURN(400, "password error")
	
	// 查找 正确的cuu
	SQL("select cuu from loginreq where username='%s'",
		username);
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	if (res->row_count == 0)
	{
		MYS_FREE(res);
		SEND_JMSG_RETURN(400, "username error")
	}
	
	// 比较 client的cuu 和 正确的cuu
	MYSQL_ROW row = MYS_NEXT_ROW(res);
	if ( strncmp(cuu, row[0], UUID_LEN) != 0 )
	{
		MYS_FREE(res);
		SEND_JMSG_RETURN(400, "password error")
	}
	MYS_FREE(res);
	
	// 获取 uid
	SQL("select uid from login where iuu='%s'",
		iuu);
	MYS_QUERY;
	res = MYS_RESULT;
	if (res->row_count == 0)
	{
		MYS_FREE(res);
		SEND_JMSG_RETURN(400, "password error")
	}
	row = MYS_NEXT_ROW(res);
	strcpy(uid, row[0]);
	MYS_FREE(res);
	
	// 随机验证码
	UUID_STR(code)
	code[8] = '\0';
	
	// 插入表-idcheck
	MYS_SQLS;
	SQL("insert into idcheck(uid, code) values('%s', '%s', now()); \
		 select last_insert_id()",
		uid, code);
	MYS_QUERY;
	res = MYS_RESULT;
	row = MYS_NEXT_ROW(res);
	strcpy(id, row[0]);
	MYS_FREE(res);
	MYS_UN_SQLS;
	
	// 返回
	Document doc_out;
	doc_out.SetObject();
	Value val;
	val = StringRef(id);
	doc_out.AddMember("id", val, doc_out.GetAllocator());
	val = StringRef(code, 8);
	doc_out.AddMember("code", val, doc_out.GetAllocator());
	
	send_json(req, 200, doc_out);
	
	// 更多安全考虑, 检查client IP
}

bool is_uuid_error(const char *uuid)
{
	const char *p = uuid;
	char mask = 0;
	int i = 0;
	
	for (; *p && i < UUID_LEN; p++, i++)
		mask |= uuid_check_map[(int)*p];
	
	return (*p || i != UUID_LEN || mask != 0x05);
}


} // namespace