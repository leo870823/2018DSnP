/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{  CirGate* mer=NULL;
   HashMap<class HashKey, class CirGate*> h(_VisitList.size());
  for( auto&x:_VisitList)
  { if(x->get_name()=="PI"||x->get_name()=="PO"||x->get_name()=="UNDEF"||x->get_name()=="CONST") {}
    else
    { HashKey k(x);
      if(h.m_check(k,mer)) 
      {  mer->struct_merge(x);
        for( auto&y:_GateList)
        {  if(y==x)
          { delete y;
            y=_GateList[_GateList.size()-1];
           _GateList.pop_back(); } //break;
        }
      }
      else  assert(h.insert(k,x));
    }
  }
   DFST();
   num_gate=num_of_non_un(); //gate數量要換
 // num_gate=_GateList.size();
}
//remark:1.思考啥時開始merge
void
CirMgr::fraig()
{  if(_fecGrps.empty()) return; //不用return 
   //initialization
   SatSolver solver;
   solver.initialize();
   //Create CNF for the circuit
   genProofModel(solver);
   //開證
   for( auto&x:_fecGrps)
   {   bool result=false;
      for(int i=0;i<x.size();i++)
      { for(int j=i+1;j<x.size();j++)
        {  Var newV = solver.newVar();
           solver.assumeRelease();  // Clear assumptions
           if(x[i]->get_name()=="CONST"||x[j]->get_name()=="CONST") 
           {solver.assumeProperty(x[j]->getVar(),true);  //0和gate證
            newV=x[j]->getVar();
           }
           else
           {solver.addXorCNF(newV,x[i]->getVar(),false,x[j]->getVar(),false);
           solver.assumeProperty(newV,!(x[i]->get_fec_b()^x[j]->get_fec_b()) ); //
           }
           //
           result = solver.assumpSolve(); //For incremental proof
           if(result)
             {_patternList.push_back(solver.getValue( newV)); //搜集pattern
                

             }
           else
           //找到一定數目的 UNSAT 之後再merge
          { ////////////////////////////////////////////
            cout<<"Fraig: ";
            if(x[i]->get_fec_b()) cout<<"!";
            cout<<x[i]->getID()<<" merging ";
            if(x[j]->get_fec_b()) cout<<"!";
             cout<<x[j]->getID()<<"..."<<endl;
            ////////////////////////////////////////////
            x[i]->struct_merge(x[j]); //暫時會照成fl gate但沒差，最後記得sweep掉ㄛ
            find_delete_gl(x[j]);
            x.erase(x.begin()+j); //更動fecgrp
            j--;//把物件往前移
          } //merge                 
        }
        //刪到只剩一個
        if(x.size()==1) { x.pop_back(); break;}//把fecgrp的砍光
      }



   }

   //掃掉floating gate
   //this->sweep();
     DFST();
     num_gate=num_of_non_un(); //gate數量要換
   //merge 完後fecgrp跑掉QWQ
    _fecGrps.clear();
    initFEC();
   //gate_list也會跑掉
   //solver reset?????????????
   //solver.reset();
   cout<<"Total #FEC Group = "<<_fecGrps.size()<<endl;
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void
CirMgr::genProofModel(SatSolver& s)
{  // Allocate and record variables; No Var ID for POs only for pi
    CONST0->setVar(0);
    for(auto &x:_piList) 
   { Var v = s.newVar();
     x->setVar(v);
   }
   for(auto &x:_GateList) 
   { Var v = s.newVar();
     x->setVar(v);
   }
   for(auto &x:_poList) 
    x->setVar(x->getFanin().at(0)->gate()->getVar());
   
  
   //////////////////////////////////
   for(auto &x:_GateList) 
   {  s.addAigCNF(x->getVar(),
      x->getFanin().at(0)->gate()->getVar(),x->getFanin().at(0)->isInv(),
      x->getFanin().at(1)->gate()->getVar(),x->getFanin().at(1)->isInv());
   }
}

void
CirMgr::find_delete_gl(CirGate* g)
{for( auto&y:_GateList) if(y==g)
            {
              y=_GateList[_GateList.size()-1];
             _GateList.pop_back(); } 
}