#ifndef WHNGX_DOC_DB_MYSQL
#define WHNGX_DOC_DB_MYSQL

#include <stdio.h>

#include <mysql/mysql.h>

namespace whdoc {


#define SQL(format, args...)      sprintf(sql_str, format, args)
#define MYS_QUERY                 mysql_query(mys_conn, sql_str)
#define MYS_RESULT                mysql_store_result(mys_conn)
#define MYS_NEXT_ROW(res)         mysql_fetch_row(res)
#define MYS_FOREACH_ROW(res,row)  while (row = mysql_fetch_row(res))


extern MYSQL *mys_conn;
extern char sql_str[1024];


} // namespace

#endif