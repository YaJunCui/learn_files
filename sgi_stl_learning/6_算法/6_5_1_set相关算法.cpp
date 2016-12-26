//Edit by cyj 2015-12-26

#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
  multiset<int> S1{ 1, 3, 5, 7, 9, 11 };
  multiset<int> S2{ 1, 1, 2, 3, 5, 8, 13 };

  for_each(S1.begin(), S1.end(), [](int val){
    cout << val << " ";
  });
  cout << endl;

  for_each(S2.begin(), S2.end(), [](int val){
    cout << val << " ";
  });
  cout << endl << endl;

  auto first1 = S1.begin();
  auto last1 = S1.end();
  auto first2 = S2.begin();
  auto last2 = S2.end();

  cout << "Union of S1 and S2:" << endl;
  set_union(first1, last1, first2, last2, ostream_iterator<int>(cout, " "));
  cout << endl << endl;

  first1 = S1.begin();
  first2 = S2.begin();
  cout << "Intersection of S1 and S2:" << endl;
  set_intersection(first1, last1, first2, last2, ostream_iterator<int>(cout, " "));
  cout << endl << endl;

  first1 = S1.begin();
  first2 = S2.begin();
  cout << "Difference of S1 and S2(S1 - S2):" << endl;
  set_difference(first1, last1, first2, last2, ostream_iterator<int>(cout, " "));
  cout << endl << endl;

  first1 = S1.begin();
  first2 = S2.begin();
  cout << "Difference of S2 and S1(S2 - S1):" << endl;
  set_difference(first2, last2, first1, last1, ostream_iterator<int>(cout, " "));
  cout << endl << endl;

  first1 = S1.begin();
  first2 = S2.begin();
  cout << "Symmetric difference of S1 and S2:" << endl;
  set_symmetric_difference(first1, last1, first2, last2, ostream_iterator<int>(cout, " "));
  cout << endl << endl;


  return 0;
}