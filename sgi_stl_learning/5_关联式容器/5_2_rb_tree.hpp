// Edit by cyj 2016-12-19
#include <cstddef>

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;         // 红色为 0
const __rb_tree_color_type __rb_tree_black = true;        // 黑色为 1

struct __rb_tree_node_base
{
  typedef __rb_tree_color_type  color_type;
  typedef __rb_tree_node_base*  base_ptr;

  color_type color;                                       //节点颜色，非红即黑
  base_ptr  parent;                                       //父节点
  base_ptr  left;                                         //左孩子
  base_ptr  right;                                        //右孩子

  static base_ptr minimum(base_ptr x)
  {
    while (x->left != 0) x = x->left;
    return x;
  }

  static base_ptr maximum(base_ptr x)
  {
    while (x->right != 0) x = x->right;
    return x;
  }
};

template <typename Value>
struct __rb_tree_node : public __rb_tree_node_base
{
  typedef __rb_tree_node<Value>*  link_type;
  Value value_field;                                      //节点值
};

struct bidirectional_iterator_tag {};

struct __rb_tree_base_iterator
{
  typedef __rb_tree_node_base::base_ptr base_ptr;
  typedef bidirectional_iterator_tag    iterator_category;
  typedef ptrdiff_t                     difference_type;

  base_ptr node;                                          //与容器之间产生一个连结关系

  void increment()
  {
    if (node->right != 0)                                 //状况1：有右节点
    {
      node = node->right;
      while (node->left != 0)
        node = node->left;
    }
    else                                                  //状况2：没有右节点
    {
      base_ptr y = node->parent;
      while (node == y->right)                            //如果当前节点本身是右节点，就一直上溯，直到不为右子节点为止
      {
        node = y;
        y = y->parent;
      }
      if (node->right != y)                               //1、当前节点(node)的右子节点 等于   父节点(y)，node即为解答，则说明父节点(y)为根节点，当前节点(node)为header
        node = y;                                         //2、当前节点(node)的右子节点 不等于 父节点(y)，父节点(y)为解答
    }
  }

  void decrement()
  {
    if (node->color == __rb_tree_red &&                   //node为header
        node->parent->parent == node)
        node = node->right;
    else if (node->left != 0)                             //左节点的右子树，最右节点
    {
      base_ptr y = node->left;
      while (y->right != 0)
        y = y->right;
      node = y;
    }
    else                                                  //既不是根节点，也没有左子节点
    {
      base_ptr y = node->parent;
      while (node == y->left)
      {
        node = y;
        y = y->parent;
      }
      node = y;
    }
  }
};