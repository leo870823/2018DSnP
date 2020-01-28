/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
  
 
  if( _dofileStack.size()==253)
   {  cerr<<"Error: dofile stack overflow ("<<_dofileStack.size()-1<<")"<<endl;
      //加了這行memory stack會被清掉
     // while(!_dofileStack.empty()) closeDofile();
      return false;
   }

 _dofile = new ifstream(dof.c_str());//dofile指著ifstream

   if( _dofile ->is_open())
   { 
     _dofileStack.push(_dofile );
    return true;
   }

   else 
   { //case: do t 
     _dofile->close();
      delete _dofile;
      if(!_dofileStack.empty())
     _dofile=_dofileStack.top();
     else _dofile=NULL;
     return false;
   }
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);

    _dofile->close();
   delete _dofile;
   if(!_dofileStack.empty())
   _dofileStack.pop();

   if(_dofileStack.empty())
   _dofile=NULL;
   else
  {_dofile =_dofileStack.top();
 
  }

}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) 
   {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd

   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   // bool check=(_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   //看command 是dofile 還是command
   
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) 
   {
      string option;
      CmdExec * e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
   for(auto& x : _cmdMap) 
   x.second->help();
   cout<<endl;
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s)) //default nprint＝-1，超過size，把長度修成size
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap

//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.

// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"

//parse command 決定哪個command會被呼叫QQ with stl<map>
//option一開始是空的用passed by ref 進來

CmdExec* CmdParser::parseCmd(string& option)
{  
      // TODO...
   assert(_tempCmdStored == false);
   assert(!_history.empty());

   string str = _history.back();
   int index_optional_initial=myStrGetTok(str,option); //自己去切割
 //assert(str[0] != 0 && str[0] != ' ');
   CmdExec*  e=getCmd(option);
   if(e==NULL) 
   {
    cerr<<"Illegal command!! "<<option<<endl;
    return NULL;
   }
  else 
  { if(index_optional_initial!=str.npos)
    option=str.substr(index_optional_initial);//把option存入
    else 
    option="";
    return e;
  }
 
}
// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
 //    --- 1.1 ---
 //    [Before] Null cmd
 //    cmd> $
 //    --- 1.2 ---
 //    [Before] Cmd with ' ' only
 //    cmd>     $
 //    [After Tab]
 //    ==> List all the commands, each command is printed out by:
 //           cout << setw(12) << left << cmd;
 //    ==> Print a new line for every 5 commands
 //    ==> After printing, re-print the prompt and place the cursor back to
 //        original location (including ' ')
 //
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
  //    --- 2.1 ---
  //    [Before] partially matched (multiple matches)
  //    cmd> h$                   // partially matched
  //    [After Tab]
  //    HELp        HIStory       // List all the parially matched commands
  //    cmd> h$                   // and then re-print the partial command
  //    --- 2.2 ---
  //    [Before] partially matched (multiple matches)
  //    cmd> h$llo                // partially matched with trailing characters
  //    [After Tab]
  //    HELp        HIStory       // List all the parially matched commands
  //    cmd> h$llo                // and then re-print the partial command
  //
// 3. LIST THE SINGLY MATCHED COMMAND
  //    ==> In either of the following cases, print out cmd + ' '
  //    ==> and reset _tabPressCount to 0
  //    --- 3.1 ---
  //    [Before] partially matched (single match)
  //    cmd> he$
  //    [After Tab]
  //    cmd> heLp $               // auto completed with a space inserted
  //    --- 3.2 ---
  //    [Before] partially matched with trailing characters (single match)
  //    cmd> he$ahah
  //    [After Tab]
  //    cmd> heLp $ahaha
  //    ==> Automatically complete on the same line
  //    ==> The auto-expanded part follow the strings stored in cmd map and
  //        cmd->_optCmd. Insert a space after "heLp"
  //    --- 3.3 ---
  //    [Before] fully matched (cursor right behind cmd)
  //    cmd> hElP$sdf
  //    [After Tab]
  //    cmd> hElP $sdf            // a space character is inserted
  //
