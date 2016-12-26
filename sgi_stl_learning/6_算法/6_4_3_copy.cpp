// Edit by cyj 2016-12-25

#include <iostream>
#include <algorithm>
#include <deque>

using namespace std;

int main()
{
  {
  int ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

  copy(ia + 2, ia + 7, ia);

  for_each(ia, ia + 9, [](int val){
    cout << val << " ";
  });
  cout << endl;
  }

  {
    int ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    copy(ia + 2, ia + 7, ia + 4);
    for_each(ia, ia + 9, [](int val){
      cout << val << " ";
    });
    cout << endl;
  }

  {
    int ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    deque<int> id(ia, ia + 9);

    auto first = id.begin();
    auto last = id.end();

    cout << *first << endl;
    advance(first, 2);
    cout << *first << endl;                //2
   
    advance(last, -2);
    cout << *last << endl;                 //7

    auto result = id.begin();
    cout << *result << endl;               //0

    copy(first, last, result);
    for (auto val : id)
    {
      cout << val << " ";
    }
    cout << endl;
  }

  {
    int ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    deque<int> id(ia, ia + 9);

    auto first = id.begin();
    auto last = id.end();

    cout << *first << endl;
    advance(first, 2);
    cout << *first << endl;                //2

    advance(last, -2);
    cout << *last << endl;                 //7

    auto result = id.begin();
    advance(result, 4);
    cout << *result << endl;               //4

    copy(first, last, result);
    for (auto val : id)
    {
      cout << val << " ";
    }
    cout << endl;
  }

  return 0;
}