/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{  friend class  BSTree<T>;
   friend class  BSTree<T>::iterator;

public:
   T& getContent()
   {return _data;}
  

   BSTreeNode
   (const T& d,int count =0,BSTreeNode<T>* p=0,BSTreeNode<T>* l=0,BSTreeNode<T>* r=0):
   _data(d),
   _count(count),
   _parent(p),
   _left(l),
   _right(r){}
  int   count(){return  _count;}
  T     data(){return  _data;}

   
   int            _count;
   T               _data;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
   BSTreeNode<T>*  _parent;      
};


//int BSTree<T>::r_count = 0;
//int r_count = 0;
int r__count = 0;

template <class T>
class BSTree
{  friend class  iterator;
  //static BSTreeNode<T>* _dummy;
   public:
   BSTree()
  {  //一開始只有root,但root裡面沒有存東西
     _root= new BSTreeNode<T>(T());
     _dummy= new BSTreeNode<T>(T());
    _root->_right= _dummy;
    _dummy->_parent=_root;
     //r_count=0;
  }
  ~BSTree()
  { 
    clear();
    delete _root;
     delete _dummy;
  }
 //////////////////////////////////////  
//https://codereview.stackexchange.com/questions/153476/iterator-through-binary-trees-with-parent-pointer-without-memory-overhead

class iterator 
{    friend class BSTree;
public:
iterator(BSTreeNode<T>* n= 0, BSTree<T>* t=0, int r=0): _node(n),tree_fn(t),r_count(r) {}
iterator(const iterator& i) : _node(i._node),tree_fn(i.tree_fn),r_count(i.r_count){}
~iterator(){ } 
// Should NOT delete _node
 BSTreeNode<T>* node(){return _node;}


 const T& operator * () const { return _node->_data;} //copy一份傳出去,r-value
 T& operator * () { return _node->_data; }//l-value

iterator& operator ++ () 
{  
   if(_node->_count==1||_node->_count==0)
   _node= tree_fn->successor(_node);

   else  if(_node->_count>1)
  {  
     ++r_count;
     int debug=r_count;
     
     if( r_count==_node->_count)
     {
      _node=tree_fn->successor(_node);
      r_count=0;
   
     }
   
  }
    return *this;
}

iterator operator ++ (int) 
{   iterator tem=*this;
           this->operator++();
           return tem;
}
iterator& operator -- () 
{  if(_node->_count==1||_node->_count==0)
   _node=tree_fn->predecessor(_node);
  

   else if(_node->_count>1)
  {  //cerr<<"debug"<<r__count<<endl;
      r_count--;
     if(r_count==_node->_count*-1)
     {_node=tree_fn->predecessor(_node);
      r_count=0;
     }
  }
    return *this;
}

 iterator operator -- (int) 
{ 
           iterator tem=*this;
           this->operator--();
           return tem; 
}

bool operator == (const iterator& i) const 
{ 
      if(i._node==this->_node&&r_count==i.r_count) return true ;
      else return false; 
}
bool operator != (const iterator& i) const
{     
       if(i==*this) return false;
        else return true;
}
iterator& operator = (const iterator& i)
{      this->r_count=i.r_count;
       this->_node=i._node;
       return (*this); 
}

private:
 BSTreeNode<T>* _node;
 BSTree<T>* tree_fn;
// bool hold_b;
 int r_count;
};
/////////////////////////////////////////////////////////////////
void print(bool p=true,bool q=true)const{}
 iterator begin() const
 { if(_root->_count==0) return  end();

   BSTreeNode<T>* tem=_root;
    while(tem)
   {  if(tem->_left==NULL)
      //return tem; //it's min
      break;
      else
      tem=tem->_left;
   }
   iterator li(tem);
   return li;
   //最左邊那個
 }
 iterator end() const 
 {
    iterator li(_dummy);
   return li;
 }

