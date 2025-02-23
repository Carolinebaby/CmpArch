#include "Tomasulo.hpp"
#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <algorithm>

std::string GetResult(Instruction* ins) // 寄存器重命名
{
    static std::string prefix[15] = {"FMA", "MA", "FSD", "SD", "AD", "SUB", "ADI", "SUBI", "MUL", "DIV", "FAD", "FSUB", "FMUL", "FDIV", "BNE"};
    return prefix[ins->op] + std::to_string(ins->idx);
}

static const int OpType[15] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5};

Tomasulo::Tomasulo()
{
    static const std::string RS_Name[4] = {"Add", "Mult", "fAdd", "fMult"};
    for (int rs_idx = 0; rs_idx < 4; rs_idx++)
    {
        ReservationStation *RS = Calc_RS[rs_idx];
        for (int i = 0; i < Calc_RS_size[rs_idx]; i++)
        {
            RS[i].name = RS_Name[rs_idx] + std::to_string(i + 1);
        }
    }

    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        Load_bf[i].name = "Load" + std::to_string(i + 1);
    }

    for (int i = 0; i < STORE_BUFFER_SIZE; i++)
    {
        Store_bf[i].name = "Store" + std::to_string(i + 1);
    }

    for (int i = 0; i < BRANCH_RS_SIZE; i++)
    {
        Branch_RS[i].name = "Branch" + std::to_string(i + 1);
    }

    remain_ins_cnt = 0;
}

int RegIdx(std::string reg)
{
    return std::stoi(reg.substr(1));
}

bool isNumber(const std::string &str)
{
    for (char ch : str)
    {
        if (!std::isdigit(ch))
        {
            return false; // 如果有任何字符不是数字，则返回false
        }
    }
    return true; // 所有字符都是数字，返回true
}

/*
@brief 设置 保留站中的 V 和 Q
@param cur_idx  当前保留站对应的指令在指令序列中的下标
@param reg_str  j 或 k 对应的字符串
@param V  保留站的 Vj 或 Vk
@param Q  保留站的 Qj 或 Qk
@param isFloat 指令是否为浮点指令
*/
void Tomasulo::Setjk(int cur_idx, std::string reg_str, std::string &V, int &Q)
{
    if (isNumber(reg_str))
    {
        V = reg_str;
        Q = -1;
        return;
    }
    RegisterStatus *reg;
    if (ins[cur_idx].isFloat)
        reg = freg;
    else
        reg = ireg;

    // 寻找最近依赖
    int nearest_dep = cur_idx - 1;
    int ri = RegIdx(reg_str);

    for (; nearest_dep >= 0; nearest_dep--)
    {
        if (ins[nearest_dep].t == reg_str && ins[nearest_dep].op != sd && ins[nearest_dep].op != fsd && ins[nearest_dep].op != bne)
        {
            break;
        }
    }
    // 如果没有最近依赖或者最近依赖执行结束
    if (nearest_dep == -1 || ins[nearest_dep].st == ENDING)
    {
        if (!reg[ri].value.length()) // 寄存器中没有存储值
        {
            V = "R(x" + std::to_string(ri) + ")";
        }
        else // 寄存器中有值
        {
            V = reg[ri].value;
        }
    }
    else // 最近依赖正在 执行
    {
        Q = nearest_dep;
        V = "";
    }
}

