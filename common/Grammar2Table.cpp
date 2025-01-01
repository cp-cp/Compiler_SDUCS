#include "../include/Grammar2Table.h"

bool operator<(const Generate &x, const Generate &y)
{
	if (x.lhs != y.lhs)
		return x.lhs < y.lhs;
	return x.rhs < y.rhs;
}

string LR0Project::GetNowString()
{
	return nowPosition == rhs.size() ? "^" : rhs[nowPosition]; // 用^表示空白符号
}

bool operator<(const LR1Project &x, const LR1Project &y)
{
	if (x.lhs != y.lhs)
		return x.lhs < y.lhs;
	if (x.rhs != y.rhs)
		return x.rhs < y.rhs;
	if (x.nowPosition != y.nowPosition)
		return x.nowPosition < y.nowPosition;
	return x.nxtstr < y.nxtstr;
}

bool operator==(const LR1Project &x, const LR1Project &y)
{
	return x.lhs == y.lhs && x.rhs == y.rhs && x.nowPosition == y.nowPosition && x.nxtstr == y.nxtstr;
}

bool Closure::operator==(const Closure &x)
{
	return shiftpro == x.shiftpro && reducepro == x.reducepro;
}

int Closure::size()
{
	return shiftpro.size() + reducepro.size();
}

void Closure::insert(const LR1Project &x)
{
	if (x.nowPosition == x.rhs.size()) // 当前位置在产生式的最后
		reducepro.insert(x);
	else
		shiftpro.insert(x);
}

void Closure::Complete(const vector<Generate> &GrammarGenerates, map<string, vector<string>> GrammarFIRST)
{
	map<string, vector<vector<string>>> Generates0;
	for (auto x : GrammarGenerates)
	{
		Generates0[x.lhs].push_back(x.rhs);
	}
	while (true)
	{
		Closure oldans = *this;
		for (auto x : oldans.shiftpro)
		{
			string y = x.GetNowString();
			// cout<<"hihihi:" <<this << y << endl;
			if (x.nowPosition == x.rhs.size() - 1) // 当前位置在产生式的最后
			{
				for (auto z : Generates0[y])
					this->shiftpro.insert(LR1Project(y, z, 0, x.nxtstr));
			}
			else
			{
				for (auto z : Generates0[y])
					for (auto w : GrammarFIRST[x.rhs[x.nowPosition + 1]])
						this->shiftpro.insert(LR1Project(y, z, 0, w));
			}
		}
		if (this->size() == oldans.size())
			break;
	}
}

void Grammar2Tables::GetTokens() // 区分文法的终结符和非终结符
{
	ifstream fin;
	fin.open(GrammarFilename, ios::in);
	if (!fin.is_open())
	{
		std::cerr << "Error: cannot open file " << GrammarFilename << std::endl;
	}
	string ss, lst;
	fin >> lst;
	GrammarTokens.clear();
	while (fin >> ss)
	{
		// cout<<ss<<endl;
		if (ss == "->")
			GrammarTokens[lst] = 1;
		else if (ss == "|")
			GrammarTokens[lst] |= 0;
		else if (lst != "->" && lst != "|")
			GrammarTokens[lst] |= 0;
		lst = ss;
	}
	GrammarTokens[lst] |= 0;
	fin.close();
}

void Grammar2Tables::GetGenerates() // 获得文法的产生式
{
	GrammarGenerates.clear();
	ifstream fin;
	fin.open(GrammarFilename, ios::in);
	if (!fin.is_open())
	{
		std::cerr << "Error: cannot open file " << GrammarFilename << std::endl;
	}
	string ss;
	while (getline(fin, ss))
	{
		istringstream sin(ss);
		string tmp1;
		vector<string> tmp2;
		sin >> tmp1;
		string tmp3;
		sin >> tmp3;
		while (sin >> tmp3)
		{
			if (tmp3 == "|")
				GrammarGenerates.push_back(Generate(tmp1, tmp2)), tmp2.clear();
			else
				tmp2.push_back(tmp3);
		}
		if (tmp2.size())
			GrammarGenerates.push_back(Generate(tmp1, tmp2));
	}
	for (unsigned int i = 0; i < GrammarGenerates.size(); ++i)
	{
		Generate_Id[GrammarGenerates[i]] = i;
	}
	fin.close();
}

