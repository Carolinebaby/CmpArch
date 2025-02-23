#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include "Tomasulo.hpp"

int main(void)
{
    std::ofstream outfile("output1.txt");
    Tomasulo tomasulo;
    tomasulo.GetInstructions("input1.txt");
    tomasulo.OutResult(0, outfile);
    int clock = 0;
    while(clock < 26)
    {
        clock++;
        tomasulo.Issue(clock);
        tomasulo.Execution(clock);
        tomasulo.WriteResult(clock);
        tomasulo.OutResult(clock, outfile);
    }
    outfile.close();
}