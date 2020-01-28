/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ ()
       {  _node++;
          return (*this); 
        }//++v
      iterator operator ++ (int)
       {    iterator tem= (*this);
            this->operator++();
           return tem;
        }//v++
      iterator& operator -- ()
       {   _node--;
           return (*this);
       }//--v
      iterator operator -- (int)
       { iterator tem= (*this);
            this->operator--();
           return tem;
       }//v--

      iterator operator + (int i) const
       { iterator tem(this->_node+i);
         return tem; 
       }
      iterator& operator += (int i)
       {  _node =_node+i;
           return (*this); 
       }

      iterator& operator = (const iterator& i) 
      {
       this->_node=i._node;
       return (*this); 
      }

      bool operator != (const iterator& i) const
       { 
          if(i==*this) return false ;
          else return true; 
       }
      bool operator == (const iterator& i) const
       { 
          if(i._node==this->_node) return true;
           return false; 
       }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   /*
    For class Array, end() points to the next address of the last element.
    If the Array is NOT yet initialized (i.e. _capacity == 0), both begin() and end() = 0. 
    */
   iterator begin() const 
   { if(_capacity==0) return 0;
      else return _data; }
   iterator end() const 
   {  if(_capacity==0) return 0;
      else return _data+_size; }

   bool empty() const
    {  if( _size==0) return true;
       else return false; }
   size_t size() const
    { return _size; }

   T& operator [] (size_t i) { return *_data+i; }
   const T& operator [] (size_t i) const { return *_data+i; }

   void push_back(const T& x) 
   { //cap ==0
     //cap >=size+1
     //cap<size+1
     
     if(_capacity==0)
     {  _capacity++;
        _data=new T[_capacity];
        _size++;
      _data[0]=x;

     }else if (_capacity>_size)
     {   _data[_size++]=x;
         
     }
     else if(_capacity<=_size)
     {
       _capacity*=2;
      T* tem=new T[_capacity];
      for(int i=0;i<_size;i++)
        tem[i]=_data[i];
        delete []_data;
         _data=tem;
      //把新加的那一個填進去
       _data[_size]=x;
         _size++;
     }
    
   }
   void pop_front()
    { if(empty()) return;
      _data[0]=_data[_size-1];
      // _data[_size-1]=0;
        _size--;
    }
   void pop_back()
    {  //_data[_size-1]=0;
        _size--;
    }

   bool erase(iterator pos) 
   { //空的set
       if(empty())
      return false; 

      iterator li(begin());
      size_t index=0;
      while(li!=pos) 
      { li++;
        index++;
      }
       //no need to check whether pos == end()).
      //if(li==pos) return false;
         _data[index]=_data[_size-1];
         _size--;
      return true;
   }
   bool erase(const T& x)
    { //空的set
       if(empty())
      return false; 

      iterator li(begin());
      for(;li!=end();li++)
       if(*li==x)
        {  //若是第一個ele
           //if(li==begin())
           //pop_front();
           //else 
           erase(li); //size 也已經減ㄌ
         return true;
        }
        return false; //沒找到
      
    }

   iterator find(const T& x) 
   { 
      iterator li(begin());
      for(;li!=end();li++)
       if(*li==x)
         return li;
        //沒找到
       return end();
   }

   void clear() 
   {  iterator li_b(begin()), li_e(end());
     for(;li_b!=li_e; li_b++) erase (li_b);
      _size=0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }
   //
   void up_capacity()
   { size_t n_tem =1;
    while(n_tem>=_size+1)
     n_tem*=2;
     _capacity=n_tem;
   }
private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
