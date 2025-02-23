#include"ReservationStation.hpp"
#include"RegisterStatus.hpp"
#include "ReorderBuffer.hpp"
#include "StoreBuffer.hpp"
#include"Instruction.hpp"
#include"LoadBuffer.hpp"
#include "config.hpp"
#include "ALU.hpp"
#include <queue>

class Tomasulo
{
public:   
    Tomasulo();  // 初始化（主要初始化各个结构的 name ）
    void GetInstructions(std::string filename);  // 从文件中得到指令序列 
    void Issue(int clock);          // 发射指令
    void Execution(int clock);      // 执行指令
    void WriteResult(int clock);    // 写 CDB
    void Commit(int clock);         // 提交
    void OutResult(int clock, std::ofstream& outfile);  // 输出结果到文件中
    void OutTestResult(int clock, std::ofstream &outfile);
    bool End();

private:
    // 设置 保留站中的 Qj Qk Vj Vk 和 存储缓冲区的 V, Q
    void Setjk(int cur_idx, std::string reg_str, std::string &V, int &Q);
    // 广播 算术运算结果 和 载入指令的结果
    void Bus(int dep_idx, std::string value, int clock);
    // 判定 存储指令 是否可以访问写存储器（前提：寄存器的值可用）
    bool StoreReady(Instruction* i);
    // 根据指令找到保存指令的重排序缓冲区
    int ROBIdx(Instruction *i);
    // 判断是否预测成功的函数（形式定义）
    bool PredictionCorrect(Instruction* jump_ins);
    // 更新寄存器的值（在 COMMIT 修改寄存器状态后使用）
    void UpdateRegSt(RegisterStatus& reg_st, std::string des_str);

    
    RegisterStatus ireg[32];    // 整型寄存器
    RegisterStatus freg[32];    // 浮点型寄存器

    std::vector<ReorderBuffer> ROB;     // 重排序缓冲区

    ReservationStation Add_RS[ADD_RS_SIZE];     // 整数加法保留站
    ReservationStation Mult_RS[MULT_RS_SIZE];   // 整数乘法保留站
    ReservationStation fAdd_RS[FADD_RS_SIZE];   // 浮点数加法保留站
    ReservationStation fMult_RS[FMULT_RS_SIZE]; // 浮点数乘法保留站
    ReservationStation Branch_RS[BRANCH_RS_SIZE];
    // 整合四个算术保留站（用于遍历）
    ReservationStation* Calc_RS[4] = {Add_RS, Mult_RS, fAdd_RS, fMult_RS};
    int Calc_RS_size[4] = {ADD_RS_SIZE, MULT_RS_SIZE, FADD_RS_SIZE, FMULT_RS_SIZE};
    bool Calc_RS_isFloat[4] = {false, false, true, true};

    LoadBuffer Load_bf[LOAD_BUFFER_SIZE];       // 载入缓冲区
    StoreBuffer Store_bf[STORE_BUFFER_SIZE];    // 存储缓冲区

    // 计算单元
    ALU intALU;  // 整型计算单元，用于：存储地址计算，整型加法乘法，跳转判断
    ALU faddALU; // 浮点加法单元
    ALU fmultALU;// 浮点乘法单元
    ALU* Calc_ALU[4] = {&intALU, &intALU, &faddALU, &fmultALU};
    // 把上面的 ALU 聚合在一起，与 四个 算术 RS 对应

    Instruction ins[100];       // 假设最多运行100条指令
    Instruction loopins[20];    // 循环内的所有指令
    int loopins_num = 0;        // 循环内的指令数目
    int ins_num = 0;            // 指令数目
    std::queue<int> issue_que;  // 发射指令的序列（存储的是ins中的指令下标）
    int remain_ins_cnt;         // 剩余执行的指令数
};