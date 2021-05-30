#ifndef _ZRAM_HASHTABLE_H
#define _ZRAM_HASHTABLE_H

#include <linux/list.h>
#include <linux/spinlock.h>

#include "uthash.h"
#include "zram_drv.h"

#define DIGEST_LEN  20
#define CNT_INC     1
#define CNT_DEC     0

struct Node
{
    spinlock_t lock;
    // will not change since init
    unsigned char digest[DIGEST_LEN];
    unsigned long handle;
    unsigned long comp_len;

    // will change by muti threads
    u32 index;
    unsigned long ref;
    struct list_head list;
    UT_hash_handle hh;
};

struct Head
{
    unsigned long ref;
    struct list_head head;
    UT_hash_handle hh;
};

struct Node *find_or_alloc_node(void* src, char* is_find_node, struct zram *zram, u32 index);
unsigned long update_node(struct Node *node, char is_inc);
void free_hashtable(void);

#endif