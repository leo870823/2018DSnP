/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
#include <algorithm>
using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
friend class iterator;
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }
    size_t getnumbucket()const {return  _numBuckets;}
   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
       iterator(vector<Data>* b=NULL,size_t c=1,vector<Data>* begin=NULL,vector<Data>* end=NULL )
       :_node(b),_counter(c),_begin(begin),_end(end){}
      iterator(const iterator& i) 
      : _node(i._node),_counter(i._counter),_begin(i._begin) {}
       ~ iterator(){}//should not delete anything
 
      const Data& operator * () const 
      { assert(!_node->empty());
      return  _node->at(_counter-1);
      }
      //++v
      iterator& operator ++ ()
       {  ++_counter; 
          if(_counter> _node->size())
          {  
             do{++_node;}
             while(_node!=_end&&_node->empty()) ;
           /*  if(_node->empty()){ ++_node;    assert(_node==end());  }//移到 end()
             else  _counter=1; */
               _counter=1;
          }
           return *(this);
       }
       //v++
         iterator operator ++ (int) 
      {  iterator tem= *(this);
         this->operator++();
          return tem;
      }
         //v--
         iterator operator -- (int) 
      {  iterator tem= *(this);
         this->operator--();
          return tem;
      }
      //--v
      iterator& operator -- ()
       {  --_counter; 
          if(_counter<=0)
          {  
             do{--_node;}
             while(_node!=_begin&_node->empty()) ;
             _counter=1;
          }
           return *(this);
       }
       //note
      bool operator == (const iterator& i) const { return i._counter==_counter&&i._node==_node; } 
      bool operator != (const iterator& i) const { return !operator==(i); }
      iterator& operator = (const iterator& i)
      {   _node=i._node;
          _counter=i._counter;
          _begin=i._begin;
          _end=i._end;
      }
     size_t  operator()()
     {
        return bucketNum(_node->at(_counter-1));
     }

   private:
    vector<Data>*     _begin;
    vector<Data>*     _end;
    vector<Data>*     _node;
    size_t            _counter; //index記得-1
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   //Q:如果找不到原宿怎麼辦QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
   iterator begin() const
    {   vector<Data>* tem=_buckets;
       while(tem->empty())  
       {  if(tem==_buckets+ _numBuckets) break;
          tem++;
       }
       return iterator(tem,1,_buckets,_buckets+ _numBuckets); 
    
    }
   // Pass the end
   iterator end() const { return iterator(_buckets+_numBuckets,1,_buckets,_buckets+_numBuckets); }//最後一個的下一個
   // return true if no valid data
   bool empty() const 
   {  for(size_t i=0;i<_numBuckets;i++) if(!(_buckets[i].empty())) return false;
      return true;
   }
   // number of valid data
   size_t size() const
 { size_t s = 0;
   for(size_t i=0;i<_numBuckets;i++) s+=_buckets[i].size();
    return s; }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
   {  
      size_t i=bucketNum(d);
      typename std::vector<Data>::iterator it=find(_buckets[i].begin(),_buckets[i].end(),d);
      if(it!=_buckets[i].end()) return true;
      
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const 
   {    
      size_t i=bucketNum(d);
      typename std::vector<Data>::iterator it=find(_buckets[i].begin(),_buckets[i].end(),d);
      if(it!=_buckets[i].end()) //有找到
       {  
         size_t hold=distance(_buckets[i].begin(), it);
         _buckets[i].at(hold)=d;
         return true;
       }
      
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d)
    { if(check(d))
      {  
        size_t index=find(_buckets[bucketNum(d)].begin(),_buckets[bucketNum(d)].end(),d)-_buckets[bucketNum(d)].begin();
        _buckets[bucketNum(d)].at(index) =d;
          return true;
      }
      else
      {  _buckets[bucketNum(d)].push_back(d);
         return false;
      }
      
     }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d)
    {  if(check(d)) return false;
       _buckets[bucketNum(d)].push_back(d);
      return true; }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
   { if(!check(d)) return false;
       size_t n =bucketNum(d);
       typename std::vector<Data>::iterator it=find(_buckets[n].begin(),_buckets[n].end(),d);
      //_buckets[bucketNum(d)].erase(it);
      size_t i =it-_buckets[n].begin();
      _buckets[n].at(i)=_buckets[n].at(_buckets[n].size()-1);
      _buckets[n].pop_back();
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
