// Edit by cyj 2016-12-26
#ifndef __CYJ_FUNCS_HPP__
#define __CYJ_FUNCS_HPP__
#include <iterator>
#include <iostream>

namespace cyj
{

template <typename ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
{
  if (first == last) return last;
  ForwardIterator next = first;

  while (++next != last)
  {
    if (*first == *next)           //找打相邻的元素值相等，就结束
      return first;
    first = next;
  }
  return last;
}

template <class InputIterator, class T>
typename std::iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value)
{
  typename std::iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first)
  {
    if (*first == value)
      ++n;
  }
  return n;
}

template <class InputIterator, class Predicate>
typename std::iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred)
{
  typename iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first)
  {
    if (pred(*first))
      ++n;
  }
  return n;
}

template<class RandomAccessIterator>
typename std::iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, std::random_access_iterator_tag)
{
  return typename std::iterator_traits<RandomAccessIterator>::difference_type(last - first);
}

template<class InputIterator>
typename std::iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, std::input_iterator_tag)
{
  typename std::iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first)
    ++n;
  return n;
}

template <class InputIterator>
typename std::iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
  return __distance(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
}

template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, 
                               ForwardIterator1 last1,
                               ForwardIterator2 first2,
                               ForwardIterator1 last2)
{
  typedef typename std::iterator_traits<ForwardIterator1>::difference_type Distance1;
  typedef typename std::iterator_traits<ForwardIterator2>::difference_type Distance2;

  Distance1 d1 = cyj::distance(first1, last1);
  Distance2 d2 = cyj::distance(first2, last2);

  if (d1 < d2)  return last1;               //如果第二序列大于第一序列，不可能成为其子序列

  ForwardIterator1 current1 = first1;
  ForwardIterator2 current2 = first2;

  while (current2 != last2)                 //遍历第二个序列
  {
    if (*current1 == *current2)
    {
      ++current1;
      ++current2;
    }
    else                                    //如果两个元素不相等
    {
      if (d1 == d2)                         //如果两个序列一样长，就不可能成功了
        return last1;                       
      else
      {
        current1 = ++first1;
        current2 = first2;
        --d1;                               //已经排除序列1的一个元素，因此序列1长度减1
      }
    }
  }
  return first1;
}

}//namespace cyj

#endif