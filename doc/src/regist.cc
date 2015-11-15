#include <ctype.h>
#include <stdio.h>
#include <uuid/uuid.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char regist_sql[256];
char cuu[37], iuu[37];
char cxx[37], ixx[37];


bool is_password_error(const string &password);
void generate_uuid_xx(char *plain, char *cipher, const void *key);


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
	sprintf(regist_sql,
		"select username from loginreq where username=('%s');",
		username.c_str());
	mysql_query(mys_conn, regist_sql);
	MYSQL_RES *res = mysql_store_result(conn);
	if (res->row_count > 0)
		SEND_JMSG_RETURN(400, "username already exists")
	
	// 生成(cuu,iuu,cxx,ixx)
	string key = password + password;
	generate_uuid_xx(cuu, cxx, key.c_str());
	generate_uuid_xx(iuu, ixx, key.c_str());
	
	// 插入新用户
	sprintf(regist_sql,
		"insert into loginreq(username,cxx,ixx,cuu) values('%s','%s','%s','%s');",
		username.c_str(), cxx, ixx, cuu);
	if ( mysql_query(mys_conn, regist_sql) )
		SEND_JMSG_RETURN(400, "username already exists")
	sprintf(regist_sql,
		"insert into login(iuu) values('%s');",
		iuu);
	if ( mysql_query(mys_conn, regist_sql) )
		SEND_JMSG_RETURN(500, "regist fail")
	
	send_jmsg(req, 200, "regist ok");
}

bool is_password_error(const string &password)
{
	int len = password.length();
	if (len != 8)
		return true;
	
	const char *p = password.c_str();
	while ( *p && ( !(*p<'0' || *p>'9') ) )
		p++;
	return *p != '\0';
}

void generate_uuid_xx(char *plain, char *cipher, const void *key)
{
	uuid_t uuid;
	uuid_generate(uuid);
	uuid_unparse(uuid, plain);
	
	tea_encrypt_n(&uuid, key, &uuid, 2);
	uuid_unparse(uuid, cipher);
}


} // namespace