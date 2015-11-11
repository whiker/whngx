#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

// 常量
const char* HttpRetStr_BadRequest = "Bad Request";
const char* HttpRetStr_InternalError = "Internal Error";

// 结构体定义
typedef struct wh_kvbuf_s wh_kvbuf_t;
struct wh_kvbuf_s {
	ngx_chain_t *b1, *b2, *b3;  // 有效区间[b1,b3], b2是等号buf块
	u_char      *p1, *p2, *p3;  // 有效区间[p1,p3), p2是等号位置
	wh_kvbuf_t  *next;
};

// 函数声明
static char*
	ngx_http_calc_set_conf(ngx_conf_t*, ngx_command_t*, void*);
static ngx_int_t
	ngx_http_calc_handler(ngx_http_request_t* req);
ngx_int_t
	whSendfile(ngx_http_request_t* req);
void
	whRequestBodyHandler(ngx_http_request_t* req);
int
	whParseRequestBodyV1(ngx_http_request_t* req,
						 wh_kvbuf_t** ret);
int
	whGetValueStrV1(wh_kvbuf_t* kv, u_char* const str, int n_str);

// 函数和宏
ngx_int_t whSendMessage(ngx_int_t status, const char* msg,
						const int len, ngx_http_request_t* req)
{
	req->headers_out.status = status;
	req->headers_out.content_length_n = len;
	ngx_int_t ret = ngx_http_send_header(req);
	if (ret == NGX_ERROR || ret > NGX_OK || req->header_only)
		return ret;
	
	ngx_buf_t* buf = ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
	if (buf==0) return NGX_ERROR;
	buf->start = buf->pos = ngx_palloc(req->pool, len+1);
	if (buf->pos==0) return NGX_ERROR;
	buf->end = buf->last = buf->pos + len;
	buf->temporary = buf->last_buf = 1;
	memcpy(buf->pos, msg, len);
	
	ngx_chain_t out = { buf, 0 };
	return ngx_http_output_filter(req, &out);
}

inline ngx_int_t whBadRequest(ngx_http_request_t* req)
{
	return whSendMessage(NGX_HTTP_BAD_REQUEST,
		HttpRetStr_BadRequest, strlen(HttpRetStr_BadRequest), req);
}

inline ngx_int_t whInternalError(ngx_http_request_t* req)
{
	return whSendMessage(NGX_HTTP_INTERNAL_SERVER_ERROR,
		HttpRetStr_InternalError, strlen(HttpRetStr_InternalError), req);
}

#define WH_DISCARD_REQUEST_BODY(REQ) \
	ret = ngx_http_discard_request_body(REQ); \
	if (ret != NGX_OK) return ret;

