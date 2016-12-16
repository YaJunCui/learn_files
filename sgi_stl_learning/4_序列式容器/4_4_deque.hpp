// Edit by cyj 2016-12-16

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

};