#include "uthash.h"
#define DIGEST_LEN (20+1)

struct zram_same_page{
    unsigned char digest[DIGEST_LEN];
    unsigned long handle;
    unsigned long cnt;
    unsigned long comp_len;
    UT_hash_handle hh;
};

int do_hash(const unsigned char *data, unsigned char *out_digest);