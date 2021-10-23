#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <stack>

class Praser
{
public:
    // Praser();
    void generatePredictTable(); // 构造预测分析表
    void predictAnalysis(const std::string &input);
    // char findTerminal(const char start, const char now);
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
    std::string tset;
    std::cin >> tset;
    p.predictAnalysis(tset);
    std::cout << std::endl;
}

// Praser::Praser()
// {
//     predictTable.resize(nonterminal.size());
//     for (int i = 0; i < nonterminal.size(); i++)
//         predictTable[i].resize(terminal.size());
// }

void Praser::generatePredictTable()
{
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
            if (terminal.find(alpha) != terminal.end() && find(first[non].begin(), first[non].end(), alpha) != first[non].end())
            {
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + str;
                continue;
            }
            for (char beta : first[alpha])
            {
                // 非终结符则寻找其first集
                if (terminal.find(beta) != terminal.end() && find(first[non].begin(), first[non].end(), beta) != first[non].end())
                {
                    predictTable[symbol[non]][symbol[beta]] = tmp + non + " -> " + str;
                    continue;
                }
            }
            
            // char now = findTerminal(non, alpha);
            // predictTable[symbol[non]][symbol[now]] = tmp + non + " -> " + str;
        }
        // 判断epsilon
        if (find(first[non].begin(), first[non].end(), '#') != first[non].end())
        {
            for (char alpha : follow[non])
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + "#";
        }
    }
    // 输出
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