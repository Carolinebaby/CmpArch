#include"RegisterStatus.hpp"
#include"ReservationStation.hpp"
#include"LoadBuffer.hpp"
#include"Instruction.hpp"
#include "ALU.hpp"
#include "StoreBuffer.hpp"
#include "config.hpp"
#include <queue>

class Tomasulo
{
public:
    Tomasulo();
    void GetInstructions(std::string filename);
    void Issue(int clock);
    void Execution(int clock);
    void WriteResult(int clock);
    void OutResult(int clock, std::ofstream& outfile);
    void OutTestResult(int clock, std::ofstream& outfile);


private:
    bool End();
    void Setjk(int cur_idx, std::string reg_str, std::string &V, int &Q);
    void Bus(int dep_idx, std::string value, int clock);
    void Jump(Instruction* cur_ins, std::string tag, int clock);
    void UpdateReg(Instruction* ins, RegisterStatus& reg);

    RegisterStatus ireg[32];
    RegisterStatus freg[32];

    ReservationStation Add_RS[ADD_RS_SIZE];     // 整数加法保留站
    ReservationStation Mult_RS[MULT_RS_SIZE];   // 整数乘法保留站
    ReservationStation fAdd_RS[FADD_RS_SIZE];   // 浮点数加法保留站
    ReservationStation fMult_RS[FMULT_RS_SIZE]; // 浮点数乘法保留站
    ReservationStation Branch_RS[BRANCH_RS_SIZE];
    ReservationStation* Calc_RS[4] = {Add_RS, Mult_RS, fAdd_RS, fMult_RS};
    int Calc_RS_size[4] = {ADD_RS_SIZE, MULT_RS_SIZE, FADD_RS_SIZE, FMULT_RS_SIZE};
    bool Calc_RS_isFloat[4] = {false, false, true, true};

    LoadBuffer Load_bf[LOAD_BUFFER_SIZE];
    StoreBuffer Store_bf[STORE_BUFFER_SIZE];
   
    ALU intALU;
    ALU faddALU;
    ALU fmultALU;
    ALU* Calc_ALU[6] = {&intALU, &intALU, &faddALU, &fmultALU};
    

    Instruction ins[100];  // 假设最多运行100条指令
    Instruction loopins[20];
    int loopins_num = 0; // 循环内的指令数目
    int ins_num = 0;  // 指令数目
    std::queue<int> issue_que;
    
    int remain_ins_cnt;
    
};