void Grammar2Tables::GetFIRST()
{
	GrammarFIRST.clear();
	for (auto x : GrammarTokens)
	{
		if (x.second == 0)
			GrammarFIRST[x.first].push_back(x.first);
	}
	while (true)
	{
		bool flg = false;
		for (auto x : GrammarGenerates) // 对于每个产生式X -> Y1Y2...Yk
		{

			bool isAllEmpty = true;
			vector<string> tmp;
			tmp.clear();
			tmp = GrammarFIRST[x.rhs[0]];
			// cout<<x.lhs<<" "<<x.rhs[0]<<endl;
			for (auto y : tmp) // 删除空字符
			{
				if (y == "^")
				{
					tmp.erase(find(tmp.begin(), tmp.end(), "^"));
					break;
				}
			}
			for (size_t i = 1; i < x.rhs.size(); ++i)
			{
				auto y_lst = x.rhs[i - 1];
				auto y = x.rhs[i];
				if (find(GrammarFIRST[y_lst].begin(), GrammarFIRST[y_lst].end(), "^") == GrammarFIRST[y_lst].end())
				{
					isAllEmpty = false;
					break;
				}
				for (auto z : GrammarFIRST[y])
				{
					if (z == "^")
						continue;
					tmp.push_back(z);
				}
			}
			if (isAllEmpty && (find(GrammarFIRST[x.rhs[x.rhs.size() - 1]].begin(), GrammarFIRST[x.rhs[x.rhs.size() - 1]].end(), "^") != GrammarFIRST[x.rhs[x.rhs.size() - 1]].end()))
				tmp.push_back("^");
			for (auto y : tmp) // Y1Y2...Yk的First集合
			{
				bool had = false;
				for (auto z : GrammarFIRST[x.lhs]) // X的First集合
				{
					if (z == y) // 有可以加入的
					{
						had = true;
						break;
					}
				}
				if (!had)
				{
					GrammarFIRST[x.lhs].push_back(y);
					flg = true;
				}
			}
		}
		if (!flg)
			break;
	}
}

void Grammar2Tables::GetFOLLOW()
{
	GrammarFOLLOW.clear();
	GrammarFOLLOW[GrammarGenerates[0].lhs].push_back("#");

	while (true)
	{
		bool flg = false;
		for (auto x : GrammarGenerates)
		{
			for (unsigned int i = 0; i < x.rhs.size(); ++i)
			{
				string y = x.rhs[i];
				if (i == x.rhs.size() - 1)
				{
					// 当前符号是右部的最后一个符号
					for (auto z1 : GrammarFOLLOW[x.lhs])
					{
						if (std::find(GrammarFOLLOW[y].begin(), GrammarFOLLOW[y].end(), z1) == GrammarFOLLOW[y].end())
						{
							GrammarFOLLOW[y].push_back(z1);
							flg = true;
						}
					}
				}
				else
				{
					// 当前符号不是右部的最后一个符号
					string z = x.rhs[i + 1];
					for (auto z1 : GrammarFIRST[z])
					{
						if (z1 != "^" && std::find(GrammarFOLLOW[y].begin(), GrammarFOLLOW[y].end(), z1) == GrammarFOLLOW[y].end())
						{
							GrammarFOLLOW[y].push_back(z1);
							flg = true;
						}
					}
					// 如果下一个符号的FIRST集中包含空白符号^，则继续将左部非终结符的FOLLOW集添加到当前符号的FOLLOW集中
					if (std::find(GrammarFIRST[z].begin(), GrammarFIRST[z].end(), "^") != GrammarFIRST[z].end())
					{
						for (auto z1 : GrammarFOLLOW[x.lhs])
						{
							if (std::find(GrammarFOLLOW[y].begin(), GrammarFOLLOW[y].end(), z1) == GrammarFOLLOW[y].end())
							{
								GrammarFOLLOW[y].push_back(z1);
								flg = true;
							}
						}
					}
				}
			}
		}
		if (!flg)
			break;
	}
}

void Grammar2Tables::GetLR0Projects() // 获得文法的LR0项目
{
	GrammarLR0Projects.clear();
	for (auto x : GrammarGenerates)
	{
		for (unsigned int i = 0; i <= x.rhs.size(); ++i)
			GrammarLR0Projects.push_back(LR0Project(x, i));
	}
}

