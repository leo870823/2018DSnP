/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "util.h"
#include "cirDef.h"
#include "cirGate.h"
#include <algorithm>
using namespace std;

class AIGateV;
typedef pair<unsigned,CirGate*> Cirpair;

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr()  
   {
     max_id=num_pi=num_po=num_latch=num_gate=0;
      CONST0 =new Const();
     cirMAP.insert(Cirpair(0,CONST0));
   }
   ~CirMgr() 
   { for(const auto&x:_piList) delete x;
     for(const auto&x:_poList) delete x;
     for(const auto&x:_GateList) delete x;
     delete CONST0 ;
   }
/*
void   clear()
  {
 cirMAP.clear();
 _piList.clear();
 _poList.clear();
 _GateList.clear();
 _traversalList.clear();
 _VisitList.clear();
 max_id=num_pi=num_po=num_latch=num_gate=0;
  }
  */



   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   vector< CirGate *> & get_piL() {return _piList; }
   vector< CirGate *> & get_poL() {return _poList; }
   vector< CirGate *> & get_gaL() {return _GateList; }
   CirGate* getGate(unsigned gid) const 
   { CirGate* tem =NULL;
        for(const auto&x:cirMAP)
          { if(x.first==gid ) //看有沒有等於
             { tem=x.second;
               break;
             }
          }
     return tem; }
   // Member functions about circuit construction
   bool readCircuit(const string&);
   CirGate* find_match_ptr( unsigned ID ) ;
   void cir_sort(vector< CirGate *> &);
   void prepare_traversallist  (vector< CirGate *>,vector< CirGate *>,vector< CirGate *>);
   void DFST() ;
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void debug() const;


private:
unordered_map<unsigned,CirGate*> cirMAP;
vector< CirGate *> _piList;
vector< CirGate *> _poList;
vector< CirGate *> _GateList;
vector< CirGate *> _traversalList;
vector< CirGate *> _VisitList;
CirGate*   CONST0;
unsigned max_id;
unsigned num_pi;
unsigned num_po;
unsigned num_latch;
unsigned num_gate;
};

#endif // CIR_MGR_H
