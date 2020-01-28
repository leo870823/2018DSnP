/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "rnGen.h"
#include <ctime>
#include <bitset>
using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{ 
  if(_fecGrps.empty())  initFEC();
  //for(int i=0;i<1000;i++) checkFEC();
  int hold=0;
  int counter=0;
  if(_GateList.size()>9000)  //_supermode
  { for(int i=0;i<_GateList.size()/100;i++)
     checkFEC();
  }
else
  while(counter!=10)
  { hold=_fecGrps.size();
     //cout<<"before:"<<_fecGrps.size()<<endl;
     checkFEC();
    // cout<<" after:"<<_fecGrps.size()<<endl;
     if(_fecGrps.size()==hold) //之前的和現在的ㄧ樣，連續10次就break
     counter++;  
  }
  fecgrpsort(); 
}

void
CirMgr::fileSim(ifstream& patternFile)
{ string hold;
  vector<string> pattern;
   while(!patternFile.eof())
   { if(!(patternFile>>hold)) break;
     if(hold.size()!=num_pi)
     { cout<<"Error: Pattern("<<hold
       <<") length("<<hold.size()<<") does not match the number of inputs("
       <<num_pi<<") in a circuit!!"<<endl;
       break;
     } 
     else pattern.push_back(hold);
   }
  // for( auto&x:pattern) cout<<x<<endl;
    int psize=pattern.size(); //最後output個數
   

   string zero_buf="";
   int compensate=sizeof(size_t)-pattern.size()%sizeof(size_t); //8-?
   for(int i=0;i<num_pi;i++) zero_buf+="0";
   if(compensate%8!=0) //8不用補償
   for(int i=0;i<compensate;i++) pattern.push_back(zero_buf);
   assert( pattern.size()%sizeof(size_t)==0);
   ////////////////////////////////////////////////////////////////////
  
     int ps=pattern.size();
   while(ps!=0)
  { string patt="";
    size_t binary;
    for(int i=1;i<=num_pi;i++) 
    {   patt="";
      for(int j=ps;j>ps-sizeof(size_t);j--)
       {patt+=pattern[j-1][i-1];
           if(pattern[j-1][i-1]=='0'||pattern[j-1][i-1]=='1'){}
          else {  string tem="";
                   for(int k=0;k<num_pi;++k) tem+=pattern[j-1][k];
                  cerr<<"Error: Pattern(";
                  cerr<<tem<<") contains a non-0/1 character('"<<pattern[j-1][i-1]
                  <<"')."<<endl;
                   cout<<"0 patterns simulated."<<endl;
                  return;
               }
       }
        binary = std::stoi(patt, nullptr, 2);
         // cout<<i<<" "<<binary<<" "<<patt<<endl;
        for(auto &x:_piList) 
        if(i==x->getID()) {x->set_sv(binary); break;}
    }
    for(auto &x:_VisitList) x->simulate();
    if(_fecGrps.empty()) check_base();//initalize
    else check_add();
     ps-=sizeof(size_t);
  }
   cout<<pattern.size()<<" patterns simulated."<<endl;
//fecgrp combine
fecgrpsort();

 if(_simLog)  //sim log有東西才需要
{
int counter =7;
int layer =pattern.size()/sizeof(size_t)-1; //0不會進來
string binary ="";
     for(const auto&x:pattern)
   {   if(counter<0) {counter =7; layer--;}
    *_simLog<<x;
    *_simLog<<" ";
    for(const auto&y:_poList)
     { 
        binary = bitset<8>(y->get_sp().at(layer)).to_string(); //to binary
       *_simLog<<binary[counter];
     }
    counter--;
    *_simLog<<endl;
    if(compensate!=8)
        if(layer==0&&counter==compensate-1) break; //暫時借你的不能輸出
   }
  }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
//用hash_key包住size_t
void
CirMgr::initFEC() //模擬第一個pattern
{  initialize_FEC();

//RandomNumGen
srand(time(0));
//pattern to gate
HashMap<HashKey, cirlist>  fecHash( getHashSize(_GateList.size() )) ;//local
for(const auto&x:_piList) x->set_sv( rand()); //note：這邊怪怪ㄉ
////////////////////////////////////////////////////////////////
//case:const0///////////////////////////////////////////////////
if 
(find(_VisitList.begin(), _VisitList.end(), CONST0) == _VisitList.end()) //找不到
{cirlist  tem;
tem.push_back(CONST0);
HashKey k(CONST0->getsv());
fecHash.insert(k,tem);
}
////////////////////////////////////////////////////////////////
//case:const0///////////////////////////////////////////////////

for(const auto&x:_VisitList)
 {     int i=0;
       x->simulate();
     if(x->get_name()!="PI"&&x->get_name()!="PO")    //pi po可模擬但不可以加到pi po grp
     {
      cirlist  tem;
      tem.push_back(x);
      HashKey k(x->getsv());
      HashKey k_i(~x->getsv()); 
      if(fecHash.check(k_i)) //相反的pattern存在
      { fecHash.get_data(k_i).push_back(x);  
         x->set_fec_b(true);    
      }
      else if(!fecHash.insert(k,tem))
       fecHash.get_data(k).push_back(x);  //相反的pattern不存在
     }                                  //insert失敗，代表second push_back
 }
for(const auto&x:fecHash)
  if(x.second.size()>1) 
   { 
     _fecGrps.push_back(x.second); 
     for(const auto&z:x.second ) z->get_FEC_ID().push_back(_fecGrps.size()-1);
   }//collect size>1 pattern
 
//fecHash will be destructed here!!
}
void
CirMgr::checkFEC()
{  initialize_FEC();
  srand(time(0));
  //simulation
   for(const auto&x:_piList) x->set_sv( rand()); //note：這邊怪怪ㄉ
   for(const auto&x:_VisitList)   x->simulate();
  //
  vector<cirlist>     newGrps;
  for(const auto&x:_fecGrps)
  { HashMap< HashKey, cirlist>  fecHash(getHashSize(x.size() )) ;
    for(const auto&y:x)
    { int i=0;
      if(y->get_name()!="PI"&&y->get_name()!="PO")
      {
      cirlist  tem;
      tem.push_back(y);
        HashKey k(y->getsv());
      HashKey k_i(~y->getsv()); 
      if(fecHash.check(k_i)) //相反的pattern存在
       {fecHash.get_data(k_i).push_back(y);  
          y->set_fec_b(true);    
       }
      else if(!fecHash.insert(k,tem))
       fecHash.get_data(k).push_back(y);  //相反的pattern不存在
      }                              //insert失敗，代表second push_back
    }
    for(const auto&z:fecHash)
    if(z.second.size()>1) 
    { newGrps.push_back(z.second); //collect size>1 pattern
      for(const auto&q:z.second ) q->get_FEC_ID().push_back(newGrps.size()-1);
    }
  }
    _fecGrps.swap(newGrps);
}
///////////////////////////////
//helper function
///////////////////////////////
void
CirMgr::initialize_FEC()
{   
  for(const auto&x:_VisitList)  
   { if(!x->get_FEC_ID().empty())x->get_FEC_ID().pop_back();
     x->set_fec_b(false); //全部reset
   }
  //const0
   if(!CONST0->get_FEC_ID().empty()) CONST0->get_FEC_ID().pop_back();
     CONST0->set_fec_b(false); //全部reset
}

void
CirMgr::check_base() //sim -file 第ㄧ波
{ //pattern to gate
 HashMap<HashKey, cirlist>  fecHash( getHashSize(_GateList.size() )) ;//local
 ////////////////////////////////////////////////////////////////
//case:const0///////////////////////////////////////////////////
if 
(find(_VisitList.begin(), _VisitList.end(), CONST0) == _VisitList.end()) //找不到
{cirlist  tem;
tem.push_back(CONST0);
HashKey k(CONST0->getsv());
fecHash.insert(k,tem);
}
////////////////////////////////////////////////////////////////
//case:const0///////////////////////////////////////////////////
  for(const auto&x:_VisitList)
 {     int i=0;
      //x->simulate();
      if(x->get_name()!="PI"&&x->get_name()!="PO") 
      {
        cirlist  tem;
      tem.push_back(x);
      HashKey k(x->getsv());
      HashKey k_i(~x->getsv()); 
      if(fecHash.check(k_i)) //相反的pattern存在
      { fecHash.get_data(k_i).push_back(x);  
         x->set_fec_b(true);    
      }
      else if(!fecHash.insert(k,tem))
       fecHash.get_data(k).push_back(x);  //相反的pattern不存在
      }                                 //insert失敗，代表second push_back
 }
for(const auto&x:fecHash)
  if(x.second.size()>1) 
   { 
     _fecGrps.push_back(x.second); 
     for(const auto&z:x.second ) z->get_FEC_ID().push_back(_fecGrps.size()-1);
   }//collect size>1 pattern
 
//fecHash will be destructed here!!
}

void
CirMgr::check_add() //sim 第二波
{ vector<cirlist>     newGrps;
  for(const auto&x:_fecGrps)
  { HashMap< HashKey, cirlist>  fecHash( getHashSize(x.size() )) ;
    for(const auto&y:x)
    { 
    if(y->get_name()!="PI"&&y->get_name()!="PO")
    {
      int i=0;
      cirlist  tem;
      tem.push_back(y);
        HashKey k(y->getsv());
      HashKey k_i(~y->getsv()); 
      if(fecHash.check(k_i)) //相反的pattern存在
       {fecHash.get_data(k_i).push_back(y);  
          y->set_fec_b(true);    
       }
      else if(!fecHash.insert(k,tem))
       fecHash.get_data(k).push_back(y);  //相反的pattern不存在  
    }                                     //insert失敗，代表second push_back                 
    }
    for(const auto&z:fecHash)
    if(z.second.size()>1) 
    { newGrps.push_back(z.second); //collect size>1 pattern
      for(const auto&q:z.second ) q->get_FEC_ID().push_back(newGrps.size()-1);
    }
  }
    _fecGrps.swap(newGrps);
}

void
CirMgr::fecgrpsort()
{   for( auto&x: _fecGrps)
  { 
     sort(x.begin(),x.end(),
     []( CirGate*  a,  CirGate* b) 
     {return b->getID() > a->getID();});
    if(!x.empty())
    {if(x[0]->get_fec_b()) //如果第1個有
       for(auto&y:x) {y->fec_b_reflect();  } //cout<<y->getID()<<y->get_fec_b()<<endl;
    }
  }
}
