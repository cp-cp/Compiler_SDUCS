#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <climits>
using namespace std;
class Objector
{
public:
    class symbol
    {
    public:
        string name;
        int type = 0;
        string value;
        int offset = 0;
    };
    class quad
    {
    public:
        string op;
        string arg1;
        string arg2;
        string result;
        quad(const string &op, const string &arg1, const string &arg2, const string &result) : op(op), arg1(arg1), arg2(arg2), result(result) {}
        quad() {}
    };
    struct use_info
    {
        int use = -1;
        int live = -1;
    };

    vector<symbol> SymbolTable;        // 符号表
    vector<quad> Arylist;              // 四元式表
    vector<pair<int, int>> BasicBlock; // 基本块
    vector<int> TempTable;             // 临时变量表
    vector<int> IsLabel;               // 标识是否为标号
    vector<vector<string>> result;     // 储存结果
    vector<string> R = {"R0", "R1", "R2"};
    vector<array<use_info, 3>> LiveTable; // 用来记录每条四元式活跃信息
    map<string, set<string>> Aval, Rval;
    map<string, int> Use; // 记录变量使用点
    map<string, string>Cal = {
        {"+", "add"}, {"-", "sub"}, {"*", "mul"}, {"/", "div"}, {"==", "cmp;sete"}, {"!=", "cmp;setne"}, {"<", "cmp;setl"}, {"<=", "cmp;setle"}, {">", "cmp;setg"}, {">=", "cmp;setge"}, {"&&", "and"}, {"||", "or"}, {"!", "not"}};
    map<string, string> Jump = {
        {"j==", "je"}, {"j!=", "jne"}, {"j<", "jl"}, {"j<=", "jle"}, {"j>", "jg"}, {"j>=", "jge"}};
    int symbol_num; // 符号表个数
    int temp_num;   // 临时变量个数
    int quad_num;   // 四元式个数
    int Offset;     // 总体偏移量
    bool bad_input = false;

    void Input();
    void Genblock();
    void GetLive();
    void Translate();
    void Output();
    string GetAddress(string str);
    string getReg(quad q, int index);
    void ReaReg(string vari);
    void GenCal(string op, string ag, string ag1, int index);
    void GenMov(int index);
    void GenJ(quad tep, int Index);
    void SignLabel(quad tep,int Index);
    void GetJ(quad tep, int Index, int temp);
    void find(string &Reg);
    void liveOut(int j);
    public:
    void generate();
};