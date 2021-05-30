#include "zram_hashtable.h"
#include "zram_hash.h"
#include <linux/slab.h>

struct Node* node_table;
struct Head* head_table;
unsigned long CAPACITY = 1024;

struct Node  *alloc_Node(void){
    struct Node *node = kzalloc(sizeof(*node), GFP_KERNEL);
	if(node == NULL){
        pr_err("can't kzalloc hash node");
        return NULL;
    }
    INIT_LIST_HEAD(&node->list);
    return node;
}

struct Head* scan_min_ref_head(void)
{
    struct Head *head;
    unsigned long ref = 1;
    HASH_FIND_INT(head_table, &ref, head);
    if(head)
        return head;
    CAPACITY *= 2;
    pr_info("capacity of hashtable enlarged to %lu", CAPACITY);
    return NULL;
}

void del_node_list(struct Node *node)
{
    struct Head *head;
    if(node->list.next != LIST_POISON1 && node->list.prev != LIST_POISON2){
        list_del(&node->list);

        HASH_FIND_INT(head_table, &node->ref, head);
        BUG_ON(head == NULL);
        if(list_empty(&head->head)){
            HASH_DEL(head_table, head);
            kfree(head);
        }
    } 
}

struct Node *reuse_LFU(void){
    struct Node* node;
    struct Head *min_head;
    struct list_head *list;

    min_head = scan_min_ref_head();
    if(min_head == NULL)
        return NULL;

    if(list_empty(&min_head->head)){
        pr_err("LFU empty list");
        return NULL;
    }
    // LRU
    list = min_head->head.prev;
    node = list_entry(list, struct Node, list);

    del_node_list(node);
    HASH_DEL(node_table, node);
    node->handle = node->comp_len = 0;
    return node;
}

struct Head* find_or_alloc_head(unsigned long ref)
{
    struct Head *head;
    HASH_FIND_INT(head_table, &ref, head);
    if(head)
        return head;
    else{
        head = kzalloc(sizeof(*head), GFP_KERNEL);
        if(!head){
            pr_err("can't kzalloc hash head");
            return NULL;
        }
        INIT_LIST_HEAD(&head->head);

        head->ref = ref;
        HASH_ADD_INT(head_table, ref, head);
        return head;   
    }
}

struct Node *find_or_alloc_node(void* src, char* is_find_node)
{
    struct Node *node = NULL;
    struct Head* head;
    unsigned char tmp_digest[DIGEST_LEN];
    int ret;

    ret = do_hash(src, tmp_digest);
    if(ret < 0)
        return ERR_PTR(ret);
    HASH_FIND_STR(node_table, tmp_digest, node);
    if(node){
        update_node(node, CNT_INC);
        *is_find_node = 1;
        return node;
    }

    *is_find_node = 0;

    if(HASH_COUNT(node_table) >= CAPACITY){
        if((node = reuse_LFU()) == NULL)
            node = alloc_Node();
    }else
        node = alloc_Node();
        
    if(node == NULL){
        return NULL;
        pr_err("can't alloc node");
    }
        

    node->ref = 1;
	memcpy(node->digest, tmp_digest, DIGEST_LEN);
    
    head = find_or_alloc_head(node->ref);
    list_add(&node->list, &head->head);

    HASH_ADD_STR(node_table, digest, node);
    return node;
}

//  return ref after update
unsigned long update_node(struct Node *node, char is_inc)
{
    struct Head* head = NULL;

    del_node_list(node);

    node->ref = is_inc? node->ref+1 : node->ref-1;
    if(node->ref == 0){
        HASH_DEL(node_table, node);
        kfree(node);
        return 0;
    }

    head = find_or_alloc_head(node->ref);
    list_add(&node->list, &head->head);
    return node->ref;
}

void free_hashtable(void)
{
    // struct Node* node, *tmp_node;
    // struct Head* head, *tmp_head;
    BUG_ON(node_table);
    // HASH_ITER(hh, node_table, node, tmp_node){
    //     del_node_list(node);
    //     HASH_DEL(node_table, node);
    //     kfree(node);
    // }
    BUG_ON(head_table);
    // HASH_ITER(hh, head_table, head, tmp_head){
    //     HASH_DEL(head_table, head);
    //     kfree(head);
    // }
}