// 发射指令
void Tomasulo::Issue(int clock)
{
    int issue_num = 0;
    // 队列不为空，且未发射2条指令
    while (!issue_que.empty() && issue_num < 2)
    {
        int top = issue_que.front(); // 即将发射的指令
        int type = OpType[ins[top].op];

        if (type == 0) // Load 或 Store
        {
            switch (ins[top].op)
            {
            case fld:
            case ld:
            {
                int free = -1;
                for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
                {
                    if (!Load_bf[i].busy)
                    {
                        free = i;
                        break;
                    }
                }
                if (free == -1)
                    return; // 如果载入缓冲无空闲，不发射指令

                // 发射指令
                issue_que.pop();
                // 载入缓冲区
                Load_bf[free].LoadInstruction(&ins[top], clock);
                // 设置寄存器
                int reg_idx = RegIdx(ins[top].t);
                if (ins[top].isFloat)
                {
                    if (!freg[reg_idx].busy)
                        freg[reg_idx].set(true, top);
                }
                else
                {
                    if (!ireg[reg_idx].busy)
                        ireg[reg_idx].set(true, top);
                }
                Setjk(top, ins[top].j, Load_bf[free].Vj, Load_bf[free].Qj); 
                issue_num++;
                break;
            }
            case fsd:
            case sd:
            {
                int free = -1;
                for (int i = 0; i < STORE_BUFFER_SIZE; i++)
                {
                    if (!Store_bf[i].busy)
                    {
                        free = i;
                        break;
                    }
                }
                if (free == -1)
                    return; // 如果载入缓冲无空闲，不发射指令
                // 发射指令
                issue_que.pop();
                // 载入缓冲区
                Store_bf[free].LoadInstruction(&ins[top], clock);
                Setjk(top, ins[top].t, Store_bf[free].Vt, Store_bf[free].Qt);
                Setjk(top, ins[top].j, Store_bf[free].Vj, Store_bf[free].Qj);  

                issue_num++;
                break;
            }
            }
        }
        else if (type == 5) // bne 指令
        {
            int free = -1;
            for (int i = 0; i < BRANCH_RS_SIZE; i++)
            {
                if (!Branch_RS[i].busy)
                {
                    free = i;
                    break;
                }
            }
            if (free == -1)
                return; // 无空闲保留站

            // 把循环指令载入
            int sz = ins_num;
            // 假设整个指令序列都是循环指令
            for (int i = 0; i < loopins_num; i++)
            {
                ins[sz + i].Set(loopins[i].op, loopins[i].t, loopins[i].j, loopins[i].k, sz + i, loopins[i].tag);
                ins[sz + i].SetFrom(ins, sz + i);
                issue_que.push(sz + i);
                remain_ins_cnt++;
            }
            ins_num += loopins_num;
            // 发射指令
            issue_que.pop();
            // 加法保留站
            Branch_RS[free].LoadInstruction(&ins[top], clock);

            // 修改保留站的信息（Vj, Vk, Qj, Qk）
            Setjk(top, ins[top].j, Branch_RS[free].Vj, Branch_RS[free].Qj);
            Setjk(top, ins[top].k, Branch_RS[free].Vk, Branch_RS[free].Qk);

            return;
        }
        else
        {
            ReservationStation *RS = Calc_RS[type - 1];
            bool isFloat = Calc_RS_isFloat[type - 1];
            int size = Calc_RS_size[type - 1];

            int free = -1;
            for (int i = 0; i < size; i++)
            {
                if (!RS[i].busy)
                {
                    free = i;
                    break;
                }
            }

            if (free == -1)
                return;

            // 发射指令
            issue_que.pop();
            // 加法保留站
            RS[free].LoadInstruction(&ins[top], clock);

            // 修改保留站的信息（Vj, Vk, Qj, Qk）
            Setjk(top, ins[top].j, RS[free].Vj, RS[free].Qj);
            Setjk(top, ins[top].k, RS[free].Vk, RS[free].Qk);

            // 设置寄存器
            // 设置寄存器
            int reg_idx = RegIdx(ins[top].t);
            if (ins[top].isFloat)
            {
                if (!freg[reg_idx].busy)
                    freg[reg_idx].set(true, top);
            }
            else
            {
                if (!ireg[reg_idx].busy)
                    ireg[reg_idx].set(true, top);
            }

            issue_num++;
        }
    }
}

