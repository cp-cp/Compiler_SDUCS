/* CodeGenerator.cpp */
#include "../include/CodeGenerator.h"

void ary4::aryprint()
{
    cout << "(" << a << "," << b << "," << c << "," << d << ")" << endl;
}

int CodeGenerator::nextquad()
{
    return code_tot;
}

void CodeGenerator::emit(const string &a, const string &b, const string &c, int d)
{
    ++code_tot;
    // cout << "--emit--" << endl;
    resultcode.push_back(ary4(a, b, c, d));
}

void CodeGenerator::emit(const string &a, const string &b, const string &c, const string &d)
{
    ++code_tot;
    // cout << "--emit--" << endl;
    resultcode.push_back(ary4(a, b, c, d));
}

void CodeGenerator::patch(int i, const std::string &a, const std::string &b, const std::string &c, int d)
{
    // std::cout << "Target index: " << resultcode.size() << " Patching index: " << i << " with (" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl;
    if (i < resultcode.size())
    {
        resultcode[i] = ary4(a, b, c, d);
    }
    else
    {
        std::cerr << "Error: Index out of bounds in patch(): " << i << std::endl;
        throw std::out_of_range("Index out of bounds in patch()");
        return;
    }
}

void CodeGenerator::CodePrint(std::ostream &out)
{
    out << resultcode.size() << endl;
    int u = 0;
    for (int i = 0; i < resultcode.size(); i++)
    {
        out << u++ << ": (" << resultcode[i].a << "," << resultcode[i].b << "," << resultcode[i].c << "," << resultcode[i].d << ")" << endl;
        // resultcode[i].aryprint();
        // if (i != resultcode.size() - 1)
        //     cout << endl;
    }
}

vector<int> CodeGenerator::merge(const vector<int> &list1, const vector<int> &list2)
{
    vector<int> mergedList = list1;
    mergedList.insert(mergedList.end(), list2.begin(), list2.end());
    return mergedList;
}

void CodeGenerator::backpatch(const vector<int> &p, int t)
{
    for (auto &i : p)
    {
        // cout <<"BackPatch: "<< i << " " << t << " New:"<<nextquad()<< endl;
        patch(i, resultcode[i].a, resultcode[i].b, resultcode[i].c, t);
    }
}
// Part0: 总程序
// PROG -> SUBPROG  {}

// SUBPROG -> M VARIABLES  *{backpatch(STATEMENT.nextlist,nxq);gen(End,-,-,-);}*
node CodeGenerator::GenSUBPROG(const Generate &gen, const vector<node> &values, int nowline)
{
    node SUBPROG;
    backpatch(values[2].nextlist, nextquad());
    emit("End", "-", "-", "-");
    return SUBPROG;
}
// M -> ^ *{OFFSET=0}*
node CodeGenerator::GenM(const Generate &gen, const vector<node> &values, int nowline)
{
    node M;
    offset.push(0);
    return M;
}
// N -> ^ *{N.quad=nxq}*
node CodeGenerator::GenN(const Generate &gen, const vector<node> &values, int nowline)
{
    node N;
    N.quad = to_string(nextquad());
    return N;
}

// Part1: 变量声明
/*
    VARIABLES -> VARIABLES VARIABLE ; *{}*
    VARIABLES -> VARIABLE ; *{}*
*/

/*
    T -> int *{T.type=int;T.width=4;}*
    T -> double *{T.type=double;T.width=8;}*
*/
node CodeGenerator::GenT(const Generate &gen, const vector<node> &values, int nowline)
{
    node T;
    if (values[0].value == "int")
    {
        T.type = "int";
        T.width = "4";
    }
    else
    {
        T.type = "double";
        T.width = "8";
    }
    return T;
}

