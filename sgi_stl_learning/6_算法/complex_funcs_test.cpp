// Edit by cyj 2016-12-29
#include "complex_funcs.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main()
{
  vector<int> iv{ 0, 2, 4, 6, 6, 6, 7, 8 };

  auto iter = cyj::lower_bound(iv.begin(), iv.end(), 5);
  cout << *iter << endl;

  for (auto val : iv)
    cout << val << " ";
  cout << endl;

  iv.insert(iter, 5);

  for (auto val : iv)
    cout << val << " ";
  cout << endl;

  return 0;
}