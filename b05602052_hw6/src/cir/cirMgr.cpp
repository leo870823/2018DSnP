/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <map>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <fstream>
#include <string>
using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr *cirMgr = 0;

enum CirParseError
{
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0; // in printint, lineNo needs to ++
static unsigned colNo = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err)
   {
   case EXTRA_SPACE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Extra space character is detected!!" << endl;
      break;
   case MISSING_SPACE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Missing space character!!" << endl;
      break;
   case ILLEGAL_WSPACE: // for non-space white space character
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Illegal white space char(" << errInt
           << ") is detected!!" << endl;
      break;
   case ILLEGAL_NUM:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal "
           << errMsg << "!!" << endl;
      break;
   case ILLEGAL_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal identifier \""
           << errMsg << "\"!!" << endl;
      break;
   case ILLEGAL_SYMBOL_TYPE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Illegal symbol type (" << errMsg << ")!!" << endl;
      break;
   case ILLEGAL_SYMBOL_NAME:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Symbolic name contains un-printable char(" << errInt
           << ")!!" << endl;
      break;
   case MISSING_NUM:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Missing " << errMsg << "!!" << endl;
      break;
   case MISSING_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Missing \""
           << errMsg << "\"!!" << endl;
      break;
   case MISSING_NEWLINE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": A new line is expected here!!" << endl;
      break;
   case MISSING_DEF:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Missing " << errMsg
           << " definition!!" << endl;
      break;
   case CANNOT_INVERTED:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": " << errMsg << " " << errInt << "(" << errInt / 2
           << ") cannot be inverted!!" << endl;
      break;
   case MAX_LIT_ID:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
           << endl;
      break;
   case REDEF_GATE:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Literal \"" << errInt
           << "\" is redefined, previously defined as "
           << errGate->getTypeStr() << " in line " << errGate->getLineNo()
           << "!!" << endl;
      break;
   case REDEF_SYMBOLIC_NAME:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Symbolic name for \""
           << errMsg << errInt << "\" is redefined!!" << endl;
      break;
   case REDEF_CONST:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Cannot redefine constant (" << errInt << ")!!" << endl;
      break;
   case NUM_TOO_SMALL:
      cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
           << " is too small (" << errInt << ")!!" << endl;
      break;
   case NUM_TOO_BIG:
      cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
           << " is too big (" << errInt << ")!!" << endl;
      break;
   default:
      break;
   }


lineNo = 0; // in printint, lineNo needs to ++
 colNo = 0;  // in printing, colNo needs to ++
 string errMsg="";
errInt=0;
//clear();
   return false;

}




/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool CirMgr::readCircuit(const string &fileName)
{
   unsigned line = 0;
   unsigned column = 1;
   fstream file;
   file.open(fileName, ios::in);
   if(!file.is_open())
   {
      cout<<"Cannot open design "<<"\""<<fileName<<"\""<<endl;
      return false;
   }

   char buffer[65536];

   file >> buffer;
   string te=buffer;

   if (te != "aag")
   {
      file.close();
      errMsg=te;
   if(te.size()>0)
   return parseError(ILLEGAL_IDENTIFIER);
   else 
   return parseError(MISSING_IDENTIFIER);
   }
   //小心是否多個space
   file >> max_id;
   file >> num_pi;
   file >> num_latch;
   file >> num_po;
   file >> num_gate;
   vector<unsigned> pi;
   vector<unsigned> po;

if(num_latch!=0) {errMsg="latches"; return parseError(ILLEGAL_NUM);}
   
   line++;
   unsigned hold = 0;
   for (unsigned i = 0; i < num_pi; i++)
   {
      file >> hold;
      pi.push_back(hold);
 if(pi[pi.size()-1]==0||pi[pi.size()-1]==1){ lineNo=line; errInt=pi[pi.size()-1]; return parseError(REDEF_CONST); }
if(pi[pi.size()-1]%2==1) { lineNo=line; errMsg="PI"; errInt=pi[pi.size()-1]; return parseError(CANNOT_INVERTED); }

      line++;
      pi.push_back(line); //以後每兩個為1單位:一個是id,一個是line,
   }
     for (unsigned i = 0; i < num_po; i++)
   {
      file >> hold;
      po.push_back(hold);
      line++;
      po.push_back(line); //以後每兩個為1單位:一個是id,一個是line,
   }


   vector<vector<unsigned>> gate_connect;
   //line被存在最後面
   for (int i = 0; i < num_gate; i++)
   {  vector<unsigned> tem;
      gate_connect.push_back(tem);
      for (int j = 0; j < 3; j++)
      { file >> hold;
         gate_connect[i].push_back(hold);
      }
      if(gate_connect[i][0]==0||gate_connect[i][0]==1){ lineNo=line; errInt=gate_connect[i][0]; return parseError(REDEF_CONST); }
      if((gate_connect[i][0]/2)>max_id){lineNo=line; errInt=gate_connect[i][0]; return parseError(MAX_LIT_ID);  }
      if((gate_connect[i][0]%2)==1){lineNo=line; errMsg="AIG gate"; errInt=gate_connect[i][0]; return parseError(CANNOT_INVERTED);  }
      line++;
      gate_connect[i].push_back(line);   
   }
/////////////////////
   vector<string> pi_name;
   vector<string> po_name;
   for(int i=0;i<num_pi;i++) pi_name.push_back("");
   for(int i=0;i<num_po;i++) po_name.push_back("");
   int test_i=0;
   int test_o=0;
while(1)
{  file >> buffer; 
       te=buffer;
      if (te== "c"|| file.eof())  //吃進來的第一個是c,break
      {   line++;
         file.close();
         break;
      }
      else if(myStrNCmp(te,"o",1)==0)
      {   
           test_o++;
      
          file >> buffer; 
          te=te.substr(1);
          for(const auto&x:te) if(!isdigit(x)) { lineNo=line; errMsg="symbol index("+te+")"; return parseError(ILLEGAL_NUM);   }
          int hold =stoi(te);

         if(hold>=num_po){ lineNo=line; errInt=hold; errMsg="PO index"; return parseError(NUM_TOO_BIG); }
          po_name[hold]=buffer;
           line++;
      }
      else if(myStrNCmp(te,"i",1)==0)
      {   
         
         
         test_i++;

          file >> buffer; 
           te=te.substr(1);
          for(const auto&x:te) if(!isdigit(x)) { lineNo=line; errMsg="symbol index("+te+")"; return parseError(ILLEGAL_NUM);   }
          int hold =stoi(te);

          if(hold>=num_pi){ lineNo=line; errInt=hold;  errMsg="PI index"; return parseError(NUM_TOO_BIG); }
          pi_name[hold]=buffer;
          line++;
         

      }
}
//注意2位數QQ


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
   //創物件
   CirGate *tem = NULL;
   AIGateV *tem_a = NULL;
   for (int i = 0; i < num_pi; i++)
   {  //if(pi_name.size()<=i) tem = new PI(pi[2 * i]/2, pi[2 * i + 1], "", 0); else
      tem = new PI(pi[2 * i]/2, pi[2 * i + 1], pi_name[i], 0);
      _piList.push_back(tem);

      cirMAP.insert(Cirpair(pi[2 * i]/2,tem)); //ID存/過的
   }
   unsigned t =max_id;
   for(int i=0;i< num_po;i++)
   {  // if(po_name.size()<=i) tem = new PO(++t, po[2 * i + 1], "", 0); else
      tem = new PO(++t, po[2 * i + 1], po_name[i], 0);
      _poList.push_back(tem);

       cirMAP.insert( Cirpair(t,tem)); //ID存的是指著po 的ptr
   }

   for (int i = 0; i < num_gate; i++)
   { //gate通常無名
     tem = new AIGate(gate_connect[i][0]/2,gate_connect[i][3],"",0);
     _GateList.push_back(tem);
      cirMAP.insert(Cirpair(gate_connect[i][0]/2,tem)); //ID存/過的
   }
   //CONNECTION
//handle gate
    for(int i = 0; i < num_gate; i++)
    {  //gate2個腳
       for(int j=1;j<3;j++) //看gate id
      { unsigned id_tem =gate_connect[i][j]/2; 
         size_t bubble =gate_connect[i][j]%2; 
          tem=find_match_ptr(id_tem);
          tem_a=new AIGateV(tem,bubble);    
      get_gaL().at(i)->getFanin().push_back(tem_a);
      //往回連接
      tem_a->gate()->getFanout().push_back(get_gaL().at(i));
      }
    }
//handle po
     for(int i = 0; i < num_po; i++)
    {  
       unsigned id_tem =po[2*i]/2; //我要連結到的id
       size_t bubble=po[2*i]%2;
        tem=find_match_ptr(id_tem);
         tem_a=new AIGateV(tem,bubble);
      get_poL().at(i)->getFanin().push_back(tem_a);
       //往回連接
      tem_a->gate()->getFanout().push_back(get_poL().at(i));
    }
/*
////////////////////////debug////////
    cout<<"debug_before connection"<<endl;
    cout<<max_id<<endl;
    cout<<num_pi<<endl;
    cout<<num_latch<<endl;
    cout<<num_po<<endl;
    cout<<num_gate<<endl;
    unsigned w =max_id;
cout<<"pi po"<<endl;
for(unsigned i=0;i<num_pi;i++)
if(pi[2 * i]/2==0) cout<<" ! "<<pi[2 * i]/2<<" "<<pi[2 * i + 1]<<endl;
else 
{ if(pi_name.size()>i)
 cout<<pi_name[i]<<" ";
 cout<<pi[2 * i]/2<<" "<<pi[2 * i + 1]<<endl;
}

for(unsigned i=0;i<num_po;i++)
if(po[2 * i]/2==0) cout<<" ! "<<po[2 * i]/2<<" "<<po[2 * i + 1]<<endl;
else
{ if(po_name.size()>i)
 cout<<po_name[i]<<" ";
 cout<<po[2 * i]/2<<" "<<po[2 * i + 1]<<endl;
}

cout<<"gate_connected"<<endl;
for(unsigned i=0;i<num_gate;i++)
{cout<<"aig ";
   for(unsigned j=0;j<3;j++)
   {if(gate_connect[i][j]%2==1) cout<<"!";
   cout<<gate_connect[i][j]/2<<" ";}
   cout<<gate_connect[i][3]<<endl;
}
   cout<<"debug_after connection"<<endl;
   cout<<"pi "<<_piList.size()<<endl;
   cout<<"po "<<_poList.size()<<endl;
   cout<<"gate"<<_GateList.size()<<endl;
//////////////////////////////////////////////////////////////////////
*/
//prepare traversallist
prepare_traversallist(_poList,_GateList,_piList);
//DFS
DFST();
   return true;
}
void CirMgr::DFST() 
{  
   CirGate::reset_GRef();//提伸global ref count
   for( auto&x:_traversalList)
   { 
     if(!x->isGlobalRef())
     x->traversal(_VisitList);//拜訪完把它填入,傳入list
   }
}

void CirMgr::prepare_traversallist (vector< CirGate *> po,vector< CirGate *>Gate,vector< CirGate *> pi)
{ 
   //sort
cir_sort(po);
//cir_sort(Gate);
//cir_sort(pi);
  for(const auto&x:po) _traversalList.push_back(x);
//  for(const auto&x:Gate) _traversalList.push_back(x);
//  for(const auto&x:pi) _traversalList.push_back(x);

}

CirGate* CirMgr::find_match_ptr(unsigned ID ) 
{  CirGate* tem =NULL;
   unordered_map<unsigned,CirGate*>::const_iterator got=cirMAP.find(ID);
   if(got!=cirMAP.end()) tem=got->second; //有找到
   //floating case
   if(tem==NULL) 
   { tem=new undefined(ID,0,"undefined",0);
     _GateList.push_back(tem);
   }
      return tem;
}

 void CirMgr::cir_sort(vector< CirGate *> & _tiList)
{
 sort(_tiList.begin(),_tiList.end(),
 [](const CirGate * a,const CirGate * b)
 {return a->getID()<b->getID();});

}

//記得要reset喔喔ㄛ
/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void CirMgr::printSummary() const
{
cout<<endl;
cout<<"Circuit Statistics"<<endl;
cout<<"=================="<<endl;
cout<<"  "<<"PI"<<setw(12)<<right<<num_pi<<endl;
cout<<"  "<<"PO"<<setw(12)<<right<<num_po<<endl;
cout<<"  "<<"AIG"<<setw(11)<<right<<num_gate<<endl;
cout<<"------------------"<<endl;
cout<<"  "<<"Total"<<setw(9)<<right<<num_pi+num_po+num_gate<<endl;

}

void CirMgr::printNetlist() const
{  cout<<endl;
   CirGate * base=NULL;
   int i=0;
   for(const auto&x:_VisitList)
   { base=x;
     if(x->getTypeStr()!="undefined")
     {
     cout<<"["<<i<<"]";
     base->printGate();
     cout<<endl;
     i++;
     }
   }
}

void CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(const auto&x:_piList) cout<<" "<<x->getID();
   cout << endl;
}

void CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(const auto&x:_poList) cout<<" "<<x->getID();
   cout << endl;
}

void CirMgr::printFloatGates() const
{  vector<unsigned> NON_USED_ID;
   vector<unsigned> undefined_ID;
   for(const auto&x: _GateList)
  {  //
     if(x->getFanout().empty())
     NON_USED_ID.push_back(x->getID());
   
    for(const auto&y:x->getFanin() )
    { if(y->gate()->getTypeStr()=="undefined")
      {undefined_ID.push_back(x->getID());
       break;
      }
    }
  }

  for(const auto&x: _piList)
  {  //
     if(x->getFanout().empty())
     NON_USED_ID.push_back(x->getID());
  }

  for(const auto&x: _poList)
  {  assert(x->getFanin().size()==1);
     for(const auto&y:x->getFanin() )
    { 
       if(y->gate()->getTypeStr()=="undefined")
      {undefined_ID.push_back(x->getID());
      break;
      }
    }
  }

 if(!undefined_ID.empty())
 {  
 sort(undefined_ID.begin(),undefined_ID.end());
 cout<<"Gates with floating fanin(s):";
  for(const auto&x: undefined_ID) cout<<" "<<x;
  cout<<endl;
 }
 if(!NON_USED_ID.empty())
 {
 sort(NON_USED_ID.begin(),NON_USED_ID.end());
 cout<<"Gates defined but not used  :";
 for(const auto&x: NON_USED_ID) cout<<" "<<x;
 cout<<endl;
 }
  


}