// ID -> id *{ID.name=id}*
node CodeGenerator::GenID(const Generate &gen, const vector<node> &values, int nowline)
{
    node ID;
    ID.name = values[0].value;
    return ID;
}
// VARIABLE ->T ID              *{enter(ID.name,type=T.type,            offset=OFFSET);OFFSET+=T.width;         VARIABLE.type=T.type;           VARIABLE.width=T.width}*
// VARIABLE -> VARIABLE_1 , ID  *{enter(ID.name,type=VARIABLE_1.type,   offset=OFFSET);OFFSET+=VARIABLE_1.width;VARIABLE.type=VARIABLE_1.type;  VARIABLE.width=VARIABLE_1.width;}*
node CodeGenerator::GenVARIABLE(const Generate &gen, const vector<node> &values, int nowline)
{
    node VARIABLE;
    int tmp = offset.top();
    if (lookup(values[values.size() - 1].name) != -1)
        error_flag = true;
    enter(values[values.size() - 1].name, values[0].type, offset.top());
    offset.pop();
    offset.push(tmp + stoi(values[0].width));
    VARIABLE.type = values[0].type;
    VARIABLE.width = values[0].width;
    id_tot++;
    return VARIABLE;
}

// Part2: 基本语句

// STATEMENT -> ASSIGN *{STATEMENT.nextlist=mklist()}*
// STATEMENT -> SCANF *{STATEMENT.nextlist=mklist()}*
// STATEMENT -> PRINTF *{STATEMENT.nextlist=mklist()}*
// STATEMENT -> ^ *{STATEMENT.nextlist=mklist()}*

// STATEMENT -> { L ; } *{STATEMENT.nextlist=L.nextlist}*

// STATEMENT -> while N_1 B do N_2 STATEMENT_1 *{backpatch(STATEMENT_1.nextlist,N_1.quad);backpatch(B.truelist,N_2.quad);STATEMENT.nextlist=B.falselist;gen(j,-,-,N_1.quad)}*
node CodeGenerator::GenWhile(const Generate &gen, const vector<node> &values, int nowline)
{
    node STATEMENT;
    backpatch(values[5].nextlist, stoi(values[1].quad));
    backpatch(values[2].truelist, stoi(values[4].quad));
    STATEMENT.nextlist = values[2].falselist;
    emit("j", "-", "-", values[1].quad);
    return STATEMENT;
}

// STATEMENT -> if B then N STATEMENT_1 *{backpatch(B.truelist,N.quad),STATEMENT.nextlist=merge(B.falselist,STATEMENT_1.nextlist)}*
node CodeGenerator::GenIf(const Generate &gen, const vector<node> &values, int nowline)
{
    node STATEMENT;
    backpatch(values[1].truelist, stoi(values[3].quad));
    // cout << "IF::" << values[1].truelist.size() << " " << values[3].quad << endl;
    STATEMENT.nextlist = merge(values[1].falselist, values[4].nextlist);
    return STATEMENT;
}

// ASSIGN -> ID = EXPR *{p=lookup(ID.name);gen(=,EXPR.place,-,p) }*
node CodeGenerator::GenASSIGN(const Generate &gen, const vector<node> &values, int nowline)
{
    node ASSIGN;
    int p = lookup(values[0].name);
    if (p == -1)
        error_flag = true;
    // 如果是int
    if (values[2].type == "int")
    {
        emit("=", values[2].place, "-", "TB" + to_string(p));
    }
    else
    {
        emit("=", values[2].place, "-", "TB" + to_string(p));
    }
    return ASSIGN;
}

// L -> L_1 ; N STATEMENT *{backpatch(L1.nextlist,N.quad),L.nextlist=STATEMENT.nextlist}*
// L -> STATEMENT *{L.nextlist=STATEMENT.nextlist}*
node CodeGenerator::GenL(const Generate &gen, const std::vector<node> &values, int nowline)
{
    node L;
    // std::cout << "GenL called at line: " << nowline << std::endl;
    if (values.size() == 1)
    {
        // std::cout << "Processing with values.size() == 1" << std::endl;
        L.nextlist = values[0].nextlist;
        // std::cout << "Updated L.nextlist" << std::endl;
    }
    else if (values.size() == 4)
    {

        // std::cout << "Processing with values.size() == 4" << std::endl;
        int quadVal = stoi(values[2].quad);
        // std::cout << "Backpatching at quad: " << quadVal << std::endl;
        backpatch(values[0].nextlist, quadVal);
        L.nextlist = values[3].nextlist;
        // std::cout << "Backpatched and updated L.nextlist" << std::endl;
    }
    else
    {
        // 错误处理
        std::cerr << "Invalid values size in GenL: " << values.size() << std::endl;
        throw std::runtime_error("Invalid values size in GenL");
    }
    // std::cout << "GenL returning" << std::endl;
    return L;
}
// Part3: 数值表达式

