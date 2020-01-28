/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Turn this on for debugging
// #define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
MemMgr<T>* const T::_memMgr = new MemMgr<T>

#define USE_MEM_MGR(T)                                                      \
public:                                                                     \
   void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
   void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
   void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
   void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
   static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
   static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
   static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16 ,toSizeT(16) = 16
#define toSizeT(t) (t+SIZE_T-1)/SIZE_T*SIZE_T // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
//8會出問題
#define downtoSizeT(t)  (t/SIZE_T)*SIZE_T   // TODO

// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) 
   {
      _begin = _ptr = new char[b];
      _end = _begin + b; 
   } 
   ~MemBlock() { delete [] _begin; }

   // Member functions
   void reset() { _ptr = _begin; }
   // 1. Get (at least) 't' bytes memory from current block
   //    Promote 't' to a multiple of SIZE_T
   // 2. Update "_ptr" accordingly
   // 3. The return memory address is stored in "ret"
   // 4. Return false if not enough memory
   //讓memory block合法
   bool getMem(size_t t, T* & ret)
    {  // TODO
            #ifdef MEM_DEBUG
            cerr << "in getmem"<<"t "<<t<<"remainsize"<<getRemainSize()<<endl;
            #endif // MEM_DEBUG
         t=toSizeT(t);
       if(t>getRemainSize())
       {    
            #ifdef MEM_DEBUG
            cerr << "get_fail:"<<getRemainSize()<<endl;
            #endif // MEM_DEBUG

         return false;
       }
       else
       { 
         ret=reinterpret_cast<T*>(_ptr);
         _ptr+=t;
         return true;
       }
      
   }
   size_t getRemainSize() const { return size_t(_end - _ptr); }
      
   MemBlock<T>* getNextBlock() const { return _nextBlock; }

   // Data members
   char*             _begin;
   char*             _ptr;
   char*             _end;
   MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
   ~MemRecycleList() 
   
   { reset(); }

   // Member functions
   // ----------------
   size_t getArrSize() const
    { return _arrSize; }
   MemRecycleList<T>* getNextList() const 
   { return _nextList; }
   void setNextList(MemRecycleList<T>* l) 
   { _nextList = l; }
   // pop out the first element in the recycle list
   T* popFront() 
   {   
       // TODO
       T* tem = _first;
       size_t number =*reinterpret_cast<size_t*>(_first);
       _first=reinterpret_cast<T*>(number);
      return tem; //return 原本指的ptr

   }
   // push the element 'p' to the beginning of the recycle list
   // case:first initial ==0
   void  pushFront(T* p) 
   {  
        size_t tem=reinterpret_cast<size_t>(_first);
        *reinterpret_cast<size_t*>(p)=tem;
        _first=p;
      // TODO
   }
   // Release the memory occupied by the recycle list(s)
   // DO NOT release the memory occupied by MemMgr/MemBlock
   void reset() 
   { 
      //recursively call
      if(_nextList) 
      {
      delete _nextList;
      _nextList=0;
      }
      _first=0;
      //////////////////////note
   }

   // Helper functions
   // ----------------
   // count the number of elements in the recycle list
   size_t numElm() const 
   { 
     // TODO
      size_t  num=0;
      T* buf = _first;
      //buf point to NULL break 
      while(buf)
      {  
        num++;
        size_t number =*reinterpret_cast<size_t*>(buf);
        buf=reinterpret_cast<T*>(number);
        //note:buf只的東西可能不是null
        
      }
      return num;
 

   }

   // Data members
   size_t              _arrSize;   // the array size of the recycled data
   T*                  _first;    // the first recycled data
   MemRecycleList<T>*  _nextList;  // next MemRecycleList
                                   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
   #define S sizeof(T)

