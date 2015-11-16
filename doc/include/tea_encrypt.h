#ifndef WHNGX_TEA_ENCRYPT
#define WHNGX_TEA_ENCRYPT

#include <stdint.h>

namespace whngx {


void tea_encrypt(const uint32_t *src, const uint32_t *key, uint32_t *dst);
void tea_decrypt(const uint32_t *src, const uint32_t *key, uint32_t *dst);

void tea_encrypt_n(const void *src, const void *key, void *dst, int n);
void tea_decrypt_n(const void *src, const void *key, void *dst, int n);


}  // namespace

#endif