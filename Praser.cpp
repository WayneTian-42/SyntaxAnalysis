#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <set>

class Praser
{
public:
    Praser();
    void generatePredictTable();
    char findTerminal(const char start, const char now);
    void test();

private:
    std::set<char> terminal, nonterminal;
    std::unordered_map<char, std::vector<char>> first, follow;
    std::unordered_map<char, std::vector<std::string>> grammar;
    std::vector<std::vector<std::string>> predictTable;
    std::unordered_map<char, int> symbol;
};

int main()
{
    Praser p;
    p.test();
    p.generatePredictTable();
    std::cout << std::endl;
}

Praser::Praser()
{
    predictTable.resize(nonterminal.size());
    for (int i = 0; i < nonterminal.size(); i++)
        predictTable[i].resize(terminal.size());
}

void Praser::generatePredictTable()
{
    for (char non : nonterminal)
    {
        std::vector<std::string> gra = grammar[non];
        std::string tmp;
        for (std::string &str : gra)
        {
            char alpha = str[0];
            if (terminal.find(alpha) != terminal.end() && find(first[non].begin(), first[non].end(), alpha) != first[non].end())
            {
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + str;
                continue;
            }
            char now = findTerminal(non, alpha);
            predictTable[symbol[non]][symbol[now]] = tmp + non + " -> " + str;
            // tmp.clear();
            // 简单处理， 没有考虑E->A|num这种情况, 想递归调用
            // while (terminal.find(alpha) != terminal.end())
            //     alpha = 
        }
        if (find(first[non].begin(), first[non].end(), '#') != first[non].end())
        {
            for (char alpha : follow[non])
                predictTable[symbol[non]][symbol[non]] = non + " -> " + alpha;
        }
    }

    for (auto &col : predictTable)
    {
        std::cout << "\t";
        for (auto &row : col)
        {
            if (!row.empty())
                std::cout << row << " ";
            else
                std::cout << "\t\t";
        }
        std::cout << std::endl;
    }
}

char Praser::findTerminal(const char start, const char now)
{
    std::vector<std::string> gra = grammar[now];
    for (std::string &str : gra)
    {
        char alpha = str[0];
        while (terminal.find(alpha) == terminal.end() || find(first[start].begin(), first[start].end(), alpha) == first[start].end())
            return findTerminal(start, alpha);
        if (predictTable[symbol[start]][symbol[alpha]].empty())
            return alpha;
    }
}

void Praser::test()
{
    grammar['S'] = {"(L)", "a"};
    grammar['L'] = {"SM"};
    grammar['M'] = {",SM", "#"};

    first['S'] = {'(', 'a'};
    first['L'] = {'(', 'a'};
    first['M'] = {',', '#'};

    follow['S'] = {'$', ',', ')'};
    follow['L'] = {')'};
    follow['M'] = {')'};

    symbol['S'] = 0;
    symbol['L'] = 1;
    symbol['M'] = 2;

    symbol['('] = 0;
    symbol[')'] = 1;
    symbol['a'] = 2;
    symbol['$'] = 3;

    std::cout << "\t" << "(\t\t" << ")\t\t" << "a\t\t" << "$\t\t" << std::endl;

    terminal.insert('(');
    terminal.insert(')');
    terminal.insert('a');
    terminal.insert('#');
    terminal.insert(',');

    nonterminal.insert('S');
    nonterminal.insert('L');
    nonterminal.insert('M');

    predictTable.resize(nonterminal.size());
    for (int i = 0; i < nonterminal.size(); i++)
        predictTable[i].resize(terminal.size());
}