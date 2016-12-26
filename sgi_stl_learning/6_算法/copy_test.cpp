// Edit by cyj 2016-12-26

#include "copy.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <deque>

using namespace std;

class C
{
public:
  C(int data=0) :data_(data){}

  int get_data()
  {
    return data_;
  }

private:
  int data_;
};

int main()
{
#if 0
  //测试1
  const char ccs[] = { 'a', 'b', 'c', 'd', 'e' };
  char ccd[6] = {'\0'};

  cyj::copy(ccs, ccs + 5, ccd);
  for (int i = 0; i < 5; ++i)
    cout << ccd[i] << " ";
  cout << endl;
#endif
 
#if 0
  //测试2
  const wchar_t cwcs[] = { 'a', 'b', 'c', 'd', 'e' };
  wchar_t cwcd[6] = { '\0' };

  cyj::copy(cwcs, cwcs + 5, cwcd);
#endif

#if 0
  //测试3
  int ia[] = { 0, 1, 2, 3, 4 };
  int ia1[6] = { '\0' };
  cyj::copy(ia, ia + 5, ia1);
  for (int i = 0; i < 5; ++i)
    cout << ia1[i] << " ";
  cout << endl;
#endif
  
#if 0
  //测试4
  int ia[] = { 0, 1, 2, 3, 4 };
  list<int> ilists(ia, ia + 5);
  list<int> ilistd(5);
  cyj::copy(ilists.begin(),ilists.end(),ilistd.begin());
#endif

#if 0
  //测试5
  int ia[] = { 0, 1, 2, 3, 4 };
  vector<int> ivecs(ia, ia + 5);
  vector<int> ivecd(5);
  cyj::copy(ivecs.begin(), ivecs.end(), ivecd.begin());
#endif

#if 0
  //测试6
  C c[5] = {C(1),C(2),C(3),C(4),C(5)};
  vector<C> Cvs(c,c+5);
  vector<C> Cvd(5);
  cyj::copy(Cvs.begin(), Cvs.end(), Cvd.begin());

  for (auto val : Cvd)
    cout << val.get_data() << " ";
  cout << endl;
#endif

#if 0
  //测试7
  C c[5] = { C(1), C(2), C(3), C(4), C(5) };
  deque<C> Cds(c, c + 5);
  deque<C> Cdd(5);
  cyj::copy(Cds.begin(), Cds.end(), Cdd.begin());

  for (auto val : Cdd)
    cout << val.get_data() << " ";
  cout << endl;
#endif

  return 0;
}