// Edit by cyj 2016-12-16

namespace cyj
{

inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n !=0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <typename T, typename Alloc = alloc, size_t BuferSize = 0>
class deque
{
public:                                         //Basic types
  typedef T             value_type;
  typedef value_type*   pointer;

protected:
  typedef pointer*      map_pointer;                //元素指针的指针

protected:                                      //Data members
  map_pointer map;                                  //map是连续空间，其内每个元素都是一个指针，指向一块缓冲区
  size_type map_size;                               //map内可容纳多少指针
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

  difference_type

};

} //namespace cyj