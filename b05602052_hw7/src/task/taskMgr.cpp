/****************************************************************************
  FileName     [ taskMgr.cpp ]
  PackageName  [ task ]
  Synopsis     [ Define member functions for task Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <string>
#include <cassert>
#include "taskMgr.h"
#include "rnGen.h"
#include "util.h"

using namespace std;

TaskMgr *taskMgr = 0;

// BEGIN: DO NOT CHANGE THIS PART
TaskNode::TaskNode() 
{
   _name.resize(NAME_LEN);
   for (int i = 0; i < NAME_LEN; ++i)
      _name[i] = 'a' + rnGen(26);
   _load = rnGen(LOAD_RN);
}

size_t
TaskNode::operator () () const 
{
   size_t k = 0, n = (_name.length() <= 5)? _name.length(): 5;
   for (size_t i = 0; i < n; ++i)
      k ^= (_name[i] << (i*6));
   return k;
}

ostream& operator << (ostream& os, const TaskNode& n)
{
   return os << "(" << n._name << ", " << n._load << ")";
}

TaskMgr::TaskMgr(size_t nMachines)
: _taskHeap(nMachines), _taskHash(getHashSize(nMachines)) { }

void
TaskMgr::clear()
{
   for (size_t i = 0, n = size(); i < n; ++i)
      cout << "Task node removed: " << _taskHeap[i] << endl;
   _taskHeap.clear(); _taskHash.clear();
}

void
TaskMgr::remove(size_t nMachines)
{        
   for (size_t i = 0, n = nMachines; i < n; ++i) {
      size_t j = rnGen(size());
      assert(_taskHash.remove(_taskHeap[j]));
      cout << "Task node removed: " << _taskHeap[j] << endl;
      _taskHeap.delData(j);
   }
}

// return true if TaskNode is successfully removed
// return false if no such node exists
bool
TaskMgr::remove(const string& s)
{
   TaskNode n(s, 0);
   if (!_taskHash.remove(n)) return false;
   for (size_t i = 0, m = size(); i < m; ++i)
      if (_taskHeap[i] == n) {
         cout << "Task node removed: " << _taskHeap[i] << endl;
         _taskHeap.delData(i);
         break;
      }
   return true;
}
// END: DO NOT CHANGE THIS PART

// TODO..
// Exactly nMachines (nodes) will be added to Hash and Heap
// Note: a new task node can be created by the default constructor
//       i.e. TaskNode newNode;
void
TaskMgr::add(size_t nMachines)
{    
    for (size_t i = 0, n = nMachines; i < n; ++i)
    {   TaskNode _newnode;
       if( _taskHash.insert(_newnode))
       { _taskHeap.insert(_newnode);
        cout << "Task node inserted: " << _newnode << endl;
       }
       else n++;
  
    }
   
   
} 
// TODO...
// return true if TaskNode is successfully inserted
// return false if equivalent node has already existed
bool
TaskMgr::add(const string& s, size_t l)
{   TaskNode _newnode(s,l);
   if( _taskHash.check(_newnode)) return false; //已經有ㄌ
    _taskHeap.insert(_newnode);
    _taskHash.insert(_newnode);
     cout << "Task node inserted: " << _newnode << endl;
   assert(_taskHash.size()==_taskHeap.size());
   return true;
}
 // TODO...
// Assign the min task node with 'l' extra load.
// That is, the load of the min node will be increased by 'l'.
// The min node in the heap should be updated accordingly.
// The corresponding node in the hash should be updated, too.
// return false if taskMgr is empty
// otherwise, return true.
bool
TaskMgr::assign(size_t l)
{ 
   if(_taskHash.empty()) return false;
   TaskNode _newnode(_taskHeap.min().getName(),_taskHeap.min().getLoad()+l);//更新min load
   _taskHeap.delMin();
   _taskHeap.insert(_newnode);
   //_taskHeap.decrease_key(0, _newnode);
   _taskHash.update(_newnode);
   return true;
}
//  cout<<(*hi)()%_taskHash.getnumbucket()<<endl;
// WARNING: DO NOT CHANGE THESE TWO FUNCTIONS!!
void
TaskMgr::printAllHash() const 
{
   HashSet<TaskNode>::iterator hi = _taskHash.begin();
   for (; hi != _taskHash.end(); ++hi)
      cout << *hi << endl;
}

void
TaskMgr::printAllHeap() const
{
   for (size_t i = 0, n = size(); i < n; ++i)
      cout << _taskHeap[i] << endl;
}
