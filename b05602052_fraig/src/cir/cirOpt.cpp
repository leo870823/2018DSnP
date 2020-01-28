/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{  bool seed=true;
   while(seed)
   { seed =false;
    for(const auto&x:_GateList)
    if(x->getFanout().empty())
     {  
       delete_redundant(x);
        seed=true;
        break;
     }
   }
  num_gate=num_of_non_un();
  // num_gate=_GateList.size();
}

void
CirMgr::delete_redundant(CirGate* del)
{ vector<CirGate*>::iterator hold =find(_GateList.begin(),_GateList.end(), del);

   for(const auto&x:del->getFanin())
   { for(const auto&y:x->gate()->getFanout())
     { if(y==del) 
       { vector<CirGate*>::iterator h =find(x->gate()->getFanout().begin(),x->gate()->getFanout().end(), y);
              x->gate()->getFanout().erase( h);
              break;
       }
     }
   }
   print_s(del);
   delete del;
  _GateList.erase( hold);
}

void
CirMgr:: print_s(CirGate*  del) const
{cout<<"Sweeping: ";
        cout<<del->get_name();
       cout<<"("<<del->getID()<<") removed..."<<endl;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{ 
   for(const auto&x:_VisitList)   merge(x);
   DFST();
   num_gate=num_of_non_un(); //gate數量要換
   //num_gate=_GateList.size();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void
CirMgr::merge( CirGate*   tem)
{ CirGate* dominator=NULL;
  bool b =false;
  if(tem->getFanin().empty()||tem->get_name()=="PI"||tem->get_name()=="PO") return;

   else //gate case
       { if(tem->getFanin().at(0)->gate()==tem->getFanin().at(1)->gate())
          { if(tem->getFanin().at(0)->isInv()!=tem->getFanin().at(1)->isInv()) 
            { for( auto&x:tem->getFanout())
               {
                for(auto&y:x->getFanin())
                 if(y->gate()==tem) 
                 { 
                   //AIGateV * a=new AIGateV(y->gate(),y->isInv() );
                    AIGateV * b=new AIGateV(getGate(0),y->isInv());
                     x->replace_fanin(y,b );
                     getGate(0)->getFanout().push_back(x);
                 }
               }
               for( auto&x:tem->getFanin())
               {  
                   for(auto&y:x->gate()->getFanout())
                   { if(y==tem)
                    {
                      vector<CirGate*>::iterator i
                      =find(x->gate()->getFanout().begin(),x->gate()->getFanout().end(),tem);
                    if(i!=x->gate()->getFanout().end())
                     x->gate()->getFanout().erase(i);//避免刪掉的gate在走到  
                    }
                   }
               }
               
               
               //dominator=tem->getFanin().at(0)->gate();
              dominator=getGate(0);
            }//case 0
             
            else //case 合併 a+a=a 1+1=1 0+0=0
            { for( auto&x:tem->getFanin().at(0)->gate()->getFanout())
              if(x==tem)    //把原本的連結斷掉
              { tem->getFanin().at(0)->gate()->replace_fanout(x,NULL);
                tem->getFanin().at(0)->gate()->getFanout().pop_back();
                b= tem->getFanin().at(0)->isInv();
              }
              

              for( auto&x:tem->getFanout()) 
              {tem->getFanin().at(0)->gate()->getFanout().push_back(x);
              for( auto&y: x->getFanin())
               { if(y->gate()==tem) 
                { AIGateV * a=new AIGateV(tem->getFanin().at(0)->gate(),b^y->isInv());
                 x->replace_fanin( y,a);
                }
               }
              } 
              dominator=tem->getFanin().at(0)->gate();
            }
          }
          //輸出不ㄧ樣
          else 
          { if(tem->getFanin().at(0)->gate()->get_name()!="CONST"&&tem->getFanin().at(1)->gate()->get_name()!="CONST")
            return ;//not should be merged

            if((tem->getFanin().at(0)->gate()->get_name()=="CONST"&&!tem->getFanin().at(0)->isInv())
          ||(tem->getFanin().at(1)->gate()->get_name()=="CONST"&&!tem->getFanin().at(1)->isInv())) //有0輸出都是0
            {     for( auto&x: tem->getFanout()) 
                  {getGate(0)->getFanout().push_back(x); //0->3
                    for(auto&y: x->getFanin() )
                    { if(y->gate()==tem)
                      { //AIGateV * a=new AIGateV(tem,y->isInv() );
                        AIGateV * b=new AIGateV(getGate(0),y->isInv());
                         x->replace_fanin(y,b );
                      }

                    }
                  }
                  for( auto&x: tem->getFanin()) 
                  { if(x->gate()!=getGate(0))
                    {  for(auto&y: x->gate()->getFanout())
                      {  if(y==tem)
                         {    vector<CirGate*>::iterator i
                      =find(x->gate()->getFanout().begin(),x->gate()->getFanout().end(),tem);
                    if(i!=x->gate()->getFanout().end())
                     x->gate()->getFanout().erase(i);//避免刪掉的gate在走到                          
                         }
                      }
                     break;
                    }
                  }
                  dominator=getGate(0);
            }
            else 
            { //input not same(exclude 0): a 1
              int i=0;
              if(tem->getFanin().at(0)->gate()->get_name()=="CONST") i=1; 
    
               for( auto&x :tem->getFanin().at(i)->gate()->getFanout() )
                  {  if(x==tem)
                     { tem->getFanin().at(i)->gate()->replace_fanout(x,NULL);
                        tem->getFanin().at(i)->gate()->getFanout().pop_back();//進來前把它清掉
                       for( auto&y :tem->getFanout() )
                       { 
                         tem->getFanin().at(i)->gate()->getFanout().push_back(y);
                         for(auto&z:y->getFanin())
                         { if(z->gate()==tem)
                           {  AIGateV * a= new AIGateV(tem->getFanin().at(i)->gate()
                                       ,z->isInv()^tem->getFanin().at(i)->isInv() );
                             y->replace_fanin(z,a);
                           }
                         }
                       }
                        dominator=tem->getFanin().at(i)->gate();
                           b=tem->getFanin().at(i)->isInv();
                     break; //因為兩者絕對不一樣所以可以break
                     }
                  }
            }
          }
if(dominator)
    { cout<<"Simplifying: "<<dominator->getID()<<" merging ";
      if(b) cout<<'!';
      cout<<tem->getID()<<"..."<<endl;
      vector<CirGate* >::iterator hold =find(_GateList.begin(),_GateList.end(),tem);
       _GateList.erase(hold);
      delete tem;
    }
       }

}
//記得delete