 bool empty() const
 {  //記得考慮重複的
    if(_root->_left==NULL&&_root->_count==0)
   return true;
   else return false;
 }
size_t size() const 
{  
      if(empty()) return 0;
      else
      {  
         size_t SIZE =begin()._node->_count;
         BSTreeNode<T>* seed =begin()._node;
         while(seed!=_dummy)
         { seed=successor(seed);
          SIZE+=seed->_count;
         }
         return SIZE;
      }
}

void insert(const T& x) 
{ 
  //一開始是空ㄉ
  if(empty())
  { _root->_data=x;
    _root->_count=1;
   //  tree_max(_root)->_right=_dummy;
  }
  else
  { 
    BSTreeNode<T>* seed=NULL;
    BSTreeNode<T>* leaf=NULL;
    seed=_root;
    while(seed!=NULL&&seed!=_dummy)
    {   leaf=seed;
      if(x>seed->getContent()) //往右走
        seed=seed->_right;

      else if(x<seed->getContent()) //往左走
       seed=seed->_left;
    
      else if(x==seed->getContent()) //走到相同狀況
      {  seed->_count++;
         return;
      }
    }
    // 跳出迴圈後, x即為NULL
    // y即為insert_node的parent
    // 將insert_node的parent pointer指向y
     // 下面一組if-else, 把insert_node接上BST
     //不用考慮相等的狀況,前面處理掉了
   BSTreeNode<T>* hold=tree_max(_root);

   if(x>leaf->getContent())
   {
    leaf->_right =new BSTreeNode<T>(x,1,leaf);
      if( hold==leaf)
       { leaf->_right->_right=_dummy;    
         _dummy->_parent=leaf->_right;
       }
       else
       {
        //it's not your business
       }
   }
   else if (x<leaf->getContent())
        leaf->_left =new BSTreeNode<T>(x,1,leaf);
   //最後將最右邊接到dummy
  }
 
}
 void clear()
 { 
 if(empty()) return ;
      else
      {  //root被刪掉ㄌ
         BSTreeNode<T>* seed =begin()._node;
         while(seed!=_dummy)
         {  
            erase(seed);
            seed=successor(seed); 
         }
 
      }
   _root= new BSTreeNode<T>(T());
    _root->_right= _dummy;
    _dummy->_parent=_root;

 }
 void pop_front(){ erase(begin());}
 void pop_back(){ erase(tree_max(_root)); }
 bool erase(iterator pos) 
   {   //trick
    //if(pos._node==_dummy) return false;
       //
      if(empty())   return false; 
      //no need to check whether pos == end()).
      //count大於一
      if(pos._node->_count>1)
       {   --pos._node->_count; 
           return true;}

   BSTreeNode<T>* hold_max =tree_max(_root);
   BSTreeNode<T>* del_node=pos._node;
   BSTreeNode<T>* del_node_child=0;
   BSTreeNode<T>* hold =del_node; //原本要被刪掉ㄉ

//special case:root with only _dummy

if(del_node==_root&&_root->_left==NULL&&_root->_right==_dummy)
{
   del_node->_count=0;
    del_node->_right=_dummy;
    _dummy->_parent=del_node;
    return true;

}



   bool case3 =false;
   if(del_node->_left==NULL||del_node->_right==NULL||del_node->_right==_dummy)
   {//CASE:1,2
   }
   else
   {//CASE:3
    del_node=successor(del_node);
    case3=true;
   }

   if(del_node->_left!=NULL)
      del_node_child=del_node->_left;
   else
     del_node_child=del_node->_right;
 
   //case2,ptr handle//包含dummy parent的handle
   if(del_node_child!=NULL) 
   {
   del_node_child->_parent=del_node->_parent;
   }

   if(del_node->_parent==NULL)                    //special:handle root
     _root=del_node_child;
    else if(del_node==del_node->_parent->_left)  //在parent的左手邊
      del_node->_parent->_left=del_node_child;
    else                                   //在parent的右手邊
       del_node->_parent->_right=del_node_child;

   if(case3)
    {hold ->_data=del_node->_data;
     hold ->_count=del_node->_count;
    }
if(hold_max==del_node&&del_node->_parent==NULL) //root_case
{  if(del_node->_left==NULL) //root是最後一人QQ
   { 
    del_node->_count=0;
    del_node->_right=_dummy;
    _dummy->_parent=del_node;
    return true;
   }
   else      //root左邊還有人QQ
   { BSTreeNode<T>* trace=del_node->_left;
           BSTreeNode<T>* hold=0;
          while(trace)
          {  hold=trace;
            trace=trace->_right;
          }
         hold->_right=_dummy;
         _dummy->_parent=hold;
   }
}
else  if(hold_max==del_node) //在他parent的右手邊
//else  if(hold_max==del_node&&del_node->_parent->_right==del_node) //在他parent的右手邊
    {  if(del_node->_left!=NULL)//case2(half)
       {   BSTreeNode<T>* trace=del_node->_left;
           BSTreeNode<T>* hold_w=0;
          while(trace)
          {  hold_w=trace;
            trace=trace->_right;
          }
         hold_w->_right=_dummy;
       _dummy->_parent=hold_w;
       }
      else//case1,3,2(half)
       { del_node->_parent->_right=_dummy;
         _dummy->_parent=del_node->_parent;
       }
    }
   hold_max =tree_max(_root);
  // assert(hold_max->_right==_dummy);
  // assert(hold_max==_dummy->_parent);
    delete del_node;
    return true;

   }
   bool erase(const T& x)
    {  //空的set
       if(empty())
      return false; 
      //
       iterator li(begin());
      for(;li!=end();++li)
       if(*li==x)
        { erase(li);
         return true;
        }
            
        return false; //沒找到
   }

