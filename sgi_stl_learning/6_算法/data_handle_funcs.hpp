// Edit by cyj 2016-12-26
#ifndef __CYJ_FUNCS_HPP__
#define __CYJ_FUNCS_HPP__

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

template <class

}//namespace cyj

#endif