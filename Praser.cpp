#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

class Praser
{
public:
    Praser();
    void generatePredictTable();
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
            if (terminal.find(alpha) != terminal.end())
            {
                predictTable[symbol[non]][symbol[alpha]] = non + " -> " + alpha;
                continue;
            }
            // 简单处理， 没有考虑E->A|num这种情况, 想递归调用
            // while (terminal.find(alpha) != terminal.end())
            //     alpha = 
        }
    }
}