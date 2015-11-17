#ifndef WHNGX_DOC_UTIL
#define WHNGX_DOC_UTIL

namespace whdoc {


#define ID_CHECK(UID) \
	string err_str; \
	if ( check_id(doc, UID, err_str) < -1 ) { \
		send_jmsg(req, 400, err_str.c_str()); \
		return; \
	}


bool is_username_error(const char *username);
int  check_id(rapidjson::Document &doc, char *uid, std::string &err_str);


}  // namespace

#endif