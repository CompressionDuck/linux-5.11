#include "zram_hashtable.h"
#include "zram_sha256.h"


struct hash_table hashtable;

struct Node * find_or_add_node(void* src, char* is_find_node)
{
    struct Node *node = NULL;
    char tmp_digest[DIGEST_LEN];
    do_sha256(src, tmp_digest);
    HASH_FIND_STR(hashtable.node_table, tmp_digest, node);
    if(node){
        update_node(node, CNT_INC);
        *is_find_node = 1;
        return node;
    }

    node = kmalloc(sizeof(struct zram_same_page), GFP_KERNEL);
	if(node < 0){
        pr_err("zram: can't kzalloc hash node");
        return NULL;
    }
    node->cnt = 1;
    node->comp_len = 0;
    node->pre = node->next = NULL; 

	strcpy(node->digest, tmp_digest);
	HASH_ADD_STR(hashtable.node_table, digest, node);
    *is_find_node = 0;
    return node;
}

void cut_node(struct Node* node){
    node->pre->next = node->next;
    node->next->pre = node->pre;
}

void add_after_head(struct Head* head, struct Node* node){
    node->next = head->next;
    node->pre = head;
    head->next->pre = node;
    head->next = node;
}

inline static int head_empty(struct Head* head)
{
    return head->next == head;
}

void update_node(struct Node *node, char is_inc)
{
    struct Head* head;
    if(is_inc && hashtable.min_cnt == node->cnt){
        HASH_FIND_INT(hashtable.head_table, &node->cnt, head);
        if(head->pre == node)
            hashtable.min_cnt++;
    }
    node->cnt = is_inc? node->cnt+1 : node->cnt-1;
    
    cut_node(node);
    HASH_FIND_INT(hashtable.head_table, &node->cnt, head);
    if(!head){
        head = kmalloc(sizeof(struct Head), GFP_KERNEL);
        head->cnt = node->cnt;
        head->pre = head->next = head;
    }
    if(!is_inc && head_empty(head))
        hashtable.min_cnt = head->cnt;
    add_after_head(head, node);
}

void del_Node(struct Node* node)
{
    cut_node(node);
    HASH_DEL(hashtable.node_table, node);
}

void init_zram_hashtable()
{
    hashtable.node_table = NULL;
    hashtable.head_table = NULL;
    hashtable.min_cnt = 0;
}