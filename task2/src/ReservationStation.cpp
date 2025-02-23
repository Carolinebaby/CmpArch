#include "ReservationStation.hpp"

std::string ReservationStation::Q2str(int Q)
{
    return Q == -1 ? "" : "ins" + std::to_string(Q);
}
bool ReservationStation::CanExec(void)
{
    if(!busy || !CanStartExec() || ins->exec_end()|| ins->st != EXEC)
        return false;
    return true;
}
ReservationStation::ReservationStation()
{
    busy = false;
    Vj = "";
    Qj = -1;
    Vk = "";
    Qk = -1;
    ins = nullptr;
}
void ReservationStation::LoadInstruction(Instruction *ins, int clock)
{
    busy = true;
    this->ins = ins;
    this->ins->issue(clock);
}

std::string ReservationStation::str(void)
{
    const std::string Op2str[15] = {"fld", "ld", "fsd", "sd", "add", "sub", "addi", "subi", "mul", "div", "fadd", "fsub", "fmul", "fdiv", "bne"};
    return name + "\t" + (busy ? "Yes" : "No") + "\t" + (busy ? Op2str[ins->op] : "") + "\t" + Vj + "\t" + Vk + "\t" + (busy ? Q2str(Qj) : "") + "\t" + (busy ? Q2str(Qk) : "");
}

bool ReservationStation::CanIssue2Exec(void)
{
    if(CanStartExec() && !ins->exec_end() && ins->st == ISSUE)
        return true;
    return false;
}
bool ReservationStation::CanStartExec(void)
{
    if(!busy) return false;
    if(ins->from) return false;
    if (Vj.length() > 0 && Vk.length() > 0)
        return true;
    return false;
}
void ReservationStation::clear(void)
{
    busy = false;
    Vj = "";
    Qj = -1;
    Vk = "";
    Qk = -1;
    ins = nullptr;
}
