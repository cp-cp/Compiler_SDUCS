/* Parser.cpp */
#include "../include/Parser.h"
#include "../include/file_in.h"

void Parser::GetGenerates()
{
    Generates.clear();
    for (auto &ss : generates)
    {
        istringstream sin(ss);
        string theleft;
        vector<string> theright;
        sin >> theleft;
        string tmp;
        sin >> tmp;
        while (sin >> tmp)
        {
            theright.push_back(tmp);
        }
        Generates.push_back(Generate(theleft, theright));
    }
}
void Parser::GetACTIONandGOTO()
{
    ACTION.clear();
    GOTO.clear();
    for (auto &ss : goto_list)
    {
        istringstream sin(ss);
        int a, d;
        string b;
        char c;
        sin >> a >> b >> c >> d;
        if (c == 'g')
            GOTO[a][b] = make_pair(c, d);
        else
            ACTION[a][b] = make_pair(c, d);
    }
}

void Parser:: GetGenerates(const string &filename)
{
    Generates.clear();
    string ss;
    ifstream fin;
    fin.open(filename, ios::in);
    if (!fin.is_open())
    {
        cout << "Error: Cannot open file " << filename << endl;
    }
    while (getline(fin, ss))
    {
        istringstream sin(ss);
        string theleft;
        sin >> theleft;
        if (sin.fail())
            break;
        vector<string> theright;
        string tmp;
        sin >> tmp;
        while (sin >> tmp)
        {
            theright.push_back(tmp);
        }
        Generates.push_back(Generate(theleft, theright));
    }
    fin.close();
}

void Parser::GetACTIONandGOTO(const string &filename)
{
    ACTION.clear();
    GOTO.clear();
    string ss;
    ifstream fin;
    fin.open(filename, ios::in);
    if (!fin.is_open())
    {
        std::cout << "Error: Failed to open file " << filename << std::endl;
    }
    while (getline(fin, ss))
    {
        istringstream sin(ss);
        int a, d;
        string b;
        char c;
        sin >> a;
        if (sin.fail())
            break;
        sin >> b >> c >> d;
        if (c == 'g')
            GOTO[a][b] = make_pair(c, d);
        else
            ACTION[a][b] = make_pair(c, d);
    }
    fin.close();
}

string Parser::PreProcess(const string &ss, bool &in_exegesis)
{
    int n = ss.length();
    for (int i = 0; i < n - 1; ++i)
    {
        string tmp = ss.substr(i, 2);
        if (tmp == "//" && !in_exegesis)
            return ss.substr(0, i);
        else if (tmp == "/*" && !in_exegesis)
            return ss.substr(0, i) + PreProcess(ss.substr(i + 2), in_exegesis = true);
        else if (tmp == "*/" && in_exegesis)
            return PreProcess(ss.substr(i + 2), in_exegesis = false);
    }
    return in_exegesis ? "" : ss;
}

void Parser::OUTPUT(const stack<pair<int, string>> &STK, const stack<bool> &followendl)
{
    stack<pair<int, string>> STK1 = STK, STK2;
    stack<bool> followendl1 = followendl, followendl2;
    while (!STK1.empty())
    {
        STK2.push(STK1.top()), STK1.pop();
        followendl2.push(followendl1.top()), followendl1.pop();
    }
    cout << endl
         << "-----------------------------------stack" << endl;
    while (!STK2.empty())
    {
        cout << STK2.top().first << ":" << STK2.top().second;
        cout << (followendl2.top() ? '\n' : ' ');
        STK2.pop(), followendl2.pop();
    }
    cout << endl
         << "-----------------------------------stack" << endl;
    getchar();
}

string Parser::stringinfile(const string &ss)
{
    return ss[0] == '\"' ? "\\\"" + ss.substr(1, ss.length() - 2) + "\\\"" : ss;
}

Parser::Parser(const string &s1, const string &s2) : CodeGenerator()
{
    GetGenerates();
    GetACTIONandGOTO();
    // GetGenerates(s1);
    // GetACTIONandGOTO(s2);
}

