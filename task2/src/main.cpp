#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include "Tomasulo.hpp"

int main(void)
{
    std::ofstream outfile("output2.txt");
    Tomasulo tomasulo;
    tomasulo.GetInstructions("input2.txt");
    tomasulo.OutResult(0, outfile);
    int clock = 0;
    while(clock < 20)
    {
        clock++;
        tomasulo.Issue(clock);
        tomasulo.Execution(clock);
        tomasulo.WriteResult(clock);
        tomasulo.OutResult(clock, outfile);
    }
    outfile.close();
}