#include "StoreBuffer.hpp"

std::string StoreBuffer::Q2str(int Q)
{
    return Q == -1 ? "" : "ins" + std::to_string(Q);
}

StoreBuffer::StoreBuffer()
{
    busy = false;
    ins = nullptr;
    Vt = "";
    Qt = -1;
    Vj = "";
    Qj = -1;
    address = "";
}

void StoreBuffer::clear()
{
    busy = false;
    ins = nullptr;
    Vt = "";
    Qt = -1;
    Vj = "";
    Qj = -1;
    address = "";
}

void StoreBuffer::LoadInstruction(Instruction *ins, int clock)
{
    busy = true;
    this->ins = ins;
    this->ins->issue(clock);
    this->address = ins->j + "(" + ins->k + ")";
}

std::string StoreBuffer::str()  // !
{
    return name + "\t" + (busy ? "Yes" : "No") + "\t" + (busy ? address : "") + "\t" + (Vt.length()? Vt: Q2str(Qt));
}

bool StoreBuffer::CanExec()
{
    if (!busy || !Vj.length() || ins->exec_end() || ins->st != EXEC)
        return false;
    return true;
}

bool StoreBuffer::CanMem()
{
    return Vt.length();
}

bool StoreBuffer::CanIssue2Exec()
{
    if (busy && Vj.length()  && !ins->exec_end() && ins->st == ISSUE)
            return true;
    return false;
}