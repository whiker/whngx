#include "whngx_util.h"

using namespace std;
using namespace rapidjson;


namespace whngx {


ngx_int_t send_json(ngx_http_request_t *req, int status, Document &doc)
{
	StringBuffer jbuf;
	Writer<StringBuffer> jw(jbuf);
	doc.Accept(jw);
	
	string jstr = jbuf.GetString();
	int jlen = jstr.length();
	
	req->headers_out.status = status;
	req->headers_out.content_length_n = jlen;
	ngx_int_t ret = ngx_http_send_header(req);
	if (ret == NGX_ERROR || ret > NGX_OK || req->header_only)
		return ret;
	
	ngx_buf_t *buf = (ngx_buf_t*) ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
	buf->start = buf->pos = (u_char*) jstr.c_str();
	buf->end = buf->last = buf->pos + jlen;
	buf->temporary = buf->last_buf = 1;
	
	ngx_chain_t out = { buf, NULL };
	return ngx_http_output_filter(req, &out);
}

ngx_int_t send_jmsg(ngx_http_request_t *req, int status, const char *msg)
{
	Document doc;
	doc.SetObject();
	Value val;
	val = StringRef(msg);
	doc.AddMember("msg", val, doc.GetAllocator());
	return send_json(req, status, doc);
}

int json_check(Document &doc, vector<JcheckPair> &items, string &err_str)
{
	bool is_type_err;
	vector<JcheckPair>::const_iterator it;
	
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ( ! doc.HasMember(it->first.c_str()) )
		{
			err_str = "lost parameter: " + it->first;
			return -1;
		}
		
		is_type_err = false;
		switch (it->second)
		{
		case JSON_TYPE_STR:
			is_type_err = ! doc[it->first.c_str()].IsString();
			break;
		case JSON_TYPE_INT:
			is_type_err = ! doc[it->first.c_str()].IsInt();
			break;
		}
		if (is_type_err)
		{
			err_str = "parameter type error: " + it->first;
			return -1;
		}
	}
	
	return 0;
}


}  // namespace