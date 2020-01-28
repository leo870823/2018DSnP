/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
#include <algorithm>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class AIGateV;
//------------------------------------------------------------------------
//                            Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

class AIGateV
{
#define NEG 0x1
public:
  AIGateV(CirGate *g, size_t phase) : _gateV(size_t(g) + phase) {}
  CirGate *gate() const
  {
    return (CirGate *)(_gateV & ~size_t(NEG));
  }
  bool isInv() const
  {
    return (_gateV & NEG);
  }
  size_t get_gateV(){return _gateV;}
private:
  size_t _gateV; //以size_t存的記憶體位置
};
/////
class CirGate
{
  static inline unsigned _globalRef = 0;

public:
  CirGate(unsigned i, unsigned l, string s, unsigned r = 0,string n="",size_t siv=0,bool fec_in=false,Var v=0)
      : _ID(i), _LineNo(l), var_ID(s), _ref(r),name(n),_simvalue(siv),fec_inv(fec_in),_var(v) {}
  virtual ~CirGate() {}
  //fraif fn
   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }
  // Traversal fn
  bool isGlobalRef() { return (_ref == _globalRef); }
  void set_GRef() { _ref = _globalRef; }
  static void reset_GRef() { _globalRef++; }
  void traversal(vector<CirGate *> &dfsList);
  // Basic access methods
  string getTypeStr() const { return var_ID; }
  unsigned getLineNo() const { return _LineNo; }
  unsigned getID() const { return _ID; }
  unsigned is_ref() const { return _ref; }
  vector<AIGateV *> & getFanin() { return Fanin; }
  vector<CirGate *> & getFanout() { return Fanout; }
  // Printing functions
  virtual void printGate() = 0;
  void reportGate() const;
  void reportFanin(int level) ;//note discard const
  void reportFanout(int level) ;//note discard const
  //help fn
   void cir_sort(vector< AIGateV*> & _tiList);
   void R_Fan_in(int level,int count,vector<CirGate*> &tem) ;
   void R_Fan_out(int level,int count,vector<CirGate*> &tem);
   string get_name() const {return name;}
   bool isAig() {return !(name=="PI"||name=="PO");}
   //sim fn 
   size_t getsv() { return _simvalue; }
   void set_sv(size_t sv){_simvalue=sv; }
   void set_fec_b(bool b) { fec_inv=b;} //default 0
   bool get_fec_b() { return fec_inv;}
   void fec_b_reflect(){ fec_inv=!fec_inv;}
   vector<size_t>& get_sp(){ return sim_pattern;}
   void simulate() //注意input output
   { //po:case
      if(Fanin.size()==1)//po case
        _simvalue=Fanin[0]->isInv()?~Fanin[0]->gate()->getsv(): Fanin[0]->gate()->getsv();

     if(!Fanin.empty()&&!Fanout.empty()) //general case :floating gate(?)
      { assert(Fanin.size()==2);
        size_t s1,s2;
        s1=Fanin[0]->isInv()?~Fanin[0]->gate()->getsv(): Fanin[0]->gate()->getsv();
        s2=Fanin[1]->isInv()?~Fanin[1]->gate()->getsv(): Fanin[1]->gate()->getsv();
        _simvalue=s1&s2; //bitwise and
      }
       sim_pattern.push_back(_simvalue); //把sim的值存進去
   }
   vector<size_t>& get_FEC_ID(){return FEC_ID;}

void replace_fanin( AIGateV*  a,AIGateV* b) //a->b
{ 
  vector<AIGateV*>::iterator hold    =                    
   find(Fanin.begin(),Fanin.end(),a); //inv確認 
  //
   assert(hold!=Fanin.end());
  // delete a->gate(); //delete這樣太早ㄌQQ
    Fanin.erase(hold);
    Fanin.push_back(b);
}
void replace_fanout(CirGate* a ,CirGate* b) //a->b
{ vector<CirGate* >::iterator hold    =   
   find(Fanout.begin(),Fanout.end(),a); 
   if(hold!=Fanout.end())
    Fanout.erase(hold);
    Fanout.push_back(b);
}
//刪除在哪個strash裡面
void struct_merge(CirGate* g)
{ //handle fanout of g
  for(auto &x:g->getFanout())
  { this->getFanout().push_back(x);
    for( auto &y:x->getFanin())
    { if(y->gate()==g)
      y=new AIGateV(this,y->isInv()^(y->gate()->get_fec_b())) ;
    }
     vector<CirGate*>::iterator i
     =find(g->getFanout().begin(),g->getFanout().end(),x);
     if(i!=g->getFanout().end()) g->getFanout().erase(i);//避免刪掉的gate在走到
  }
  //handle fanin of g
  for( auto &x:g->getFanin())
  { //cout<<x->gate()->getFanout().size()<<endl;
    for( auto &z:x->gate()->getFanout())
        { if(z==g)
          { vector<CirGate*>::iterator i=
            find(x->gate()->getFanout().begin(),x->gate()->getFanout().end(),z);
           if(i!=x->gate()->getFanout().end())  x->gate()->getFanout().erase(i);
           //break; 不用break因為可能兩個都ㄧ樣
          }
        }    
  }
   // delete g;
  cout<<"Strashing: "<<this->getID()<<" merging "<<g->getID()<<"..."<<endl;
}