// EXPR ->EXPR_1 || ORITEM *{EXPR.place=newtemp(int);EXPR.type=int;gen(||,EXPR_1.place,ORITEM.place,EXPR.place)}*
// EXPR -> ORITEM *{EXPR.place=ORITEM.place;EXPR.type=ORITEM.type}*
node CodeGenerator::GenEXPR(const Generate &gen, const vector<node> &values, int nowline)
{
    node EXPR;
    if (values.size() == 1)
    {
        EXPR.place = values[0].place;
        EXPR.type = values[0].type;
        // cout << "EXPR:::" << EXPR.place << " " << EXPR.type << endl;
    }
    else
    {
        EXPR.place = "T" + newtemp("int") + "_i";
        EXPR.type = "int";
        emit("||", values[0].place, values[2].place, EXPR.place);
    }
    return EXPR;
}
// ORITEM -> ORITEM_1 && ANDITEM *{ORITEM.place=newtemp(int);ORITEM.type=int;gen(&&,ORITEM_1.place,ANDITEM.place,ORITEM.place)}*
// ORITEM -> ANDITEM *{ORITEM.place=ANDITEM.place;ORITEM.type=ANDITEM.type}*
node CodeGenerator::GenORITEM(const Generate &gen, const vector<node> &values, int nowline)
{
    node ORITEM;
    if (values.size() == 1)
    {
        ORITEM.place = values[0].place;
        ORITEM.type = values[0].type;
    }
    else
    {
        ORITEM.place = "T" + newtemp("int") + "_i";
        ORITEM.type = "int";
        emit("&&", values[0].place, values[2].place, ORITEM.place);
    }
    return ORITEM;
}
// ANDITEM -> NOITEM *{ANDITEM.place=NOITEM.place;ANDITEM.type=NOITEM.type;}*
// ANDITEM -> ! NOITEM *{ANDITEM=newtemp(int);ANDITEM.type=int;gen(!,NOITEM.place,-,ANDITEM.place)}*
node CodeGenerator::GenANDITEM(const Generate &gen, const vector<node> &values, int nowline)
{
    node ANDITEM;
    if (values.size() == 1)
    {
        ANDITEM.place = values[0].place;
        ANDITEM.type = values[0].type;
    }
    else
    {
        ANDITEM.place = "T" + newtemp("int") + "_i";
        ANDITEM.type = "int";
        emit("!", values[1].place, "-", ANDITEM.place);
    }
    return ANDITEM;
}
// NOITEM -> NOITEM_1 REL RELITEM *{NOITEM.place=newtemp(int);NOITEM.type=int;gen(REL.op,NOITEM_1.place,RELITEM.place,NOITEM.place)}*
// NOITEM -> RELITEM *{NOITEM.place=RELITEM.place;NOITEM.type=RELITEM.type}*
node CodeGenerator::GenNOITEM(const Generate &gen, const vector<node> &values, int nowline)
{
    node NOITEM;
    if (values.size() == 1)
    {
        NOITEM.place = values[0].place;
        NOITEM.type = values[0].type;
    }
    else
    {
        NOITEM.place = "T" + newtemp("int") + "_i";
        NOITEM.type = "int";
        // cout<<"NOITEM:::"<<NOITEM.place << " " << NOITEM.type << endl;
        emit(values[1].op, values[0].place, values[2].place, NOITEM.place);
    }
    return NOITEM;
}
// RELITEM -> RELITEM_1 PLUS_MINUS ITEM *{RELITEM.place=newtemp(RELITEM_1.type);RELITEM.type=RELITEM_1.type;gen(PLUS_MINUS.op,RELITEM_1.place,ITEM.place,RELITEM.place)}*
// RELITEM -> ITEM *{RELITEM.place=ITEM.place;RELITEM.type=ITEM.type}*
node CodeGenerator::GenRELITEM(const Generate &gen, const vector<node> &values, int nowline)
{
    node RELITEM;
    if (values.size() == 1)
    {
        // cout << "ITEM->RELITEM:::" << values[0].place << " " << values[0].type << endl;
        RELITEM.place = values[0].place;
        RELITEM.type = values[0].type;
    }
    else
    {
        RELITEM.place = "T" + newtemp(values[0].type) + "_" + values[0].type[0];
        RELITEM.type = values[0].type;
        // cout << "RELITEM:::" << RELITEM.place << " " << RELITEM.type << endl;
        emit(values[1].op, values[0].place, values[2].place, RELITEM.place);
    }
    return RELITEM;
}
/*
    ITEM -> FACTOR  *{ITEM.place=FACTOR.place;ITEM.type=FACTOR.type}*
    ITEM -> ITEM MUL_DIV FACTOR *{ITEM.place=newtemp(FACTOR.type);ITEM.type=FACTOR.type;gen(MUL_DIV.op,ITEM_1.place,FACTOR.place,ITEM.place)}*
*/
node CodeGenerator::GenITEM(const Generate &gen, const vector<node> &values, int nowline)
{
    node ITEM;
    if (values.size() == 1)
    {
        ITEM.place = values[0].place;
        ITEM.type = values[0].type;
    }
    else
    {
        ITEM.place = "T" + newtemp(values[2].type) + "_" + values[2].type[0];
        ITEM.type = values[2].type;
        // cout << "ITEM:::" << ITEM.place << " " << ITEM.type << endl;
        emit(values[1].op, values[0].place, values[2].place, ITEM.place);
    }
    return ITEM;
}
/*
    FACTOR -> ID  *{FACTOR.place=lookup(ID.name);FACTOR.type=lookup_type(ID.name)}*
    FACTOR -> UINT *{FACTOR.place=newtemp(int);FACTOR.type=int;gen(=,UINT,-,FACTOR.place)}*
    FACTOR -> UFLOAT *{FACTOR.place=newtemp(double);FACTOR.type=double;gen(=,UFLOAT,-,FACTOR.place)}*
    FACTOR -> ( EXPR ) *{FACTOR.place=EXPR.place;FACTOR.type=EXPR.type}*
    FACTOR -> PLUS_MINUS FACTOR_1 *{FACTOR.place=newtemp(FACTOR_1.type);FACTOR.type=FACTOR_1.type;gen(PLUS_MINUS.op,0,FACTOR_1.place,FACTOR.place)}*
*/
node CodeGenerator::GenFACTOR(const Generate &gen, const vector<node> &values, int nowline)
{
    node FACTOR;
    if (values.size() == 1)
    {
        // cout << "type: " << gen.rhs[0] << endl;
        if (gen.rhs[0] == "UINT")
        {
            FACTOR.place = "T" + newtemp("int") + "_i";
            FACTOR.type = "int";
            emit("=", values[0].value, "-", FACTOR.place);
        }
        else if (gen.rhs[0] == "UFLOAT")
        {
            FACTOR.place = "T" + newtemp("double") + "_d";
            FACTOR.type = "double";
            emit("=", values[0].value, "-", FACTOR.place);
        }
        else if (gen.rhs[0] == "ID")
        {
            FACTOR.place = "TB" + to_string(lookup(values[0].name));
            FACTOR.type = lookup_type(values[0].name);
        }
    }
    else if (values.size() == 2)
    {
        FACTOR.place = "T" + newtemp(values[1].type) + "_" + values[1].type[0];
        FACTOR.type = values[1].type;
        emit(values[0].op, "0", values[1].place, FACTOR.place);
    }
    else
    {
        FACTOR.place = values[1].place;
        FACTOR.type = values[1].type;
        // cout << "FACTOR::: " << FACTOR.type << endl;
    }
    return FACTOR;
}

