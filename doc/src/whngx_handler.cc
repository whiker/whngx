#include <string.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "whngx_util.h"
#include "whdoc_action.h"

using namespace rapidjson;
using namespace whngx;
using namespace whdoc;


static void wh_handle_body(ngx_http_request_t *req);


ngx_int_t ngx_http_doc_handler(ngx_http_request_t *req)
{
	ngx_int_t ret = ngx_http_read_client_request_body(req, wh_handle_body);
	if (ret >= NGX_HTTP_SPECIAL_RESPONSE)
		return ret;
	return NGX_DONE;
}

void wh_handle_body(ngx_http_request_t *req)
{
	int content_len = req->headers_in.content_length_n;
	
	char *jstr = (char*) ngx_palloc(req->pool, content_len+1);
	char *jstrp = jstr;
	
	ngx_chain_t* it = req->request_body->bufs;
	for (int len; it; it = it->next)
	{
		len = it->buf->last - it->buf->pos;
		memcpy(jstrp, it->buf->pos, len);
		jstrp += len;
	}
	*jstrp = '\0';
	
	Document doc;
	doc.Parse(jstr);
	if ( doc.HasParseError() )
	{
		send_jmsg(req, 400, "json format error");
		return;
	}
	
	// 调用 Action 处理函数
	if ( ngx_strncmp(req->uri.data, "/doc/regist", req->uri.len) == 0 )
		regist(req, doc);
	else if ( ngx_strncmp(req->uri.data, "/doc/loginreq", req->uri.len) == 0 )
		loginreq(req, doc);
	else
		login(req, doc);
}