void fraig_merge(CirGate* g)
{ //handle fanout of g
  for(const auto &x:g->getFanout())
  { this->getFanout().push_back(x);
    for( auto &y:x->getFanin())
    { if(y->gate()==g)
      y=new AIGateV(this,y->isInv()^(y->gate()->get_fec_b())) ;
    }
  }
  //handle fanin of g
  for( auto &x:g->getFanin())
  {   for( auto &z:x->gate()->getFanout())
        { if(z==g)
          { z=x->gate()->getFanout().at(x->gate()->getFanout().size()-1);
            x->gate()->getFanout().pop_back();
           //break; 不用break因為可能兩個都ㄧ樣
          }
        }    
  }
  //delete g;
}
private:
  unsigned _ID;
  unsigned _LineNo;
  unsigned _ref;
  bool fec_inv;
  Var     _var;
  string var_ID;
  string name;
  vector<AIGateV *> Fanin;
  vector<CirGate *> Fanout;
  vector<size_t> FEC_ID;
  vector<size_t> sim_pattern;
  size_t _simvalue;
  
};

class AIGate : public CirGate
{
public:
  AIGate(unsigned i, unsigned l, string s, unsigned r = 0,string n="AIG",size_t siv=0,bool fec_in=false,Var v=0)
      : CirGate(i, l, s, r,n,siv,fec_in,v) { }

  void printGate() 
  { assert(getFanin().size()==2);
    

     cout << " AIG " << getID()<<" ";
     int i=getFanin().size();
     for( auto&x:getFanin())
       { //undefined
         if( x->gate()->getTypeStr()=="undefined" )
          cout << "*";
         //inv
         if(x->isInv()) cout << "!";
         cout<<x->gate()->getID();
         if(i!=1) cout<<" "; 
         i--;
       }
       //cout<<'\b';
      
  }
};

class PI : public CirGate
{
public:
  PI(unsigned i, unsigned l, string s, unsigned r = 0,string n="PI",size_t siv=0,bool fec_in=false,Var v=0)
      : CirGate(i, l, s, r,n,siv,fec_in,v) {}

  void printGate() 
  {
    cout << " PI  " << getID();
    if (getTypeStr() != "")
      cout << " (" << getTypeStr() << ")";
  }
};

class PO : public CirGate
{
public:
  PO(unsigned i, unsigned l, string s, unsigned r = 0,string n="PO",size_t siv=0,bool fec_in=false,Var v=0)
      : CirGate(i, l, s, r,n,siv,fec_in,v) {}
  void printGate() 
  { assert(getFanin().size()<=1);

    cout << " PO  " << getID();
    if(getFanin().empty()||getFanin().at(0)->gate()->getTypeStr()=="undefined"||getFanin().at(0)->isInv())
    cout<<" ";
    if (getFanin().empty()) //float case
      cout << "*";
    else 
    { if(getFanin().at(0)->gate()->getTypeStr()=="undefined")
      cout<<"*";
      if(getFanin().at(0)->isInv())
      cout << "!";
      if(getFanin().at(0)->gate()->getTypeStr()=="undefined"||getFanin().at(0)->isInv())
      cout<<getFanin().at(0)->gate()->getID();
      else  cout<<" "<<getFanin().at(0)->gate()->getID();
    }

      if (getTypeStr() != "")
        cout << " (" << getTypeStr() << ")";
  }
};

class Const : public CirGate
{
public:
  Const(unsigned i = 0, unsigned l = 0, string s = "", unsigned r = 0,string n="CONST",size_t siv=0,bool fec_in=false,Var v=0)
      : CirGate(i, l, s, r,n,siv,fec_in,v) {}
  void printGate() 
  {cout<<" CONST0";}

};

class undefined : public CirGate
{
public:
  undefined(unsigned i = 0, unsigned l = 0, string s = "undefined", unsigned r = 0,string n="UNDEF",size_t siv=0,bool fec_in=false,Var v=0)
      : CirGate(i, l, s, r,n,siv,fec_in,v) {}
  void printGate()  {}//undefined print nothing

};


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
     HashKey(size_t x) {in0=x; in1=0; } //fec pair使用 in0就是pattern
 
    size_t operator() () const { return in0+in1; }  // as hash function 
    bool operator == (const HashKey& k) const { return (k.in0==in0&&k.in1==in1); }
  private:
  size_t in0, in1;
 };




#endif // CIR_GATE_H
