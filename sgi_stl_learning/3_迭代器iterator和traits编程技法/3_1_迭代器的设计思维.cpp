// Edit by cyj 2016-12-6
// find�㷨: <stl_algo.h> Դ��
#if 0
template<class InputIterator, class T>
InputIterator find(InputIterator first,
                   InputIterator last,
                   const T &value)
{
    while (first != last && *first != value)
        ++first;
    return first;
}
#endif

#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <iostream>

using namespace std;

int main_3_1()
{
    const int arraySize = 7;
    int ia[arraySize] = { 0, 1, 2, 3, 4, 5, 6 };

    vector<int> ivec(ia, ia + arraySize);         //��̬����
    list<int> ilist(ia, ia + arraySize);          //˫���б�
    deque<int> ideque(ia, ia + arraySize);        //˫�����

    vector<int>::iterator it1 = find(ivec.begin(), ivec.end(), 4);
    if (it1 == ivec.end())
        cout << "4 not found." << endl;
    else
        cout << "4 found." << endl;

    list<int>::iterator it2 = find(ilist.begin(), ilist.end(), 6);
    if (it2 == ilist.end())
        cout << "6 not found." << endl;
    else
        cout << "6 found." << endl;

    deque<int>::iterator it3 = find(ideque.begin(), ideque.end(), 8);
    if (it3 == ideque.end())
        cout << "8 not found." << endl;
    else
        cout << "8 found." << endl;

    return 0;
}