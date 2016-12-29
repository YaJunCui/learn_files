// Edit by cyj 2016-12-29
#ifndef __CYJ_NEXT_PERMUTATION_HPP__
#define __CYJ_NEXT_PERMUTATION_HPP__

namespace cyj
{

template <typename BidirectionalIterator>
bool next_premutation(BidirectionalIterator first,
                      BidirectionalIterator last)
{
  if (first == last) return false;           //空区间
  BidirectionalIterator i = first;
  ++i;
  if (i == last) return false;               //只有一个元素
  i = last;
  --i;

  for (;;)
  {
    BidirectionalIterator ii = i;
    --i;
    //以上，锁定一组（两个）相邻元素
    if (*i < *ii)                           //如果前一个元素小于后一个元素
    {
      BidirectionalIterator j = last;
      while (!(*i < *--j));                 //1、从尾端往前找，直到遇到比 *i 大的元素
      std::iter_swap(i, j);                 //2、交换i,j
      std::reverse(ii, last);               //3、将ii之后的元素全部逆序重排
      return true;
    }
    if (i == first)                         //进行至最前面
    {
      reverse(first, last);
      return false;
    }
  }
}

template <typename BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first,
                      BidirectionalIterator last)
{
  if (first == last) return false;               //空区间
  BidirectionalIterator i = first;
  ++i;
  if (i == last) return false;                   //只有一个元素

  i = last;
  --i;

  for (;;)
  {
    BidirectionalIterator ii = i;
    --i;
    if (*ii < *i)
    {
      BidirectionalIterator j = last;
      while (!(*--j < *i));                      //1、由尾部往前找，知道遇上比 *i 小的元素
      std::iter_swap(i, j);                      //2、交换 i,j
      std::reverse(ii, last);                    //3、将 ii 之后的元素全部逆序重排
      return true;
    }
    if (i == first)
    {
      std::reverse(first, last);
      return false;
    }
  }
}

}  //namespace cyj

#endif