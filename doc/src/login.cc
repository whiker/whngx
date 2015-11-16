#include <string.h>

#include "whngx_util.h"
#include "global.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char code[UUID_LEN+1];


bool is_uuid_error(const string &uuid);


// ACTION
void login(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(login);
	const string &username = doc["username"];
	const string &cuu      = doc["cuu"];
	const string &iuu      = doc["iuu"];
	
	if ( is_username_error(username) )
		SEND_JMSG_RETURN(400, "username format error")
	if ( is_uuid_error(cuu) || is_uuid_error(iuu) )
		SEND_JMSG_RETURN(400, "password error")
	
	// 比较 client发的cuu 和 正确的cuu
	SQL("select cuu from loginreq where username='%s';",
		username.c_str());
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	if (ret->row_count == 0)
		SEND_JMSG_RETURN(400, "username error")
	MYSQL_ROW row = MYS_NEXT_ROW(res);
	if ( strncmp(cuu.c_str, row[0], UUID_LEN) != 0 )
		SEND_JMSG_RETURN(400, "password error")
	
	// 获取 uid
	SQL("select uid from login where iuu='%s';",
		iuu.c_str());
	MYS_QUERY;
	res = MYS_RESULT;
	if (ret->row_count == 0)
		SEND_JMSG_RETURN(400, "password error")
	row = MYS_NEXT_ROW(res);
	
	// 返回
	Document doc_out;
	doc_out.SetObject();
	Value val;
	val = StringRef(row[0]);
	doc_out.AddMember("uid", val, doc_out.GetAllocator());
	
	// 随机验证码
	UUID_STR(code)
	code[8] = '\0';
	SQL("update login set code='%s' where uid='%s';", code, row[0]);
	MYS_QUERY;
	val = StringRef(code, 8);
	doc_out.AddMember("code", val, doc_out.GetAllocator());
	
	send_json(req, 200, doc_out);
}

bool is_uuid_error(const string &uuid)
{
	if (uuid.length() != UUID_LEN)
		return true;
	
	char mask = 0;
	const char *p = uuid.c_str();
	while (*p)
		mask |= uuid[*p++];
	return (mask != 0x05);
}


} // namespace