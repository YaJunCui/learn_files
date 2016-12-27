﻿// Edit by cyj 2016-12-26
#ifndef __CYJ_FUNCS_HPP__
#define __CYJ_FUNCS_HPP__
#include <iterator>

using std::input_iterator_tag;
using std::forward_iterator_tag;
using std::random_access_iterator_tag;
using std::bidirectional_iterator_tag;
using std::iterator_traits;
using std::reverse_iterator;

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
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value)
{
  typename iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first)
  {
    if (*first == value)
      ++n;
  }
  return n;
}

template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
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
typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
  return typename iterator_traits<RandomAccessIterator>::difference_type(last - first);
}

template<class InputIterator>
typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
  typename iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first)
    ++n;
  return n;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
  return __distance(first, last, typename iterator_traits<InputIterator>::iterator_category());
}

template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, 
                               ForwardIterator1 last1,
                               ForwardIterator2 first2,
                               ForwardIterator1 last2)
{
  typedef typename iterator_traits<ForwardIterator1>::difference_type Distance1;
  typedef typename iterator_traits<ForwardIterator2>::difference_type Distance2;

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

template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1,
                                 ForwardIterator1 last1,
                                 ForwardIterator2 first2,
                                 ForwardIterator2 last2,
                                 forward_iterator_tag,
                                 forward_iterator_tag)
{
  if (first2 == last2)                            //如果查询目标是空的
    return first1;
  else
  {
    ForwardIterator1 result = last1;
    while (1)
    {
      //利用search()查找某个子序列，找不到话返回last1
      ForwardIterator1 new_result = cyj::search(first1, last1, first2, last2);
      if (new_result == last1)
        return result;
      else
      {
        result = new_result;
        first1 = new_result;
        ++first1;
      }
    }
  }
}

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator1 __find_end(BidirectionalIterator1 first1,
                                         BidirectionalIterator1 last1,
                                         BidirectionalIterator2 first2,
                                         BidirectionalIterator2 last2,
                                         bidirectional_iterator_tag,
                                         bidirectional_iterator_tag)
{
  //利用反向查找，reverse_iterator
  typedef reverse_iterator<BidirectionalIterator1> reviter1;
  typedef reverse_iterator<BidirectionalIterator2> reviter2;

  reviter1 rlast1(first1);
  reviter2 rlast2(first2);
  //查找时将序列1和序列2逆序
  reviter1 rresult = cyj::search(reviter1(last1), rlast1,
                                 reviter2(last2), rlast2);
  if (rresult == rlast1)                    //没有找到
    return last1;
  else
  {
    BidirectionalIterator1 result = rresult.base();
    std::advance(result, -cyj::distance(first2, last2));
    return result;
  }
}

template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1,
                                 ForwardIterator1 last1,
                                 ForwardIterator2 first2,
                                 ForwardIterator2 last2)
{
  typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
  typedef typename iterator_traits<ForwardIterator2>::iterator_category category2;

  return __find_end(first1, last1, first2, last2, category1(), category2());
}

}//namespace cyj

#endif