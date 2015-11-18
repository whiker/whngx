#include <string.h>

#include <uuid/uuid.h>

#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"
#include "tea_encrypt.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


char key[16];
char cuu[UUID_LEN+1], iuu[UUID_LEN+1];
char cxx[UUID_XXX_LEN+1], ixx[UUID_XXX_LEN+1];


bool is_password_error(const char *password);
void generate_uuid_xxx(char *plain, char *cipher, const void *key);


// ACTION
void regist(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(regist)
	const char *username = doc["username"].GetString();
	const char *password = doc["password"].GetString();
	
	if ( is_username_error(username) )
		SEND_JMSG_RETURN(400, "username format error")
	if ( is_password_error(password) )
		SEND_JMSG_RETURN(400, "password format error")
	
	// 判断是否已注册过
	SQL("select username from loginreq where username=('%s')",
		username);
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	int row_cnt = res->row_count;
	MYS_FREE(res);
	if (row_cnt > 0)
		SEND_JMSG_RETURN(400, "username already exists")
	
	// 生成(cuu,iuu,cxx,ixx)
	memcpy(key, password, 8);
	memcpy(key+8, password, 8);
	generate_uuid_xxx(cuu, cxx, key);
	generate_uuid_xxx(iuu, ixx, key);
	
	// 插入新用户
	SQL("insert into loginreq(username,cxx,ixx,cuu) values('%s','%s','%s','%s')",
		username, cxx, ixx, cuu);
	if ( MYS_QUERY )
		SEND_JMSG_RETURN(400, "username already exists")
	
	SQL("insert into login(iuu) values('%s')", iuu);
	MYS_QUERY;
	
	SQL("select uid from login where iuu='%s'", iuu);
	MYS_QUERY;
	res = MYS_RESULT;
	MYSQL_ROW row = MYS_NEXT_ROW(res);
	strcpy(uid, row[0]);
	MYS_FREE(res);
	
	// 创建-存在的文档表
	SQL("create table docl_%s ( \
			docname varchar(64) not null, \
			docid int not null auto_increment, \
			primary key (docname), key (docid) \
		) default charset=utf8",
		uid);
	MYS_QUERY;
	
	// 创建-删除的文档表
	SQL("create table docd_%s ( \
			docname varchar(64) not null, \
			docid int not null, \
			primary key (docname), key (docid) \
		) default charset=utf8",
		uid);
	MYS_QUERY;
	
	send_jmsg(req, 200, "regist ok. warnning: if you forget password, we forget you.");
}

bool is_password_error(const char *password)
{
	const char *p = password;
	char mask = 0;
	int i = 0;
	
	for (; *p && i < 8; p++, i++)
		mask |= password_check_map[(int)*p];
	
	return (*p || i != 8 || mask != 0x05);
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