void Tomasulo::Execution(int clock)
{
    // 载入 & 存储 缓冲区
    // 先进行写入操作
    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        if (Load_bf[i].CanMem())
            Load_bf[i].ins->memory(clock);
    }

    for (int i = 0; i < STORE_BUFFER_SIZE; i++)
    {
        if (Store_bf[i].CanMem())
        {
            Store_bf[i].ins->memory(clock);
            Store_bf[i].clear();
            remain_ins_cnt--;
        }
    }

    // 处理执行操作 + 处理 ALU 分配
    // 由于假设每个 ALU 一个周期只能执行一个指令
    // 所以选择可执行的 idx 最小的执行
    // 逻辑：如果没有指令占用 ALU，选取 idx 最小的指令占用ALU
    // 如果有指令占用ALU，则运行 ALU 载入的指令
    // 提前判断是否结束，结束则释放 ALU，避免后续阶段释放ALU导致当前时钟周期没有指令执行

    // 执行计算功能 -- 浮点数加法、浮点数乘法
    for (int rs_idx = 2; rs_idx < 4; rs_idx++)
    { // rs_idx = 2 --> fAdd_RS
        // rs_idx = 3 --> fMult_RS
        ReservationStation *RS = Calc_RS[rs_idx];
        int size = Calc_RS_size[rs_idx];
        bool isFloat = Calc_RS_isFloat[rs_idx];

        if (Calc_ALU[rs_idx]->busy)
        { // ALU 繁忙，则直接执行ALU中的指令
            Calc_ALU[rs_idx]->ins->exec(clock);
        }
        else // Calc_ALU 不繁忙，选择一个 idx 最小的进入 ALU 执行
        {
            int exec_idx = ins_num;
            for (int i = 0; i < size; i++)
            {
                if (RS[i].CanExec())
                    exec_idx = std::min(exec_idx, RS[i].ins->idx);
            }

            if (exec_idx != ins_num)
            {                                                // 有可执行的
                Calc_ALU[rs_idx]->set(true, &ins[exec_idx]); // 把指令载入 ALU 中执行
                ins[exec_idx].exec(clock);                   // 执行
            }
        }

        // 执行结束，判断是否执行期结束
        if (Calc_ALU[rs_idx]->busy && Calc_ALU[rs_idx]->ins->exec_end())
        { // ALU中的指令执行结束，释放 ALU 资源
            Calc_ALU[rs_idx]->clear();
        }
    }

    // 整数ALU：整数加法、整数乘法，分支判断，地址计算（Load、Store）
    if (intALU.busy)
    {
        intALU.ins->exec(clock);
    }
    else
    {
        int exec_idx = ins_num;
        // 执行计算功能 -- 整数加法 乘法
        for (int rs_idx = 0; rs_idx < 2; rs_idx++)
        {
            ReservationStation *RS = Calc_RS[rs_idx];
            int size = Calc_RS_size[rs_idx];
            bool isFloat = Calc_RS_isFloat[rs_idx];
            for (int i = 0; i < size; i++)
            {
                if (RS[i].CanExec())
                    exec_idx = std::min(exec_idx, RS[i].ins->idx); // 选择最小的指令下标
            }
        }

        // 分支判断指令
        for (int i = 0; i < BRANCH_RS_SIZE; i++)
        {
            if (Branch_RS[i].CanExec())
                exec_idx = std::min(exec_idx, Branch_RS[i].ins->idx);
        }

        // 载入、存储地址计算
        for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
        {
            if (Load_bf[i].CanExec())
                exec_idx = std::min(exec_idx, Load_bf[i].ins->idx);
        }

        for (int i = 0; i < STORE_BUFFER_SIZE; i++)
        {
            if (Store_bf[i].CanExec())
                exec_idx = std::min(exec_idx, Store_bf[i].ins->idx);
        }

        // 如果有可执行的
        if (exec_idx != ins_num)
        {
            intALU.set(true, &ins[exec_idx]);
            ins[exec_idx].exec(clock);
        }
    }

    // 如果结束完成，则 ALU 清空，以便下一个时钟周期使用
    if (intALU.busy && intALU.ins->exec_end())
    {
        intALU.clear();
    }

    // 处理处于 ISSUE 阶段 的指令，把可执行的指令状态设置为 EXEC
    // 处理运算指令
    for (int rs_idx = 0; rs_idx < 4; rs_idx++)
    {
        ReservationStation *RS = Calc_RS[rs_idx];
        int size = Calc_RS_size[rs_idx];
        bool isFloat = Calc_RS_isFloat[rs_idx];

        // 处理 ISSUE 阶段的指令
        for (int i = 0; i < size; i++)
        {
            if (RS[i].CanIssue2Exec())
                RS[i].ins->exec(clock); // ISSUE->EXEC
        }
    }

    for (int i = 0; i < BRANCH_RS_SIZE; i++)
    {
        if (Branch_RS[i].CanIssue2Exec())
            Branch_RS[i].ins->exec(clock);
    }

    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        if (Load_bf[i].CanIssue2Exec())
            Load_bf[i].ins->exec(clock); // ISSUE->EXEC
    }

    for (int i = 0; i < STORE_BUFFER_SIZE; i++)
    {
        if (Store_bf[i].CanIssue2Exec())
            Store_bf[i].ins->exec(clock); // ISSUE->EXEC
    }
}

