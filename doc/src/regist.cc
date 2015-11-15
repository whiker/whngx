#include <ctype.h>
#include <stdio.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char regist_sql[128];


static bool is_password_error(const string &password);


// ACTION
void regist(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(regist)
	
	if ( is_username_error(doc["username"]) )
		SEND_JMSG_RETURN(400, "username format error")
	if ( is_password_error(doc["password"]) )
		SEND_JMSG_RETURN(400, "password format error")
	
	// 判断是否已注册过
	sprintf(regist_sql,
		"select username from loginreq where username=('%s');",
		doc["username"].c_str());
	mysql_query(mys_conn, regist_sql);
	MYSQL_RES *res = mysql_store_result(conn);
	if (res->row_count > 0)
		SEND_JMSG_RETURN(400, "username already exists")
	
	// 生成(cxx,ixx,cuu,iuu)
	
	
	// 插入新用户
	sprintf(regist_sql,
		"insert into loginreq(username,cxx,ixx,cuu) values('%s','%s','%s','%s');",
		doc["username"].c_str());
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
	if (len != 6)
		return true;
	
	const char *p = password.c_str();
	while ( *p && ( !(*p<'0' || *p>'9') ) )
		p++;
	return *p != '\0';
}


} // namespace