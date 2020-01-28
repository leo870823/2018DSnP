/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
//參考   DBRead
/*
Guarded command errors:
1. lexOptions() == false
2. If the specified “numObjects” is not a legal integer or is not a positive number.
3. If the parameter “numObjects” is not specified or is specified multiple times.
4. If the specified “arraySize” is not a legal integer or is not a positive number.
5. If the parameter “arraySize” is specified multiple times.
6. Any other syntax error with respect to the command usage.
7. Requested memory of the object or array is greater than the block size of MemBlock (by catching “bad_alloc()”).
*/
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // Use try-catch to catch the bad_alloc exception

   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
//沒有東西在裡面
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
//有大於2個command
 //  if(options.size()>2)
   //   return CmdExec::errorOption(CMD_OPT_EXTRA,options[options.size()-1]);
      
  bool array_mode = false;
  bool array_hold = false;
  vector<int> number_count;
  vector< size_t > array_count;
   size_t numObjects=0;

  for (size_t i = 0, n = options.size(); i < n; ++i) 
  {
     if (myStrNCmp("-Array", options[i], 2) == 0) 
      {
         if (array_mode) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         array_mode = true;
         array_hold = true;
      }   
      else
      {  int object_static =static_cast<int>(numObjects);
         //2. If the specified “numObjects” is not a legal integer or is not a positive number.
         if(!(myStr2Int(options[i], object_static)))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i] );
         else
         {       
                 numObjects=static_cast<size_t>(object_static);
                 //not a positive number.
                 if(numObjects<=0||numObjects>INT_MAX) 
                 return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i] );
                 
        /*
        4. If the specified “arraySize” is not a legal integer or is not a positive number.
        5. If the parameter “arraySize” is specified multiple times.
        */
       //是array到array count
          if(array_hold) 
         { 
          array_count.push_back(numObjects);
          array_hold=false;
         }
         else number_count.push_back(numObjects);

         }
      }

}

//3. If the parameter “numObjects” is not specified or is specified multiple times.
if(number_count.empty()) return  CmdExec::errorOption(CMD_OPT_MISSING,"");
if(number_count.size()>=2) return  CmdExec::errorOption(CMD_OPT_EXTRA,to_string(number_count[1]));

/////////////////////////////////////////////////////////////////////////////////////////////////////
  if(array_mode)
  { 
    if(array_count.empty()) return  CmdExec::errorOption(CMD_OPT_MISSING,"");
    else if(array_count.size()>=2) CmdExec::errorOption(CMD_OPT_EXTRA,to_string(array_count[1]));
    else
    { 
    try{ mtest.newArrs(number_count[0],array_count[0]);}
    catch(...) {}
    }
  }

  else 
{   try
    {mtest.newObjs(number_count[0]);}
     catch(...){}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
/*
1. lexOptions() == false
2. If both “-Index” and “-Random” are specified.
4
3. If none of “-Index” or “-Random” is specified.
4. If the parameter “-Index” is specified multiple times.
5. If the specified “objId” (for “-Index”) is not a legal integer, is smaller
than 0, or is greater than or equal to the size of “_objList” (if no “-Array”) or “_arrList” (if with “-Array”). Print out error message like:
// Assume size of object/array list is 10, and 12 is the specified parameter Size of object list (10) is <= 12!! or
Size of array list (10) is <= 12!!
6. If the parameter “-Random” is specified multiple times.
7. If the specified “numRandId” is not a legal integer, or is not a positive number.
8. If the parameter “-Random (size_t numRandId)” is specified but the “_objList” (if no “-Array”) or “_arrList” (if with “-Array”) is empty. In such case, print out error message:
Size of object list is 0!! or
Size of array list is 0!!
9. Any other syntax error with respect to the command usage.
*/
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{ // TODO
 
  // check option
   vector<string> options;
   bool array_mode=false;

   bool index_mode=false;
   bool index_hold=false;
   int index_count =0;

   bool random_mode=false;
   bool random_hold=false;
   int random_count =0;

   size_t index=0;
   //不只有一個command，就應該這樣讀

   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

  
   for (size_t i = 0, n = options.size(); i < n; ++i) 
   {  
       if ( myStrNCmp ("-Array", options[i], 2) == 0) 
      {
         if (array_mode) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         array_mode = true;
      }   
      
      else if (myStrNCmp("-Index", options[i], 2) == 0) 
      {  //case: -r -i
        index_count++;
        if (index_mode||random_mode) 
        {  
        if(random_hold) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
        else  return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);

        }
        index_mode= true;
        index_hold=true;

      }
      else if (myStrNCmp("-Random", options[i], 2) == 0)
      {   random_count ++;
              if (index_mode||random_mode) 
          { //case:-i -r
           if(index_hold) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
           else   return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);

          }
         random_mode=true;
         random_hold= true;
         
      }
//////////////////////////////////////////////////////////////////////////////////////////////////
      //handle index
     else  if(index_hold||random_hold)
     {   
         int object_static=0;

         if(!(myStr2Int(options[i], object_static)))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i] );
         else 
        { if(object_static<0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(object_static));
         index=static_cast<size_t>(object_static);
         index_hold=false;
         random_hold=false;
        }
     }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
     //handle 不同的delete方式 randon >=1 index>=0
     //assert(index_mode!=random_mode);
 //if (options.size()<2) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
  //if(index<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(index));

  


