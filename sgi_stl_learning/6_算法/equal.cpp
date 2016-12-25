// Edit by cyj 2016-12-25

#include <iostream>
#include <vector>
#include <functional>

using namespace std;

namespace cyj
{

template <typename InputIterator1, typename InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2)
{
  for (; first != last; ++first1, ++first2)
  {
    if (*first1 != *first2)
      return false;
  }
  return true;
}

template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                  BinaryPredicate binary_pred)
{
  for (; first1 != last1; ++first1, ++first2)
  {
    if (!(binary_function(*first1, *first2)))
      return false;
  }
  return true;
}

}

int main()
{
  vector<int> iv1{ 0, 1, 2, 3, 4 };
  vector<int> iv2{ 1, 2, 3, 4, 5 };

  cout << equal(iv1.begin(), iv1.end(), iv2.begin()) << endl;

  cout << equal(iv1.begin(), iv1.end(), iv2.begin(),less<int>()) << endl;

  return 0;
}