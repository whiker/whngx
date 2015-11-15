#include "whngx_util.h"

using namespace std;
using namespace whngx;

// worker进程的全局变量
vector<JcheckPair> regist_params;
vector<JcheckPair> loginreq_params;
vector<JcheckPair> login_params;


extern int whdoc::init_db_mysql();

static void init_json_check_params();


ngx_int_t whngx_doc_init_process(ngx_cycle_t *cycle)
{
	init_json_check_params();
	
	// 连接数据库
	if ( whdoc::init_db_mysql() < 0 )
		return NGX_ERROR;
	
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
	login_params.push_back(JcheckPair("cuu", JSON_TYPE_STR));
	login_params.push_back(JcheckPair("iuu", JSON_TYPE_STR));
}