// 写结果
void Tomasulo::WriteResult(int clock)
{
    for (int i = 0; i < BRANCH_RS_SIZE; i++)
    {
        if (!Branch_RS[i].busy || Branch_RS[i].ins->st == ISSUE || Branch_RS[i].ins->st == EXEC)
            continue;
        if (Branch_RS[i].ins->st == EXEC_COMP)
        {
            Branch_RS[i].ins->st = WRITE_RESULT;
            // 广播跳转结果
            Jump(Branch_RS[i].ins, Branch_RS[i].ins->t, clock);
            Branch_RS[i].ins->st = ENDING;
            Branch_RS[i].clear();
            remain_ins_cnt--;
        }
    }
    
    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        if (!Load_bf[i].busy || Load_bf[i].ins->st == ISSUE || Load_bf[i].ins->st == EXEC)
            continue;
        if (Load_bf[i].ins->st == EXEC_COMP && Load_bf[i].ins->mem_end())
        {
            Load_bf[i].ins->st = WRITE_RESULT;
            continue;
        }
        else if (Load_bf[i].ins->st == WRITE_RESULT)
        {
            Load_bf[i].ins->write_result(clock);
            // 运行结束, 写结果
            std::string result = GetResult(Load_bf[i].ins); 
            Bus(Load_bf[i].ins->idx, result, clock); // 广播结果

            int ri = RegIdx(Load_bf[i].ins->t);
            // 结果直接写入寄存器
            if (Load_bf[i].ins->isFloat)
            {
                freg[ri].set(false, result, -1);
                UpdateReg(Load_bf[i].ins, freg[ri]);
            }
            else
            {
                ireg[ri].set(false, result, -1);
                UpdateReg(Load_bf[i].ins, ireg[ri]);
            }

            Load_bf[i].ins->st = ENDING;
            Load_bf[i].clear();
            remain_ins_cnt--;
        }
    }

    for (int rs_idx = 0; rs_idx < 4; rs_idx++)
    {
        ReservationStation *RS = Calc_RS[rs_idx];
        int size = Calc_RS_size[rs_idx];
        bool isFloat = Calc_RS_isFloat[rs_idx];

        for (int i = 0; i < size; i++)
        {
            if (!RS[i].busy || !RS[i].CanStartExec() || RS[i].ins->st == EXEC)
                continue;
            if (RS[i].ins->st == EXEC_COMP)
            {
                RS[i].ins->st = WRITE_RESULT;
                continue;
            }
            else if (RS[i].ins->st == WRITE_RESULT)
            {
                RS[i].ins->write_result(clock);
                // 运行结束，写结果
                ReservationStation &rs = RS[i];

                std::string result = GetResult(rs.ins);

                int ri = RegIdx(rs.ins->t);
                // 结果直接写入寄存器
                if (rs.ins->isFloat)
                {
                    freg[ri].set(false, result, -1);
                    UpdateReg(rs.ins, freg[ri]); 
                }
                else
                {
                    ireg[ri].set(false, result, -1);
                    UpdateReg(rs.ins, ireg[ri]);
                }

                rs.ins->st = ENDING;
                Bus(rs.ins->idx, result, clock);
                rs.clear();
                remain_ins_cnt--;
            }
        }
    }
}

void Tomasulo::UpdateReg(Instruction* instr, RegisterStatus &reg) 
{
    for (int i = instr->idx + 1; i < ins_num; i++)
    {
        if (ins[i].isFloat == instr->isFloat && ins[i].st != IN_QUEUE && ins[i].st != ENDING && ins[i].op != sd && ins[i].op != fsd && ins[i].op != bne && ins[i].t == instr->t)
        { // 算术指令或 Load 指令
            reg.set(true, i);
            return;
        }
    }
}

void Tomasulo::Jump(Instruction *cur_ins, std::string tag, int clock)
{
    int cur_idx = cur_ins->idx;

    for (int i = cur_idx + 1; i < ins_num; i++)
    {
        if (ins[i].tag == tag)
        {
            ins[i].from = false;
            if (ins[i].st == ISSUE)
                ins[i].exec(clock);
            for (int j = i + 1; j < ins_num; j++)
            {
                if (ins[j].tag.length())
                    return;
                ins[j].from = false;
                if (ins[j].st == ISSUE)
                    ins[j].exec(clock);
            }
            return;
        }
    }
}

