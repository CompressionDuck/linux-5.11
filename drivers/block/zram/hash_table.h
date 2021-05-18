#include <linux/module.h>
#include "uthash.h"
#include <crypto/hash.h>
#include <linux/types.h>
#define DIGEST_LEN 257

struct zram_same_page{
    unsigned char digest[DIGEST_LEN];
    unsigned long handle;
    unsigned long cnt;
    unsigned long comp_len;
    UT_hash_handle hh;
};

int do_sha256(const unsigned char *data, unsigned char *out_digest);