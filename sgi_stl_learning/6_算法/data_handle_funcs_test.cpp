// Edit by cyj 2016-12-26

#include "data_handle_funcs.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main()
{
  vector<int> iv{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 };

  cout << *cyj::adjacent_find(iv.begin(), iv.end()) << endl;

  return 0;
}