public:
   MemMgr(size_t b = 65536) : _blockSize(b)
    {
      assert(b % SIZE_T == 0);
      _activeBlock = new MemBlock<T>(0, _blockSize);
      for (int i = 0; i < R_SIZE; ++i)
         _recycleList[i]._arrSize = i;
   }
   ~MemMgr() {
    reset(); 
   delete _activeBlock; }

   // 1. Remove the memory of all but the firstly allocated MemBlocks
   //    That is, the last MemBlock searchd from _activeBlock.
   //    reset its _ptr = _begin (by calling MemBlock::reset())
   // 2. reset _recycleList[]
   // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
   //    if (b != _blockSize) reallocate the memory for the first MemBlock
   // 4. Update the _activeBlock pointer
   
   void reset(size_t b = 0) 
   {
      assert(b % SIZE_T == 0);
      #ifdef MEM_DEBUG
      cout << "Resetting memMgr...(" << b << ")" << endl;
      #endif // MEM_DEBUG
      //TODO
      // 1.
      /*
      MemBlock<T>* next= _activeBlock->getNextBlock();
      MemBlock<T>* tem;
      while(next)
      {   tem =next->getNextBlock();
        delete next;  
          next=tem;
          //當next null break
      }
      */
     // 1.
      MemBlock<T>* next= _activeBlock;
      MemBlock<T>* tem;
       while(next)
       {  tem=next->getNextBlock();
         if(tem!=NULL)
         delete next;  
         else break;
           next=tem;
       }

       _activeBlock=next;
       _activeBlock->reset();


      // 2.
       for (int i = 0; i < R_SIZE; i++)
         _recycleList[i].reset();
       // 3.
       if(b==0||b==_blockSize)
        _activeBlock->reset();
       else  if (b != _blockSize)
       {
         delete _activeBlock;
         //用new
         _blockSize=b;
         _activeBlock= new MemBlock<T>(0,b);
       }


   }
   // Called by new
   T* alloc(size_t t) {
      assert(t == S);
      #ifdef MEM_DEBUG
      cout << "Calling alloc...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      return getMem(t);
   }
   // Called by new[]
   T* allocArr(size_t t) {
      #ifdef MEM_DEBUG
      cout << "Calling allocArr...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // Note: no need to record the size of the array == > system will do
      return getMem(t);
   }
   // Called by delete
   void  free(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling free...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      getMemRecycleList(0)->pushFront(p);
   }
   // Called by delete[]
   void  freeArr(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling freeArr...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      size_t n =*reinterpret_cast<size_t*>(p);
      //getMemRecycleList(n)->pushFront(p);
   
      // Get the array size 'n' stored by system,
      // which is also the _recycleList index
      
      #ifdef MEM_DEBUG
      cout << ">> Array size = " << n << endl;
      cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
      #endif // MEM_DEBUG
      // add to recycle list...
      getMemRecycleList(n)->pushFront(p);
   }
   void print() const {
      cout << "=========================================" << endl
           << "=              Memory Manager           =" << endl
           << "=========================================" << endl
           << "* Block size            : " << _blockSize << " Bytes" << endl
           << "* Number of blocks      : " << getNumBlocks() << endl
           << "* Free mem in last block: " << _activeBlock->getRemainSize()
           << endl
           << "* Recycle list          : " << endl;
      int i = 0, count = 0;
      while (i < R_SIZE) {
         const MemRecycleList<T>* ll = &(_recycleList[i]);
         while (ll != 0) {
            size_t s = ll->numElm();
            if (s) {
               cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                    << setw(10) << left << s;
               if (++count % 4 == 0) cout << endl;
            }
            ll = ll->_nextList;
         }
         ++i;
      }
      cout << endl;
   }