void Tomasulo::Bus(int dep_idx, std::string value, int clock)
{
    for (int rs_idx = 0; rs_idx < 4; rs_idx++)
    {
        ReservationStation *RS = Calc_RS[rs_idx];
        int size = Calc_RS_size[rs_idx];
        bool isFloat = Calc_RS_isFloat[rs_idx];

        for (int i = 0; i < size; i++)
        {
            ReservationStation &rs = *(RS + i);
            if (rs.busy && (rs.Qj == dep_idx || rs.Qk == dep_idx)) // 处于发射阶段
            {
                if (rs.Qj == dep_idx)
                {
                    rs.Vj = value;
                    rs.Qj = -1;
                }
                if (rs.Qk == dep_idx)
                {
                    rs.Vk = value;
                    rs.Qk = -1;
                }
                if (rs.CanStartExec())
                {
                    if (rs.ins->st == ISSUE)
                        rs.ins->exec(clock);
                }
            }
        }
    }

    for (int i = 0; i < BRANCH_RS_SIZE; i++)
    {
        if (!Branch_RS[i].CanStartExec() && (Branch_RS[i].Qj == dep_idx || Branch_RS[i].Qk == dep_idx))
        {
            if (Branch_RS[i].Qj == dep_idx)
            {
                Branch_RS[i].Vj = value;
                Branch_RS[i].Qj = -1;
            }
            if (Branch_RS[i].Qk == dep_idx)
            {
                Branch_RS[i].Vk = value;
                Branch_RS[i].Qk = -1;
            }
            if (Branch_RS[i].CanStartExec())
            {
                if (Branch_RS[i].ins->st == ISSUE)
                    Branch_RS[i].ins->exec(clock);
            }
        }
    }

    for (int i = 0; i < STORE_BUFFER_SIZE; i++)
    {
        if (Store_bf[i].busy && !Store_bf[i].Vt.length() && (Store_bf[i].Qt == dep_idx))  
        {
            Store_bf[i].Vt = value;
            Store_bf[i].Qt = -1;
        }
        if (Store_bf[i].busy && !Store_bf[i].Vj.length() && (Store_bf[i].Qj == dep_idx))  
        {
            Store_bf[i].Vj = value;
            Store_bf[i].Qj = -1;
        }
    }

    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        if (Load_bf[i].busy && !Load_bf[i].Vj.length() && (Load_bf[i].Qj == dep_idx))  
        {
            Load_bf[i].Vj = value;
            Load_bf[i].Qj = -1;
            // 下一个阶段可以开始执行了
            if (Load_bf[i].ins->st == ISSUE)
                Load_bf[i].ins->exec(clock);  // ISSUE -> EXEC
        }
    }
}

bool Tomasulo::End()
{
    return remain_ins_cnt == 0;
}

