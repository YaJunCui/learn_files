// Edit by cyj 2017-1-5

#include <iostream>
#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>

using std::cout;
using std::endl;
using std::pair;
using std::iterator_traits;
using std::vector;

namespace cyj
{

template <typename ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last, const T& vlaue, 
              std::forward_iterator_tag)
{
  typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
  Distance len = std::distance(first,last);
  Distance half;
  ForwardIterator middle, left, right;

  while (len > 0)
  {
    half >>= 1;
    middle = first;
    std::advance(middle, half);

    if (*middle < value)
    {
      first = middle;
      ++first;
      len = len - half - 1;
    }
    else if (value < *middle)
    {
      len = half;
    }
    else                        //中值与 value 相等
    {
      left = std::lower_bound(first, middle, value);     //不大于 value 的最小值
      std::advance(first, len);
      right = std::upper_bound(++middle, first, value);  //大于 value 的最小值

      return pair<ForwardIterator, ForwardIterator>(left, right);
    }
  }

  return pair<ForwardIterator, ForwardIterator>(first, first);
}

template <typename RandomAccessIterator, class T>
inline pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last, const T& value,
              std::random_access_iterator_tag)
{
  typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle, left, right;

  while (len > 0)
  {
    half = len >> 1;
    middle = first + half;

    if (*middle < value)
    {
      first = middle + 1;
      len = len - half - 1;
    }
    else if (value < *middle)
    {
      len = half;
    }
    else
    {
      left = std::lower_bound(first, middle, value);
      right = std::upper_bound(++middle, first + len, value);

      return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
    }
  }

  return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template <typename ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value)
{
  typedef typename iterator_traits<ForwardIterator>::iterator_category iterator_category;
  return __equal_range(first, last, value, iterator_category());
}

}

int main_equal()
{
  vector<int> ivec{ 1, 2, 3, 4, 4, 4, 4, 5, 6, 7 };

  for (auto val : ivec)
    cout << val << " ";
  cout << endl;

  auto pair_bound = cyj::equal_range(ivec.begin(), ivec.end(), 4);

  for (auto iter = pair_bound.first; iter != pair_bound.second; ++iter)
  {
    cout << *iter << " ";
  }
  cout << endl;

  return 0;
}