#include <ctype.h>
#include <string.h>

#include "global.h"
#include "whngx_util.h"
#include "db_mysql.h"

using namespace std;
using namespace rapidjson;
using namespace whngx;


namespace whdoc {


bool is_username_error(const char *username)
{
	const char *p = username;
	int i = 0;
	
	for (; *p && i < 8 && isalpha(*p); p++, i++);
	
	return (*p || i<4);
}

int check_id(Document &doc, char *uid, string &err_str)
{
	if ( json_check(doc, checkid_params, err_str) < 0 )
		return -1;
	
	int id = doc["id"].GetInt();
	const char *code = doc["code"].GetString();
	
	SQL("select uid from idcheck where id='%d' and code='%s'\
		 and time_to_sec(now())-time_to_sec(time)<3600",
		 id, code);
	MYS_QUERY;
	MYSQL_RES *res = MYS_RESULT;
	if (res->row_count == 0)
	{
		MYS_FREE(res);
		err_str = "you need login";
		return -1;
	}
	else
	{
		MYSQL_ROW row = MYS_NEXT_ROW(res);
		strcpy(uid, row[0]);
		MYS_FREE(res);
		return 0;
	}
}


}  // namespace