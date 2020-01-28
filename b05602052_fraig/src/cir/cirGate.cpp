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
#include <bitset>
using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

void CirGate::traversal(vector< CirGate *> & dfsList)
{ if(getFanin().empty()) 
   { //base case
   set_GRef(); 
   dfsList.push_back(this);
   return;
   }//撞到 po/floating
   for( auto&y:getFanin())
  { 
     if(!y->gate()->isGlobalRef())
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



/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
  string tem;
   cout<<"================================================================================"<<endl;
   cout<<"= ";
   if(getTypeStr()=="")
   cout<<get_name()<<"("<<getID()<<")"<<", line "<<getLineNo()<<endl;
   else
   cout<<get_name()<<"("<<getID()<<")"<<"\""<<getTypeStr()<<"\""<<", line "<<getLineNo()<<endl;
   
    cout<<"= FECs:";
    /*
    for(const auto&x:cirMgr->get_gaL()) 
    { if(this!=x)
      if((!FEC_ID.empty()&&!x->FEC_ID.empty())&& FEC_ID[0]==x->get_FEC_ID().at(0)) //印出同一個grp的
       { cout<<" ";
         if(x->get_fec_b()) cout<<"!"; 
         cout<<x->getID();
       }
    }
    */
   if(!FEC_ID.empty())  
        
     for  (const auto&x:cirMgr->getfecGrps().at(FEC_ID[0]))
      {  if(x->getID()!=getID())
         {cout<<" ";
         if(x->get_fec_b()) cout<<"!"; 
         cout<<x->getID();}
      }
    cout<<endl;
    cout<<"= Value: ";
    string binary =""; //to binary
    int s=sim_pattern.size();
    if(s<8) 
    {   cout<<"00000000";
       for(int i=0;i<7-s;i++) cout<<"_00000000";
       for(int i=0;i<s;i++) 
      {  binary = bitset<8>(sim_pattern[i]).to_string(); //to binary
         cout<<'_'<<binary;
       }
         cout<<endl;

    }
    else //s>=8
    {  for(int i=sim_pattern.size()-1;i>sim_pattern.size()-8;i--)
        {   binary = bitset<8>(sim_pattern[i]).to_string(); //to binary
        cout<<binary<<'_';}
         cout<<bitset<8>( sim_pattern[sim_pattern.size()-8]).to_string()<<endl;
    }
    cout<<"================================================================================"<<endl;
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

