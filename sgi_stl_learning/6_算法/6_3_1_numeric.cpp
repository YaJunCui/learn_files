// Edit by cyj 2016-12-23

#include <iostream>
#include <numeric>
#include <functional>
#include <vector>
#include <iterator>

using namespace std;

int main()
{
  vector<int> iv{ 1, 2, 3, 4, 5 };

  cout << accumulate(iv.begin(), iv.end(), 0) << endl;

  cout << accumulate(iv.begin(), iv.end(), 0, minus<int>()) << endl;

  cout << inner_product(iv.begin(), iv.end(), iv.begin(), 10) << endl;

  cout << inner_product(iv.begin(), iv.end(), iv.begin(), 10, minus<int>(), plus<int>()) << endl;

  ostream_iterator<int> oiter(cout, " ");

  partial_sum(iv.begin(), iv.end(), oiter);
  cout << endl;

  partial_sum(iv.begin(), iv.end(), oiter, minus<int>());
  cout << endl;

  adjacent_difference(iv.begin(), iv.end(), oiter);
  cout << endl;

  adjacent_difference(iv.begin(), iv.end(), oiter, plus<int>());
  cout << endl;

  cout << pow(10, 3) << endl;

  int n = 3;
  iota(iv.begin(), iv.end(), n);
  for (int i = 0; i < iv.size(); ++i)
    cout << iv[i] << " ";
  cout << endl;


  return 0;
}