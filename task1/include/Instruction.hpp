#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<string>

enum Operator
{
    fld, // 载入
    ld,
    fsd,
    sd,
    add,
    sub,
    addi,
    subi,
    mul,
    idiv,
    fadd,
    fsub,
    fmul,
    fdiv,
    bne
};

enum InsStatus
{
    IN_QUEUE,
    ISSUE,
    EXEC,
    EXEC_COMP,
    WRITE_RESULT,
    ENDING
};

class Instruction
{
public:
    Instruction();
    void Set(Operator op, std::string t, std::string j, std::string k, int idx, std::string tag);
    Instruction(const Instruction& ins);
    void issue(int clock);
    void exec(int clock);
    void memory(int clock);
    void write_result(int clock);
    void SetFrom(Instruction* instruction, int cur_idx);
    bool exec_end();
    bool mem_end();
    bool exec_start();
    std::string str();
    std::string status();

    Operator op;
    std::string t;  // 跳转指令为跳转的标签
    std::string j;
    std::string k;
    std::string tag; // 跳转标签，循环第一个指令有
    bool from;
    int idx;
    bool isFloat;
    InsStatus st;

private:
    const std::string Op2str[15] = {"fld", "ld", "fsd", "sd", "add", "sub", "addi", "subi", "mul", "div", "fadd", "fsub", "fmul", "fdiv", "bne"};
    int exec_remaining_time; // 执行时间
    int mem_remaining_time;
    int mem_time;
    int issue_time;
    int exec_start_time;
    int exec_comp_time;
    int write_result_time;
};

#endif