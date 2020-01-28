/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

#include <cstring>
template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//

template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() 
   { // _isSorted=false;
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() 
   { 
    clear();
    delete _head;
   }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data;} //copy一份傳出去,r-value
      T& operator * () { return _node->_data; }//l-value
      //
      //++v
      iterator& operator ++ () 
      { _node=_node->_next;
       return *(this); 
      }
      //v++
      iterator operator ++ (int) 
      {   iterator tem=*this;
           this->operator++();
           return tem;
      }
      //--v
      iterator& operator -- () 
      {  _node=_node->_prev;
          return *(this);
       }
      //v--
      iterator operator -- (int) 
      { 
           iterator tem=*this;
           this->operator--();
           return tem; 
      }

      iterator& operator = (const iterator& i)
       { 
          this-> _node= i._node;
       
       return *this; 
       }

      bool operator != (const iterator& i) const
      {
       if(i==*this) return false;
        else return true;
       }

      bool operator == (const iterator& i) const 
      { 
      if(i._node==this->_node) return true ;
      else return false; 
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const
   {  if(empty()) return _head;
       else return _head->_next;
   }
   iterator end() const 
   {  return _head;
     
   }
   bool empty() const
    {  if(_head==_head->_prev) //下一個和上一個都指向自己
       return true;
       else return false; 
    }
   size_t size() const 
   {  
      if(empty()) return 0;
      else
      {  
         size_t count =0;
         iterator li_b(begin()),li_e(end());
         for( ;li_b!=li_e;li_b++) count++;
         return count;
      }
      
    }
   //need to properly update _head, _tail
   // what happen if it has just one element or is empty?
   void push_back(const T& x) 
   { if(empty()) 
      {    
         _head->_prev=_head->_next=new DListNode<T>(x,_head,_head);
      
      }
      else
      { //non-empty 
        DListNode<T> * hold =_head->_prev;
       hold->_next=_head->_prev= new DListNode<T>(x,hold,_head);
      }
      //cout<<size()<<endl;
   }

   void pop_front()
    { 
   if(empty()) return;
   
    DListNode<T> * hold=_head->_next;
   _head->_next=hold->_next;
   hold->_next->_prev =_head;
    delete hold;
    }
   //head==NULL return error
   void pop_back()
    {
    if(empty()) return;

    DListNode<T> * hold=_head->_prev;
    _head->_prev=hold->_prev;
    hold->_prev->_next=_head;

      delete hold;

    }

   // return false if nothing to erase
   bool erase(iterator pos) 
   {   //空的set
       if(empty())
      return false; 
      //
      iterator li(begin());
      while(li!=pos)  li++;
      // return false if nothing to erase
      //no need to check whether pos == end()).
      //if(li==pos) return false;
      DListNode<T> * hold_n =li._node->_next;
      DListNode<T> * hold_p =li._node->_prev;
     delete li._node;
      hold_p->_next=hold_n ;
      hold_n->_prev=hold_p ;
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
    {   
      iterator li(begin());
      for(;li!=end();++li)
       if(*li==x)
         return li;
        //沒找到
       return end(); 
   }

   void clear()
    { 
      /*
       DListNode<T>*  thisnode=_head->_next;
      DListNode<T>*  nextnode=NULL;
      while( thisnode!=_head)
      {   
         nextnode=thisnode->_next;
         delete thisnode;
         thisnode=nextnode;
      }
      */
     iterator li_b(begin()), li_e(end());
     for(;li_b!=li_e; ++li_b) erase (li_b);
      
      //head 還沒刪掉

    }  // delete all nodes except for the dummy node
    //set _issorted
void sort() const
{
  if (empty()) return ;
  /*
  //bubble sort 
   iterator li_b(begin()), li_e(end());
      for(;li_b!=li_e;li_b++)
      {  
       for(iterator li_b_2(begin());li_e!=li_b_2;li_b_2++)
       { 
         //bubble sort 要換前面
         //不能換到最前面ㄉ
         //if(li_b_2!=li_b||li_b!=begin()) 
         if(li_b_2!=begin()) 
          if( string_test(li_b_2._node->_prev->_data,li_b_2._node->_data))
        // swap(li_b_2._node);
         {  T tem =li_b_2._node->_prev->_data;
            li_b_2._node->_prev->_data=li_b_2._node->_data;
            li_b_2._node->_data=tem;
         }
       }
     }
   */
   iterator li_b(begin()), li_e(end()),min;

   for(;li_b!=li_e;++li_b)
   { min=li_b;
     for( iterator li_b2(li_b);li_b2!=li_e;++li_b2)
     { //if(li_b2!=li_b) 
       if(min._node->_data>li_b2._node->_data)
       min=li_b2;
     }
     if (min!=li_b) swap(min._node->_data,li_b._node->_data);
   }

}
   

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   //bug在你head也換ㄌQQ
   void swap(T &first,T &second)const
   {
     T tem =first;
     first=second;
     second=tem;
   }
   void swap( DListNode<T>* & swap) const
   {  
      // b s_l s a  -> b s s_l a
      DListNode<T>* swap_l=swap->_prev;
      DListNode<T>* hold_a =swap->_next;
      DListNode<T>* hold_b =swap_l->_prev;
      //link next
      hold_b->_next=swap;
      swap->_next=swap_l;
      swap_l->_next=hold_a;
      //link prev
      hold_a->_prev=swap_l;
      swap_l->_prev=swap;
      swap->_prev=hold_b; 
    
   }
   //前大後小->swap
   //https://codescracker.com/cpp/program/cpp-program-sort-string.htm
   bool string_test(T first,T second) const
   { 
   if( first>second)
     return true;
     else return false;
   }
};

#endif // DLIST_H
