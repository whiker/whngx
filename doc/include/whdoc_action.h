#ifndef WHNGX_DOC_ACTION
#define WHNGX_DOC_ACTION

namespace whdoc {


ngx_int_t regist(ngx_http_request_t *req, Document &doc);
ngx_int_t loginreq(ngx_http_request_t *req, Document &doc);
ngx_int_t login(ngx_http_request_t *req, Document &doc);


}  // namespace

#endif