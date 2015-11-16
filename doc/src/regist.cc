#include <ctype.h>
#include <uuid/uuid.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char cuu[UUID_LEN+1],     iuu[UUID_LEN+1];
char cxx[UUID_XXX_LEN+1], ixx[UUID_XXX_LEN+1];


bool is_password_error(const string &password);
void generate_uuid_xxx(char *plain, char *cipher, const void *key);


// ACTION
void regist(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(regist)
	const string &username = doc["username"];
	const string &password = doc["password"];
	
	if ( is_username_error(username) )
		SEND_JMSG_RETURN(400, "username format error")
	if ( is_password_error(password) )
		SEND_JMSG_RETURN(400, "password format error")
	
	// 判断是否已注册过
	SQL("select username from loginreq where username=('%s')",
		username.c_str());
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	int row_cnt = res->row_count;
	MYS_FREE(res);
	if (row_cnt > 0)
		SEND_JMSG_RETURN(400, "username already exists")
	
	// 生成(cuu,iuu,cxx,ixx)
	string key = password + password;
	generate_uuid_xxx(cuu, cxx, key.c_str());
	generate_uuid_xxx(iuu, ixx, key.c_str());
	
	// 插入新用户
	SQL("insert into loginreq(username,cxx,ixx,cuu) values('%s','%s','%s','%s')",
		username.c_str(), cxx, ixx, cuu);
	if ( MYS_QUERY )
		SEND_JMSG_RETURN(400, "username already exists")
	SQL("insert into login(iuu) values('%s')", iuu);
	MYS_QUERY;
	
	send_jmsg(req, 200, "regist ok");
}

bool is_password_error(const string &password)
{
	if (password.length() != 8)
		return true;
	
	char mask = 0;
	const char *p = password.c_str();
	while (*p)
		mask |= password_check_map[*p++];
	return (mask != 0x03);
}

void generate_uuid_xxx(char *plain, char *cipher, const void *key)
{
	uuid_t uuid;
	uuid_generate(uuid);
	uuid_unparse(uuid, plain);
	
	tea_encrypt_n(&uuid, key, &uuid, 2);
	uuid_unparse(uuid, cipher);
}


} // namespace