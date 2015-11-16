#include "whngx_util.h"
#include "global.h"
#include "whdoc_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


// ACTION
void loginreq(ngx_http_request_t *req, Document &doc)
{
	JSON_CHECK(loginreq);
	const string &username = doc["username"];
	
	if ( is_username_error(username) )
		SEND_JMSG_RETURN(400, "username format error")
	
	// 获取 cxx ixx
	SQL("select cxx, ixx from loginreq where username='%s';",
		username.c_str());
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	if (ret->row_count == 0)
		SEND_JMSG_RETURN(400, "username doesn't exist")
	MYSQL_ROW row = MYS_NEXT_ROW(res);
	
	// 返回
	Document doc_out;
	doc_out.SetObject();
	Value val;
	val = StringRef(row[0], UUID_XXX_LEN);
	doc_out.AddMember("cxx", val, doc_out.GetAllocator());
	val = StringRef(row[1], UUID_XXX_LEN);
	doc_out.AddMember("ixx", val, doc_out.GetAllocator());
	
	send_json(req, 200, doc_out);
}


} // namespace