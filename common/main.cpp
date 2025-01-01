#include "../include/Parser.h"
#include "../include/Objector.h"
#include <iostream>
#include <cassert>
#include <fstream>

std::string SrcInput()
{
    std::string code;
    std::string line;
    while (std::getline(std::cin, line))
    {
        code += line + "\n";
        // assert no '\r' in line
        for (auto ch : line)
        {
            assert(ch != '\r');
        }
    }
    return code;
}

int main()
{

    freopen("./files/test1.in", "r", stdin);
    // freopen("./files/test.out", "w", stdout);
    std::string code = SrcInput();
    Lexer lexer("./files/keywords.txt", "./files/symbols.txt");
    std::vector<Token> tokens = lexer.analyze(code);

    Parser parser("./files/Generates.txt", "./files/ACTIONAndGOTOPrivate.txt");
    tokens.push_back(Token("#", "NULL"));
    parser.AnalyzeAndOutput("", 0, tokens);


    bool is_Objector = false;
    std::stringstream ss;
    std::ostream& out = is_Objector ? ss : std::cout;
    parser.TablePrint(out);
    parser.CodePrint(out);

    // 生成目标代码:需要单独生成
    // freopen("./files/test.out", "r", stdin);
    // freopen("./files/ObjectCode.txt", "w", stdout);
    // Objector objector;
    // objector.generate();

    return 0;
}