#include "../include/Lexer.h"
#include <fstream>
#include <cctype>
#include <iostream>

// 构造函数，从文件加载关键字和符号
Lexer::Lexer(const std::string &keywordsFile, const std::string &symbolsFile)
{
    loadKeywords(keywordsFile);
    loadSymbols(symbolsFile);
}

Lexer::Lexer(const std::map<std::string, std::string> &keywords, const std::map<std::string, std::string> &symbols)
{
    this->keywords = keywords;
    this->symbols = symbols;
}

// 从代码字符串中解析出 Token 流
std::vector<Token> Lexer::analyze(const std::string &code)
{
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < code.size())
    {
        // 跳过空白字符
        if (std::isspace(code[pos]))
        {
            ++pos;
            continue;
        }

        // 检查是否为关键字或标识符
        if (std::isalpha(code[pos]))
        {
            std::string word;
            while (pos < code.size() && (std::isalnum(code[pos]) || code[pos] == '_'))
            {
                word += code[pos++];
            }

            // 检查是否为关键字
            if (keywords.find(word) != keywords.end())
            {
                tokens.push_back({word, keywords[word]});
            }
            else
            {
                tokens.push_back({word, "IDENT"}); // 否则为标识符
            }
        }
        // 检查数字
        else if (std::isdigit(code[pos]) || code[pos] == '.')
        {
            std::string number;
            int pointNum = 0;
            bool malformed = false;

            while (pos < code.size() && (std::isdigit(code[pos]) || code[pos] == '.'))
            {
                if (code[pos] == '.')
                {
                    pointNum++;
                }
                number += code[pos++];
            }

            if ((number.back() == '.') || (number.front() == '.'))
            {
                malformed = true;
            }
            if (pointNum > 1)
            { // 对应题干中的错误2：小数点开头或结尾
                std::cout << "Malformed number: More than one decimal point in a floating point number.\n";
                tokens.clear();
                return tokens;
            }
            else if (malformed)
            { // 对应题干中的错误1：一个数字中有多个小数点
                std::cout << "Malformed number: Decimal point at the beginning or end of a floating point number.\n";
                tokens.clear();
                return tokens;
            }
            else if (number.front() == '0' && number.size() > 1 && number[1] != '.')
            { // 对应题干中的错误3：前导0开头
                std::cout << "Malformed number: Leading zeros in an integer.\n";
                tokens.clear();
                return tokens;
            }
            else if (pointNum == 1)
            { // 说明是浮点数
                std::string tmp=std::to_string(stod(number));
                tokens.push_back({tmp, "DOUBLE"});
            }
            else
            { // 说明是整数
                tokens.push_back({number, "INT"});
            }
            // tokens.push_back({number, pointNum ? "DOUBLE" : "INT"});
        }
        // 检查符号
        else
        {
            std::string symbol;
            symbol += code[pos];
            // 观察到所有的符号都是单或双字符的，所以这里尝试多检查一个字符即可。
            if (pos + 1 < code.size() && symbols.find(symbol + code[pos + 1]) != symbols.end())
            {
                symbol += code[++pos];
            }
            if (symbol.front() == '/')
            { // 检查是否为注释
                if (code[pos + 1] == '/')
                {
                    while (pos < code.size() && code[pos] != '\n')
                    {
                        ++pos;
                    }
                    continue;
                }
                else if (code[pos + 1] == '*')
                {
                    while (pos < code.size() && (code[pos] != '*' || code[pos + 1] != '/'))
                    {
                        ++pos;
                    }
                    pos += 2;
                    continue;
                }
            }
            // 此处为检验是否为符号表内的符号
            if (symbols.find(symbol) != symbols.end())
            {
                tokens.push_back({symbol, symbols[symbol]});
            }
            else
            {
                std::cout << "Unrecognizable characters.\n";
                tokens.clear();
                return tokens;
            }
            ++pos;
        }
    }
    return tokens;
}

// 从文件加载关键字
void Lexer::loadKeywords(const std::string &filename)
{
    std::ifstream file(filename);
    // std::cout<<filename<<std::endl;
    if (!file)
    {
        std::cout << "Error: Unable to open keywords file.\n";
        return;
    }
    std::string word, type;
    while (file >> word >> type)
    {
        keywords[word] = type;
    }
}

// 从文件加载符号
void Lexer::loadSymbols(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cout << "Error: Unable to open symbols file.\n";
        return;
    }
    std::string symbol, type;
    while (file >> symbol >> type)
    {
        symbols[symbol] = type;
    }
}