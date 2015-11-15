#include "ngx_header.h"


extern ngx_int_t whngx_doc_init_process(ngx_cycle_t *cycle);
extern ngx_int_t ngx_http_doc_handler(ngx_http_request_t *req);

static char* ngx_http_doc_set_conf(ngx_conf_t*, ngx_command_t*, void*);


static ngx_command_t ngx_http_doc_commands[] = {
	{
		ngx_string("doc"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|
			NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
		ngx_http_doc_set_conf,
		NGX_HTTP_LOC_CONF_OFFSET,
		0,
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t ngx_http_doc_module_ctx = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_http_doc_module = {
	NGX_MODULE_V1,
	&ngx_http_doc_module_ctx,
	ngx_http_doc_commands,
	NGX_HTTP_MODULE,
	NULL, NULL, whngx_doc_init_process, NULL, NULL, NULL, NULL,
	NGX_MODULE_V1_PADDING
};


char* ngx_http_doc_set_conf(ngx_conf_t* cf,
	ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t* clcf
		= (ngx_http_core_loc_conf_t*)
		ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_doc_handler;
	return NGX_CONF_OK;
}