void Grammar2Tables::GetClosuresAndGo() // 获得文法的项目集规范族和Go函数
{
	GrammarClosures.clear();
	GrammarGo.clear();
	Closure nwclosure;
	nwclosure.insert(LR1Project(GrammarGenerates[0], 0, "#"));
	nwclosure.Complete(GrammarGenerates, GrammarFIRST);
	GrammarClosures.push_back(nwclosure);
	unsigned int op = 0;				// operate-position
	while (op < GrammarClosures.size()) // 类似广度优先搜索的思路，不断扩展直到所有闭包的Go都求出来为止
	{
		map<string, Closure> string2closure;
		for (auto x : GrammarClosures[op].shiftpro)
			string2closure[x.GetNowString()].insert(LR1Project(x.lhs, x.rhs, x.nowPosition + 1, x.nxtstr));

		for (auto j = string2closure.begin(); j != string2closure.end(); ++j)
		{
			nwclosure = j->second;
			nwclosure.Complete(GrammarGenerates, GrammarFIRST);
			bool flg = false;
			for (unsigned int i = 0; i < GrammarClosures.size(); ++i)
				if (nwclosure == GrammarClosures[i])
				{
					GrammarGo[make_pair(op, j->first)] = i;
					flg = true;
					break;
				}
			if (!flg)
				GrammarClosures.push_back(nwclosure), GrammarGo[make_pair(op, j->first)] = GrammarClosures.size() - 1;
		}
		++op;
	}
}

void Grammar2Tables::GetACTIONAndGOTO() // 获得文法的ACTION和GOTO表
{
	for (unsigned int i = 0; i < GrammarClosures.size(); ++i)
	{
		for (auto x : GrammarClosures[i].reducepro)
		{
			GrammarACTION[i][x.nxtstr] = make_pair('r', Generate_Id[Generate(x.lhs, x.rhs)]);
			if (Generate_Id[Generate(x.lhs, x.rhs)] == 0)
			{
				GrammarACTION[i][x.nxtstr].first = 'a';
			}
		}
	}
	// 先ACTION后GOTO，目的是移进优先
	for (auto x : GrammarGo)
	{
		if (GrammarTokens[x.first.second])
			GrammarGOTO[x.first.first][x.first.second] = make_pair('g', x.second);
		else
			GrammarACTION[x.first.first][x.first.second] = make_pair('s', x.second);
	}
}

bool Grammar2Tables::CheckIsLR1(const string &filename) // 检查是否是LR1文法
{
	bool FLG = true;
	ofstream fout;
	fout.open(filename, ios::out);
	if (!fout.is_open())
	{
		// Error(-1);
		std::cerr << "Error: cannot open file " << filename << std::endl;
	}
	for (auto x : GrammarClosures)
	{
		set<string> stringset;
		for (auto y : x.shiftpro)
		{
			if (GrammarTokens[y.GetNowString()] == 0)
			{
				stringset.insert(y.GetNowString());
			}
		}
		bool flg = true;
		set<string> samestring;
		for (auto y : x.reducepro)
		{
			if (stringset.count(y.nxtstr))
			{
				flg = false;
				samestring.insert(y.nxtstr);
			}
			else
				stringset.insert(y.nxtstr);
		}
		if (!flg)
		{
			FLG = false;
			fout << "this is bad!" << endl;
			for (auto y : x.shiftpro)
			{
				if (!samestring.count(y.GetNowString()))
					continue;
				fout << y.lhs << " -> ";
				for (unsigned int i = 0; i < y.rhs.size(); ++i)
				{
					if (i == y.nowPosition)
						fout << ". ";
					fout << y.rhs[i] << ' ';
				}
				fout << endl
					 << y.nowPosition << ' ' << y.nxtstr << endl
					 << endl;
			}
			for (auto y : x.reducepro)
			{
				if (!samestring.count(y.nxtstr))
					continue;
				fout << y.lhs << " -> ";
				for (unsigned int i = 0; i < y.rhs.size(); ++i)
					fout << y.rhs[i] << ' ';
				fout << ". ";
				fout << endl
					 << y.nowPosition << ' ' << y.nxtstr << endl
					 << endl;
			}
		}
	}
	fout.close();
	return FLG;
}

