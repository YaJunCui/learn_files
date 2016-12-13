// Edit by cuiyajun 2016-12-13

#if 0                                                        //advance

template <typename InputIterator,typename Distance>
void advance_II(InputIterator& i, Distance n)
{
    while (n--) ++i;               //单向，逐一前进
}

template <typename BidirectionalIterator, typename Distance>
void advance_BI(BidirectionalIterator& i, Distance n)
{
    if (n >= 0)                    //双向，逐一前进
        while (n--) ++i;
    else
        while (n++) --i;
}

template <typename RandomAccessIterator, typename Distance>
void advance_RAI(RandomAccessIterator& i, Distance n)
{
    i += n;                        //随机，跳跃前进
}

#endif

#if 0

#include <iterator>
using namespace std;

template <typename InputIterator>
inline iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last)                                           //逐一累计距离
    {
        ++first; ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;                                            //直接计算
}

template <typename InputIterator>
inline iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

#endif