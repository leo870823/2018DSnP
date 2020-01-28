/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"
#include <sstream>

using namespace std;
#define READ_BUF_SIZE    65536

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{ 
    os << "\"" << j._key << "\" : " << j._value;
    return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   //讀一個ele
{   char buffer[READ_BUF_SIZE ];
            int counter =0 ;
            int n_hold=0 ;
       
            string hold;
             while(!is.eof())
            {
            is>>buffer;
             if(buffer[0]=='"')     //key
                {
                    char cold[READ_BUF_SIZE ];
                    int i=1;
                    while(buffer[i]!='"')
                    {   cold[i-1]=buffer[i];
                        i++;
                    }
                    cold[i-1]='\0';
                    cold[i]='\0';
                    hold=cold;
                    counter++;
                   for(int i=0;i<READ_BUF_SIZE ;i++)cold[i]='\0';
                }
            if(buffer[0]==':')   //value
                { is.eof();
                  is>>n_hold;
                    counter++;
                }
            if(counter==2)
                {
                    DBJsonElem element(hold,n_hold);
                   j._obj.push_back(element);
                    counter=0;
                }
        }
}
//assert(!j._obj.empty());
   j.is_read_in=true;

  is.clear();
   return is;
}
//output DB json全部的東東
ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
    if(j._obj.size()==0)
     {
      cout<<'{'<<endl;
      cout<<'}'<<endl;
     }
    else
    { 
    cout<<'{'<<endl;
    for(int i=0;i<j._obj.size()-1;i++)
    cout<<"  "<<j._obj[i]<<','<<endl;
    cout<<"  "<<j._obj[j._obj.size()-1]<<endl;
    cout<<'}'<<endl;
    }
   return os;

}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{_obj.clear();
is_read_in=false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{  
   // TODO
   string check=elm.key();
   bool check_is_repeat=false;

   for(int i=0;i<_obj.size();i++)
   if(_obj[i].key()==check) check_is_repeat=true;
   //看key有沒有重複
   if(check_is_repeat)
     return false;
   
   else
   {
   _obj.push_back(elm);
    return true;
   }
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{  
   // TODO
  
  if(size()==0) return NAN;
  else
  { float ave=0.0;
    for(int i=0;i<size();i++)
    ave+=_obj[i].value();
    ave/=size();
    return ave;
  }

   

}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;

  if(size()!=0)
  { auto max=std::max_element(  _obj.begin(),_obj .end(),
                             [](  DBJsonElem a,  DBJsonElem b )
                             {
                                 return a.value()< b.value();
                             } );
   maxN=(max-_obj.begin());
   idx= maxN;
  }
  
  return maxN;
 
}
//Q:不是很懂為啥一開始要傳idx進來 ,還要 resize她？？？？
// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
  if(size()!=0)
  { auto min=std::min_element(_obj.begin(), _obj .end(),
                             [](  DBJsonElem a,  DBJsonElem b )
                             {
                                 return a.value()< b.value();
                             } );
  minN=(min-_obj.begin());
  idx= minN;
  }
  
return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(size()==0) return 0;
  else
  {int s = 0;
  for(int i=0;i<size();i++)
   s=s+_obj[i].value();
   return s;
  }
}
