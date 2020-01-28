/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"
#include <string>
using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream &);
bool checknullstr(const char *);
ParseChar getChar(istream &);
string trim(const char *);

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
 /*
void CmdParser::cleanreadbufup()
{
 
  moveBufPtr(_readBuf);
  for (; _readBufPtr < _readBufEnd; _readBufPtr++)
    cout << ' ';
  moveBufPtr(_readBuf);
  _readBufPtr = _readBufEnd = _readBuf;
  *_readBufPtr = 0;
  for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';
}
void CmdParser::cleanreadbuf()
{
  moveBufPtr(_readBuf);
  _readBufPtr = _readBufEnd = _readBuf;
  *_readBufPtr = 0;
  for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';
}

*/
string trim(const char *_read)
{ //字串去頭去尾
  string tmp(_read);

  int begin, end = 0;
  begin = tmp.find_first_not_of(' ');
  end = tmp.find_last_not_of(' ');
  tmp = tmp.substr(begin, end - begin + 1);
  //
  return tmp;
}
bool checknullstr(const char *_read)
{
  string tmp(_read);
  bool checknull = false;
  for (int i = 0; i < tmp.size(); i++)
    if (tmp[i] != ' ')
      checknull = true;
  return checknull;
}

void CmdParser::readCmd()
{
  if (_dofile.is_open())
  {
    readCmdInt(_dofile);
    _dofile.close();
  }
  else
    readCmdInt(cin);
}

void CmdParser::readCmdInt(istream &istr) //利用繼承 傳fstream 和istream
{
  resetBufAndPrintPrompt();

  while (1)
  {
    ParseChar pch = getChar(istr);
    if (pch == INPUT_END_KEY)
      break;
    switch (pch)
    {
    case LINE_BEGIN_KEY:
    case HOME_KEY:
      moveBufPtr(_readBuf);
      break;
    case LINE_END_KEY:
    case END_KEY:
      moveBufPtr(_readBufEnd);
      break;
    case BACK_SPACE_KEY:

      if (_readBuf==_readBufPtr)
      mybeep();
      else
      { moveBufPtr(_readBufPtr - 1);
        deleteChar();}

      break;
    case DELETE_KEY:
      deleteChar();
      break;
    case NEWLINE_KEY:
      addHistory();
      cout << char(NEWLINE_KEY);
      resetBufAndPrintPrompt();
      break;
    case ARROW_UP_KEY:
      moveToHistory(_historyIdx - 1);
      break;
    case ARROW_DOWN_KEY:
      moveToHistory(_historyIdx + 1);
      break;
    case ARROW_RIGHT_KEY:
      moveBufPtr(_readBufPtr + 1); /* TODO */
      break;
    case ARROW_LEFT_KEY:
      moveBufPtr(_readBufPtr - 1); /* TODO */
      break;
    case PG_UP_KEY:
      moveToHistory(_historyIdx - PG_OFFSET);
      break;
    case PG_DOWN_KEY:
      moveToHistory(_historyIdx + PG_OFFSET);
      break;
    case TAB_KEY: /* TODO */

      insertChar(' ', TAB_POSITION - (_readBufPtr - _readBuf) % TAB_POSITION);
      break;
    case INSERT_KEY: // not yet supported; fall through to UNDEFINE
    case UNDEFINED_KEY:
      mybeep();
      break;
    default: // printable character
      insertChar(char(pch));
      break;
    }
#ifdef TA_KB_SETTING
    taTestOnly();
#endif
  }
}

// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool CmdParser::moveBufPtr(char *const ptr)
{
  if (ptr >= _readBuf && ptr <= _readBufEnd)
  {

    char *pos = _readBufPtr;
    if (ptr > _readBufPtr) //向右走
      for (; ptr > pos; pos++)
        cout << *pos;

    if (ptr < _readBufPtr) //往左走
      for (; ptr < pos; pos--)
        cout << '\b';

    _readBufPtr = pos; //assign
    return true;
  }

  else
  {
    mybeep();
    return false;
  }
}

// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool CmdParser::deleteChar()
{
  if (_readBufPtr == _readBufEnd)
  {
    mybeep();
    return false;
  }
  else
  {
    for (char *pos = _readBufPtr; pos < _readBufEnd; pos++)
      *pos = *(pos + 1);
    for (char *pos = _readBufPtr; pos < _readBufEnd; pos++)
      cout << *pos;
    cout << " ";
    for (char *pos = _readBufEnd; pos > _readBufPtr; pos--)
      cout << "\b";
    _readBufEnd--;
  }
  return true;

  // TODO...
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^          ^
//
// After calling insertChar('k', 3) ---
//
// cmd> This is the command
//              ^             ^
// cmd> This is kkkthe command
//                 ^          ^
//
void CmdParser::insertChar(char ch, int repeat)
{
  _readBufPtr += repeat;
  _readBufEnd += repeat;
  for (char *pos = _readBufEnd; pos >= _readBufPtr; pos--)
    *pos = *(pos - repeat);

  for (int i = 1; i <= repeat; i++)
    *(_readBufPtr - i) = ch; //insert 的

  for (char *pos = _readBufPtr - repeat; pos < _readBufEnd; pos++) //原本 的
    cout << *pos;
  for (char *pos = _readBufEnd - 1; pos >= _readBufPtr; pos--)
    cout << '\b';
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void CmdParser::deleteLine()
{
  // TODO...
  //////////////////////////////////////////////////
  char *ptr = _readBufPtr;
  for (; ptr > _readBuf; ptr--)
    cout << '\b';

  _readBufPtr = ptr; //_readBufPtr指在最前面

  for (; ptr < _readBufEnd; ptr++)
  {
    cout << " ";
    *_readBufPtr = '\0';
  }

  for (; ptr > _readBuf; ptr--)
    cout << '\b';

  ////////////////////////////////////////////

  _readBufEnd = _readBuf;
  *_readBufEnd = 0;
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void CmdParser::moveToHistory(int index)
{

  //傳進來的index做一下處理
  // If moving up... (i.e. index < _historyIdx)
  if (index < _historyIdx)
  {
    if (_historyIdx == 0) //1.
    {
      mybeep();
      return;
    }

   if (_historyIdx == _history.size()) //2.
    {
      string tem;
      tem.assign(_readBuf);
      _history.push_back(tem);
      //////////////////////////////////////////////
            moveBufPtr(_readBuf);
  for (; _readBufPtr < _readBufEnd; _readBufPtr++)
    cout << ' ';
  moveBufPtr(_readBuf);
  _readBufPtr = _readBufEnd = _readBuf;
  *_readBufPtr = 0;
  for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';

      /////////////////////////////////////////////
      _tempCmdStored = true;
    }
     if (index < 0)
      index = 0; //3.
  }
  

  if (index > _historyIdx)
  {

    if (_historyIdx == _history.size()) //1.
    {
      mybeep();
      return;
    }

    if (index >= _history.size())
      index = _history.size() - 1; //2.

     if (_tempCmdStored == true && _history.size() == index + 1) //先上後下
    {
      _historyIdx = index;
      retrieveHistory();
      _history.pop_back();
      _tempCmdStored = false;
      return;
    }
  }

  _historyIdx = index;
  retrieveHistory();
}
// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void CmdParser::addHistory()
{
  //checknullstr
  if (!checknullstr(_readBuf))
    return;
  //

  if (_history.size() != 0) //second case
  {

    if (_tempCmdStored == true)
    {
      if (_historyIdx = _history.size())
      {
        _history.pop_back();
        _history.push_back(trim(_readBuf));
        {    moveBufPtr(_readBuf);
         _readBufPtr = _readBufEnd = _readBuf;
         *_readBufPtr = 0;
          for (int i = 0; i < READ_BUF_SIZE; i++)
          _readBuf[i] = '\0';
                
        }
        _tempCmdStored = false;
      }
      else
      {
        *_history.end() = (trim(_readBuf));
        {
            moveBufPtr(_readBuf);
  _readBufPtr = _readBufEnd = _readBuf;
  *_readBufPtr = 0;
  for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';

        }
        _tempCmdStored = false;
      }
    }
    //temp沒存東西
    else
    {
      _history.push_back(trim(_readBuf));
     {
       moveBufPtr(_readBuf);
  _readBufPtr = _readBufEnd = _readBuf;
  *_readBufPtr = 0;
  for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';



     }
    }
  }

  else //initial case
  {
    _history.push_back(trim(_readBuf));
    {
   moveBufPtr(_readBuf);
   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufPtr = 0;
   for (int i = 0; i < READ_BUF_SIZE; i++)
    _readBuf[i] = '\0';
    }
  }

  _historyIdx = _history.size();
}

// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void CmdParser::retrieveHistory()
{
  deleteLine();
  strcpy(_readBuf, _history[_historyIdx].c_str());
  cout << _readBuf;
  _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
