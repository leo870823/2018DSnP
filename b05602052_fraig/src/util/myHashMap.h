/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.
class CirGate;
class AIGateV;
class HashKey;
//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
/* 
 class HashKey
 {
 public:
    HashKey(CirGate* g) 
    {   assert(g->getFanin().size()==2);
        in0 =g->getFanin().at(0)->get_gateV();
        in1 =g->getFanin().at(1)->get_gateV();
        if(in0>in1) //0到1由小到大
        { size_t tem;
         tem=in1;
         in1=in0;
         in0=tem;
        }
    }
 
    size_t operator() () const { return in0+in1; }  // as hash function 
    bool operator == (const HashKey& k) const { return (k.in0==in0&&k.in1==in1); }
  private:
  size_t in0, in1;
 };
*/
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
   iterator(vector<HashNode>* b=NULL,size_t c=1,vector<HashNode>* begin=NULL,vector<HashNode>* end=NULL )
       :_node(b),_counter(c),_begin(begin),_end(end){}
      iterator(const iterator& i) 
      : _node(i._node),_counter(i._counter),_begin(i._begin) {}
       ~ iterator(){}//should not delete anything
    const HashNode& operator * () const 
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
    vector<HashNode>*     _begin;
    vector<HashNode>*     _end;
    vector<HashNode>*     _node;
    size_t            _counter; //index記得-1
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const
    { vector<HashNode>* tem=_buckets;
       while(tem->empty())  
       {  if(tem==_buckets+ _numBuckets) break;//end case
          tem++;
       }
       return iterator(tem,1,_buckets,_buckets+ _numBuckets); 
    }
   // Pass the end
   iterator end() const { return iterator(_buckets+_numBuckets,1,_buckets,_buckets+_numBuckets); }
   // return true if no valid data
   bool empty() const 
   { for(size_t i=0;i<_numBuckets;i++) if(!(_buckets[i].empty())) return false;
      return true; }
   // number of valid data
   size_t size() const
    { size_t s = 0;
   for(size_t i=0;i<_numBuckets;i++) s+=_buckets[i].size();
    return s; }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const
    { size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       if(x.first==k) return true;
      return false;
    }
//////////////////////////////////////////////
   bool m_check(const HashKey& k, HashData& m) //告束我要merge誰
    { size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       if(x.first==k) 
       {  m=x.second;
          return true;
       }
      return false;
    } 
/////////////////////////////////////////////
   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const
    {  size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       if(x.first==k) 
        {x.second=d;
         return true;
        }
      return false;
    }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) 
   {  size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       if(x.first==k) 
        {x.second=d;
         return true;
        }
      _buckets[i].push_back(HashNode(k,d));
      return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d)
    { size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       if(x.first==k) return false;
       _buckets[i].push_back(HashNode(k,d));
       return true;
    }
    //把inv case也加入考量
    //0:true 10:false 11:false+inv
   int insert_sim(const HashKey& k, const HashData& d)
    { size_t i=bucketNum(k);
      for(const auto&x:_buckets[i])
       {if(x.first==k) return 10;
          if((x.first() ^ k() )==~0) {return 11;}
       } 
       //考慮inv case，如果找不到找inv case
       _buckets[i].push_back(HashNode(k,d));
       return 0;
    }



   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k)
    {  size_t i=bucketNum(k);
      for(const auto&x:_buckets[i]) if(x.first==k) 
      { x=_buckets[_buckets.size()-1];
        _buckets.pop_back();
        return true;
      }
     return false;
    }
   //
   HashData&  get_data(const HashKey& k)
    {   size_t i=bucketNum(k);
         for( auto&x:_buckets[i]) if(x.first==k) 
         return x.second;
    }
private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;
   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }
   

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
