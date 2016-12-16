// Edit by cyj 2016-12-16

namespace cyj
{

inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n !=0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <typename T, typename Ref, typename Ptr, size_t BufSiz>
struct __deque_iterator
{
  typedef __deque_iterator<T, T*, T&, BufSiz>                       iterator;
  typedef __deque_iterator<const T, const T*, const T&, BufSiz>     const_iterator;
  typedef __deque_iterator                                          self;

  static size_t buffer_size() 
  { 
    return __deque_buffer_size(BufSiz, sizeif(T)); 
  }

  //五种必要的迭代器类型
  typedef random_access_iterator_tag  iterator_category;    // 1                
  typedef T                           value_type;           // 2         
  typedef Ptr                         pointer;              // 3      
  typedef Ref                         reference;            // 4        
  typedef ptrdiff_t                   difference_type;      // 5              
  typedef size_t                      size_type;
  typedef T**                         map_iterator;

  //保持与容器的联结
  T*  cur;                                                  //指向缓冲区当前元素
  T*  first;                                                //缓冲区的头
  T*  last;                                                 //缓冲区的尾
  map_pointer node;                                         //指向管控中心

  void set_node(map_pointer new_node)                       //跳到下一个缓冲区
  {
    node = new_node;
    first = *new_node;
    last = first + difference_type(buffer_size());
  }

  reference operator*() const { return *cur; }
  pointer operator->() const { return &(operator*()); }

  difference_type operator-(const self& x) const
  {
    return difference_type(buffer_size())*(node - x.node -1) + 
      (cur - first) + (x.last - x.cur);
  }

  self& operator++()                                        //前向++
  {
    ++cur;
    if(cur == last)                                           //已经达到缓冲区尾端
    {
      set_node(node+1);
      cur = first;
    }
    return *this;
  }

  self operator++(int)                                      //后置++
  {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--()                                        //前置--
  {
    if(cur == first)                                          //已经到达缓冲区头端                     
    {
      set_node(node-1);
      cur = last;
    }
    --cur;
    return *this;
  }

  self operator--(int)
  {
    self tmp = *this;
    --*this;
    return tmp;
  }

  //实现迭代器可以直接跳跃 n 个距离
  self& operator+=(difference_type n)
  {
    difference_type offset = n + (cur - first);
    if(offset >= 0 && offset <= difference_type(buffer_size())) //目前位置在同一缓冲区
      cur += n;
    else
    {
      difference_type node_offset = 
        offset > 0 ? offset / difference_type(buffer_size())
          : -difference_type((-offset-1) / buffer_size()) - 1;
      set_node(node + node_offset);                            //切换至正确的缓冲区
      cur = first + (offset - node_offset * difference_type(buffer_size()));
    }
    return *this;
  }

  self& operator+(difference_type n)
  {
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n)
  {
    return *this+= -n;
  }

  self operator-(difference_type n)
  {
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const
  {
    return *(*this + n);
  }

  bool operator==(const self& rhs) const
  {
    return cur == rhs.cur;
  }

  bool  operator!=(const self& rhs) const
  {
    return !(*this == rhs);
  }

  bool operator<(const self& rhs) const
  {
    return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node);
  }
};

template <typename T, typename Alloc = alloc, size_t BuferSize = 0>
class deque
{
public:                                         //Basic types
  typedef T             value_type;
  typedef value_type*   pointer;
  typedef size_t        size_type;

public:                                         //Iterator
  typedef __deque_iterator<T, T&, T*,BufSiz>  iterator;

protected:                                      //Internal typedefs
  typedef pointer*      map_pointer;                //元素指针的指针

protected:                                      //Data members
  iterator  start;                                  //第一个节点
  iterator  finish;                                 //最后一个节点
  map_pointer map;                                  //map是连续空间，其内每个元素都是一个指针，指向一块缓冲区
  size_type map_size;                               //map内可容纳多少指针

public:                                         //Basic accessors
  iterator begin()  
  {
    return start;
  }

  iterator end()
  {
    return finish;
  }

  reference operator[](size_type n)
  {
    return start[difference_type(n)];
  }

  reference front()
  {
    return *start;
  }

  reference back()
  {
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }

  size_type size() const
  {
    return finish - start;
  }

  size_type max_size() const 
  {
    return size_type(-1);
  }

  bool empty() const
  {
    return finish == start;
  }
};

} //namespace cyj