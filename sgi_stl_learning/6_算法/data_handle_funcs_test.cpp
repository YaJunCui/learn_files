// Edit by cyj 2016-12-26

#include "data_handle_funcs.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <functional>

using namespace std;

int main()
{
  //vector<int> iv{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 };
  //vector<int> iv1{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 };
  vector<int> iv{ 0, 0, 1, 0, 0, 0, 1 };
  vector<int> iv1{ 0, 0, 0, 1 };
  list<int> ilist(iv.begin(), iv.end());

  cout << "test cyj::adjacent_find" << endl;
  cout << *cyj::adjacent_find(iv.begin(), iv.end()) << endl;

  cout << "test cyj::count" << endl;
  cout << cyj::count(iv.begin(), iv.end(), 6) << endl;

  cout << "test cyj::count_if" << endl;
  cout << cyj::count_if(iv.begin(), iv.end(), bind(equal_to<int>(),placeholders::_1,7)) << endl;

  //cout << "test diatance" << endl;
  //cout << cyj::distance(iv.begin(), iv.end()) << endl;

  cout << "test diatance" << endl;
  cout << cyj::distance(ilist.begin(), ilist.end()) << endl;

  cout << "test search" << endl;
  auto iter = cyj::search(iv.begin(), iv.end(), iv1.begin(), iv1.end());
  if (iter != iv.end())
    cout << cyj::distance(iv.begin(), iter) << endl;

  return 0;
}