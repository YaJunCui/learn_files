#include <iostream>
#include <vector>
#include <functional>
#include <iterator>

using namespace std;

namespace cyj
{

template <typename InputIterator, typename OutputIterator, class T>
OutputIterator __adjacent_difference(InputIterator first, InputIterator last,
                                     OutputIterator result, T*)
{
  T value = *first;
  while (++first != last)
  {
    T tmp = *first;
    *++result = tmp - value;
    value = tmp;
  }
  return ++result;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result)
{
  if (first == last) return result;
  *result = *first;

  typename iterator_traits<InputIterator>::value_type value = *first;
  return __adjacent_difference(first, last, result, &value);
}

}

int main()
{
  vector<int> iv = { 1, 2, 3, 4, 5 };

  ostream_iterator<int> oiter(cout, " ");

  cyj::adjacent_difference(iv.begin(), iv.end(), oiter);
  cout << endl;

  return 0;
}