#ifndef _ZRAM_HASHTABLE_H
#define _ZRAM_HASHTABLE_H

#include <linux/list.h>
#include "uthash.h"
#define DIGEST_LEN  20
#define CAPACITY    1000    
#define CNT_INC     1
#define CNT_DEC     0

struct Node
{
    unsigned char digest[DIGEST_LEN];
    unsigned long ref;
    unsigned long handle;
    unsigned long comp_len;
    struct list_head list;
    UT_hash_handle hh;
};

struct Head
{
    unsigned long ref;
    struct list_head head;
    UT_hash_handle hh;
};

struct hash_table
{
    struct Node* node_table;
    struct Head* head_table;
    unsigned long min_ref;
    unsigned long cnt;
};


struct Node * find_or_add_node(void* src, char* is_find_node);
void update_node(struct Node *node, char is_inc);
void init_zram_hashtable(void);
void free_hashtable(void);

#endif