private:
   size_t                     _blockSize;
   MemBlock<T>*               _activeBlock;
   MemRecycleList<T>          _recycleList[R_SIZE];

   // Private member functions
   // size轉換成array
   // t: #Bytes; MUST be a multiple of SIZE_T
   // return the size of the array with respect to memory size t
   // [Note] t must >= S
   // [NOTE] Use this function in (at least) getMem() to get the size of array
   //        and call getMemRecycleList() later to get the index for
   //        the _recycleList[]
   size_t getArraySize(size_t t) const {
      // t: #Bytes; MUST be a multiple of SIZE_T
     
      assert(t % SIZE_T == 0);
      assert(t >= S);

      return (t-SIZE_T)/S;

      // TODO
  
   }
   // Go through _recycleList[m], its _nextList, and _nexList->_nextList, etc,
   //    to find a recycle list whose "_arrSize" == "n"
   // If not found, create a new MemRecycleList with _arrSize = n
   //    and add to the last MemRecycleList
   // So, should never return NULL
   // [Note]: This function will be called by MemMgr->getMem() to get the
   //         recycle list. Therefore, the recycle list is first created
   //         by the MTNew command, not MTDelete.
   MemRecycleList<T>* getMemRecycleList(size_t n) 
   {   // TODO
      size_t m = n % R_SIZE;
      //第一個是的狀況，recycle list的東西直接還回去
       if(m==n) return  &_recycleList[m];

      //第一個不是的狀況
      MemRecycleList<T> * buf = _recycleList[m].getNextList();
      MemRecycleList<T> * tem=NULL;
      MemRecycleList<T> *  finallist =NULL;
      while(buf)
      { 
        if(n==buf->getArrSize()) 
        {
         tem=buf;
         break;
        }
        //存著buff被蓋掉之前指的
        finallist=buf;
        buf=buf->getNextList();
      }

      //沒找到東西，需要創新的list
      if(tem==NULL) 
      {    
        tem = new  MemRecycleList<T>(n) ;
        
        //第一個之後有無東西
        if(finallist)
        finallist->setNextList(tem);
        else 
        _recycleList[m].setNextList(tem);
      }
    // So, should never return NULL
      assert(!(tem==NULL));
      return tem;
      

     
  
   }
   // t is the #Bytes requested from new or new[]
   // Note: Make sure the returned memory is a multiple of SIZE_T
   //把記憶體還給系統
   T* getMem(size_t t) 
{
      T* ret = 0;
      #ifdef MEM_DEBUG
      cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // 1. Make sure to promote t to a multiple of SIZE_T
      t = toSizeT(t);
      // 2. Check if the requested memory is greater than the block size.
      //    If so, throw a "bad_alloc()" exception.
      //    Print this message for exception
      //    cerr << "Requested memory (" << t << ") is greater than block size"
      //         << "(" << _blockSize << "). " << "Exception raised...\n";
      // TODO
  
       if(t> _blockSize) 
        { cerr << "Requested memory (" << t << ") is greater than block size"
             << "(" << _blockSize << "). " << "Exception raised...\n";
      
        throw bad_alloc();
        }
      // [NOTE] Use this function in (at least) getMem() to get the size of array
      //     and call getMemRecycleList() later to get the index for
      //     the _recycleList[]
      // 3. Check the _recycleList first...
      //    Print this message for memTest.debug
      //    #ifdef MEM_DEBUG
      //    cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
      //    #endif // MEM_DEBUG
      //    => 'n' is the size of array
      //    => "ret" is the return address
      
      
     
       
      // If no match from recycle list...
      // 4. Get the memory from _activeBlock
      // 5. If not enough, recycle the remained memory and print out ---
      //    Note: recycle to the as biggest array index as possible
      //    Note: rn is the array size
      //    Print this message for memTest.debug
      //    #ifdef MEM_DEBUG
      //    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
      //    #endif // MEM_DEBUG
      //    ==> allocate a new memory block, and print out ---
      //    #ifdef MEM_DEBUG
      //    cout << "New MemBlock... " << _activeBlock << endl;
      //    #endif // MEM_DEBUG
      // TODO
      size_t n = getArraySize(t);
      // TODO
        ret=(getMemRecycleList(n)->_first);
      if(ret)
      {  //若剛好有可以recycle的
         ret=getMemRecycleList(n)->popFront(); 
         #ifdef MEM_DEBUG
          cerr << "Recycled from _recycleList[" << n << "]..." << ret << endl;
         #endif // MEM_DEBUG
      
      }
      else
      {  //ret近來是null
        if(_activeBlock->getMem(t,ret))
        { // 4. Get the memory from _activeBlock
        
            #ifdef MEM_DEBUG
            cerr << "get_succed from activeblock "<< _activeBlock->getRemainSize()<<endl;
            cerr << "get from activeblock "<<t <<endl;
            #endif // MEM_DEBUG
            
        }
        else
        { // 5. If not enough, recycle the remained memory and print out ---
          ///////////////////////////////////////////////////////////////////////////////////////////
          size_t rn = _activeBlock->getRemainSize(); //單位是byte

          if(rn>=S)
          {  rn = getArraySize(rn);
             if(rn)    //rn>0 ->can be recycled as array 
             { _activeBlock->getMem(rn*S+SIZE_T,ret);
               getMemRecycleList(rn)->pushFront(ret);
             }

             else   //rn==0 only ->can be recycled as object
             {  _activeBlock->getMem(S,ret);
               getMemRecycleList(rn)->pushFront(ret);

             }
            #ifdef MEM_DEBUG
            cerr << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
            #endif // MEM_DEBUG
            //   ==> allocate a new memory block, and print out 

          }

            _activeBlock =new MemBlock<T>(_activeBlock,_blockSize);
            _activeBlock->getMem(t,ret);

            #ifdef MEM_DEBUG
             cerr << "New MemBlock... " << _activeBlock << endl;
            #endif // MEM_DEBUG
           
        }
        


      }

      // 6. At the end, print out the acquired memory address
      #ifdef MEM_DEBUG
      cerr << "Memory acquired... " << ret << endl;
      #endif // MEM_DEBUG

      return ret;
}
   // Get the currently allocated number of MemBlock's
   size_t getNumBlocks() const
    { // TODO
      size_t  num=0;
      MemBlock<T>*  buf= _activeBlock;
      while(buf)
      {  
         num++;
         buf=buf->getNextBlock();
         #ifdef MEM_DEBUG
         cerr <<  num << endl;
         #endif // MEM_DEBUG
      }
      return num;
      
   }

};

#endif // MEM_MGR_H