void Tomasulo::GetInstructions(std::string filename)
{
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    while (std::getline(file, line))
    {
        // 预处理步骤：将逗号和括号替换为空格
        std::replace(line.begin(), line.end(), ',', ' '); // 替换逗号为空格
        std::replace(line.begin(), line.end(), '(', ' '); // 替换左括号为空格
        std::replace(line.begin(), line.end(), ')', ' '); // 替换右括号为空格

        std::stringstream tt(line);
        std::string op_str, t, j, k, tag, to;

        // 跳过空行
        if (line.empty())
            continue;

        // 如果是标签行，提取标签
        size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            tag = line.substr(0, pos);   // 获取标签名
            line = line.substr(pos + 1); // 去掉标签部分
        }

        std::stringstream ss(line);

        // 读取操作符
        ss >> op_str;

        // 将操作符字符串转换为 Operator 枚举值
        Operator op;
        if (op_str == "fld")
            op = fld;
        else if (op_str == "ld")
            op = ld;
        else if (op_str == "fsd")
            op = fsd;
        else if (op_str == "sd")
            op = sd;
        else if (op_str == "add")
            op = add;
        else if (op_str == "sub")
            op = sub;
        else if (op_str == "addi")
            op = addi;
        else if (op_str == "subi")
            op = subi;
        else if (op_str == "mul")
            op = mul;
        else if (op_str == "div")
            op = idiv;
        else if (op_str == "fadd" || op_str == "fadd.d")
            op = fadd;
        else if (op_str == "fsub" || op_str == "fsub.d")
            op = fsub;
        else if (op_str == "fmul" || op_str == "fmul.d")
            op = fmul;
        else if (op_str == "fdiv" || op_str == "fdiv.d")
            op = fdiv;
        else if (op_str == "bne")
            op = bne;
        else
        {
            std::cerr << "Unknown operator: " << op_str << std::endl;
            continue;
        }
        // 解析操作数
        if (op == bne)
        {
            ss >> j >> k >> t; // 三个操作数
        }
        else if(op == ld || op == fld || op == sd || op == fsd)
        {
            ss >> t >> k >> j;
        }
        else
        {
            ss >> t >> j >> k; // bne 指令的格式
        }

        // 对于 addi 和 bne，tag 只有在第一次指令时才需要设置
        if (!tag.empty())
        {
            int idx = ins_num++;
            ins[idx].Set(op, t, j, k, idx, tag);
            ins[idx].SetFrom(ins, idx);
            loopins[loopins_num++].Set(op, t, j, k, idx, tag);
        }
        else
        {
            int idx = ins_num++;
            ins[idx].Set(op, t, j, k, idx, "");
            ins[idx].SetFrom(ins, idx);
            loopins[loopins_num++].Set(op, t, j, k, idx, "");
        }
    }

    file.close();
    remain_ins_cnt = ins_num;
    for (int i = 0; i < remain_ins_cnt; i++)
    {
        issue_que.push(i);
    }
}

void Tomasulo::OutResult(int clock, std::ofstream &outfile) 
{
    outfile << "\n------------------ clock " << clock << "--------------------\n";
    outfile << "\nReservation Stations: \n";
    outfile << "Name\tBusy\tOp\tVj\tVk\tQj\tQk\n";

    for (int i = 0; i < FADD_RS_SIZE; i++)
    {
        outfile << fAdd_RS[i].str() << "\n";
    }
    for (int i = 0; i < FMULT_RS_SIZE; i++)
    {
        outfile << fMult_RS[i].str() << "\n";
    }
    for (int i = 0; i < ADD_RS_SIZE; i++)
    {
        outfile << Add_RS[i].str() << "\n";
    }
    for (int i = 0; i < MULT_RS_SIZE; i++)
    {
        outfile << Mult_RS[i].str() << "\n";
    }
    for (int i = 0; i < BRANCH_RS_SIZE; i++)
    {
        outfile << Branch_RS[i].str() << "\n";
    }

    outfile << "\nLoad Buffer: \n";
    outfile << "Name\tBusy\tAddress\n";

    for (int i = 0; i < LOAD_BUFFER_SIZE; i++)
    {
        outfile << Load_bf[i].str() << "\n";
    }

    outfile << "\nStore Buffer: \n";
    outfile << "Name\tBusy\tAddress\tDes\n";

    for (int i = 0; i < STORE_BUFFER_SIZE; i++)
    {
        outfile << Store_bf[i].str() << "\n";
    }

    outfile << "\nRegister Status \n";
    outfile << "Reg\tBusy\tValue\n";
    for (int i = 0; i < 32; i++)
    {
        if (freg[i].busy || freg[i].value.length())
        {
            outfile << "f" << i << "\t" << freg[i].str() << "\n";
        }
    }
    for (int i = 0; i < 32; i++)
    {
        if (ireg[i].busy || ireg[i].value.length())
        {
            outfile << "x" << i << "\t" << ireg[i].str() << "\n";
        }
    }
    outfile << "\n";
}


void Tomasulo::OutTestResult(int clock, std::ofstream &outfile) 
{
    outfile << "\n------------------ clock " << clock << "--------------------\n";
    outfile << "Instruction status: \n";
    outfile << "tag\tinstruction\t\t\tIS\tEX\tMEM\tWR\n";

    for (int i = 0; i < ins_num; i++)
    {
        outfile << ins[i].str() << "\t" << ins[i].status() << "\n";
    }
    outfile << "\n";
    outfile << "remaining instructions: " << remain_ins_cnt << "\n";
}