   iterator find(const T& x)
    {    iterator li(begin());
      for(;li!=end();++li)
       if(*li==x)
         return li;
        //沒找到
       return end(); 
   }


void sort(){}
///////////////////////////////////////////////////////////////////
///helper fn.
//////////////////////////////////////////////////////////////////
 BSTreeNode<T>* find_root(BSTreeNode<T>* tem) const
{ while(tem!=NULL)
   {  if(tem->_parent==NULL)
      return tem; //it's root
      else
      tem=tem->_parent;
   }
}
//從root出發
 BSTreeNode<T>* find_dummy( BSTreeNode<T>* tem) const
{ while(tem->_right) //走到dummy break
   {  
       {  //進來的話tem->_right絕對不會是NULL
         // if(tem->_right->_count==0) return tem->_right;
          tem=tem->_right;
       }
   }
   return tem;
}

 BSTreeNode<T>*  tree_max(BSTreeNode<T>* tem) const
{  tem=find_root(tem);
  BSTreeNode<T>* d=find_dummy(tem); // 把root餵進去
    while(tem)
   {  if(tem->_right==d)
      return tem; //it's max
      else
      tem=tem->_right;
   }
}
 BSTreeNode<T>*  tree_min(BSTreeNode<T>* tem) const
{ tem=find_root(tem);
 BSTreeNode<T>* d=find_dummy(tem); // 把root餵進去
    while(tem)
   {  if(tem->_left==d)
      return tem; //it's min
      else
      tem=tem->_left;
   }
}
BSTreeNode<T>* successor(BSTreeNode<T>* son ) const
{  //find_dummy要餵進去的是，root
   BSTreeNode<T>* d=find_dummy(find_root(son));
   //自己就是MAX
   //if(tree_max(son )==son) return son->_right;
   if(d==son->_right) 
   {//trick d->_parent=son;
    return son->_right;
   }
   //case 1
   if(son->_right!=NULL&&son->_right!=d)
   {  BSTreeNode<T>* trace=son->_right;
      while(trace) //trace will not be null at begin 
      { 
       if(trace->_left==NULL) 
       return trace; 
       else
       trace=trace->_left;
      }
   }
   //case 2 
   else
   {  BSTreeNode<T>* trace=son->_parent;
    //你的grand_parent會是你左邊的小孩
     while( trace!=NULL&&son==trace->_right)
     { son=trace;
      trace=trace->_parent;
     }
     return trace;
   }
}
BSTreeNode<T>* predecessor(BSTreeNode<T>* son) const
{  //for dummy case
   if(son->_count==0) return son->_parent;

   BSTreeNode<T>* d=find_dummy(find_root(son));
   //倒著走到最底，return dummy
    if(tree_min(son )==son) return d;
   //case 1
    if(son->_left!=NULL)
   {  BSTreeNode<T>* trace=son->_left;
      while(trace)  //trace will not be null at begin 
      { 
       if(trace->_right==NULL) 
       return trace; 
       else
       trace=trace->_right;
      }
  
   }
   else
   {  BSTreeNode<T>* trace=son->_parent;
    //你的grand_parent會是你右邊的小孩
     while( trace!=NULL&&son==trace->_left)
     { son=trace;
      trace=trace->_parent;
     }
     return trace;
   }
}
private:
 BSTreeNode<T>*  _root; 
 BSTreeNode<T>* _dummy;
//int r_count;
};

#endif // BST_H
