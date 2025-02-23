#include "Instruction.hpp"

struct ReservationStation
{
    ReservationStation();   // 初始化各个参数
    void LoadInstruction(Instruction *ins, int clock);  // 载入 指令
    bool CanIssue2Exec(void);   // 判断指令是否有ISSUE状态转为EXEC状态
    bool CanStartExec(void);    // 判断指令是否可以开始执行(Vj和Vk有值)
    bool CanExec(void);         // 判断指令是否能执行
    void clear(void);           // 清空保留站
    
    /* 输出使用的函数 */
    std::string Q2str(int Q);
    std::string str(void);

    std::string name;   // 保留站名称
    bool busy;          // 保留站是否繁忙
    std::string Vj;     // V-表示值
    std::string Vk;     
    int Qj;             // Q-int型-表示所依赖的指令下标
    int Qk;

    Instruction *ins;   // 指令
};