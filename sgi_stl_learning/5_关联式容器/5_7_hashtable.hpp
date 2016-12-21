// Edit bu cyj 2016-12-20

#include <vector>
#include <algorithm>

#include <unordered_set>

using std::vector;
using std::lower_bound;

template <typename Value>                                     //hash节点定义
struct __hashtable_node
{
  __hashtable_node* next;
  Value val;
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc=alloc>
class hashtable;

template <class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;

template <class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
struct __const_hashtable_iterator;

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

// Note: 假设 long 至少有 32 bit
enum { __stl_num_primes = 28 };

static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53ul, 97ul, 193ul, 389ul, 769ul,
  1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
  49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
  1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
  50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long __stl_next_prime(unsigned long __n)
{
  const unsigned long* __first = __stl_prime_list;
  const unsigned long* __last = __stl_prime_list + (int)__stl_num_primes;
  const unsigned long* pos = lower_bound(__first, __last, __n);
  return pos == __last ? *(__last - 1) : *pos;
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
bool operator==(const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht1,
                const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht2);

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
class hashtable
{
public:
  typedef HashFcn         hasher;
  typedef EqualKey        key_equal;                //判断键值是否相等
  typedef size_t          size_type;

private:
  hasher  hash;
  key_equal equals;
  ExtractKey  get_key;

  typedef __hashtable_node<Value> node;
  typedef simple_alloc<node, Alloc> node_allocator;

  vector<node*, Alloc> buckets;                     //以vector完成
  size_type num_elements;

public:
  hashtable(size_type n,
            const HashFcn& hf,
            const EqualKey& eql)
            :hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
  {
    initialize_buckets(n);
  }

  size_type bucket_count() const { return buckets.size(); } //桶的个数

  size_type max_bucket_count() const                        //最多有多少桶
  {
    return __stl_prime_list[(int)__stl_num_primes - 1];
  }

  pair<iterator, bool> insert_unique(const value_type& obj)
  {
    resize(num_elements + 1);
    return insert_unique_noresize(obj);
  }

  void resize(size_type num_elements_hint);
  pair<iterator, bool> insert_unique_noresize(const value_type& obj);

private:
  void initialize_buckets(size_type n)
  {
    const size_type n_buckets = next_size(n);
    buckets.reserve(n_buckets);
    buckets.insert(buckets.end(), n_buckets, (node*)0);
    num_elements = 0;
  }

  size_type next_size(size_type n) const
  {
    return __stl_next_prime(n);
  }

  node* new_node(const value_type& obj)
  {
    node* n = node_allocator::allocte();
    n->next = 0;
    __STL_TRY
    {
      construct(&n->val, obj);
      return n;
    }
    __STL_UNWIND(node_allocator(n));
  }

  void delete_node(node* n)
  {
    destroy(&n->val);
    node_allocator::deallocate(n);
  }

};

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint)
{
  //判断是否重建的原则是：拿元素的个数（把新增元素计算在内）与 bucket vector 的大小相比，
  //如果前者大于后者，就重建表格
  //由此可见，每个bucket(list)最大容量和buckets vector的大小相同
  const size_type old_n = buckets.size();
  if (num_elements_hint > old_n)             // 确定需要真的重新配置
  {
    const size_type n = next_size(num_elements_hint);    //找出先一个质数
    if (n > old_n)
    {
      vector<node*, A> tmp(n, (node*)0);
      __STL_TRY
      {
        for (size_type bucket = 0; bucket < old_n; ++bucket)
        {
          node* first = buckets[bucket];
          while (first)
          {
            size_type new_bucket = bkt_num(first->val, n);         //找出节点落在哪一个新的bucket内
            //以下4个操作很巧妙
            //1、令旧的bucket执行器所对应的串行的下一个节点
            buckets[bucket] = first->next;
            //2,3、将当前节点插入到新bucket内，成为其对应串行的第一个节点
            first->next = tmp[new_bucket];
            tmp[new_bucket] = first;
            //4、回归旧bucket所指的待处理串行，准备处理下一个节点
            first = buckets[bucket];
          }
        }
        buckets.swap(tmp);       //对调两个buckets
      }
    }
  }
}

template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename template <V, K, HF, Ex, Eq, A>::iterator, bool>
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj)
{
  const size_type n = bkt_num(obj);
  node* first = buckets[n];                         //令first指向bucket对应的串行头部

  //如果buckets[n]已被占用
  for (node* cur = first; cur; cur = cur->next)
  {
    if (equals(get_key(cur->val), get_key(obj)))    //如果发现与链表某键值相等，不插入，直接返回
      return pair<iterator, bool>(iterator(cur, this), false);
  }

  //离开上述循环时，first执行bucket所指链表头部节点
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return pair<iterator, bool>(iterator(tmp, this), true);
}
