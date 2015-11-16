#include <mysql/mysql.h>

namespace whdoc {


MYSQL *mys_conn = NULL;
char sql_str[1024];


int init_db_mysql()
{
	const char *host   = "localhost";
	const char *user   = "root";
	const char *pwd    = "wh1991";
	const char *dbname = "whdoc";
	
	mys_conn = mysql_init(NULL);
	mys_conn = mysql_real_connect(mys_conn, host, user,
								  pwd, dbname, 0, NULL, 0);
	if (mys_conn == NULL)
		return -1;
	return 0;
}


} // namespace