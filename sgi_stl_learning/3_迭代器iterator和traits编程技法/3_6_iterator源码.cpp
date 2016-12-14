// Edit by cyj 2016-12-13

// 五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag :public input_iterator_tag {};
struct bidirectional_iterator_tag :public forward_iterator_tag {};
struct random_access_iterator_tag :public bidirectional_iterator_tag {};

// 为了避免写代码时漏写，自行开发的迭代器最好继承自下面这个std::iterator
template <typename Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
};

// "榨汁机" traits
template <typename Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

// 针对原生指针(native pointer)而设计的 traits 偏特化版
template <typename T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

// 针对原生的 pointer_to_const 而设计的 traits 偏特化版
template <typename T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

// 这个函数可以很方便地决定某个迭代器的类型(category)
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_category
iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// 这个函数可以很方便地决定某个迭代器的 distance_type
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 这个函数可以很方便地决定某个迭代器的 value type
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 以下是整组 distance 函数
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last)
    {
        ++first; ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

// 以下是整组 advance 函数
template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while (n--) ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n)
{
    typedef typename iterator_traits<InputIterator>::iterator_category _Category_t;
    __advance(i, n, _Category_t());
}