#include "LoadBuffer.hpp"
LoadBuffer::LoadBuffer()
{
    busy = false;
    ins = nullptr;
    address = "";
}

void LoadBuffer::clear()
{
    busy = false;
    ins = nullptr;
    address = "";
}
bool LoadBuffer::CanIssue2Exec()
{
    if (busy&& Vj.length()  && !ins->exec_end() && ins->st == ISSUE)
            return true;
    return false;
}
bool LoadBuffer::CanExec()
{
    if (!busy || !Vj.length() || ins->exec_end() || ins->st != EXEC)
        return false;
    return true;
}
bool LoadBuffer::CanMem()
{
    if (!busy || !ins->exec_end() || ins->mem_end())
        return false;
    return true;
}

void LoadBuffer::LoadInstruction(Instruction *ins, int clock)
{
    busy = true;
    this->ins = ins;
    this->ins->issue(clock);
    this->address = ins->j + "(" + ins->k + ")";
}

std::string LoadBuffer::str()
{
    return name + "\t" + (busy ? "Yes" : "No") + "\t" + address;
}