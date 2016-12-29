// Edit by cyj 2016-12-29
#ifndef __CYJ_NEXT_PERMUTATION_HPP__
#define __CYJ_NEXT_PERMUTATION_HPP__

namespace cyj
{

template <typename BidirectionalIterator>
bool next_premutation(BidirectionalIterator first,
                      BidirectionalIterator last)
{
  if (first == last) return false;           //������
  BidirectionalIterator i = first;
  ++i;
  if (i == last) return false;               //ֻ��һ��Ԫ��
  i = last;
  --i;

  for (;;)
  {
    BidirectionalIterator ii = i;
    --i;
    //���ϣ�����һ�飨����������Ԫ��
    if (*i < *ii)                           //���ǰһ��Ԫ��С�ں�һ��Ԫ��
    {
      BidirectionalIterator j = last;
      while (!(*i < *--j));                 //1����β����ǰ�ң�ֱ�������� *i ���Ԫ��
      std::iter_swap(i, j);                 //2������i,j
      std::reverse(ii, last);               //3����ii֮���Ԫ��ȫ����������
      return true;
    }
    if (i == first)                         //��������ǰ��
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
  if (first == last) return false;               //������
  BidirectionalIterator i = first;
  ++i;
  if (i == last) return false;                   //ֻ��һ��Ԫ��

  i = last;
  --i;

  for (;;)
  {
    BidirectionalIterator ii = i;
    --i;
    if (*ii < *i)
    {
      BidirectionalIterator j = last;
      while (!(*--j < *i));                      //1����β����ǰ�ң�֪�����ϱ� *i С��Ԫ��
      std::iter_swap(i, j);                      //2������ i,j
      std::reverse(ii, last);                    //3���� ii ֮���Ԫ��ȫ����������
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