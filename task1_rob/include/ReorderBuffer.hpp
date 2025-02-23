#ifndef REORDERBUFFER_H
#define REORDERBUFFER_H

#include "Instruction.hpp"

struct ReorderBuffer
{
    int entry;
    bool busy;
    Instruction *ins;
    std::string destination;
    std::string value;
    

    ReorderBuffer();
    ReorderBuffer(int e, Instruction *i);
    std::string str(void);
};

#endif