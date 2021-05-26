#ifndef _ZRAM_HASHTABLE_H
#define _ZRAM_HASHTABLE_H

#include <linux/list.h>
#include "uthash.h"
#define DIGEST_LEN  20
#define CAPACITY    5000    
#define CNT_INC     1
#define CNT_DEC     0

struct Node
{
    unsigned char digest[DIGEST_LEN];
    unsigned long ref;
    struct list_head list;
    UT_hash_handle hh;

    unsigned long handle;
    unsigned long comp_len;
};

struct Head
{
    unsigned long ref;
    struct list_head head;
    UT_hash_handle hh;
};

struct Node * find_or_add_node(void* src, char* is_find_node);
void update_node(struct Node *node, char is_inc);
void free_hashtable(void);

#endif