#include <iostream>
#include <list>
using namespace std;

void print_list(const list<int>& ilist)
{
    for (auto val : ilist)
        cout << val << " ";
    cout << endl;
}

void list_sort(list<int>& ilist)
{
    if (ilist.size() <= 1)                 //个数小于1不需要排序
        return;

    list<int> carry;                       //保存下层merge返回的结果
    list<int> counter[64];                 //模拟merge sort使用的堆栈，保存部分有序的list,64是最大的递归调用层次
    int fill = 0;                          //指示堆栈的层次

    while (!ilist.empty())
    {
        carry.splice(carry.begin(), ilist, ilist.begin());  //将begin处的元素从list取下，insert到carry中
        int i = 0;
        while (i<fill && !counter[i].empty())               // 模拟递归时对下层的返回结果进行merge，并将结果交给carry
        {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }

        carry.swap(counter[i]);                             // carry将结果保存到堆栈
        if (i == fill) ++fill;                              // 更新递归层次
    }

    for (int i = 1; i < fill; ++i)                          // 逐级获取结果
        counter[i].merge(counter[i - 1]);
    ilist.swap(counter[fill - 1]);                          // 保存最终结果
}

int main_list_sort()
{
    list<int> ilist{ 10, 9, 11, 8, 1 };

    print_list(ilist);

    list_sort(ilist);

    print_list(ilist);

    return 0;
}