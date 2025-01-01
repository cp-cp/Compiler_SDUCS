/* Tables.cpp */
#include "../include/Tables.h"

string Tables::getaddress(const string &ss)
{
  istringstream sin(ss);
  string ans = "";
  sin >> ans;
  sin >> ans;
  sin >> ans;
  return "( " + ans + " )";
}

int Tables::enter(const string &name, const string &type, int tboff)
{
  int nowtot = value_tot++;
  IDTables[name] = tb_value(name, type, tboff, nowtot);
  insertionOrder.push_back(name); // 记录插入顺序
  return nowtot;
}

string Tables::newtemp(const string &type)
{
  int tmp = offset.top();
  int ans = enter(to_string(value_tot) + "temp", type, offset.top()) - id_tot;
  offset.pop();
  offset.push(tmp + width(type));
  return to_string(ans);
}

int Tables::width(const string &type)
{
  if (type == "int")
    return 4;
  else if (type == "double")
    return 8;
  else
    return 0;
}

int Tables::lookup(const string &namearr)
{
  istringstream sin(namearr);
  string name;
  sin >> name;
  while (true)
  {
    if (IDTables.count(name))
      return IDTables[name].tb_order;
    else
      return -1;
  }
}

string Tables::lookup_type(const string &namearr)
{
  istringstream sin(namearr);
  string name;
  sin >> name;
  while (true)
  {
    if (IDTables.count(name))
      return IDTables[name].tb_type;
    else
      return "";
  }
}

void tb_value::tb_print(std::ostream &out)
{
  if (tb_type == "int")
    out << tb_name << " 0 null " << tb_offset << endl;
  else
    out << tb_name << " 1 null " << tb_offset << endl;
}

void Tables::TablePrint(std::ostream &out)
{
  if(out.tellp() == 0)
  {
    out << id_tot << endl;
    for (const auto &name : insertionOrder)
    {
    if ((name.size() <= 4 || "temp" != name.substr(name.size() - 4, 4)))
        IDTables[name].tb_print(out);
    }
    out << IDTables.size() - id_tot << endl;
  }
}