#include "Instruction.hpp"
#include "config.hpp"
#include<iostream>

void Instruction::SetFrom(Instruction *instruction, int cur_idx)
{
    for (int i = cur_idx - 1; i >= 0; i--)
    {
        if (instruction[i].t == tag)
        {
            from = true;
            return;
        }
        if (instruction[i].from)
        {
            from = true;
            return;
        }
    }
    from = false;
}

static int OpType[15] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5};

Instruction::Instruction() {}
void Instruction::Set(Operator op, std::string t, std::string j, std::string k, int idx, std::string tag = "")
{
    this->op = op, this->t = t, this->j = j, this->k = k, this->idx = idx, this->tag = tag;
    switch (op)
    {
    case ld:
    case sd:
        exec_remaining_time = LD_EXEC_TIME;
        mem_remaining_time = LD_MEM_TIME;
        isFloat = false;
        break;
    case fsd:
    case fld:
        exec_remaining_time = FLD_EXEC_TIME;
        mem_remaining_time = FLD_MEM_TIME;
        isFloat = true;
        break;
    case add:
    case sub:
    case addi:
    case subi:
        exec_remaining_time = ADD_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = false;
        break;
    case mul:
        exec_remaining_time = MUL_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = false;
        break;
    case idiv:
        exec_remaining_time = DIV_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = false;
        break;
    case fadd:
    case fsub:
        exec_remaining_time = FADD_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = true;
        break;
    case fmul:
        exec_remaining_time = FMUL_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = true;
        break;
    case fdiv:
        exec_remaining_time = FDIV_EXEC_TIME;
        mem_remaining_time = 0;
        isFloat = true;
        break;
    default:
        exec_remaining_time = 1;
        mem_remaining_time = 0;
        isFloat = false;
        break;
    }
    st = IN_QUEUE;
    issue_time = exec_start_time = exec_start_time = mem_time = exec_comp_time = write_result_time = 0;
}
void Instruction::issue(int clock)
{
    st = ISSUE;
    issue_time = clock;
}
void Instruction::exec(int clock)
{
    if (st == ISSUE)
    {
        st = EXEC;
        return;
    }
    if (!exec_start_time && st == EXEC)
    {
        exec_start_time = clock;
    }

    exec_remaining_time--;
    if (!exec_remaining_time)
    {
        st = EXEC_COMP;
        exec_comp_time = clock;
    }
}

void Instruction::memory(int clock)
{
    mem_remaining_time--;
    if(!mem_time)
        mem_time = clock;
    if( !mem_remaining_time && op == sd)
        st = ENDING;
}

void Instruction::write_result(int clock)
{
    write_result_time = clock;
}
bool Instruction::exec_end()
{
    return exec_comp_time;
}

bool Instruction::mem_end()
{
    return mem_time && !mem_remaining_time;
}

bool Instruction::exec_start()
{
    if (exec_start_time)
        return true;
    return false;
}

std::string Instruction::str()
{
    return std::string(tag) + "\t" + Op2str[op] + "\t" + t + "\t" + j + "\t" + k;
}

std::string time2str(int time)
{
    return time ? std::to_string(time) : "";
}

std::string Instruction::status()
{
    return time2str(issue_time) + "\t" + time2str(exec_start_time) + (exec_start_time==exec_comp_time ? "" : "~" + time2str(exec_comp_time)) + "\t" + time2str(mem_time) + "\t" + time2str(write_result_time);
}

Instruction::Instruction(const Instruction &other) : op(other.op), t(other.t), j(other.j), k(other.k)
{

    switch (op)
    {
    case ld:
    case sd:
        exec_remaining_time = LD_EXEC_TIME;
        mem_remaining_time = LD_MEM_TIME;
        break;
    case fsd:
    case fld:
        exec_remaining_time = FLD_EXEC_TIME;
        mem_remaining_time = FLD_MEM_TIME;
        break;
    case add:
    case sub:
    case addi:
    case subi:
        exec_remaining_time = ADD_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    case mul:
        exec_remaining_time = MUL_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    case idiv:
        exec_remaining_time = DIV_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    case fadd:
    case fsub:
        exec_remaining_time = FADD_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    case fmul:
        exec_remaining_time = FMUL_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    case fdiv:
        exec_remaining_time = FDIV_EXEC_TIME;
        mem_remaining_time = 0;
        break;
    default:
        exec_remaining_time = 1;
        mem_remaining_time = 0;
        break;
    }
    st = ISSUE;
    issue_time = exec_start_time = mem_time = exec_comp_time = write_result_time = 0;
}
