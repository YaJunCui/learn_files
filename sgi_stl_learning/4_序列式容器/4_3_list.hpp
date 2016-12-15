// Edit by cyj 2016-12-15

namespace cyj {

template <typename T> 
struct __list_node 
{
  typedef void* void_pointer;           //在使用时需要将 void_pointer 转换为 __list_node<T>* 
  void_pointer  prev;
  void_pointer  next;
  T             data;
};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator
{
  typedef __list_iterator<T, T&, T*>    iterator;               
  //typedef __list_iterator<T, Ref, Ptr>  self;             //这两个类型别名是一致的   
  typedef __list_iterator               self;                  

  typedef bidirectional_iterator_tag  iterator_category;
  typedef T                           value_type;
  typedef Ptr                         pointer;
  typedef Ref                         reference;
  typedef __list_node<T>*             link_type;
  typedef size_t                      size_type;
  typedef ptrdiff_t                   difference_type;

  link_type node;     // 迭代器内部当然要有一个普通指针，指向 list 的节点

  // constructor
  __list_iterator(link_type x):node(x) {}
  __list_iterator() {}
  __list_iterator(iterator& x):node(x.node) {}

  bool operator==(const self& x) const { return node == x.node; }
  bool operator!=(const self& x) const { return node != x.node; }
  
  reference operator*() const { return (*node).data; }                  // 迭代器取值
  pointer operator->()  const { return &(operator*()); }                // 成员存取

  self& operator++()                                                    // 前进一个节点
  {
    node = (link_type)((*node).next)
    return *this;
  }
  self operator++(int)
  {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--()                                                      //后退一个节点
  {
    node = (link_type)((*node).prev);
    return *this;
  }
  self operator--(int)
  {
    self tmp = *this;
    --*this;
    return tmp;
  }
};

template <class T, typename Alloc = alloc>
class list
{
protected:
  typedef __list_node<T>  list_node;
  typedef simple_alloc<list_node, Alloc>  list_node_allocator; //专属的空间配置器，每次配置一个节点的大小

public:
  typedef list_node*      link_type;
  typedef size_t          size_type;

  iterator  begin() { return (link_type)((*node).next); }
  iterator  end() { return node(); }                            //最后一个节点是空白节点，即 node
  bool empty() { return node->next == node; }
  size_type size() const 
  {
    size_type result = 0;
    distance(begin(), end(), result);
    return result;
  }

  reference front() { return *begin(); }                        // 取头节点的内容
  reference back()  { return *(--end()); }                      // 取尾节点的内容

protected:
  link_type node;
};

} // namespace cyj