// 定义结构体变量
static ngx_command_t ngx_http_calc_commands[] = {
	{
		ngx_string("calc"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|
			NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
		ngx_http_calc_set_conf,
		NGX_HTTP_LOC_CONF_OFFSET,
		0,
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t ngx_http_calc_module_ctx = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_http_calc_module = {
	NGX_MODULE_V1,
	&ngx_http_calc_module_ctx,
	ngx_http_calc_commands,
	NGX_HTTP_MODULE,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NGX_MODULE_V1_PADDING
};

// 函数实现
char* ngx_http_calc_set_conf(ngx_conf_t* cf,
	ngx_command_t* cmd, void* conf)
{
	ngx_http_core_loc_conf_t* clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_calc_handler;
	return NGX_CONF_OK;
}

ngx_int_t ngx_http_calc_handler(ngx_http_request_t* req)
{
	ngx_int_t ret;
	
	if (req->method & NGX_HTTP_POST)
	{
		if (req->headers_in.content_length_n > 64)
		{
			WH_DISCARD_REQUEST_BODY(req);
			return whBadRequest(req);
		}
		
		ret = ngx_http_read_client_request_body(
			req, whRequestBodyHandler);
		if (ret >= NGX_HTTP_SPECIAL_RESPONSE)
			return ret;
		return NGX_DONE;
	}
	else
	{
		WH_DISCARD_REQUEST_BODY(req);
		return whSendfile(req);
	}
}

ngx_int_t whSendfile(ngx_http_request_t* req)
{
	const char* filename = "calc/calc.html";
	const char* filetype = "text/html;";
	
	// 打开文件
	ngx_buf_t* buf = ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
	if (buf == NULL)
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	buf->in_file = 1;
	
	buf->file = ngx_pcalloc(req->pool, sizeof(ngx_file_t));
	if (buf->file == NULL)
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	ngx_file_t* file = buf->file;
	
	file->fd = ngx_open_file(filename,
		NGX_FILE_RDONLY|NGX_FILE_NONBLOCK, NGX_FILE_OPEN, 0);
	if (file->fd <= 0)
		return NGX_HTTP_NOT_FOUND;
	file->name.data = (u_char*) filename;
	file->name.len = strlen(filename);
	file->log = req->connection->log;
	if (ngx_file_info(filename, &file->info) == NGX_FILE_ERROR)
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	buf->file_pos = 0;
	buf->file_last = file->info.st_size;
	buf->last_buf = 1;
	
	// 关闭文件
	ngx_pool_cleanup_t* cln = ngx_pool_cleanup_add(req->pool,
		sizeof(ngx_pool_cleanup_t));
	if (cln == NULL)
	{
		close(file->fd);
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	cln->handler = ngx_pool_cleanup_file;
	cln->next = NULL;
	
	ngx_pool_cleanup_file_t* clnf = cln->data;
	clnf->fd = file->fd;
	clnf->name = file->name.data;
	clnf->log = req->pool->log;
	
	// 发送头部
	ngx_http_headers_out_t* hout = &req->headers_out;
	hout->status = NGX_HTTP_OK;
	hout->content_length_n = file->info.st_size;
	hout->content_type.data = (u_char*) filetype;
	hout->content_type.len = strlen(filetype);
	ngx_int_t ret = ngx_http_send_header(req);
	if (ret == NGX_ERROR || ret > NGX_OK || req->header_only)
		return ret;
	
	// 发送文件
	ngx_chain_t out = { buf, 0 };
	return ngx_http_output_filter(req, &out);
}

#define WH_GET_VALUE_INBODY(KV, STR, N_STR) \
	len = whGetValueStrV1(KV, STR, N_STR); \
	if (len < 1) { \
		whBadRequest(req); \
		return; \
	}

void whRequestBodyHandler(ngx_http_request_t* req)
{
	// 初步解析请求包体
	wh_kvbuf_t* kvs = 0;
	int kv_num = whParseRequestBodyV1(req, &kvs);
	if (kv_num != 3) {
		if (kv_num == -2)
			whInternalError(req);
		else
			whBadRequest(req);
		return;
	}
	
	// 解析包体信息
	double d1, d2;
	u_char _operator;
	u_char* str = ngx_palloc(req->pool, 32);
	int len;
	
	WH_GET_VALUE_INBODY(kvs, str, 10)
	str[len] = '\0';
	sscanf((char*)str, "%lf", &d1);
	
	WH_GET_VALUE_INBODY(kvs->next, str, 3)
	if (str[0] == '%')
	{
		if (len != 3) {
			whBadRequest(req);
			return;
		}
		if (str[2]=='f' || str[2]=='F')
			_operator = '/';
		else
			_operator = '+';
	}
	else
		_operator = str[0];
	
	WH_GET_VALUE_INBODY(kvs->next->next, str, 10)
	str[len] = '\0';
	sscanf((char*)str, "%lf", &d2);
	
	// 计算结果
	if (_operator == '+')
		d1 += d2;
	else if (_operator == '-')
		d1 -= d2;
	else if (_operator == '*')
		d1 *= d2;
	else if (abs(d2) > 0.000005)
		d1 /= d2;
	
	// 发送结果
	len = sprintf((char*)str, "%.4lf", d1);
	whSendMessage(NGX_HTTP_OK, (char*)str, len, req);
}

int whParseRequestBodyV1(ngx_http_request_t* req,
						 wh_kvbuf_t** ret)
{
	*ret = 0;
	wh_kvbuf_t* head = ngx_pcalloc(req->pool, sizeof(wh_kvbuf_t));
	if (head == 0) return -2;
	wh_kvbuf_t *kv = head;  // 此处头节点是空节点
	
	int kv_num = 0;
	char state=0;
	u_char *start=0, *pos=0, *last=0;
	ngx_chain_t* buf1=0;
	
	ngx_chain_t* it = req->request_body->bufs;
	pos = it->buf->pos;
	last = it->buf->last;
	while (it)
	{
		if (start == 0) {
			start = pos;
			buf1 = it;
			state = 0;
		}
		
		if (state == 0) {
			for (; *pos!='=' && *pos!='&' && pos!=last; pos++);
			if (*pos == '&') return -1;
			if (pos != last) {
				state = 1;
				kv_num++;
				
				kv->next = ngx_pcalloc(req->pool, sizeof(wh_kvbuf_t));
				if (kv->next == 0) return -2;
				kv = kv->next;
				
				kv->b1 = buf1;
				kv->b2 = kv->b3 = it;
				kv->p1 = start;
				kv->p2 = kv->p3 = pos;
				if (++pos != last)  // 当前buf块未读完
					continue;
			}
		}
		else if (state == 1) {
			for (; *pos!='&' && *pos!='=' && pos!=last; pos++);
			if (*pos == '=') return -1;
			if (pos!=last || it->next==0) {
				state = 0;
				start = 0;
				kv->p3 = pos;
				kv->b3 = it;
				if (pos!=last && ++pos != last)  // 当前buf块未读完
					continue;
			}
		}
		
		it = it->next;
		if (it==0) break;
		pos = it->buf->pos;
		last = it->buf->last;
	}
	
	*ret = head->next;
	return kv_num;
}

int whGetValueStrV1(wh_kvbuf_t* kv, u_char* const str, int n_str)
{
	if (kv->p2 == kv->p3)
		return 0;
	
	int len = 0, n;
	u_char* dst = str;
	u_char* pos = kv->p2;
	ngx_chain_t* buf = kv->b2;
	
	pos++;
	if (pos == buf->buf->last)
	{
		buf = buf->next;
		if (buf == 0) return 0;
		pos = buf->buf->pos;
	}
	
	for (; buf != kv->b3; buf = buf->next)
	{
		n = buf->buf->last - pos;
		if (len + n > n_str) return -1;
		len += n;
		memcpy(dst, pos, n);
		dst += n;
		pos = buf->buf->pos;
	}
	if (buf == kv->b3)
	{
		n = kv->p3 - pos;
		if (len + n > n_str) return -1;
		len += n;
		memcpy(dst, pos, n);
	}
	return len;
}