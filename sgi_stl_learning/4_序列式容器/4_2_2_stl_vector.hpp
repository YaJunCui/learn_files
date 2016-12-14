// Edit by cyj 2016-12-14

template <typename T, typename Alloc = alloc>
class vector
{
public:
  // vector 的嵌套类别定义
  typedef T           value_type;
  typedef value_type* pointer;
  typedef value_type* iterator;
  typedef value_type& reference;
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_size;

protected:
  // simple_alloc 是 SGI STL 的空间配置器
  typedef simple_alloc<value_type, Alloc> data_allocator;
  iterator start;          // 表示目前使用空间的头
  iterator finish;         // 表示目前使用空间的尾
  iterator end_of_storage; // 表示目前可用空间的尾

  void insert_aux(iterator position, const T &x);
  void deallocate()
  {
    if (start)
      data_allocator::deallocate(start, end_of_storage - start);
  }

  void fill_initialize(size_type n, const T &value)
  {
    start = allocate_and_fill(n, value);
    finish = start + n;
    end_of_storage = finish;
  }

public:
  iterator begin() { return start; }
  iterator end() { return finish; }
  size_type size() const { return size_type(end() - begin()); }
  size_type capacity() const { return size_type(end_of_storage - begin()); }
  bool empty() const { return begin() == end(); }
  reference operator[](size_type n) { return *(begin() + n); }

  vector():start(0), finish(0), end_of_storage(0) {}
  vector(size_type n, const T& value) { fill_initialize(n, value); }
  vector(int n, const T& value) { fill_initialize(n, value); }
  vector(long n, const T& vlaue) { fill_initialize(n,value); }
  explicit vector(size_type n) { fill_initialize(n, T()); }

  ~vector()
  {
    destory(start, finish);
    deallocate();
  }

  reference front() { return *begin(); }             // 第一个元素
  reference back() { return *(end() - 1); }          // 最后一个元素
  void push_back(const T& x)                         // 将元素插入至最尾端元素
  {
    if(finish != end_of_storage)
    {
      construct(finish, x);
      ++finish;
    }
    else
      insert_aux(end(), x);
  }

  void pop_back()                                     // 将最尾端元素取出
  {
    --finish;
    destroy(finish);
  }

  iterator erase(iterator position)                   // 清除某位置上的元素
  {
    if(position + 1 != end())
      copy(position + 1, end(), position);            //将后续元素往前移动
    --finish;
    destroy(finish);
    return position;
  }

  void resize(size_type new_size, const T& x)
  {
    if(new_size < size())
      erase(begin() + new_size, end());
    else
      insert_aux(end(), new_size - size(), x);
  }

  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { erase(begin(), end()); }

protected:
  // 配置空间并填满内容
  iterator allocate_and_fill(size_type n, const T& x)
  {
    iterator result = data_allocator::allocator(n);
    uninitialized_fill_n(result, n, x);                 
    return result;
  }
};

template <typename T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
  if(finish != end_of_storage)                 //还有备用空间
  {
    construct(finish,*(finish-1));                              
    ++finish;                                        //调整水位
    T x_copy = x;
    copy_backward(position, finish-2, finish-1);     //反向copy
    *position = x_copy;
  }
  else                                        //无备用空间
  {
    const size_type old_size = size();
    const size_type len = old_size !=0 ? 2 * old_size : 1;           //如果原大小为0，则配置为 1，否则配置为原来的 2 倍

    iterator new_start = data_allocator::allocate(len);              //实际配置
    iterator new_finish = new_start;

    try
    {
      new_finish = uninitialized_fill_n(start, position, new_start);         //将 vector 内容的[start, position)拷贝到新vector
      construct(new_finish, x);                                              //为新元素设置为初值 x
      ++new_finish;
      new_finish = uninitialized_fill_n(position, finish, new_finish);       //将 vector 内容的[position, finish)拷贝到新vector
    }
    catch(...)
    {
      // "commit or rollback" semantics
      destroy(new_start, new_finish);
      data_allocator::deallocate(new_start, len);
      throw;
    }

    //析构并释放原来 vector
    destroy(begin(), end());
    deallocate();

    //调整迭代器，指向新 vector
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
  }
}