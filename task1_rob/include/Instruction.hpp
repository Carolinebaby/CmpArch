#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<string>

enum Operator
{
    fld,
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
    COMMIT,
    ENDING
};

class Instruction
{
public:
    void Set(Operator op, std::string t, std::string j, std::string k, int idx, std::string tag);
    
    /* 设置时间 */
    void issue(int clock);      // 设置发射时间
    void exec(int clock);       // 剩余执行时间-1，同时设置执行开始和结束时间
    void commit(int clock);     // 设置提交时间
    void memory(int clock);     // 设置访存时间
    void write_result(int clock);      // 设置写结果的时间

    /* 指令状态判定 */
    bool exec_start();  // 判定执行是否已经开始
    bool exec_end();    // 判定是否结束执行
    bool mem_end();     // 判断是否访存结束
    bool ready();       // 判定是否可以提交

    /* 输出指令状态 */
    std::string str();      // 输出结果使用的，指令字符串
    std::string status();   // 指令各个状态的时间字符串化展示

    Operator op;
    std::string t;  // 源寄存器（跳转指令的t为跳转的标签）
    std::string j;
    std::string k;
    std::string tag; // 跳转标签，循环第一个指令有
    InsStatus st;
    bool isFloat;
    int idx;

private:
    int exec_remaining_time;    // 执行时间
    int mem_remaining_time;     // 访存时间

    int issue_time;             // 发射时间
    int exec_start_time;        // 执行开始时间
    int exec_comp_time;         // 执行结束时间
    int mem_time;               // 访存时间
    int write_result_time;      // 写结果的时间
    int commit_time;            // 提交时间
};

#endif