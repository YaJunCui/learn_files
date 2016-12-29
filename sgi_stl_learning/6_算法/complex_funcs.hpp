// Edit by cyj 2016-12-29
#ifndef __CYJ_COMPLEX_FUNCS_HPP__
#define __CYJ_COMPLEX_FUNCS_HPP__
#include <iterator>

using std::input_iterator_tag;
using std::forward_iterator_tag;
using std::random_access_iterator_tag;
using std::bidirectional_iterator_tag;
using std::iterator_traits;
using std::reverse_iterator;
using std::iter_swap;

namespace cyj
{

template <typename ForwardIterator, class T>
inline ForwardIterator __lower_bound(ForwardIterator first,
                                     ForwardIterator last,
                                     const T& value,
                                     forward_iterator_tag)
{
  typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
  Distance len = std::distance(first, last);
  Distance half = 0;
  ForwardIterator middle;

  while (len > 0)
  {
    half = len >> 1;                  //除2
    middle = first;
    std::advance(middle, half);       //令middle指向中间位置

    if (*middle < value)
    {
      first = middle;
      ++first;
      len = len - half - 1;           //修正len，回头测试循环的结束条件
    }
    else
      len = half;                     //修正len，回头测试循环的结束条件
  }
  return first;
}

template <typename RandomAccessIterator, class T>
inline RandomAccessIterator __lower_bound(RandomAccessIterator first,
                                          RandomAccessIterator last,
                                          const T& value,
                                          random_access_iterator_tag)
{
  typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
  Distance len = last - first;
  Distance half = 0;
  RandomAccessIterator middle;

  while (len > 0)
  {
    half = len >> 1;
    middle = first + half;

    if (*middle < value)
    {
      first = middle + 1;
      len = len - half - 1;
    }
    else
      len = half;
  }
  return first;
}

template <typename ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first,
                                   ForwardIterator last,
                                   const T& value)
{
  typedef typename iterator_traits<ForwardIterator>::iterator_category iterator_category;
  return __lower_bound(first, last, value, iterator_category());
}

}

#endif