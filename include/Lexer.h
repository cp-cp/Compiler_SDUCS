#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <vector>

struct Token
{
    std::string value;
    std::string type;
    Token(const std::string &value, const std::string &type) : value(value), type(type) {}
};

class Lexer
{
public:
    // Lexer();
    Lexer(const std::string &keywordsFile, const std::string &symbolsFile);
    Lexer(const std::map<std::string, std::string> &keywords, const std::map<std::string, std::string> &symbols);
    std::vector<Token> analyze(const std::string &code);

private:
    std::map<std::string, std::string> keywords;
    std::map<std::string, std::string> symbols;

    void loadKeywords(const std::string &filename);
    void loadSymbols(const std::string &filename);
};

#endif