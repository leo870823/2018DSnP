/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s);}
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const {  return _data[i]; }   
   Data& operator [] (size_t i) {  return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const {  return _data[0]; }

   void insert(const Data& d) 
   { _data.push_back(d);
     decrease_key(_data.size()-1,d); //傳入做後一個的id
       heapify(0);
   }

   void insert_n(const Data& d) 
   { _data.push_back(d);

   }
   
   void delData(size_t i) 
   { _data[i]=_data[_data.size()-1];
      _data.pop_back();
      decrease_key(i,_data[i]); //傳入做後一個的id
      heapify(i);
   }

   void delMin() { delData(0);}

   //helper fn
   void heapify(size_t n) //由上至下進行修正  
   {  size_t l=2*n+1,
             r=2*n+2,
             s;
    if(l<=_data.size()-1&&_data[l]<_data[n]) s=l; else s=n;//找出最小的index
    if(r<=_data.size()-1&&_data[r]<_data[s]) s=r; 
    if(s!=n) 
    {swap(_data[s],_data[n]);
    heapify(s);}  //交換玩的那傢伙，繼續往下搜

   }
   void build_min_heap()
   {  int n=_data.size()/2-1;  //note :應該要減ㄧ
      for(;n>=0;n--) heapify(n); }

   void decrease_key(size_t n,const Data& d)
   {  
       _data[n]=d;
       
     while(n>0&&  _data[n]<_data[(n-1)/2])//找他的parent
     { swap( _data[(n-1)/2],_data[n]);
       n=(n-1)/2;}
       heapify(n);
    //build_min_heap();
   }
   
private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
