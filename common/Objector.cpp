#include "../include/Objector.h"

void Objector::Input()
{
    // freopen("/Users/cp_cp/CS/大三上/编译原理/实验/Compiler_C/files/test.in", "r", stdin);
    // freopen("../files/test.out", "w", stdout);
    string tmp;
    cin >> tmp;
    if (tmp == "Syntax")
    {
        cout << "halt" << endl;
        bad_input = true;
        return;
    }
    // cout<<tmp<<endl;
    symbol_num = stoi(tmp);
    // str = "";
    for (int i = 1; i <= symbol_num; i++)
    {
        symbol tmp;
        cin >> tmp.name >> tmp.type >> tmp.value >> tmp.offset;
        SymbolTable.push_back(tmp);
    }
    cin >> temp_num >> quad_num;
    if (quad_num == 1) // 如果四元式个数为1，则直接输出halt
    {
        cout << "halt" << endl;
        bad_input = true;
        return;
    }
    TempTable.resize(temp_num, 0); // 初始化临时变量
    auto t = SymbolTable.back();
    if (!t.type)
        Offset = t.offset + 4;
    else
        Offset = t.offset + 8;
    for (int i = 1; i <= quad_num; i++) // 读取四元式
    {
        string tmp, inputStr, str;
        string op, arg1, arg2, result;
        cin >> tmp;
        cin >> inputStr;
        int pos=inputStr.find(',');
        op=inputStr.substr(inputStr.find('(')+1,pos-inputStr.find('(')-1);
        arg1=inputStr.substr(pos+1,inputStr.find(',',pos+1)-pos-1);
        pos=inputStr.find(',',pos+1);
        arg2=inputStr.substr(pos+1,inputStr.find(',',pos+1)-pos-1);
        pos=inputStr.find(',',pos+1);
        result=inputStr.substr(pos+1,inputStr.find(')',pos+1)-pos-1);
        // cout<<op<<' '<<arg1<<' '<<arg2<<' '<<result<<endl;
        // str = "";
        // quad q(vec[0], vec[1], vec[2], vec[3]);
        quad q(op, arg1, arg2, result);
        Arylist.push_back(q);
    }
    IsLabel.resize(quad_num, 0);
    result.resize(quad_num);
}


void Objector::Genblock()
{
    int size = quad_num;
    vector<int> is_in; // 基本块入口数组,值为1表示是基本块入口
    is_in.resize(quad_num, 0);
    is_in[0] = 1; // 第一条四元式是入口
    for (int i = 0; i < quad_num; i++)
    {
        quad line = Arylist[i];
        if (line.op[0]=='j')
        {
            
            if (line.op == "j")
            {
                int tmp = stoi(line.result);
                is_in[tmp] = 1;
            }
            else
            {
                int tmp = stoi(line.result);
                is_in[tmp] = 1;
                if (i < quad_num - 1)
                    is_in[i + 1] = 1;
            }
        }
        else if (line.op == "R")
            is_in[i] = 1;
        else if (line.op == "W")
            is_in[i] = 1;
    }
    int i = 0;
    while (i < quad_num)
    {
        quad line = Arylist[i];
        if (is_in[i])//i是基本块入口
        { 
            if (i == quad_num - 1)
            {
                BasicBlock.push_back(make_pair(i, i));
                break;
            }
            for (int j = i + 1; j < quad_num; j++)
            {
                if (is_in[j])
                {
                    BasicBlock.push_back(make_pair(i, j - 1));
                    i = j;
                    break;
                }
                else if (Arylist[j].op[0] == 'j' || Arylist[j].op == "End")
                {
                    BasicBlock.push_back(make_pair(i, j));
                    i = j + 1;
                    break;
                }
            }
        }
        else
            i++;
    }
}

void Objector::GetLive()
{
    LiveTable.resize(quad_num);
    for (auto item : BasicBlock)
    {
        vector<use_info> SymbolInfo(symbol_num, {-1, 1}); // 符号表
        vector<use_info> TempInfo(temp_num, {-1, 0});     // 临时变量
        for (int i = item.second; i >= item.first; i--)
        {
            array<string, 3> ary = {Arylist[i].arg1, Arylist[i].arg2, Arylist[i].result};
            int index;
            for (int j = 2; j >= 0; j--)
            {
                if (ary[j][0] == 'T')
                {
                    if (ary[j].substr(0, 2) != "TB")
                        index = stoi(ary[j].substr(1, ary[j].find('_') - 1)); // 局部变量
                    else
                        index = stoi(ary[j].substr(2)); // 全局变量
                    auto &vars = ary[j][1] == 'B' ? SymbolInfo : TempInfo;
                    LiveTable[i][j] = vars[index];
                    if (j == 2)
                        vars[index] = {-1, 0};
                    else
                        vars[index] = {i, 1};
                }
            }
        }
    }
}

void Objector::find(string &Reg)
{

}

