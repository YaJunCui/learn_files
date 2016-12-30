//Edit by cyj 2016-12-20
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace cyj
{

template <typename RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first,
                         RandomAccessIterator middle,
                         RandomAccessIterator last)
{
  typedef typename iterator_traits<RandomAccessIterator>::value_type T;

  make_heap(first, middle);
  for (RandomAccessIterator i = middle; i < last; ++i)
  {
    if (*i < *first)
    {
      std::iter_swap(first, i);
      std::make_heap(first, middle);
    }
  }
  std::sort_heap(first, middle);
}

}

int main()
{
  vector<int> iv{ 69, 23, 80, 42, 17, 15, 26, 51, 19, 12, 35, 8 };
  cyj::partial_sort(iv.begin(), iv.begin() + 7, iv.end());

  for (auto val : iv)
  {
    cout << val << " ";
  }
  cout << endl << endl;

  return 0;
}