// Part3: 条件控制

// B -> B_1 || N BORTERM *{backpatch(B_1.falselist,N.quad);B.truelist=merge(B_1.truelist,BORTERM.truelist);B.falselist=BORTERM.falselist}*
// B -> BORTERM *{B.truelist=BORTERM.truelist;B.falselist=BORTERM.falselist}*
node CodeGenerator::GenB(const Generate &gen, const vector<node> &values, int nowline)
{
    node B;
    if (values.size() == 1)
    {
        // cout << "BORTERM->B" << values[0].falselist.size() << " " << values[0].truelist.size() << endl;
        B.truelist = values[0].truelist;
        B.falselist = values[0].falselist;
    }
    else
    {
        // cout << "B::" << values[0].falselist.size() << " " << values[2].quad << endl;
        backpatch(values[0].falselist, stoi(values[2].quad));
        B.truelist = merge(values[0].truelist, values[3].truelist);
        B.falselist = values[3].falselist;
        // cout << "||->B::" << B.truelist.size() << " " << B.falselist.size() << endl;
    }
    return B;
}
// BORTERM -> BORTERM_1 && N BANDTERM *{backpatch(BORTERM_1.truelist,N.quad);BORTERM.falselist=merge(BORTERM_1.falselist,BANDTERM.falselist;BORTERM.truelist=BANDTERM.truelist)}*
// BORTERM -> BANDTERM *{BORTERM.truelist=BANDTERM.truelist;BORTERM.falselist=BANDTERM.falselist}*
node CodeGenerator::GenBORTERM(const Generate &gen, const vector<node> &values, int nowline)
{
    node BORTERM;
    if (values.size() == 1)
    {
        BORTERM.truelist = values[0].truelist;
        BORTERM.falselist = values[0].falselist;
    }
    else
    {
        backpatch(values[0].truelist, stoi(values[2].quad));
        BORTERM.falselist = merge(values[0].falselist, values[3].falselist);
        BORTERM.truelist = values[3].truelist;
    }
    return BORTERM;
}
// BANDTERM -> ( B ) {BANDTERM.truelist=B.truelist;BANDTERM.falselist=B.falselist}
// BANDTERM ->  ! BANDTERM_1 *{BANDTERM.truelist=BANDTERM_1.falselist;BANDTERM.falselist=BANDTERM_1.truelist}*
// BANDTERM -> BFACTOR_1 REL BFACTOR_2 *{BANDTERM.truelist=mklist(nxq);BANDTERM.falselist=mklist(nxq+1);gen(j+REL.op,BFACTOR_1.place,BFACTOR_2.place,0);gen(j,-,-,0);}*
// BANDTERM -> BFACTOR *{BANDTERM.truelist=mklist(nxq);BANDTERM.falselist=mklist(nxq+1);gen(jnz,BFACTOR.place,-,0);gen(j,-,-,0)}*
node CodeGenerator::GenBANDTERM(const Generate &gen, const vector<node> &values, int nowline)
{
    node BANDTERM;
    if (values.size() == 1)
    {
        BANDTERM.truelist.clear();
        BANDTERM.truelist.push_back(nextquad());
        BANDTERM.falselist.clear();
        BANDTERM.falselist.push_back(nextquad() + 1);
        emit("jnz", values[0].place, "-", 0);
        emit("j", "-", "-", 0);
    }
    else if (values.size() == 2)
    {
        BANDTERM.truelist = values[1].falselist;
        BANDTERM.falselist = values[1].truelist;
    }
    else if (values[0].value == "(")
    {
        BANDTERM.truelist = values[1].truelist;
        BANDTERM.falselist = values[1].falselist;
    }
    else
    {
        BANDTERM.truelist.clear();
        BANDTERM.truelist.push_back(nextquad());
        BANDTERM.falselist.clear();
        BANDTERM.falselist.push_back(nextquad() + 1);
        emit("j" + values[1].op, values[0].place, values[2].place, 0);
        emit("j", "-", "-", 0);
    }
    return BANDTERM;
}
// BFACTOR -> UINT *{BFACTOR.place=newtemp(int);BFACTOR.type=int;gen(=,UINT,-,FACTOR.place)}*
// BFACTOR -> UFLOAT *{BFACTOR.place=newtemp(double);BFACTOR.type=double;gen(=,UFLOAT,-,BFACTOR.place)}*
// BFACTOR -> ID *{BFACTOR.place=lookup(ID.name);BFACTOR.type=lookup_type(ID.name)}*
node CodeGenerator::GenBFACTOR(const Generate &gen, const vector<node> &values, int nowline)
{
    node BFACTOR;
    if (gen.rhs[0] == "UINT")
    {
        BFACTOR.place = "T" + newtemp("int") + "_i";
        BFACTOR.type = "int";
        emit("=", values[0].value, "-", BFACTOR.place);
    }
    else if (gen.rhs[0] == "UFLOAT")
    {
        BFACTOR.place = "T" + newtemp("double") + "_d";
        BFACTOR.type = "double";
        emit("=", values[0].value, "-", BFACTOR.place);
    }
    else
    {
        BFACTOR.place = "TB" + to_string(lookup(values[0].name));
        BFACTOR.type = getaddress(values[0].name);
    }
    return BFACTOR;
}

