#include <string.h>

#include "whngx_util.h"
#include "db_mysql.h"

using namespace std;
using namespace whngx;

// worker进程的全局变量
vector<JcheckPair> regist_params;
vector<JcheckPair> loginreq_params;
vector<JcheckPair> login_params;

char password_check_map[128];
char uuid_check_map[128];


static void init_json_check_params();
static void init_check_map();


ngx_int_t whngx_doc_init_process(ngx_cycle_t *cycle)
{
	// 连接数据库
	if ( whdoc::init_db_mysql() < 0 )
		return NGX_ERROR;
	
	init_json_check_params();
	
	init_check_map();
	
	return NGX_OK;
}

void init_json_check_params()
{
	// regist
	regist_params.push_back(JcheckPair("username", JSON_TYPE_STR));
	regist_params.push_back(JcheckPair("password", JSON_TYPE_STR));
	
	// loginreq
	loginreq_params.push_back(JcheckPair("username", JSON_TYPE_STR));
	
	// login
	login_params.push_back(JcheckPair("username", JSON_TYPE_STR));
	login_params.push_back(JcheckPair("cuu", JSON_TYPE_STR));
	login_params.push_back(JcheckPair("iuu", JSON_TYPE_STR));
}

void init_check_map()
{
	memset(password_check_map, 0, 128);
	for (int i = '0'; i <= '9'; i++)
		password_check_map[i] = 0x01;
	for (int i = 'A'; i <= 'Z'; i++)
		password_check_map[i] = 0x02;
	for (int i = 'a'; i <= 'z'; i++)
		password_check_map[i] = 0x02;
	
	memset(uuid_check_map, 0, 128);
	for (int i = '0'; i <= '9'; i++)
		uuid_check_map[i] = 0x01;
	for (int i = 'a'; i <= 'z'; i++)
		uuid_check_map[i] = 0x02;
	uuid_check_map['-'] = 0x04;
}