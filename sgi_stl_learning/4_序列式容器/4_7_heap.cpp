// Edit by cyj 2016-12-18

#include <iostream>
#include <vector>

using namespace std;

void print_vec(const vector<int>& ivec)
{
  for (auto val : ivec)
    cout << val << " ";
  cout << endl;
}

template <typename RandomAccessIterator, typename Distance, typename T>
inline void __push_heap(RandomAccessIterator first,
                        Distance curIndex,
                        Distance topIndex,
                        T value)
{
  Distance parentIndex = (curIndex - 1) / 2;
  while (curIndex > topIndex && *(first + parentIndex) < value)          //未到顶端，且父节点小于新值，不符合heap，需要调整
  {
    *(first + curIndex) = *(first + parentIndex);                        //令当前索引的值为父值
    curIndex = parentIndex;
    parentIndex = (curIndex - 1) / 2;
  } //持续到顶端，或满足heap的次序特性为止
  *(first + curIndex) = value;
}

template <typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
{
  __push_heap(first, last - first - 1, 0, *(last - 1));
}

#if 0
template <typename RandomAccessIterator, typename Distance, typename T>
inline void __adjust_heap(RandomAccessIterator first,
                          Distance curIndex,
                          Distance len,
                          T value)
{
  Distance topIndex = curIndex;
  Distance secondChild = 2 * (curIndex + 1);               //找出当前节点的右子节点

  while (secondChild < len)
  {
    if (*(first + secondChild) < *(first + secondChild - 1))
      --secondChild;
    *(first + curIndex) = *(first + secondChild);
    curIndex = secondChild;
    secondChild = 2 * (curIndex + 1);                     //找出当前节点的右子节点
  }

  if (secondChild == len)                                 //只有左子节点
  {
    *(first + curIndex) = *(first + secondChild - 1);
    curIndex = secondChild - 1;
  }
  __push_heap(first, curIndex, topIndex, value);          //可能尚未满足次序特性，执行一次上溯
}
#else
template <typename RandomAccessIterator, typename Distance, typename T>
inline void __adjust_heap(RandomAccessIterator first,
                          Distance curIndex,
                          Distance len,
                          T value)
{
  Distance secondChild = 2 * (curIndex + 1);               //找出当前节点的右子节点

  while (secondChild <= len)
  {
    if (secondChild == len || *(first + secondChild) < *(first + secondChild - 1)) //secondChild==len：说明只有左节点
      --secondChild;
    if (value >= *(first + secondChild)) break;           //筛选结束

    *(first + curIndex) = *(first + secondChild);
    curIndex = secondChild;
    secondChild = 2 * (curIndex + 1);                     //找出当前节点的右子节点
  }
  *(first + curIndex) = value;
}
#endif


template <typename RandomAccessIterator, typename T>
inline void __pop_heap(RandomAccessIterator first,
  RandomAccessIterator last,
  RandomAccessIterator result,
  T value)
{
  *result = *first;                                      //尾部元素设置为首值
  __adjust_heap(first, 0, last - first, value);
}

template <typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
  __pop_heap(first, last - 1, last - 1, *(last - 1));
}

template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first,
               RandomAccessIterator last)
{
  while (last - first > 1)               
    pop_heap(first, last--);               //每执行pop_heap()一次，操作范围退缩一格
}


template <typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first,
                      RandomAccessIterator last)
{
  typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;

  if (last - first < 2) return;
  Distance len = last - first;
  Distance parent = (len - 2) / 2;

  while (true)
  {
    __adjust_heap(first, parent, len, *(first + parent));
    if (parent == 0) break;
    --parent;
  }
}

int main()
{
  //vector<int> ivec{ 10, 8, 9, 6, 7, 10 };

  //print_vec(ivec);
  //push_heap(ivec.begin(), ivec.end());
  //print_vec(ivec);

  //pop_heap(ivec.begin(), ivec.end());
  //print_vec(ivec);

  //vector<int> ivec{ 10, 8, 9, 6, 7, 8,7 };
  vector<int> ivec{ 10, 8, 6,9, 7, 7,8 };

  print_vec(ivec);
  make_heap(ivec.begin(), ivec.end());

  print_vec(ivec);
  sort_heap(ivec.begin(), ivec.end());
  print_vec(ivec);

  return 0;
}