// Part5: 运算符号

// PLUS_MINUS -> + *{PLUS_MINUS.op='+'}*
// PLUS_MINUS -> - *{ PLUS_MINUS.op='-'}*
node CodeGenerator::GenPLUS_MINUS(const Generate &gen, const vector<node> &values, int nowline)
{
    node PLUS_MINUS;
    PLUS_MINUS.op = values[0].value;
    return PLUS_MINUS;
}

// MUL_DIV -> \*  *{MUL_DIV.op='\*'}*
// MUL_DIV -> /  *{ MUL_DIV.op='/'}*
node CodeGenerator::GenMUL_DIV(const Generate &gen, const vector<node> &values, int nowline)
{
    node MUL_DIV;
    MUL_DIV.op = values[0].value;
    return MUL_DIV;
}
// REL -> == *{REL.op='=='}*
// REL -> !=  *{ REL.op='!='}}*
// REL -> <  *{ REL.op='<'}}*
// REL -> <= *{ REL.op='<='}*
// REL -> >  *{ REL.op='>'}}*
// REL -> >= *{ REL.op='>='}*
node CodeGenerator::GenREL(const Generate &gen, const vector<node> &values, int nowline)
{
    node REL;
    REL.op = values[0].value;
    return REL;
}

// Part6: 读写语句

