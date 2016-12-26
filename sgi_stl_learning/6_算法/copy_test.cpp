// Edit by cyj 2016-12-26

#include "copy.hpp"

#include <iostream>
#include <vector>
#include <list>

using namespace std;

class C
{
public:
  C() :data_(3){}

private:
  int data_;
};

class A
{
public:
  A() :data_(new int(10)){}

private:
  int* data_;
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

#if 1
  //测试6
  C c[5];
  vector<C> Cvs(c,c+5);
  vector<C> Cvd(5);

#endif

#if 0

#endif
#if 0

#endif

#if 0

#endif

#if 0

#endif

#if 0

#endif

  return 0;
}