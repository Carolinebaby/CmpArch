#ifndef ALU_H
#define ALU_H

#include"Instruction.hpp"

struct ALU
{
    bool busy;
    Instruction* ins;
    ALU();
    void clear();
    void set(bool b, Instruction* i);
};

#endif