// SCANF -> SCANF_BEGIN ) *{}*

// SCANF_BEGIN -> SCANF_BEGIN , ID *{p=lookup(ID.name);gen(R,-,-,p)}*
// SCANF_BEGIN -> scanf ( ID       *{p=lookup(ID.name);gen(R,-,-,p)}*
node CodeGenerator::GenSCANF(const Generate &gen, const vector<node> &values, int nowline)
{
    node SCANF;
    int p = lookup(values[2].name);
    // cout << "value name: " << values[2].name << endl;
    emit("R", "-", "-", "TB" + to_string(p));
    return SCANF;
}
// PRINTF -> PRINTF_BEGIN ) *{}*

// PRINTF_BEGIN -> printf ( ID       *{p=lookup(ID.name);gen(W,-,-,p)}*
// PRINTF_BEGIN -> PRINTF_BEGIN , ID {p=lookup(ID.name);gen(W,-,-,p)}
node CodeGenerator::GenPRINTF(const Generate &gen, const vector<node> &values, int nowline)
{
    node PRINTF;
    int p = lookup(values[2].name);
    emit("W", "-", "-", "TB" + to_string(p));
    return PRINTF;
}

CodeGenerator::CodeGenerator() : Tables(), code_tot(0)
{
    offset.push(0);
}

