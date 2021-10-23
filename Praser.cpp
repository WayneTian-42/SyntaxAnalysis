#define DEBUG
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <stack>
#include <fstream>

class Praser
{
public:
    // Praser();
    void readFromFile(const std::string &fileName); // 从文件中获取文法
    void caculateFirstSet();
    void caculaleFollowSet();
    void generatePredictTable(); // 构造预测分析表
    void predictAnalysis(const std::string &input);
    // char findTerminal(const char start, const char now);
    void test();

private:
    void getFirstSet(const char non);
    void getFollowSet(const char non);

private:
    char start; // 文法开始符号
    std::set<char> terminal, nonterminal;
    std::unordered_map<char, std::set<char>> first, follow, preFollow;
    std::unordered_map<char, std::vector<std::string>> grammar;
    std::vector<std::vector<std::string>> predictTable;
    std::unordered_map<char, int> symbol;
};

int main(int argc, char *argv[])
{
    // if (argc != 2)
    // {
    //     std::runtime_error("Error: Need 2 parameters\n");
    // }
    Praser p;
    p.readFromFile("test1.in");
    p.caculateFirstSet();
    p.caculaleFollowSet();
    // p.test();
    p.generatePredictTable();
    std::string tset;
    std::cin >> tset;
    // (i(i(n))(i))
    p.predictAnalysis(tset);
    std::cout << std::endl;
}

// Praser::Praser()
// {
//     predictTable.resize(nonterminal.size());
//     for (int i = 0; i < nonterminal.size(); i++)
//         predictTable[i].resize(terminal.size());
// }
void Praser::readFromFile(const std::string &fileName)
{
    std::fstream fileIn;
    fileIn.open(fileName);
    if (!fileIn)
    {
        throw std::runtime_error("Error: No such file\n");
    }
    std::string generator;
    std::set<char> allSymbol; // 记录所有符号
    int ter = 0, non = 0;
    while (!fileIn.eof())
    {
        getline(fileIn, generator);
        int ptr = 0;
        if (!non)
            start = generator[0];
        nonterminal.insert(generator[0]);
        if (symbol.find(generator[0]) == symbol.end())
            symbol[generator[0]] = non++;
        for (ptr = 5; ptr < generator.size(); ptr++)
        {
            std::string gra;
            while (ptr < generator.size() && generator[ptr] != ' ' && generator[ptr] != '|')
            {
                gra += generator[ptr];
                allSymbol.insert(generator[ptr++]);
            }
            if (!gra.empty())
                grammar[generator[0]].push_back(gra);
        }
    }
    // 添加终结符
    for (char alpha : allSymbol)
    {
        if (nonterminal.find(alpha) != nonterminal.end())
            continue;
        terminal.insert(alpha);
        if (alpha != '#')
            symbol[alpha] = ter++;
    }
    symbol['$'] = ter++;
}

void Praser::caculateFirstSet()
{
    for (char ter : terminal)
        first[ter].insert(ter);
    for (char non : nonterminal)
        getFirstSet(non);
}
void Praser::getFirstSet(const char non)
{
    for (auto &gra : grammar[non])
    {
        if (terminal.find(gra[0]) != terminal.end())
            first[non].insert(gra[0]);
        else
        {
            bool flg = true; // 标记该产生式是否全为非终结符
            for (char alpha : gra)
            {
                if (nonterminal.find(alpha) == nonterminal.end())
                {
                    flg = false;
                    break;
                }
            }
            if (flg)
            {
                getFirstSet(gra[0]);
                first[non].insert(first[gra[0]].begin(), first[gra[0]].end());
            }
            else
            {
                for (char alpha : gra)
                {
                    getFirstSet(alpha);
                    first[non].insert(first[alpha].begin(), first[alpha].end());
                }
            }
        }
    }
}

void Praser::caculaleFollowSet()
{
    for (char non : nonterminal)
        getFollowSet(non);
    for (auto &iter : preFollow)
        for (char non : iter.second)
            follow[non].insert(follow[iter.first].begin(), follow[iter.first].end());
    for (auto &iter : preFollow)
        for (char non : iter.second)
            follow[non].insert(follow[iter.first].begin(), follow[iter.first].end());
}
void Praser::getFollowSet(const char non)
{
    if (non == start)
        follow[non].insert('$');
    for (auto &gra : grammar[non])
    {
        for (int i = 1; i < gra.size(); i++)
        {
            if (nonterminal.find(gra[i - 1]) != nonterminal.end())
            {
                follow[gra[i - 1]].insert(first[gra[i]].begin(), first[gra[i]].end());
                follow[gra[i - 1]].erase('#');
            }
            // else
            // {
            //     if (gra[i] != '#')
            //         follow[gra[i - 1]].insert(gra[i]);
            //     i += 2;
            // }
        }
        for (int i = gra.size() - 1; i >= 0; i--)
        {
            if (nonterminal.find(gra[i]) == nonterminal.end())
                break;
            if (i == gra.size() - 1)
            {
                preFollow[non].insert(gra[i]);
                continue;
            }
            if (first[gra[i + 1]].find('#') != first[gra[i + 1]].end())
                preFollow[non].insert(gra[i]);
        }
    }
}

