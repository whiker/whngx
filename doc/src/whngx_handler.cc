#include <string.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "whngx_util.h"
#include "whdoc_action.h"

using namespace rapidjson;
using namespace whngx;
using namespace whdoc;


#define URI_IF(URI) \
	if ( ngx_strncmp(req->uri.data+5, "URI", req->uri.len-5)==0 ) \
		URI(req, doc);
#define URI_EIF(URI) \
	else if ( ngx_strncmp(req->uri.data+5, "URI", req->uri.len-5)==0 ) \
		URI(req, doc);
#define URI_ELSE(URI) \
	else \
		URI(req, doc);


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
	URI_IF(list_doc)
	URI_EIF(open_doc)
	URI_EIF(create_doc)
	URI_EIF(rename_doc)
	URI_EIF(delete_doc)
	URI_EIF(restore_doc)
	URI_EIF(loginreq)
	URI_EIF(login)
	URI_ELSE(regist)
}