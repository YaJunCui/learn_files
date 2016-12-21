// Edit bu cyj 2016-12-20

template <typename Value>                                     //hash节点定义
struct __hashtable_node
{
  __hashtable_node* next;
  Value val;
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator
{
  typedef __hashtable<Value, Key, HashFcn, EqualKey, Alloc>         hashtable;
  typedef __hashtable_iterator<Value, Key, HashFcn,
                               ExtractKey, EqualKey, Alloc>         iterator;
  typedef __hashtable_const_iterator<Value, Key, HashFcn,
                                     ExtractKey, EqualKey, Alloc>   const_iterator;
  typedef __hashtable_node<Value>                                   node;

  typedef forward_iterator_tag        iterator_category;
  typedef Value                       value_type;
  typedef ptrdiff_t                   difference_type;
  typedef Value&                      reference;
  typedef Value*                      pointer;

  node* cur;                              //迭代器目前所指节点
  hashtable* ht;                          //保持对容器的连结关系

  __hashtable_iterator(node* n, hashtable* tab) :cur(n), ht(tab){}
  __hashtable_iterator(){}
};