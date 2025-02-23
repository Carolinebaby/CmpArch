在当前文件夹输入
g++ -Iinclude src/*.cpp -o project.exe
./project
即可编译运行代码

文件结构：
Project
│  input1.txt
│  output1.txt
│  project.exe
│  README.txt
│  test_input_loop.txt
│  test_input_loop_course.txt
│
├─include
│      ALU.hpp     # 计算单元(主要存储ALU的状态和当前运行执行的指令)
│      config.hpp  # 配置文件，对保留站等作出数量设置，对指令执行时钟周期数作设置
│      Instruction.hpp          # 指令
│      LoadBuffer.hpp           # 载入缓冲区
│      RegisterStatus.hpp       # 寄存器状态
│      ReservationStation.hpp  	# 保留站
│      StoreBuffer.hpp          # 存储缓冲区
│      Tomasulo.hpp
│
└─src  # 头文件对应的实现程序 + 主程序入口文件
        ALU.cpp
        Instruction.cpp
        LoadBuffer.cpp
        main.cpp  # 主程序
        RegisterStatus.cpp
        ReservationStation.cpp
        StoreBuffer.cpp
        Tomasulo.cpp