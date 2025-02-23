#ifndef CONFIG_H
#define CONFIG_H

#define MULT_RS_SIZE 2
#define ADD_RS_SIZE 4
#define FMULT_RS_SIZE 2
#define FADD_RS_SIZE 3
#define RS_SIZE (MULT_RS_SIZE + ADD_RS_SIZE)
#define LOAD_BUFFER_SIZE 3
#define STORE_BUFFER_SIZE 3
#define REORDER_BUFFER_SIZE 6
#define BRANCH_RS_SIZE 3

#define LD_EXEC_TIME 1
#define LD_MEM_TIME 1
#define FLD_EXEC_TIME 1
#define FLD_MEM_TIME 1
#define ADD_EXEC_TIME 1
#define MUL_EXEC_TIME 3
#define DIV_EXEC_TIME 6
#define FADD_EXEC_TIME 2  // !
#define FMUL_EXEC_TIME 6
#define FDIV_EXEC_TIME 12

#endif