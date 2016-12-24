#include <iostream>
#include <vector>

using namespace std;

namespace cyj
{

template <typename InputIterator, typename OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result)
{
  typedef typename iterator_traits<InputIterator>::value_type value_type;

  if (first == last) return result;
  *result = *first;

  value_type value = *first;
  while (++first != last)
  {
    value += *first;
    *++result = value;
  }

  return ++result;
}

}

int main()
{
  vector<int> iv{ 1, 2, 3, 4, 5 };

  vector<int> ov=iv;

  cyj::partial_sum(iv.begin(), iv.end(), ov.begin());

  for (auto val : ov)
  {
    cout << val << " ";
  }
  cout << endl;
  

  return 0;
}