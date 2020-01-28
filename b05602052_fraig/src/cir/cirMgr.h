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
typedef vector<CirGate*>         cirlist;
extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr()  
   {
     max_id=num_pi=num_po=num_latch=num_gate=0;
      CONST0 =new Const();
      //_simLog =NULL;
     cirMAP.insert(Cirpair(0,CONST0));
   }
   ~CirMgr() 
   { for(const auto&x:_piList) delete x;
     for(const auto&x:_poList) delete x;
     for(const auto&x:_GateList) delete x;
     delete CONST0 ;
    // if(_simLog) delete _simLog;
   }

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

   // Member functions about circuit optimization
   void sweep();
   void optimize();
   void delete_redundant(CirGate*) ;
   void print_s(CirGate *)  const;
   void merge(CirGate*);
   void find_delete_gl(CirGate* g);//not delete
   
   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   void initFEC();
   void checkFEC();
   void initialize_FEC();
   void check_base();
   void check_add();
   void fecgrpsort();
   vector<cirlist> & getfecGrps(){return _fecGrps;}


   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();
   void genProofModel(SatSolver& s);
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
   void printFECPairs() ;//const
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   void debug() const;
   unsigned num_of_non_un() const
   { unsigned hold=0;
     for(const auto&x:_GateList) 
      if(x->getTypeStr()!="undefined") hold++;
      return hold;
   }

private:
ofstream           *_simLog;
unordered_map<unsigned,CirGate*> cirMAP;
vector< CirGate *> _piList;
vector< CirGate *> _poList;
vector< CirGate *> _GateList;
vector< CirGate *> _traversalList;
vector< CirGate *> _VisitList;
vector<cirlist>     _fecGrps; //typedef vector<CirGate*>         cirlist;
vector<int>        _patternList;

CirGate*   CONST0;
unsigned max_id;
unsigned num_pi;
unsigned num_po;
unsigned num_latch;
unsigned num_gate;




};

#endif // CIR_MGR_H

