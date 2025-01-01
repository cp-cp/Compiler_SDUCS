/* CodeGenerator.h  中间代码生成器类，继承符号表和常数表类*/

#pragma once

#include "Tables.h"
#include "Grammar2Table.h"
#include <map>

using namespace std;

class ary4
{
public:
  string a, b, c, d;
  ary4() {}
  ary4(const string &a, const string &b, const string &c, int d) : a(a), b(b), c(c), d(to_string(d)) {};
  ary4(const string &a, const string &b, const string &c, const string &d) : a(a), b(b), c(c), d(d) {};
  void aryprint(void);
};

// quad,name,nextlist,truelist,falselist,type,width,op,place。
class node
{
public:
  string name, type, place, quad, value, op, width;
  vector<int> nextlist, truelist, falselist;
  node() {}
  node(const node &other)
      : name(other.name), type(other.type), place(other.place), quad(other.quad), value(other.value), op(other.op), width(other.width),
        nextlist(other.nextlist), truelist(other.truelist), falselist(other.falselist) {}

  node &operator=(const node &other)
  {
    if (this != &other)
    {
      name = other.name;
      type = other.type;
      place = other.place;
      quad = other.quad;
      nextlist = other.nextlist;
      truelist = other.truelist;
      falselist = other.falselist;
      value = other.value;
      op = other.op;
      width = other.width;
    }
    return *this;
  }
  void PrintNextList(void)
  {
    cout << "NextList: ";
    for (auto x : nextlist)
      cout << x << " ";
    cout << endl;
  };
  void PrintTrueList(void)
  {
    cout << "TrueList: ";
    for (auto x : truelist)
      cout << x << " ";
    cout << endl;
  };
  void PrintFalseList(void)
  {
    cout << "FalseList: ";
    for (auto x : falselist)
      cout << x << " ";
    cout << endl;
  };
  ~node() {}
};

class CodeGenerator : public Tables
{
private:
  vector<ary4> resultcode;
  int code_tot;
  int nextquad(void);
  void emit(const string &, const string &, const string &, int);
  void emit(const string &, const string &, const string &, const string &);
  void patch(int, const string &, const string &, const string &, int);

  node GenSUBPROG(const Generate &, const vector<node> &, int);
  node GenM(const Generate &, const vector<node> &, int);
  node GenN(const Generate &, const vector<node> &, int);

  node GenT(const Generate &, const vector<node> &, int);
  node GenID(const Generate &, const vector<node> &, int);
  node GenVARIABLE(const Generate &, const vector<node> &, int);

  node GenWhile(const Generate &, const vector<node> &, int);
  node GenIf(const Generate &, const vector<node> &, int);
  node GenASSIGN(const Generate &, const vector<node> &, int);
  node GenL(const Generate &, const vector<node> &, int);

  node GenEXPR(const Generate &, const vector<node> &, int);
  node GenORITEM(const Generate &, const vector<node> &, int);
  node GenANDITEM(const Generate &, const vector<node> &, int);
  node GenNOITEM(const Generate &, const vector<node> &, int);
  node GenRELITEM(const Generate &, const vector<node> &, int);
  node GenITEM(const Generate &, const vector<node> &, int);
  node GenFACTOR(const Generate &, const vector<node> &, int);

  node GenB(const Generate &, const vector<node> &, int);
  node GenBORTERM(const Generate &, const vector<node> &, int);
  node GenBANDTERM(const Generate &, const vector<node> &, int);
  node GenBFACTOR(const Generate &, const vector<node> &, int);

  node GenPLUS_MINUS(const Generate &, const vector<node> &, int);
  node GenMUL_DIV(const Generate &, const vector<node> &, int);
  node GenREL(const Generate &, const vector<node> &, int);

  node GenSCANF(const Generate &, const vector<node> &, int);
  node GenPRINTF(const Generate &, const vector<node> &, int);

  void backpatch(const vector<int> &, int);
  vector<int> merge(const vector<int> &, const vector<int> &);

public:
  CodeGenerator(void);
  node Gen(const Generate &, const vector<node> &, int);

  void CodePrint(std::ostream &out);
  void check_ins(stack<pair<int, string>>);
  bool error_flag = false;
};