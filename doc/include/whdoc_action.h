#ifndef WHNGX_DOC_ACTION
#define WHNGX_DOC_ACTION

namespace whdoc {


void regist(ngx_http_request_t *req, rapidjson::Document &doc);

void loginreq(ngx_http_request_t *req, rapidjson::Document &doc);

void login(ngx_http_request_t *req, rapidjson::Document &doc);

void list_doc(ngx_http_request_t *req, rapidjson::Document &doc);

void create_doc(ngx_http_request_t *req, rapidjson::Document &doc);

void delete_doc(ngx_http_request_t *req, rapidjson::Document &doc);

void open_doc(ngx_http_request_t *req, rapidjson::Document &doc);

void restore_doc(ngx_http_request_t *req, rapidjson::Document &doc);

void rename_doc(ngx_http_request_t *req, rapidjson::Document &doc);


}  // namespace

#endif