// 4. NO MATCH IN FITST WORD
  //    --- 4.1 ---
  //    [Before] No match
  //    cmd> hek$
  //    [After Tab]
  //    ==> Beep and stay in the same location
  //
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
  //    --- 5.1 ---
  //    [Before] Already matched on first tab pressing
  //    cmd> help asd$gh
  //    [After] Print out the usage for the already matched command
  //    Usage: HELp [(string cmd)]
  //    cmd> help asd$gh
  
// 6簡單的說，如下情形，當 "cursor 落在第二個字開頭，且 tab 已經被按過
  //一次之後"，要考慮三種情形：

  //1. 目前目錄下有多個檔案，且彼此之間沒有共同的 prefix
  //2. 目前目錄下有多個檔案，且彼此之間有共同的 prefix
  //3. 目前目錄下只有單一個檔案
  //
  // FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
  //    ==> Note: command usage has been printed under first tab press
  //    ==> Check the word the cursor is at; get the prefix before the cursor
  //    ==> So, this is to list the file names under current directory that
  //        match the prefix
  //    ==> List all the matched file names alphabetically by:
  //           cout << setw(16) << left << fileName;
  //    ==> Print a new line for every 5 commands
  //    ==> After printing, re-print the prompt and place the cursor back to
  //        original location
  //    --- 6.1 ---
  //    Considering the following cases in which prefix is empty:
  //    --- 6.1.1 ---
  //    [Before] if prefix is empty, and in this directory there are multiple
  //             files and they do not have a common prefix,
  //    cmd> help $sdfgh
  //    [After] print all the file names
  //    .               ..              Homework_3.docx Homework_3.pdf  Makefile
  //    MustExist.txt   MustRemove.txt  bin             dofiles         include
  //    lib             mydb            ref             src             testdb
  //    cmd> help $sdfgh
  //    --- 6.1.2 ---
  //    [Before] if prefix is empty, and in this directory there are multiple
  //             files and all of them have a common prefix,
  //    cmd> help $orld
  //    [After]
  //    ==> auto insert the common prefix and make a beep sound
  //    ==> DO NOT print the matched files
  //    cmd> help mydb-$orld
  //    --- 6.1.3 ---
  //    [Before] if prefix is empty, and only one file in the current directory
  //    cmd> help $ydb
  //    [After] print out the single file name followed by a ' '
  //    cmd> help mydb $

  //    --- 6.2 ---
  //    [Before] with a prefix and with mutiple matched files
  //    cmd> help M$Donald
  //    [After]
  //    Makefile        MustExist.txt   MustRemove.txt
  //    cmd> help M$Donald

  //    --- 6.3 ---
  //    [Before] with a prefix and with mutiple matched files,
  //             and these matched files have a common prefix
  //    cmd> help Mu$k
  //    [After]
  //    ==> auto insert the common prefix and make a beep sound
  //    ==> DO NOT print the matched files
  //    cmd> help Must$k
  //    --- 6.4 ---
  //    [Before] with a prefix and with a singly matched file
  //    cmd> help MustE$aa
  //    [After] insert the remaining of the matched file name followed by a ' '
  //    cmd> help MustExist.txt $aa
  //    --- 6.5 ---
  //    [Before] with a prefix and NO matched file
  //    cmd> help Ye$kk
  //    [After] beep and stay in the same location
  //    cmd> help Ye$kk
  //
  //    [Note] The counting of tab press is reset after "newline" is entered.
  //
// 7. FIRST WORD NO MATCH
  //    --- 7.1 ---
  //    [Before] Cursor NOT on the first word and NOT matched command
  //    cmd> he haha$kk
  //    [After Tab]
  //    ==> Beep and stay in the same location

