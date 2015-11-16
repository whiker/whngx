#ifndef WHNGX_DOC_GLOBAL
#define WHNGX_DOC_GLOBAL

#define UUID_LEN      36
#define UUID_XXX_LEN  36

extern std::vector<whngx::JcheckPair> regist_params;
extern std::vector<whngx::JcheckPair> loginreq_params;
extern std::vector<whngx::JcheckPair> login_params;

extern char password_check_map[128];
extern char uuid_check_map[128];

#endif