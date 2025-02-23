#include "ALU.hpp"

ALU::ALU()
{
    busy = false;
    ins = nullptr;
}

void ALU::clear()
{
    busy = false;
    ins = nullptr;
}
void ALU::set(bool b, Instruction *i)
{
    busy = b, ins = i;
}