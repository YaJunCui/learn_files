// Edit by cyj 2016-12-29
#include "next_permutation.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main()
{
  vector<int> iv{ 0, 2, 4};
  cout << "test next_premutation" << endl;

  do
  {
    for (auto val : iv)
      cout << val << " ";
    cout << endl;
  } while (cyj::next_premutation(iv.begin(), iv.end()));
  cout << endl;

  vector<int> iv1{ 4, 2, 0 };
  cout << "test prev_premutation" << endl;
  do
  {
    for (auto val : iv1)
      cout << val << " ";
    cout << endl;
  } while (cyj::prev_permutation(iv1.begin(), iv1.end()));

  return 0;
}