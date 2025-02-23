#ifndef REGISTERSTATUS_H
#define REGISTERSTATUS_H


#include<string>
struct RegisterStatus
{
    int entry = -1;  // 和 ReorderBuffer 存储的 entry 相匹配 
    // entry 又相当于寄存器正在等待的指令
    bool busy;                  // 是否繁忙
    std::string value = "";     // 当前的结果

    std::string str(void);      // 表示寄存器状态的字符串，用于输出
    void set(bool b, int e);    // 设置 busy, entry
    void set(bool b, std::string v, int e); // 设置 busy, value, entry
};

#endif