/* Parser.h 语法分析器，继承(调用)词法分析器 */

#include <map>
#include <string>
#include <stack>
// #include "Token.h"
#include "Grammar2Table.h"
#include "Lexer.h"
#include "CodeGenerator.h"

using namespace std;

class Parser : public CodeGenerator
{
private:
    vector<Generate> Generates;
    map<int, map<string, pair<char, int>>> ACTION, GOTO;

    void GetGenerates(const string &);
    void GetGenerates();
    void GetACTIONandGOTO(const string &);
    void GetACTIONandGOTO();
    string PreProcess(const string &, bool &);
    void OUTPUT(const stack<pair<int, string>> &, const stack<bool> &);
    string stringinfile(const string &);

public:
    Parser(const string &, const string &);
    void AnalyzeAndOutput(const string &, int = 0, const vector<Token> & = {});
    void Debug(void);
};