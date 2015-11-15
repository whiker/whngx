#ifndef WHNGX_WHNGX_UTIL
#define WHNGX_WHNGX_UTIL

#include <utility>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "ngx_header.h"


namespace whngx {

#define JSON_TYPE_STR 1

#define JSON_CHECK(ITEMS) \
	string err_str; \
	if ( json_check(doc, ITEMS##_params, err_str) < 0 ) { \
		send_jmsg(req, 400, err_str.c_str()); \
		return; \
	}

typedef std::pair<std::string, int> JcheckPair;


ngx_int_t send_json(ngx_http_request_t *req, int status, rapidjson::Document &doc);

ngx_int_t send_jmsg(ngx_http_request_t *req, int status, const char *msg);

int json_check(rapidjson::Document &doc, std::vector<JcheckPair> &items,
			   std::string &err_str);


} // namespace

#endif