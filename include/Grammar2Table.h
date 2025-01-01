#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

class Generate//产生式
{
    public:
	    string lhs;
	vector<string> rhs;

	Generate(const string &a, const vector<string> &b) : lhs(a), rhs(b) {}
};

bool operator<(const Generate &, const Generate &);//重载<:保证可以插入到map中。

class LR0Project : public Generate//LR0项目=产生式+当前位置
{
public:
	unsigned int nowPosition;

	string GetNowString(void);
	LR0Project(const string &a, const vector<string> &b, unsigned int c) : Generate(a, b), nowPosition(c) {}
	LR0Project(const Generate &b, unsigned int c) : Generate(b.lhs, b.rhs), nowPosition(c) {}
};

class LR1Project : public LR0Project//LR1项目=LR0项目+向前看符号=产生式+当前位置+向前看符号
{
public:
	string nxtstr;

	LR1Project(const string &a, const vector<string> &b, unsigned int c, const string &d) : LR0Project(a, b, c), nxtstr(d) {}
	LR1Project(const Generate &b, unsigned int c, const string &d) : LR0Project(b, c), nxtstr(d) {}
};

bool operator<(const LR1Project &, const LR1Project &);
bool operator==(const LR1Project &, const LR1Project &);

class Closure//项目集规范族=LR1项目的闭包
{
public:
	set<LR1Project> shiftpro, reducepro;

	bool operator==(const Closure &);
	int size(void);
	void insert(const LR1Project &);
	void Complete(const vector<Generate> &, map<string, vector<string>>);
};

class Grammar2Tables//文法转换为ACTION和GOTO表
{
private:
	string GrammarFilename;
	map<string, int> GrammarTokens; // map<int,int> a; a[1]=1,a[2]=2;cout<<a[1]<<' '<<a[3];// 1 0
	vector<Generate> GrammarGenerates;
	map<Generate, unsigned int> Generate_Id;
	map<string, vector<string>> GrammarFIRST, GrammarFOLLOW;
	vector<LR0Project> GrammarLR0Projects;
	vector<Closure> GrammarClosures;
	map<pair<int, string>, int> GrammarGo; // GrammarGo[make_pair(1,"+")]=2
	map<int, map<string, pair<char, int>>> GrammarACTION, GrammarGOTO;

	void GetTokens(void);
	void GetGenerates(void);
	void GetFIRST(void);
	void GetFOLLOW(void);
	void GetLR0Projects(void);
	void GetClosuresAndGo(void);
	void GetACTIONAndGOTO(void);

public:
	Grammar2Tables(const string &);
	bool CheckIsLR1(const string & = "CON");
	void DebugGrammarTokens(const string & = "CON");
	void DebugGrammarGenerates(const string & = "CON");
	void DebugGrammarFIRST(const string & = "CON");
	void DebugGrammarFOLLOW(const string & = "CON");
	// void DebugGrammarLR0Projects(const string & = "CON");
	void DebugGrammarClosures(const string & = "CON");
	// void DebugGrammarGo(const string & = "CON");
	void DebugGrammarACTIONAndGOTO(const string & = "CON");
	void DebugGrammarACTIONAndGOTOprivate(const string & = "CON");
};
