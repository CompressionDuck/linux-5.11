#include "zram_hashtable.h"
#include "zram_sha256.h"
#include <linux/slab.h>

static struct hash_table hashtable;

struct Node  *alloc_Node(void){
    struct Node *node = kzalloc(sizeof(*node), GFP_KERNEL);
	if(!node){
        pr_err("zram: can't kzalloc hash node");
        return NULL;
    }
    node->ref = 1;
    INIT_LIST_HEAD(&node->list);
    hashtable.cnt++;
    return node;
}

struct Head *alloc_Head(void){
    struct Head *head = kzalloc(sizeof(*head), GFP_KERNEL);
    if(!head){
        pr_err("zram: can't kzalloc hash head");
        return NULL;
    }
    INIT_LIST_HEAD(&head->head);
    return head;
}

struct Node *del_LFU(void){
    struct Head *min_head;
    struct list_head *list;
    HASH_FIND_INT(hashtable.head_table, &hashtable.min_ref, min_head);
    if(min_head == NULL){
        pr_err("zram: del_LFU error");
        return NULL;
    }
    list = min_head->head.prev;
    list_del(list);
    return list_entry(list, struct Node, list);
}

struct Node *find_or_add_node(void* src, char* is_find_node)
{
    struct Node *node = NULL;
    struct Head* one_head;
    char tmp_digest[DIGEST_LEN];

    do_sha256(src, tmp_digest);
    HASH_FIND_STR(hashtable.node_table, tmp_digest, node);
    if(node){
        update_node(node, CNT_INC);
        *is_find_node = 1;
        return node;
    }

    if(hashtable.cnt >= CAPACITY)
        node = del_LFU();
    else
        node = alloc_Node();
    if(node == NULL)
        return NULL;

    node->ref = 1;
    node->handle = node->comp_len = 0;
	strcpy(node->digest, tmp_digest);
    
    HASH_FIND_INT(hashtable.head_table, &node->ref, one_head);

    list_add(&node->list, &one_head->head);
    hashtable.min_ref = 1;

	HASH_ADD_STR(hashtable.node_table, digest, node);

    *is_find_node = 0;
    return node;
}

void free_node(struct Node* node)
{
    HASH_DEL(hashtable.node_table, node);
    hashtable.cnt--;
    kfree(node);
}

//TODO update use
void update_node(struct Node *node, char is_inc)
{
    struct Head* head = NULL;
    if(is_inc && hashtable.min_ref == node->ref){
        HASH_FIND_INT(hashtable.head_table, &node->ref, head);
        if(list_is_last(&node->list, &head->head))
            hashtable.min_ref++;
    }

    list_del(&node->list);

    node->ref = is_inc? node->ref+1 : node->ref-1;
    if(node->ref == 0){
        free_node(node);
        return ;
    }

    HASH_FIND_INT(hashtable.head_table, &node->ref, head);
    if(!head){
        head = alloc_Head();
        if(head == NULL)
            return ;
        head->ref = node->ref;
    }
    if(!is_inc && list_empty(&head->head))
        hashtable.min_ref = head->ref;
    list_add(&node->list, &head->head);
}

void init_zram_hashtable(void)
{
    hashtable.node_table = NULL;
    hashtable.head_table = NULL;
    hashtable.min_ref = 1;
    hashtable.cnt = 0;
}

void free_hashtable(void)
{
    struct Node* node, *tmp_node;
    struct Head* head, *tmp_head;
    HASH_ITER(hh, hashtable.node_table, node, tmp_node){
        HASH_DEL(hashtable.node_table, node);
        kfree(node);
    }
    hashtable.cnt = 0;

    HASH_ITER(hh, hashtable.head_table, head, tmp_head){
        HASH_DEL(hashtable.head_table, head);
        kfree(head);
    }
}