string Objector::GetAddress(string str) // 获取变量地址
{
    int ans;
    if (str[1] == 'B')//全局变量
        ans = SymbolTable[stoi(str.substr(2))].offset;
    else//局部变量
    {
        int x = stoi(str.substr(1, str.find('_') - 1));
        if (!TempTable[x])//如果临时变量表中没有该变量
        {
            Aval[str].insert(str);
            TempTable[x] = Offset;
            if (str.back() == 'i')
                Offset += 4;
            else
                Offset += 8;
        }
        ans = TempTable[x];
    }
    return "[ebp-" + to_string(ans) + "]";//返回变量地址
}

string Objector::getReg(quad q, int index)
{
    if (q.op[0] != 'j' && q.op != "W" && q.op != "R" && q.op != "End") // 如果四元式不是跳转、写、读、结束
    {
        for (auto Reg : Aval[q.arg1])
        {
            auto Val = Rval[Reg];
            if (Val.size() == 1 && *Val.begin() == q.arg1)
                if (q.arg1 == q.result || LiveTable[index][0].live == 0)
                    return Reg;
        }
    }
    for (int i = 0; i < R.size(); i++)//遍历寄存器,找到一个空闲寄存器
    {
        string Reg = R[i];
        bool flag = true;
        for (auto var : Rval[Reg])
            if (!Aval[var].count(var) && var != q.result)
            {
                flag = false;
                break;
            }
        if (flag)
            return Reg;
    }
    string Reg;
    for (const auto reg : R)
    {
        bool flag = true;
        for (auto Vari : Rval[reg])
            if (!Aval[Vari].count(Vari))
            {
                flag = false;
                break;
            }
        if (flag)
        {
            Reg = reg;
            break;
        }
    }
    if (Reg.empty()) // 如果寄存器为空
    {
        int maxx = -1;
        for (int i=0;i<R.size();i++)
        {
            string reg = R[i];
            int minn = INT_MAX;
            for (auto var : Rval[reg])//minn为当前寄存器中变量在Use表中的最小值
                minn = min(minn, Use[var]);
            if (minn > maxx)//如果minn大于maxx,则更新Reg和maxx
            {
                Reg = reg;
                maxx = minn;
            }
        }
    }

    for (auto var : Rval[Reg])
    {
        if (!Aval[var].count(var) && var != q.result)
        {
            result[index].push_back("mov " + GetAddress(var) + ", " + Reg);
        }
        Aval[var] = {var};
        if (var == q.arg1 || (var == q.arg2 && Rval[Reg].count(q.arg1)))
            Aval[var].insert(Reg);
    }
    Rval[Reg].clear();
    return Reg;
}

void Objector::ReaReg(string vari)
{
    if (vari.substr(0, 2) != "TB") // 如果变量不是全局变量
    {
        if (!Aval[vari].empty()) // 如果变量在寄存器中
        {
            vector<string> Era;
            for (auto &reg : Aval[vari])
            {
                if (reg[0] == 'R') // 如果寄存器
                {
                    Rval[reg].erase(vari);
                    Era.push_back(reg);
                }
            }
            for (auto &reg : Era)
                Aval[vari].erase(reg);
        }
    }
}

void Objector::GenCal(string op, string ag, string ag1, int index)
{
    if (Cal.find(op) != Cal.end())
    {
        string str = Cal[op];
        size_t pos = str.find(';');
        if (pos != string::npos)
        {
            result[index].push_back(str.substr(0, pos) + " " + ag + ", " + ag1);
            result[index].push_back(str.substr(pos + 1) + " " + ag);
        }
        else
        result[index].push_back(str + " " + ag + ", " + ag1);
    }
}