void Grammar2Tables::DebugGrammarTokens(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	for (auto x : GrammarTokens)
	{
		cout << x.first << " ----- " << x.second << endl;
	}
	freopen("CON", "w", stdout); // 定向输出到控制台
}
void Grammar2Tables::DebugGrammarGenerates(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	for (auto x : GrammarGenerates)
	{
		cout << x.lhs << " -> ";
		for (auto y : x.rhs)
			cout << y << ' ';
		cout << endl;
	}
}
void Grammar2Tables::DebugGrammarFIRST(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	for (auto x : GrammarFIRST)
	{
		cout << x.first << " (FIRST) : ";
		for (auto y : x.second)
			cout << y << ' ';
		cout << endl;
	}
	freopen("CON", "w", stdout); // 定向输出到控制台
}
void Grammar2Tables::DebugGrammarFOLLOW(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	for (auto x : GrammarFOLLOW)
	{
		cout << x.first << " (FOLLOW) : ";
		for (auto y : x.second)
			cout << y << ' ';
		cout << endl;
	}
	freopen("CON", "w", stdout); // 定向输出到控制台
}
void Grammar2Tables::DebugGrammarClosures(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	cout << GrammarClosures.size() << endl;
	if (true)
	{
		int op = -1;
		for (auto xx : GrammarClosures)
		{
			++op;
			for (auto x : xx.shiftpro)
			{
				cout << op << " : ";
				cout << x.lhs << " -> ";
				for (unsigned i = 0; i < x.rhs.size(); ++i)
				{
					if (i == x.nowPosition)
						cout << ". ";
					cout << x.rhs[i] << ' ';
				}
				cout << ", " << x.nxtstr << endl;
			}
			for (auto x : xx.reducepro)
			{
				cout << op << " : ";
				cout << x.lhs << " -> ";
				for (auto y : x.rhs)
					cout << y << ' ';
				cout << ". ";
				cout << ", " << x.nxtstr << endl;
			}
		}
	}
	freopen("CON", "w", stdout); // 定向输出到控制台
}
void Grammar2Tables::DebugGrammarACTIONAndGOTO(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	cout << "ACTION,GOTO" << endl;
	cout << "ClosureId";
	for (auto x : GrammarTokens)
		if (!x.second)
			cout << ',' << (x.first == "," ? "\",\"" : x.first); //','处理
	cout << ',' << "#";
	for (auto x : GrammarTokens)
		if (x.second)
			cout << ',' << x.first;
	cout << endl;
	for (unsigned int i = 0; i < GrammarClosures.size(); ++i)
	{
		cout << i;
		for (auto x : GrammarTokens)
			if (!x.second)
			{
				cout << ',';
				if (GrammarACTION[i].count(x.first))
				{
					if (GrammarACTION[i][x.first].first == 'a')
						cout << "acc";
					else
						cout << GrammarACTION[i][x.first].first << GrammarACTION[i][x.first].second;
				}
			}
		cout << ',';
		if (GrammarACTION[i].count("#"))
		{
			if (GrammarACTION[i]["#"].first == 'a')
				cout << "acc";
			else
				cout << GrammarACTION[i]["#"].first << GrammarACTION[i]["#"].second;
		}
		for (auto x : GrammarTokens)
			if (x.second)
			{
				cout << ',';
				if (GrammarGOTO[i].count(x.first))
				{
					cout << GrammarGOTO[i][x.first].second;
				}
			}
		cout << endl;
	}
	freopen("CON", "w", stdout); // 定向输出到控制台
}
void Grammar2Tables::DebugGrammarACTIONAndGOTOprivate(const string &filename)
{
	freopen(filename.c_str(), "w", stdout);
	for (auto x : GrammarACTION)
		for (auto y : x.second)
			cout << x.first << ' ' << y.first << ' ' << y.second.first << ' ' << y.second.second << endl;
	for (auto x : GrammarGOTO)
		for (auto y : x.second)
			cout << x.first << ' ' << y.first << ' ' << y.second.first << ' ' << y.second.second << endl;
	freopen("CON", "w", stdout); // 定向输出到控制台
	// freopen("/dev/console", "w", stdout); // 定向输出到控制台
	// fclose(stdout);
}

Grammar2Tables::Grammar2Tables(const string &filename) // 构造函数
{
	GrammarFilename = filename;
	GetTokens();
	GetGenerates();
	GetFIRST();
	GetFOLLOW();
	GetLR0Projects();
	GetClosuresAndGo();
	GetACTIONAndGOTO();
}