// Edit by cyj 2016-12-19

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
  Value value_field;                                       //节点值
};