/////////////////////////////////////////////////////////////////////////////////////////////////////
     if(array_mode)
     {    //cout<<index<<endl;
          if(index_mode==random_mode) 
          if(index_mode==true) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
          else return CmdExec::errorOption(CMD_OPT_MISSING,"");
          
          if(index_mode)
          {  if(index>= mtest.getArrListSize()&&index!=0) 
           { cerr<<"Size of array list ("<<mtest.getArrListSize()<<") is <= "<<index<<"!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(index));
           }
           if(index==0&&mtest.getArrListSize()==0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,"0");

           mtest.deleteArr(index);
          }
          else if(random_mode)
          {   /*
             for(int i=0;i<index;i++)
             mtest.deleteArr(rnGen(mtest.getArrListSize()));
             */
            if(index==0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(index));
             size_t x=0;
             if(mtest.getArrListSize()==0) 
             { cerr<<"Size of array list is 0!!"<<endl;
              cerr<<"Error: Illegal option!! (-r)"<<endl;
             }
             else 
             for(int i=0;i<index;i++)
             {  x=rnGen(mtest.getArrListSize());
                if(x<mtest.getArrListSize())
                mtest.deleteArr(x);
             }
          }
        
     }
     else
     {  
        if(index_mode==random_mode) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
         if(index_mode)
         { if(index>= mtest.getObjListSize()) 
           { cerr<<"Size of object list ("<<mtest.getObjListSize()<<") is <= "<<index<<"!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(index));
           }
           mtest.deleteObj(index);
         }
          else if(random_mode)
          {  if(index==0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(index));
                   size_t x=0;
             for(int i=0;i<index;i++)
             {  x=rnGen(mtest.getObjListSize());
                if(x<mtest.getObjListSize())
             mtest.deleteObj(x);
             }
          }
     }
   
   return CMD_EXEC_DONE;

    if(index_count==1&&random_count ==0&&index==0&&!array_mode) 
  return CmdExec::errorOption(CMD_OPT_MISSING,"-i");
   else if(index_count>=2&&random_count ==0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,"-i");

     if(index_count==0&&random_count ==1&&index==0&&!array_mode) 
     if(!array_mode) return CmdExec::errorOption(CMD_OPT_MISSING,"-r");
   else if(index_count==0&&random_count >=2) return CmdExec::errorOption(CMD_OPT_ILLEGAL,"-r");







}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


