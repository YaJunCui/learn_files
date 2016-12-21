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

  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(operator*()); }
  iterator& operator++();
  iterator operator++(int);

  bool operator==(const iterator& it) const { return cur == it.cur; }
  bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++()
{
  const node* old = cur;
  cur = cur->next;                                          //如果存在，就是它
  if (!cur)
  {
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size())           //注意，operator++
    {
      cur = ht->buckets[bucket];
    }
  }
  return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
  iterator tmp = *this;
  ++*this;
  return tmp;
}