void Objector::GenMov(int index)
{
    for (int k = 0; k < SymbolTable.size(); k++)
    {
        string str = "TB" + to_string(k);
        if (!Aval[str].empty() && Aval[str].count(str) == 0)
            for (auto Reg : Aval[str])
                if (Reg[0] == 'R')
                {
                    result[index].emplace_back("mov [ebp-" + to_string(SymbolTable[k].offset) + "], " + Reg);
                    break;
                }
    }
}
void Objector::GenJ(quad tmp, int index)
{
    string xx = tmp.arg1;
    string yy = tmp.arg2;
    for (auto str : Aval[xx])
        if (str[0] == 'R')
        {
            xx = str;
            break;
        }
    for (auto str : Aval[yy])
        if (str[0] == 'R')
        {
            yy = str;
            break;
        }
    if (xx == tmp.arg1)
    {
        xx = getReg(tmp, index);
        result[index].push_back("mov " + xx + ", " + GetAddress(tmp.arg1));
    }
    string tmp_yy;
    if (yy[0] == 'T')
        result[index].push_back("cmp " + xx + ", " + GetAddress(yy));
    else
        result[index].push_back("cmp " + xx + ", " + yy);
    if (Jump.find(tmp.op) != Jump.end())
        result[index].push_back(Jump[tmp.op] + " ?" + tmp.result);
    int islabel = stoi(tmp.result);
    IsLabel[islabel] = true;
}
void Objector::SignLabel(quad tmp, int index)
{
    int islabel = stoi(tmp.result);
    for (int i = 0; i < BasicBlock.size(); i++)
    {
        if (BasicBlock[i].first == islabel)
        {
            IsLabel[islabel] = true;
            break;
        }
    }
}
void Objector::GetJ(quad tmp, int index, int temp)
{
    if (tmp.op == "jnz")
    {
        string Op1 = tmp.arg1;
        for (auto str : Aval[Op1])
            if (str[0] == 'R')
            {
                Op1 = str;
                break;
            }
        if (Op1 == tmp.arg1)
        {
            Op1 = getReg(tmp, temp);
            result[index].push_back("mov " + Op1 + ", " + GetAddress(tmp.arg1));
        }
        result[index].push_back("cmp " + Op1 + ", 0");
        result[index].push_back("jne ?" + tmp.result);
        SignLabel(tmp, index);
    }else if (tmp.op == "j")
    {
        result[index].push_back("jmp ?" + Arylist[temp].result);
        SignLabel(tmp, index);
    }
    else if (tmp.op == "End")
        result[index].push_back("halt");
    else if (tmp.op[0] == 'j')
        GenJ(tmp, index);
}
void Objector::liveOut(int j)
{
    GenMov(BasicBlock[j].second);
    GetJ(Arylist[BasicBlock[j].second], BasicBlock[j].second, BasicBlock[j].second);
    for (auto &Rv : Rval)
        Rv.second.clear();
    for (auto &Av : Aval)
        Av.second.clear();
    Use.clear();
}
void Objector::Translate()
{
    for (int j = 0; j < BasicBlock.size(); j++)
    {
        for (int i = BasicBlock[j].first; i <= BasicBlock[j].second; i++)
        {
            quad line = Arylist[i];
            if (line.op == "R")
                result[i].push_back("jmp ?read(" + GetAddress(line.result) + ")");
            else if (line.op == "W")
                result[i].push_back("jmp ?write(" + GetAddress(line.result) + ")");
            else if (line.op[0] != 'j' && line.op != "W" && line.op != "R" && line.op != "End")
            {
                if (line.arg1[0] == 'T')
                {
                    if (LiveTable[i][0].use == -1)
                        Use[line.arg1] = INT_MAX;
                    else
                        Use[line.arg1] = LiveTable[i][0].use;
                }
                if (line.arg2[0] == 'T')
                {
                    if (LiveTable[i][1].use == -1)
                        Use[line.arg2] = INT_MAX;
                    else
                        Use[line.arg2] = LiveTable[i][1].use;
                }
                if (line.result[0] == 'T')
                {
                    if (LiveTable[i][2].use == -1)
                        Use[line.result] = INT_MAX;
                    else
                        Use[line.result] = LiveTable[i][2].use;
                }
                string Rz = getReg(line, i); // 分配寄存器
                string x_prime = line.arg1;
                for (auto item : Aval[x_prime])
                    if (item[0] == 'R')
                    {
                        x_prime = item;
                        break;
                    }
                string y_prime = line.arg2;
                if (y_prime != "-")
                {
                    for (auto item : Aval[y_prime])
                        if (item[0] == 'R')
                        {
                            y_prime = item;
                            break;
                        }
                }
                if (x_prime == Rz) // 如果x_prime是寄存器
                {
                    if (y_prime != "-")
                    {
                        string tmp;
                        if (y_prime == line.arg2 && y_prime[0] == 'T')
                            tmp = GetAddress(y_prime);
                        else
                            tmp = y_prime;
                        GenCal(line.op, Rz, tmp, i);
                    }
                    if (line.op == "!")
                        result[i].push_back("not " + x_prime);
                    Aval[line.arg1].erase(Rz);
                }
                else // 如果x_prime不是寄存器
                {
                    string x;
                    if (x_prime == line.arg1 && x_prime[0] == 'T')
                        x = GetAddress(x_prime);
                    else
                        x = x_prime;
                    result[i].push_back("mov " + Rz + ", " + x);
                    if (y_prime != "-")
                    {
                        string tmp;
                        if (y_prime == line.arg2 && y_prime[0] == 'T')
                            tmp = GetAddress(y_prime);
                        else
                            tmp = y_prime; // 寄存器
                        GenCal(line.op, Rz, tmp, i);
                    }
                }
                if (y_prime == Rz)
                    Aval[line.arg2].erase(Rz);

                Rval[Rz] = {line.result};
                Aval[line.result] = {Rz};
                ReaReg(line.arg1);
                ReaReg(line.arg2);

                if (LiveTable[i][2].use == -1)
                    Use[line.result] = INT_MAX;
                else
                    Use[line.result] = LiveTable[i][2].use;
            }
        }
        // cout<<"----liveOut----"<<endl;
        liveOut(j);
    }
}

void Objector::Output()
{
    for (auto it : BasicBlock)
    {
        if (IsLabel[it.first])
            cout << "?" << it.first << ":" << endl;
        for (int i = it.first; i <= it.second; i++)
            for (auto str : result[i])
                cout << str << endl;
    }
}

void Objector::generate()
{
    Input(); // 读取输入
    if (!bad_input)
    {
        Genblock();  // 划分基本块
        GetLive();   // 求解活跃信息
        Translate(); // 生成目标代码
        Output();
    }
}