void CirMgr::writeAag(ostream &outfile) const
{  //prepare_gate_num
    int num_Gate=0;
    for(const auto&x:_VisitList)  
    if(x->get_name()=="AIG") num_Gate++;

   
   //header
   outfile <<"aag "<< max_id<<" ";
   outfile << num_pi;
   outfile <<" ";
   outfile << num_latch;
   outfile <<" ";
   outfile << num_po;
   outfile <<" ";
   outfile <<num_Gate<<endl;
   for(const auto&x:_piList)  outfile <<2*x->getID()<<endl;
   for(const auto&x:_poList)  outfile <<2*x->getFanin().at(0)->gate()->getID()+x->getFanin().at(0)->isInv()<<endl;
   //gate只能print有拜訪過ㄉ
   for(const auto&x:_VisitList)  
   { 
   if(x->get_name()=="AIG")
     {outfile <<2*x->getID();
    for(const auto&y:x->getFanin())
    outfile <<" "<<2*y->gate()->getID()+y->isInv();
    outfile<<endl;
     }
   }
//name
int i=0;
for(const auto&x:_piList)
 {  if(x->getTypeStr()!="")
    outfile<<"i"<<i<<" "<<x->getTypeStr()<<endl; i++; }
 i=0;
for(const auto&x:_poList) 
{  if(x->getTypeStr()!="")
   outfile<<"o"<<i<<" "<<x->getTypeStr()<<endl; i++; }
outfile <<"c\n"<<"AAG output by Leo Shu 2018 NTU\n";

}
//note:output map 
//output出來的東西是default inorder的
//
