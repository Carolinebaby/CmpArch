#ifndef STOREBUFFER_H
#define STOREBUFFER_H

#include "Instruction.hpp"

struct StoreBuffer // 把寄存器的值写入存储器
{
    StoreBuffer();  // 初始化参数
    void clear();   // 清空
    void LoadInstruction(Instruction *ins, int clock);  // 载入指令
    bool CanMem();  // 判断是否可以写入存储器
    bool CanExec();
    bool CanIssue2Exec();

    // 输出字符串使用的函数
    std::string str();
    std::string Q2str(int Q);

    std::string name;       // 缓冲区的名称
    bool busy;              // 是否繁忙
    std::string address;    // 存储地址
    std::string Vj;
    int Qj;        
    std::string Vt; 
    int Qt;
    Instruction *ins;       // 存储指令
};
#endif