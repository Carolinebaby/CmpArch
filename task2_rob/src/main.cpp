#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include "Tomasulo.hpp"

int main(void)
{
    std::ofstream outfile("output2_rob.txt");
    Tomasulo tomasulo;
    tomasulo.GetInstructions("input2.txt");
    tomasulo.OutResult(0, outfile);
    int clock = 0;
    while(clock < 20)
    {
        clock++;
        tomasulo.Issue(clock);         // 发射指令
        tomasulo.Execution(clock);     // 执行指令
        tomasulo.WriteResult(clock);   // 写结果
        tomasulo.Commit(clock);        // 提交
        tomasulo.OutResult(clock, outfile);     // 把结果写到输出文件中
    }
    outfile.close();
}