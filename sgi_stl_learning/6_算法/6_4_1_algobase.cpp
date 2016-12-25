// Edit by cyj 2016-12-25

#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

using namespace std;

int main()
{
  vector<int> iv{ 0, 1, 2, 3, 4, 5, 6, 7, 8};
  vector<int> iv1(iv.begin(), iv.begin() + 5);
  vector<int> iv2 = iv;

  //cout << *(mismatch(iv1.begin(), iv1.end(), iv2.begin()).first) << endl;
  cout << *(mismatch(iv1.begin(), iv1.end(), iv2.begin()).second) << endl;

  cout << equal(iv1.begin(), iv1.end(), iv2.begin()) << endl;

  cout << equal(iv1.begin(), iv1.end(), iv2.begin() + 3) << endl;

  fill(iv1.begin(), iv1.end(), 9);
  for_each(iv1.begin(), iv1.end(), [](int val){
    cout << val << " ";
  });
  cout << endl;

  fill_n(iv1.begin(), 3, 7);
  for_each(iv1.begin(), iv1.end(), [](int val)
  {
    cout << val << " ";
  });
  cout << endl;

  auto iter1 = iv1.begin();
  auto iter2 = iter1;
  advance(iter2, 3);
  iter_swap(iter1, iter2);
  cout << *iter1 << " " << *iter2 << endl;
  for_each(iv1.begin(), iv1.end(), [](int val)
  {
    cout << val << " ";
  });
  cout << endl;


  return 0;
}