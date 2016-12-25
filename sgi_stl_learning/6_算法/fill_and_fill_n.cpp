// Edit by cyj 2016-12-25

#include <iostream>
#include <vector>

using namespace std;

namespace cyj
{

template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
  for (; first != last; ++first)
    *first = value;
}

template <typename OutputIterator,typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
  for (; n > 0; --n, ++first)
  {
    *first = value;
  }
  return first;
}

}

int main()
{
  vector<int> iv1{ 0, 1, 2, 3, 4 };
  vector<int> iv2{ 1, 2, 3, 4, 5 };

  return 0;
}