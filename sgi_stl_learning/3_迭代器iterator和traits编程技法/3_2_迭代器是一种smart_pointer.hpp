//
// Edit by cyj 2016-12-06
//

#if 0
template <typename T>
class auto_ptr
{
public:
    explicit auto_ptr(T *p = 0) :pointee(p){}
    template <typename U>
    auto_ptr(auto_ptr<U> &rhs) : pointee(rhs.release()){}
    ~auto_ptr(){ delete pointee; }

    template <typename U>
    auto_ptr<T>& operator=(auto_ptr<U>& rhs)
    {
        if (this != &rhs) reset(rhs.release());
        return *this;
    }

    T& operator*() const { return *pointee; }
    T* operator->() const { return pointee; }

    T* get() const { return pointee; }

    T* release()
    {
        T* tmp(pointee);
        pointee = NULL;
        return tmp;
    }

    void reset(T* ptr = NULL)
    {
        if (pointee != ptr)
        {
            delete pointee;
            pointee = ptr;
        }
    }

private:
    T *pointee;
};
#endif

#if 0
#include <iostream>

template <typename T>
class ListItem
{
public:
    ListItem(T value = 0) :value_(value), next_(nullptr){}
    
    T value() const { return value_; }
    ListItem*& next() { return next_; }                    //必须加 &,否则返回值是右值

private:
    T value_;
    ListItem* next_;                                      //单向链表（single linked list）
};

template <typename T>
class List
{
public:
    List() :front_(nullptr), end_(nullptr), size_(0){}
    ~List()
    {
        for (int i = 0; i < size_; ++i)
        {
            ListItem<T> *tmp = front_;
            front_ = tmp->next();
            delete tmp;
            tmp = nullptr;
        }
    }

    void insert_front(T value)                              //头插法
    {
        if (front_ == nullptr)
        {
            front_ = end_ = new ListItem<T>(value);
        }
        else
        {
            ListItem<T> *tmp = new ListItem<T>(value);
            tmp->next() = front_;
            front_ = tmp;
        }
        ++size_;                                            //链表节点总数加1
    }

    void insert_end(T value)                                //尾插法
    {
        if (end_ == nullptr)
        {
            front_ = end_ = new ListItem<T>(value);
        }
        else
        {
            end_->next() = new ListItem<T>(value);
            end_ = end_->next();
        }
        ++size_;                                            //链表节点总数加1
    }

    void display(std::ostream &os = std::cout) const
    {
        ListItem<T> *tmp_front = front_;
        for (int i = 0; i < size_; ++i)
        {
            os << tmp_front->value() << " ";
            tmp_front = tmp_front->next();
        }
    }

    ListItem<T> *front() { return front_; }

private:
    ListItem<T> *front_;
    ListItem<T> *end_;                                    //指向最后一个元素，而不是末尾
    long size_;
};

template <typename Item>
struct ListIter
{
    Item *ptr;                                            //保持与容器之间的一个联系

    ListIter(Item *p = nullptr) :ptr(p){}

    Item& operator*() const{ return *ptr; }
    Item* operator->() const { return ptr; }

    ListIter& operator++() 
    {
        ptr = ptr->next();
        return *this;
    }
    ListIter operator++(int)
    {
        ListIter tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const ListIter &rhs) const
    {
        return ptr == rhs.ptr;
    }
    bool operator!=(const ListIter &rhs) const
    {
        return ptr != rhs.ptr;
    }
};

template <typename T>
bool operator!=(const ListItem<T>& item, T n)
{
    return item.value() != n;
}

template<class InputIterator, class T>
InputIterator find(InputIterator first,
    InputIterator last,
    const T &value)
{
    while (first != last && *first != value)
        ++first;
    return first;
}

int main()
{
    List<int> my_list;
    for (int i = 0; i < 5; ++i)
    {
        my_list.insert_front(i);
        my_list.insert_end(i + 2);
    }
    my_list.display();
    std::cout << "\n\n\n";

    ListIter<ListItem<int>> begin(my_list.front());
    ListIter<ListItem<int>> end;
    ListIter<ListItem<int>> iter;

    iter = find(begin, end, 3);
    if (iter == end)
        std::cout << "3 not found." << std::endl;
    else
        std::cout << "3 found." << std::endl;

    iter = find(begin, end, 7);
    if (iter == end)
        std::cout << "7 not found." << std::endl;
    else
        std::cout << "7 found." << std::endl;



    return 0;
}

#endif