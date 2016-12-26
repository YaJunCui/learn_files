// Edit by cyj 2016-12-26
#ifndef __CYJ_COPY_H__
#define __CYJ_COPY_H__
#include <cstring>
#include <iterator>

using namespace std;

namespace cyj
{

inline char* copy(const char* first, const char* last, char* result)
{
  memmove(result, first, last - first);
  return result;
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
  memmove(result, first, last - first);
  return result;
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator 
__copy_d(RandomAccessIterator first, RandomAccessIterator last,
         OutputIterator result, Distance*)
{
  // 以 n 决定循环的执行次数。速度快
  for (Distance n = last - first; n > 0; --n, ++result, ++first)
    *result = *first;
  return result;
}

//适应指针所指对象具备 trivial assignment operator
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, true_type)
{
  memmove(result, first, sizeof(T)*(last - first));
  return result + (last - first);
}

//适应指针所指对象具备 non-trivial assignment operator
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, false_type)
{
  return __copy_d(first, last, result, (ptrdiff_t*)0);
}

// InputIterator 版本
template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last,
                             OutputIterator result, input_iterator_tag)
{
  // 以迭代器等同与否，决定循环是否继续。速度慢
  for (; first != last; ++result, ++first)
    *result = *first;
  return result;
}

// RandomIterator 版本
template <class RandomAccessIterator, class OutputIterator>
inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last,
                             OutputIterator result, random_access_iterator_tag)
{
  // 又划出一个函数，为的是其他地方可以用到
  typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
  return __copy_d(first, last, result, (Distance*)(0));
}

template <class InputIterator,class OutputIterator>
struct __copy_dispatch
{
  OutputIterator operator()(InputIterator first, InputIterator last,
                            OutputIterator result)
  {
    typedef typename iterator_traits<InputIterator>::iterator_category Category;
    return __copy(first, last, result, Category());
  }
};

template <class T>
struct __copy_dispatch<T*,T*>
{
  T* operator()(T* first, T* last, T* result)
  {
    return __copy_t(first, last, result, typename is_trivially_copy_assignable<T>::type());
  }
};

template <class T>
struct __copy_dispatch<const T*, const T*>
{
  const T* operator()(const T* first, const T* last, T* result)
  {
    return __copy_t(first, last, result, typename is_trivially_copy_assignable<T>::type());
  }
};

// 对外接口
template <class InputIterator,class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last,
                           OutputIterator result)
{
  return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

} //namespcace cyj

#endif // __CYJ_COPY_H__