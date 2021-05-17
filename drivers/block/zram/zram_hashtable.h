#ifndef _ZRAM_HASHTABLE_H
#define _ZRAM_HASHTABLE_H

#include <linux/hashtable.h>
#include "uthash.h"
#define DIGEST_LEN  257
#define CAPACITY    1000    
#define CNT_INC     1
#define CNT_DEC     0

struct Node
{
    unsigned char digest[DIGEST_LEN];
    unsigned long handle;
    unsigned long cnt;
    unsigned long comp_len;
    struct Node* pre, *next;
    UT_hash_handle hh;
};

struct Head
{
    unsigned long cnt;
    struct Node* pre, *next;
    UT_hash_handle hh;
};

struct hash_table
{
    struct Node* node_table;
    struct Head* head_table;
    unsigned long min_cnt;
};


struct Node * find_or_add_node(void* src, char* is_find_node);
void update_node(struct Node *node, char is_inc);
void del_Node(struct Node*);
void init_zram_hashtable(void);

#endif