void
CmdParser::listCmd(const string& str)
{ 
  ////////////////////////////////////////////////////////////////////////////////
  //////////////////////initialize///////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
    bool string_is_all_space=true;
     for(int i=0;i<str.length();i++)
     if(str[i]!=' ') string_is_all_space=false;

   string Tem_s;

   if(!string_is_all_space)
   Tem_s=str.substr(str.find_first_not_of(" "));


  int str_find_pos=Tem_s.find_last_of(" ");

  string after_whitespace;
 //check其沒有找出去
  if(str_find_pos<Tem_s.length())
  after_whitespace=Tem_s.substr(str_find_pos+1); 
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////case1///////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  if(string_is_all_space) //讓前面有' '進得去也讓有多個' '也進得去
 { 
    cout<<endl;
   int i=1;
    for(const auto& x : _cmdMap) 
   { cout<< setw(12) << left <<x.first+x.second->getOptCmd();
    if(i%5==0) cout<<endl;
    i++;
   }
   reprintCmd();
   return;
 }
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////5+6+7///////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  //case5:配對
if(0<str_find_pos&&str_find_pos<str.length()) //whitespace
{ 
  if(_tabPressCount==1) 
  
     {
         bool command_match=false;
       string tem_s(Tem_s);//存入tem暫存
       int check;
       int pos_first_before_whitespace=tem_s.find_first_of(' ');
       tem_s=tem_s.erase(pos_first_before_whitespace);//first_before_whitespace

       for(const auto& x : _cmdMap)
       {
        
        check =myStrNCmp(x.first+x.second->getOptCmd(),tem_s,x.first.length());
         if( check==0)
         { 
           command_match=true;
           x.second->usage(cout<<endl);
           reprintCmd();
          // tab_hold=_tabPressCount;
           return;
         }
         //都沒找到qq->case7
       }
      
      // if(!command_match)
          { //_tabPressCount=0;
            mybeep();
           return;
          }
         
     }
     else//_tabPressCount!=0
   {     //tab_hold++;
     vector<string> files;
       string prefix =after_whitespace;//如果＆前有東西，就不是null
        
       if(listDir(files,after_whitespace,".")==0)//目錄讀取成功
    {
       if(after_whitespace=="")
     {     
          //--- 6.1.3 --- only one file
         if(files.size()==1) 
         { 
          for(const auto&x:files[0]) insertChar(x);
          insertChar(' ');
           return;
         }  
    
        else
        {    string file_common_prefix =find_common_prefix(files);
           //--- 6.1.1 --- multiple file //沒有common prefix

          if(file_common_prefix=="")
         {  cout<<endl;
         int i=1;
           for(const auto&x:files) 
            { 
              cout<< setw(16) << left <<x ;
              i++;
             if(i%5==0&&i!=files.size()) cout<<endl;
           }
            reprintCmd();

         }

         else
          //--- 6.1.2 ---//有common prefix
           {for(int i=0;i<file_common_prefix.size();i++) 
           insertChar(file_common_prefix[i]);
           //reprintCmd();
           mybeep();
           }
        }
       
     }
      else //有prefix case
      {   
          if(files.size()!=0)
            { 
            string check_common=find_common_prefix(files);
            if(files.size()!=1)
            {     //--- 6.2 ---
                if(check_common==after_whitespace)
               {
                int i=1;
                cout<<endl;
                for(const auto& x : files ) 
                 {
                   cout<< setw(16) << left << x;
                   if(i%5==0&&i!=files.size()) cout<<endl;
                    i++;
                 }
                 reprintCmd();
                 return;
               }
               else
               //--- 6.3 ---
                { 
                  check_common=check_common.substr(after_whitespace.size());
                 // reprintCmd();
                  for(const auto&x:check_common) insertChar(x);
                  mybeep();
                  return;
                }
            }
            else 
            // --- 6.4 --- 只有一個match
            { check_common=check_common.substr(after_whitespace.size());
              //reprintCmd();
              for(const auto&x:check_common) insertChar(x);
              insertChar(' ');
              return;
            }
          }
      //    --- 6.5 ---
      else 
      {mybeep();
       return;
      }      
      }
    }
  }
}

//case2+3+4
//  else
if(!str.empty())
{  
   //存一個vector來check
  vector<string> tem;
  int check;
  for(const auto& x : _cmdMap) 
 { 

   tem.push_back(x.first+x.second->getOptCmd());//把東西存進vector
    ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////case3///////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
   
   check= myStrNCmp(tem[tem.size()-1],Tem_s,x.first.length());
    string tem_s(Tem_s);//存入tem暫存

  if(check==0)//help history之後再考慮
{     tem_s=tem[tem.size()-1].erase(0,Tem_s.length());
      for(int i=0;i< tem_s.length();i++)
      insertChar(tem_s[i]);
      insertChar(' ');
      _tabPressCount=0;
      return;
} 

if(tem[tem.size()-1]=="HELp")
  {  std::transform(tem_s.begin(), tem_s.end(),tem_s.begin(), ::tolower); //tem先轉小寫
    if(tem_s=="he") 
   {  tem_s=tem[tem.size()-1].erase(0,Tem_s.length());
      for(int i=0;i< tem_s.length();i++)
      insertChar(tem_s[i]);
      insertChar(' ');
      _tabPressCount=0;
      return;
   }
  }
if(tem[tem.size()-1]=="HIStory")
  {  std::transform(tem_s.begin(), tem_s.end(),tem_s.begin(), ::tolower); //tem先轉小寫
    if(tem_s=="hi") 
   {  tem_s=tem[tem.size()-1].erase(0,Tem_s.length());
      for(int i=0;i< tem_s.length();i++)
      insertChar(tem_s[i]);
      insertChar(' ');
      _tabPressCount=0;
      return;
   }
  }
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////case2+4///////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  
/*
bool is_case2=false;
  string hold=tem[tem.size()-1];
  std::transform(hold.begin(), hold.end(),hold.begin(), ::tolower); //tem先轉小寫
  is_case2=(hold.find(Tem_s)!=std::string::npos);
  */
 if(Tem_s.length()<=tem[tem.size()-1].size())
 check = check= myStrNCmp(tem[tem.size()-1],Tem_s,Tem_s.length());
 else check=-1;
  static int end_line=1;  //作為case1 break trigger
  if(check==0)
  { 
    if(end_line==1) cout<<endl;
    cout<< setw(12) << left <<tem[tem.size()-1];
    if(end_line%5==0&&end_line<10) cout<<endl;
   // is_case2=false;
    end_line++;
  }
  //case4
  if(tem[tem.size()-1]=="Quit"&&end_line==1)
 {    /////////
   mybeep();
    return;
 }
  if(tem[tem.size()-1]=="Quit"&&end_line!=1) 
    {
     end_line=1;
     reprintCmd();
     return;
    }
//////////////////////////////////////////////////////////////////////////////////
  }
}

}



//為了達成polymorphism去控制介面
//
// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{   
  // TODO...
   CmdExec* e = 0;
   int check;
    for(const auto& x : _cmdMap) 
   { 
    check=myStrNCmp( x.first + ((x.second)->getOptCmd()),cmd,x.first.size());
      if(check ==0)
     {
     e=x.second;
     break;
     }
   }
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token

//不用後面多加指令case
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size())
    {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true

//抓出optional的指令, optional default = true
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
//return 下一個 del 的 position.
   if (!optional) 
   {
      if (token.size() == 0) 
      {  
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
  // No matter it is optional or not, n should be the end
   if (n != string::npos)
    {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
    }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   //return 下一個 del 的 position.
   while (token.size()) 
   {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
       //n:return 下一個 del 的 position.
   }
   if (nOpts != 0)
    {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

 string 
 CmdParser::find_common_prefix(vector<string> & files)
 {
    string standard=files[0];
    
    for(const auto & x:files)
    {
        int i=0;
       
        while(i!=x.length()||i!=standard.length())
       {
        if(x[i]!=standard[i])//找到第i個不ㄧ樣
        { standard=standard.substr(0,i);
            break;
        }
        i++;
       }
     //   if(x.length()>files[0].length()) break;
        
    }
 return standard;
 }