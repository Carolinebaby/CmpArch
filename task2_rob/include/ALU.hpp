#ifndef ALU_H
#define ALU_H

#include"Instruction.hpp"

struct ALU
{
    bool busy;          // 是否繁忙
    Instruction* ins;   // 正在执行的指令

    ALU();              // 初始化状态
    void clear();       // 清空状态
    void set(bool b, Instruction* i);  // 设置
};

#endif