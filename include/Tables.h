/* Tables.h  符号表和常数表类*/

#pragma once

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <sstream>

using namespace std;

class tb_value
{
public:
  string tb_name, tb_type;
  int tb_offset, tb_order;
  void tb_print(std::ostream &out);
  tb_value() {}
  tb_value(const string &a, const string &b, int c, int d) : tb_name(a), tb_type(b), tb_offset(c), tb_order(d) {}
};

class Tables
{
private:
  int value_tot;
  map<string, tb_value> IDTables;
  map<int, tb_value> ConstTables;
  std::vector<std::string> insertionOrder;

public:
  int id_tot = 0;
  stack<int> offset;
  Tables() : value_tot(0) {}
  string getaddress(const string &);
  int enter(const string &, const string &, int);
  string newtemp(const string &);
  int width(const string &);
  int lookup(const string &);
  string lookup_type(const string &);
  void TablePrint(std::ostream &out);
};
