#ifndef WHNGX_DOC_ACTION
#define WHNGX_DOC_ACTION

namespace whdoc {


ngx_int_t regist(ngx_http_request_t *req, rapidjson::Document &doc);

ngx_int_t loginreq(ngx_http_request_t *req, rapidjson::Document &doc);

ngx_int_t login(ngx_http_request_t *req, rapidjson::Document &doc);

ngx_int_t list_doc(ngx_http_request_t *req, rapidjson::Document &doc);


}  // namespace

#endif