// Edit by cyj 2016-12-25

#include <iostream>
#include <functional>

using namespace std;

namespace cyj
{

template <typename T, typename Integer, typename MonoidOperation>
inline T power(T x, Integer n, MonoidOperation op)
{
  if (n == 0)
    return 1;
  else if (n < 0)
    return 1.0 / power(x, -n, op);

  while ((n & 1) == 0)                            //如果 n 偶数
  {
    n >>= 1;
    x = op(x, x);
  }

  T result = x;                                   //先保留一个x
  n >>= 1;                                        //n 右移，把末尾的 1 去掉

  while (n != 0)
  {
    x = op(x, x);
    if ((n & 1) != 0)                             //说明 n 又变为奇数
      result = op(result, x);
    n >>= 1;
  }
  return result;
}

template <typename T, typename Integer>
inline T power(T x, Integer n)
{
  return power(x, n, multiplies<T>());
}

}

int main()
{
  cout << cyj::power(10, 3) << endl;

  cout << cyj::power(10.0, -3) << endl;

  return 0;
}