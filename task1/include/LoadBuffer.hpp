#ifndef LOADBUFFER_H
#define LOADBUFFER_H

#include "Instruction.hpp"
// 载入缓冲区
struct LoadBuffer // 把存储器的值写入寄存器
{
    LoadBuffer();   // 初始化参数
    void clear();   // 清空
    bool CanIssue2Exec();
    bool CanExec();
    bool CanMem();
    void LoadInstruction(Instruction *ins, int clock);  // 载入指令
    std::string str();  // 输出显示的字符串

    std::string name;       // 缓冲区的名称
    bool busy;              // 是否繁忙
    std::string address;    // 载入的地址
    std::string Vj; 
    int Qj; 
    Instruction *ins;       // 缓冲区对应的载入指令
};


#endif