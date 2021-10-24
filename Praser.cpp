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
    void readFromFile(const std::string &fileName); // 从文件中获取文法
    void caculateFirstSet(); // 计算FIRST集
    void caculaleFollowSet(); // 计算FOLLOW集
    void generatePredictTable(); // 构造预测分析表
    void predictAnalysis(const std::string &input); // 进行预测分析

private:
    void getFirstSet(const char non); // 得到指定非终结符的FIRST集
    void getFollowSet(const char non); // 得到指定非终结符的FOLLOW集
    inline std::string cutStr(const std::string &str, int pos); // 对字符串进行切片, 便于从指定位置开始输出

private:
    char start; // 文法开始符号
    std::set<char> terminal, nonterminal; // 终结符与非终结符
    std::unordered_map<char, std::set<char>> first, follow, preFollow; // first集, follow集, 构造follow集的准备
    std::unordered_map<char, std::vector<std::string>> grammar; // 文法
    std::vector<std::vector<std::string>> predictTable; // 预测分析表
    std::unordered_map<char, int> symbol; // 符号与数字相对应, 便于输出
};

int main(int argc, char *argv[])
{
    if (argc != 2)
        std::runtime_error("Error: Need 2 parameters\n");
    Praser praser;
    praser.readFromFile(argv[1]);
    praser.caculateFirstSet();
    praser.caculaleFollowSet();
    praser.generatePredictTable();

    std::string input;
    std::cout << "Please enter the symbols:" << std::endl;
    std::cin >> input;
    praser.predictAnalysis(input);

    return 0;
}

void Praser::readFromFile(const std::string &fileName)
{
    std::fstream fileIn;
    fileIn.open(fileName);
    if (!fileIn)
        throw std::runtime_error("Error: No such file\n");
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
    fileIn.close(); // 关闭文件
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
    std::cout << "First Set" << std::endl;
    for (char non : nonterminal)
    {
        std::cout << non << "  ";
        for (char alpha : first[non])
            std::cout << alpha << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
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
    // 重复执行多次，更新follow集
    for (int i = 0; i < preFollow.size(); i++)
    {
        for (auto &iter : preFollow)
            for (char non : iter.second)
                follow[non].insert(follow[iter.first].begin(), follow[iter.first].end());
    }
    std::cout << "Follow Set" << std::endl;
    for (char non : nonterminal)
    {
        std::cout << non << "  ";
        for (char alpha : follow[non])
            std::cout << alpha << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
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
        }
        // 判断epsilon
        if (first[non].find('#') != first[non].end())
        {
            for (char alpha : follow[non])
                predictTable[symbol[non]][symbol[alpha]] = tmp + non + " -> " + "#";
        }
    }
    // 输出
    std::cout << "Predict Analysis Table\n";
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
    std::cout << std::endl;
}

void Praser::predictAnalysis(const std::string &input)
{
    std::string target = input + "$";
    std::stack<char> stk;
    std::string analysisStk;
    analysisStk = analysisStk + "$" + start;
    stk.push('$');
    stk.push(start);
    int ptr = 0;

    std::cout << "\n" << std::setw(20) << std::left << "Stack" << std::setw(20) << std::left << "Input"
              << std::setw(20) << std::left << "Output" << std::endl;
    while (!stk.empty())
    {
        char snow = stk.top(), inow = target[ptr];
        if (terminal.find(snow) != terminal.end() || snow == '$')
        {
            if (snow == inow)
            {
                stk.pop();
                std::cout << std::setw(20) << std::left << analysisStk;
                analysisStk.pop_back();
                std::cout << std::setw(20) << std::left << cutStr(target, ptr);
                ptr++;
                std::cout << "accept " << snow << std::endl;
            }
            else if (snow == '#')
                stk.pop();
            else
            {
                std::cout << "error " << snow << " " << inow << std::endl;
                return;
            }
        }
        else
        {
            if (!predictTable[symbol[snow]][symbol[inow]].empty())
            {
                stk.pop();
                std::cout << std::setw(20) << std::left << analysisStk;
                analysisStk.pop_back();
                std::string tmp = predictTable[symbol[snow]][symbol[inow]];
                for (int i = tmp.size() - 1; i >= 5; i--)
                {
                    stk.push(tmp[i]);
                    if (tmp[i] != '#')
                        analysisStk += tmp[i];
                }
                std::cout << std::setw(20) << std::left << cutStr(target, ptr);
                std::cout << std::setw(20) << std::left << tmp << std::endl;
            }
            else
            {
                std::cout << "error " << snow << " " << inow << std::endl;
                return;
            }
        }
    }
    std::cout << "accept all successfully\n";
}

inline std::string Praser::cutStr(const std::string &str, int pos)
{
    std::string tmp;
    for (int i = pos; i < str.size(); i++)
        tmp += str[i];
    return tmp;
}