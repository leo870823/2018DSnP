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

using namespace std;
class AIGateV;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
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

private:
  size_t _gateV; //以size_t存的記憶體位置
};
/////
class CirGate
{
  static inline unsigned _globalRef = 0;

public:
  CirGate(unsigned i, unsigned l, string s, unsigned r = 0,string n="")
      : _ID(i), _LineNo(l), var_ID(s), _ref(r),name(n) {}
  virtual ~CirGate() {}
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
private:
  unsigned _ID;
  unsigned _LineNo;
  unsigned _ref;
  string var_ID;
  vector<AIGateV *> Fanin;
  vector<CirGate *> Fanout;
//protected:
 
  string name;
};

class AIGate : public CirGate
{
public:
  AIGate(unsigned i, unsigned l, string s, unsigned r = 0,string n="AIG")
      : CirGate(i, l, s, r,n) { }

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
  PI(unsigned i, unsigned l, string s, unsigned r = 0,string n="PI")
      : CirGate(i, l, s, r,n) {}

  void printGate() 
  {
    cout << " PI  " << getID()<<" ";
    if (getTypeStr() != "")
      cout << "(" << getTypeStr() << ")";
  }
};

class PO : public CirGate
{
public:
  PO(unsigned i, unsigned l, string s, unsigned r = 0,string n="PO")
      : CirGate(i, l, s, r,n) {}
  void printGate() 
  { assert(getFanin().size()<=1);

    cout << " PO  " << getID()<<" ";
    if (getFanin().empty()) //float case
      cout << "*";
    else 
    { if(getFanin().at(0)->gate()->getTypeStr()=="undefined")
      cout<<"*";
      if(getFanin().at(0)->isInv())
      cout << "!";
      cout<<getFanin().at(0)->gate()->getID();
    }

      if (getTypeStr() != "")
        cout << " (" << getTypeStr() << ")";
  }
};

class Const : public CirGate
{
public:
  Const(unsigned i = 0, unsigned l = 0, string s = "", unsigned r = 0,string n="CONST")
      : CirGate(i, l, s, r,n) {}
  void printGate() 
  {cout<<" CONST0";}

};

class undefined : public CirGate
{
public:
  undefined(unsigned i = 0, unsigned l = 0, string s = "undefined", unsigned r = 0,string n="UNDEF")
      : CirGate(i, l, s, r,n) {}
  void printGate()  {}//undefined print nothing

};


#endif // CIR_GATE_H