void Parser::AnalyzeAndOutput(const string &filename, int op, const vector<Token> &newtokens)
{
    string ss;

    stack<pair<int, string>> STK;
    stack<node> node_value;
    stack<bool> followendl;
    stack<int> node_id;

    vector<pair<int, string>> tmpnode;
    bool in_exegesis = false;
    int tot = -1, nowline = 0;
    STK.push(make_pair(0, "#"));
    node_value.push(node());
    node_id.push(tot);
    followendl.push(false);

    for (unsigned int i = 0; i < newtokens.size(); ++i)
    {
        if (op)
            OUTPUT(STK, followendl);
        string a;
        if (newtokens[i].type == "IDENT")
            a = "id";
        else if (newtokens[i].type == "INT")
            a = "UINT";
        else if (newtokens[i].type == "DOUBLE")
            a = "UFLOAT";
        else
            a = newtokens[i].value;
        int cur_state = STK.top().first;
        if (ACTION[cur_state].count(a))
        {
            pair<char, int> cur_action = ACTION[cur_state][a];
            if (cur_action.first == 'a')
                break;
            if (cur_action.first == 's')
            {
                STK.push(make_pair(cur_action.second, a));
                node tmpmap;
                tmpmap.value = newtokens[i].value;
                node_value.push(tmpmap);
                node_id.push(++tot);
                followendl.push(false);
            }
            else
            {
                tmpnode.clear();
                int tmp_num = cur_action.second;
                Generate tmp = Generates[tmp_num];
                vector<node> node_value_vec;
                for (unsigned int j = tmp.rhs.size() - 1; j >= 0; --j)
                {
                    tmpnode.push_back(make_pair(node_id.top(), STK.top().second));
                    node_value_vec.push_back(node_value.top());
                    STK.pop();
                    node_id.pop();
                    node_value.pop();
                    followendl.pop();
                    if (j == 0)
                        break;
                }
                cur_state = STK.top().first;
                STK.push(make_pair(GOTO[cur_state][tmp.lhs].second, tmp.lhs));
                reverse(node_value_vec.begin(), node_value_vec.end());
                node_value.push(CodeGenerator::Gen(tmp, node_value_vec, ++nowline));
                node_id.push(++tot);
                followendl.push(false);
                --i;
            }
        }
        else if (ACTION[cur_state].count("^")) // 存在空串转移，直接移进
        {
            pair<char, int> cur_action = ACTION[cur_state]["^"];
            if (cur_action.first == 'a')
                break;
            else if (cur_action.first == 's')
            {
                STK.push(make_pair(ACTION[cur_state]["^"].second, "^")); // 移进
                node_id.push(++tot);
                node_value.push(node()); // 空map
                followendl.push(false);
                --i;
            }
            else
            {
                tmpnode.clear();

                int tmp_num = cur_action.second;
                Generate tmp = Generates[tmp_num];
                vector<node> node_value_vec;
                for (unsigned int j = tmp.rhs.size() - 1; j >= 0; --j)
                {
                    tmpnode.push_back(make_pair(node_id.top(), STK.top().second));
                    node_value_vec.push_back(node_value.top());
                    STK.pop();
                    node_id.pop();
                    node_value.pop();
                    followendl.pop();
                    if (j == 0)
                        break; // unsigned int
                }
                cur_state = STK.top().first;

                STK.push(make_pair(GOTO[cur_state][tmp.lhs].second, tmp.lhs));
                reverse(node_value_vec.begin(), node_value_vec.end());
                node_value.push(CodeGenerator::Gen(tmp, node_value_vec, ++nowline));
                node_id.push(++tot);
                followendl.push(false);
                --i;
            }
        }
        else
        {
            break;
        }
    }

    if (op)
        OUTPUT(STK, followendl);
}

void Parser::Debug()
{
    cout << Generates.size() << endl;
    cout << Generates[0].rhs.size() << endl;
    cout << Generates[1].rhs.size() << endl;
    cout << Generates[2].rhs.size() << endl;
    cout << ACTION.size() << endl;
    cout << GOTO.size() << endl;
}