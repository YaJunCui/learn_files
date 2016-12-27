// Edit by cyj 2016-12-26

#include "data_handle_funcs.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::list;

int main()
{
  vector<int> iv{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 };
  vector<int> iv1{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 };
  //vector<int> iv{ 0, 0, 1, 0, 0, 0, 1 };
  //vector<int> iv1{ 0, 0, 0, 1 };
  list<int> ilist(iv.begin(), iv.end());

  cout << "test cyj::adjacent_find" << endl;
  cout << *cyj::adjacent_find(iv.begin(), iv.end()) << endl;

  cout << "test cyj::count" << endl;
  cout << cyj::count(iv.begin(), iv.end(), 6) << endl;

  cout << "test cyj::count_if" << endl;
  cout << cyj::count_if(iv.begin(), iv.end(), std::bind(std::equal_to<int>(),std::placeholders::_1,7)) << endl;

  //cout << "test diatance" << endl;
  //cout << cyj::distance(iv.begin(), iv.end()) << endl;

  cout << "test diatance" << endl;
  cout << cyj::distance(ilist.begin(), ilist.end()) << endl;

  cout << "test search" << endl;
  auto iter = cyj::search(iv.begin(), iv.end(), iv1.begin(), iv1.end());
  if (iter != iv.end())
    cout << cyj::distance(iv.begin(), iter) << endl;

  cout << "test find_end" << endl;
  vector<int> iv2(iv.begin() + 6, iv.begin() + 8);
  cout << *(cyj::find_end(iv.begin(), iv.end(), iv2.begin(), iv2.end()) + 3) << endl;

  cout << "test merge" << endl;
  vector<int> iv3{1,3,5,6,7};
  vector<int> iv4{ 2, 4, 5, 7, 8, 9 };
  vector<int> iv5(iv3.size() + iv4.size(),0);
  cyj::merge(iv3.begin(), iv3.end(), iv4.begin(), iv4.end(), iv5.begin());
  for (auto val : iv5)
    cout << val << " ";
  cout << endl;

  cout << "test partition" << endl;
  vector<int> iv7{ 1, 6, 3, 9, 2, 4, 5 };
  for (auto val : iv7)
    cout << val << " ";
  cout << endl;
  cyj::partition(iv7.begin(), iv7.end(), std::bind(std::less_equal<int>(), std::placeholders::_1, 4));
  //std::stable_partition(iv7.begin(), iv7.end(), std::bind(std::less_equal<int>(), std::placeholders::_1, 4));
  for (auto val : iv7)
    cout << val << " ";
  cout << endl;


  return 0;
}