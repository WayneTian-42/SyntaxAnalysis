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
private:
    std::set<char> terminal, nonterminal;
    std::unordered_map<char, std::vector<char>> grammar, first, follow;
    std::vector<std::vector<std::string>> predictTable;
    std::unordered_map<char, int> symbol;
};

int main()
{

}

Praser::Praser()
{
    predictTable.resize(terminal.size());
    for (int i = 0; i < terminal.size(); i++)
        predictTable[i].resize(nonterminal.size());
}

void Praser::generatePredictTable()
{
    for (auto &non : nonterminal)
    {
        std::vector<char> gra = grammar[non];
        for (char &alpha : gra)
        {
            if (terminal.find(alpha) != terminal.end() && find(first[non].begin(), first[non].end(), alpha) != first[non].end())
            {
                predictTable[symbol[non]][symbol[alpha]] = non + " -> " + alpha;
                continue;
            }
            char now = findTerminal(non, alpha);
            predictTable[symbol[non]][symbol[now]] = now + " -> " + alpha;
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
}

char Praser::findTerminal(const char start, const char now)
{
    std::vector<char> gra = grammar[now];
    for (char &alpha : gra)
    {
        while (terminal.find(alpha) == terminal.end() || find(first[start].begin(), first[start].end(), alpha) == first[start].end())
            return findTerminal(start, alpha);
        if (predictTable[symbol[start]][symbol[alpha]].empty())
            return alpha;
    }
}