#include <iostream>
#include <vector>
#include <functional>

using namespace std;

namespace cyj
{

template <typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator last, T init)
{
  for (; first != last; ++first)
    init = init + *first;
  return init;
}

template <typename InputIterator, typename T, typename BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, 
             BinaryOperation binary_op)
{
  for (; first != last; ++first)
    init = binary_op(init, *first);
  return init;
}

}

int main()
{
  vector<int> iv = { 1, 2, 3, 4, 5 };

  cout << cyj::accumulate(iv.begin(), iv.end(), 0) << endl;

  cout << cyj::accumulate(iv.begin(), iv.end(), 0, minus<int>()) << endl;
  

  return 0;
}