void Praser::generatePredictTable()
{
    predictTable.resize(nonterminal.size());
    for (int i = 0; i < nonterminal.size(); i++)
        predictTable[i].resize(terminal.size());
    for (char non : nonterminal)
    {
        std::vector<std::string> gra = grammar[non];
        std::string tmp;
        for (std::string &str : gra)
        {
            char alpha = str[0];
            if (alpha == '#') // epsilon不在此处考虑
                continue;
            // 终结符且之前未出现过
            if (terminal.find(alpha) != terminal.end() && first[non].find(alpha) != first[non].end())
            {
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + str;
                continue;
            }
            for (char beta : first[alpha])
            {
                // 非终结符则寻找其first集
                if (terminal.find(beta) != terminal.end() && first[non].find(beta) != first[non].end())
                {
                    predictTable[symbol[non]][symbol[beta]] = tmp + non + " -> " + str;
                    continue;
                }
            }
            
            // char now = findTerminal(non, alpha);
            // predictTable[symbol[non]][symbol[now]] = tmp + non + " -> " + str;
        }
        // 判断epsilon
        if (first[non].find('#') != first[non].end())
        {
            for (char alpha : follow[non])
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + "#";
        }
    }
    // 输出
    std::cout << "\t";
    for (int i = 0; i < terminal.size(); i++)
    {
        for (char ter : terminal)
        {
            if (ter == '#')
                continue;
            if (symbol[ter] == i)
                std::cout << std::setw(15) << std::left << ter;
        }
    }
    std::cout << std::setw(15) << std::left << "$";
    std::cout << std::endl;
    for (auto &col : predictTable)
    {
        for (auto &row : col)
        {
            if (!row.empty())
            {
                std::cout << row[0] << "\t";
                break;
            }
        }
        for (auto &row : col)
        {
            if (!row.empty())
                std::cout << std::setw(15) << std::left <<  row;
            else
                std::cout << std::setw(15) << std::left << " ";
        }
        std::cout << std::endl;
    }
}

// char Praser::findTerminal(const char start, const char now)
// {
//     std::vector<std::string> gra = grammar[now];
//     for (std::string &str : gra)
//     {
//         char alpha = str[0];
//         while (terminal.find(alpha) == terminal.end() || find(first[start].begin(), first[start].end(), alpha) == first[start].end())
//             return findTerminal(start, alpha);
//         if (predictTable[symbol[start]][symbol[alpha]].empty())
//             return alpha;
//     }
// }

void Praser::predictAnalysis(const std::string &input)
{
    std::string target = input + "$";
    std::stack<char> stk;
    stk.push('$');
    stk.push('E');
    int ptr = 0;

    while (!stk.empty())
    {
        char snow = stk.top(), inow = target[ptr];
        if (terminal.find(snow) != terminal.end() || snow == '$')
        {
            if (snow == inow)
            {
                stk.pop();
                ptr++;
            }
            else if (snow == '#')
                stk.pop();
            else
            {
                std::cout << "error" << snow << " " << inow << std::endl;
                return;
            }
        }
        else
        {
            if (!predictTable[symbol[snow]][symbol[inow]].empty())
            {
                stk.pop();
                std::string tmp = predictTable[symbol[snow]][symbol[inow]];
                for (int i = tmp.size() - 1; i >= 5; i--)
                {
                    stk.push(tmp[i]);
                }
                std::cout << tmp << std::endl;
            }
            else
            {
                std::cout << "error" << snow << " " << inow << std::endl;
                return;
            }
        }
    }
    std::cout << "accept\n";
}

# ifdef DEBUG
void Praser::test()
{
    // 习题4.4
    grammar['E'] = {"A", "B"};
    grammar['A'] = {"n", "i"};
    grammar['B'] = {"(L)"};
    grammar['L'] = {"EM"};
    grammar['M'] = {"EM", "#"};

    first['E'] = {'(', 'n', 'i'};
    first['A'] = {'n', 'i'};
    first['B'] = {'('};
    first['L'] = {'(', 'n', 'i'};
    first['M'] = {'(', 'n', 'i', '#'};

    follow['E'] = {'$', '(', ')', 'n', 'i'};
    follow['A'] = {'$', '(', ')', 'n', 'i'};
    follow['B'] = {'$', '(', ')', 'n', 'i'};
    follow['L'] = {')'};
    follow['M'] = {')'};

    symbol['E'] = 0;
    symbol['A'] = 1;
    symbol['B'] = 2;
    symbol['L'] = 3;
    symbol['M'] = 4;

    symbol['('] = 0;
    symbol[')'] = 1;
    symbol['n'] = 2;
    symbol['i'] = 3;
    symbol['$'] = 4;

    // std::cout << "\t" <<  std::setw(15) << std::left << "(" << std::setw(15) << std::left << ")" << std::setw(15) << std::left << "n" 
    //          << std::setw(15) << std::left << "i" << std::setw(15) << std::left << "$" << std::endl;

    terminal.insert('(');
    terminal.insert(')');
    terminal.insert('n');
    terminal.insert('#');
    terminal.insert('i');

    std::cout << "\t";
    for (int i = 0; i < terminal.size(); i++)
    {
        for (char ter : terminal)
        {
            if (ter == '#')
                continue;
            if (symbol[ter] == i)
                std::cout << std::setw(15) << std::left << ter;
        }
    }
    std::cout << std::endl;

    nonterminal.insert('E');
    nonterminal.insert('A');
    nonterminal.insert('B');
    nonterminal.insert('L');
    nonterminal.insert('M');

    predictTable.resize(nonterminal.size());
    for (int i = 0; i < nonterminal.size(); i++)
        predictTable[i].resize(terminal.size());
}
#endif