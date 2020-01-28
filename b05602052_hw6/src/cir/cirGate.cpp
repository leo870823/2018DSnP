/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;


void CirGate::traversal(vector< CirGate *> & dfsList)
{ if(getFanin().size()==0) 
   { //base case
   set_GRef(); 
   dfsList.push_back(this);
   return;
   }//撞到 po/floating
 //fanout由大到小排好
// if(getFanin().size()>1) cir_sort(getFanin());
   for( auto&y:getFanin())
  {  if(!y->gate()->isGlobalRef())
     y->gate()->traversal(dfsList);
  } 
   set_GRef();
   dfsList.push_back(this);
}
void CirGate::cir_sort(vector< AIGateV*> & _tiList)
{
sort(_tiList.begin(),_tiList.end(),
[](const AIGateV * a,const AIGateV* b)
{return a->gate()->getID()<b->gate()->getID();}
);
}


// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
  string tem;
   cout<<"=================================================="<<endl;
   cout<<"= ";
tem=get_name()+to_string(getID())+"87"+", line "+to_string(getLineNo());
   if(getTypeStr()=="")
   {  
      cout<<get_name()<<"("<<getID()<<")"<<", line "<<getLineNo();
      
   }
   else
   {  tem+=(getTypeStr()+"87");
      cout<<get_name()<<"("<<getID()<<")"
         <<"\""<<getTypeStr()<<"\""<<", line "<<getLineNo();
   }
   cout<<setw(48-tem.size())<<right<<" ="<<endl;

   
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) 
{   assert (level >= 0);
    vector<CirGate*> tem;
    cout<<get_name()<<" "<<getID()<<endl;
    tem.push_back(this);
    R_Fan_in( level,0,tem) ;
}

void
CirGate::R_Fan_in(int level,int count,vector<CirGate*> &tem) 
{  if(level==0) return;   
   count++;
       for(const auto&x:getFanin()) 
   {    bool check=false;
        for(int i=1;i<=count;i++) cout<<"  ";
      
      if(x->isInv()) cout<<"!";
      cout<<x->gate()->get_name()<<" "<<x->gate()->getID();
        //碰到相同狀況case
       for(const auto&y:tem)  if(y==x->gate()&&count!=level) 
       {cout<<" (*)";
        check=true;
        break;
       }

       cout<<endl;

      if( !x->gate()->getFanin().empty()&&count!=level&&!check)
      { if(!check) tem.push_back(x->gate());//把output過的東西傳入
         x->gate()->R_Fan_in(level,count,tem);
      }
   }
}


void
CirGate::reportFanout(int level) 
{  assert (level >= 0);
   vector<CirGate*> tem;
    cout<<get_name()<<" "<<getID()<<endl;
    tem.push_back(this);
    R_Fan_out( level,0,tem) ;
}

void
CirGate::R_Fan_out(int level,int count,vector<CirGate*> &tem) 
{ if(level==0) return;  
  count++;
   vector<CirGate*> sorted =getFanout();
   cirMgr->cir_sort(sorted);
   for(const auto&x:sorted)
   {    bool check=false;
      for(const auto&y:x->getFanin())
      { if(y->gate()==this)//來回指到自己
        {   for(int i=1;i<=count;i++) cout<<"  ";
            if(y->isInv()) cout<<"!";
            cout<<x->get_name()<<" "<<x->getID();

            for(const auto&z:tem)  if(z==x&&count!=level) 
             {cout<<" (*)";
              check=true;
              break;}

            tem.push_back(this);//把output過的東西傳入
               cout<<endl;
               break;
            //指到同一個東西
         }
        
      }
             
        if( !x->getFanout().empty()&&count!=level&&!check)
           x->R_Fan_out(level,count,tem);
   } 

}

