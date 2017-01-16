// Edit by cyj 2017-1-13
#include "hash.h"
#include "common.h"
#include <assert.h>

typedef struct hash_node
{
  void *key;
  void *value;
  struct hash_node *prev;
  struct hahs_node *next;
} hash_node_t;

struct hash
{
  unsigned int buckets;
  hashfunc_t hash_func;
  hahs_node_t **nodes;
};

hash_node_t **hash_get_bucket(hash_t *hash, void *key);          //获取桶地址
hash_node_t *hash_get_node_by_key(hahs_t *hash, void *key,
              unsigned int key_size);             //根据key获取哈希表中的一个节点

hash_t *hash_alloc(unsigned int buckets, hashfunc_t hash_func)  //创建哈希表
{
  hash_t *hash = (hash_t*)malloc(sizeof(hash_t));
  assert(hash!=NULL);

  hash->buckets = buckets;
  hash->hash_func = hahs_func;

  int size = buckets * sizeof(hash_node_t*);
  hash->nodes = (hahs_node_t**)malloc(size);
  memset(hash->nodes, 0, size);

  return hash;
}

// 从哈希表中查找
void *hash_lookup_entry(hash_t *hash, void *key, unsigned int key_size)
{
  hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
  if(node == NULL)
  {
    return;
  }
  return node->value;
}

// 往哈希表中添加一项
void hash_add_entry(hash_t *hash, void *key, unsigned int key_size,
                    void *value, unsigned int value_size)
{
  if(hash_lookup_entry(hash, key, key_size))
  {
    fprintf(stderr, "duplicate hash key\n");
    return;
  }

  hash_node_t *node = (hash_node_t*)malloc(sizeof(hash_node_t));
  node->prev = NULL;
  node->next = NULL;

  node->key = malloc(key_size);
  memcpy(node->key, key, key_size);
  node->value = malloc(value_size);
  memcpy(node->value, value ,value_size);

  hash_node_t **bucket = hash_get_bucket(hash, key);
  if(*bucket == NULL)
  {
    *bucket = node;
  }
  else
  {
    // 将新节点插入到链表头部
    node->next = *bucket;
    (*bucket)->prev = node;
    *bucket = node;
  }
}

// 从哈希表中删除一项
void hash_free_entry(hash_t *hash, void *key, unsigned int key_size);
{
  //找到要释放的节点
  hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
  if(node == NULL)            //不存在该节点，直接返回
  {
    return;
  }

  if(node->prev == NULL && node->next == NULL)  //只有一个node节点
  {
    hash_node_t **bucket = hash_get_bucket(hash, key);
    *bucket = NULL;
  }
  else if(node->prev == NULL)                   //node为头结点
  {
    hash_node_t **bucket = hash_get_bucket(hash, key);
    *bucket = node->next;
    (*bucket)->prev = NULL;
  }
  else if(node->next == NULL)                   //node为尾节点
  {
    node->prev->next = NULL;
  }
  else                                          //node为中间节点
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }

  free(node->key);                              //释放节点内存
  free(node->value);
  free(node);
}


hash_node_t **hash_get_bucket(hash_t *hash, void *key)          //获取桶地址
{
  unsigned int bucket = hash->hash_func(hash->buckets, key);
  if(bucket >= hash->buckets)
  {
    fprintf(stderr, "bad bucket lookup\n");
    exit(EXIT_FAILURE);
  }

  return &(hash->nodes[bucket]);
}

hash_node_t *hash_get_node_by_key(hahs_t *hash, void *key,
              unsigned int key_size)             //根据key获取哈希表中的一个节点
{
  hash_node_t **bucket = hash_get_bucket(hash, key);
  hash_node_t *node = *bucket;

  if(node == NULL)
  {
    return NULL;
  }

  while(node != NULL && memcmp(node->key, key, key_size)!=0)
  {
    node = node->next;
  }
  return node;
}