node CodeGenerator::Gen(const Generate &gen, const vector<node> &values, int nowline)
{
    if (gen.lhs == "SUBPROG")
        return GenSUBPROG(gen, values, nowline);
    if (gen.lhs == "M")
        return GenM(gen, values, nowline);
    if (gen.lhs == "N")
        return GenN(gen, values, nowline);
    // 变量声明
    if (gen.lhs == "T")
        return GenT(gen, values, nowline);
    if (gen.lhs == "ID")
        return GenID(gen, values, nowline);
    if (gen.lhs == "VARIABLE")
        return GenVARIABLE(gen, values, nowline);
    // 基本语句
    if (gen.lhs == "STATEMENT")
    {
        if (gen.rhs[0] == "ASSIGN" || gen.rhs[0] == "SCANF" || gen.rhs[0] == "PRINTF" || gen.rhs[0] == "^")
        {
            node STATEMENT;
            return STATEMENT;
        }
        if (gen.rhs[0] == "{")
        {
            node STATEMENT;
            STATEMENT.nextlist = values[1].nextlist;
            return STATEMENT;
        }
        if (gen.rhs[0] == "while")
            return GenWhile(gen, values, nowline);
        if (gen.rhs[0] == "if")
            return GenIf(gen, values, nowline);
    }
    if (gen.lhs == "ASSIGN")
        return GenASSIGN(gen, values, nowline);
    if (gen.lhs == "L")
        return GenL(gen, values, nowline);
    // 数值表达式
    if (gen.lhs == "EXPR")
        return GenEXPR(gen, values, nowline);
    if (gen.lhs == "ORITEM")
        return GenORITEM(gen, values, nowline);
    if (gen.lhs == "ANDITEM")
        return GenANDITEM(gen, values, nowline);
    if (gen.lhs == "NOITEM")
        return GenNOITEM(gen, values, nowline);
    if (gen.lhs == "RELITEM")
        return GenRELITEM(gen, values, nowline);
    if (gen.lhs == "ITEM")
        return GenITEM(gen, values, nowline);
    if (gen.lhs == "FACTOR")
        return GenFACTOR(gen, values, nowline);
    // 条件控制
    if (gen.lhs == "B")
        return GenB(gen, values, nowline);
    if (gen.lhs == "BORTERM")
        return GenBORTERM(gen, values, nowline);
    if (gen.lhs == "BANDTERM")
        return GenBANDTERM(gen, values, nowline);
    if (gen.lhs == "BFACTOR")
        return GenBFACTOR(gen, values, nowline);

    // 运算符号
    if (gen.lhs == "PLUS_MINUS")
        return GenPLUS_MINUS(gen, values, nowline);
    if (gen.lhs == "MUL_DIV")
        return GenMUL_DIV(gen, values, nowline);
    if (gen.lhs == "REL")
        return GenREL(gen, values, nowline);

    // 读写语句
    if (gen.lhs == "SCANF_BEGIN")
        return GenSCANF(gen, values, nowline);
    if (gen.lhs == "PRINTF_BEGIN")
        return GenPRINTF(gen, values, nowline);

    return node();
}
void CodeGenerator::check_ins(stack<pair<int, string>> stk)
{
    if (stk.empty())
        return;
    if (stk.top().second == "\"else\"")
        emit("", "", "", 0);
    else
    {
        string a, b, c, d;
        d = stk.top().second;
        stk.pop();
        if (stk.empty())
            return;
        c = stk.top().second;
        stk.pop();
        if (stk.empty())
            return;
        b = stk.top().second;
        stk.pop();
        if (stk.empty())
            return;
        a = stk.top().second;
        if (b == "\"(\"" && d == "\")\"" && c == "Expression" && (a == "\"if\"" || a == "While"))
            emit("", "", "", 0), emit("", "", "", 0);
    }
}