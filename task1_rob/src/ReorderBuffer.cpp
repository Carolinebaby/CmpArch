#include "ReorderBuffer.hpp"

ReorderBuffer::ReorderBuffer()
{
    entry = -1;
    busy = false;
    ins = nullptr;
}
ReorderBuffer::ReorderBuffer(int e, Instruction *i)
{
    entry = e;
    busy = true;
    ins = i;
    destination = ins->t;
}

std::string ReorderBuffer::str(void)
{
    static const std::string Status2str[7] = {"ON_QUE", "ISSUE", "EXEC", "EX_CM", "WB", "COMMIT", "END"};
    return std::to_string(entry) + "\t" + (busy ? "Yes" : "No") + "\t" + ins->str() + "\t" + Status2str[ins->st] + "\t" + destination + "\t" + value;
}
