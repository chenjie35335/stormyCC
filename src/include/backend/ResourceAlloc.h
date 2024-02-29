#include <string>
#include <unordered_map>
#include "ValueKind.h"
using namespace std;
//寄存器名称
const char *regs[] = {
  "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
//Resource整体父类
class Resource {
    
};

//描述内存
class Memory : public Resource{
    public:
        int offset;
};
//描述每一个寄存器
class Register : public Resource{
    public:
        string  name;
        bool  used;//true表示使用,false表示未使用
};

//用于协同寄存器和原始值的工具
//先实现一个简单的，对于整个系统来说只需要实现一个manager
//最后一个问题：如何实现未满时刻的寄存器?
//还是使用原来的方法，这里为了方便，实现一些简单的方法：
/*
1、 不考虑是否需要保存（估计编译器会自动保存），仅不考虑0-4号寄存器和10,11号寄存器
2、 对于剩下的寄存器，如果使用已满，则使用换出方法
*/
class RegisterManager{
    public:
    Register RegisterAvail[32];
    vector<RawValue*> rawValueTable;//表示当前以处理的所有原始值的指针
    int tempReg;//表示当前状态下即将分配的寄存器号
    bool RegisterFull;//表示寄存器均被使用
    RegisterManager() {
        int i = 0;
        for(auto &reg : RegisterAvail) {
            reg.name = regs[i++];
            reg.used = false;
        }
        tempReg = 5;
        RegisterFull = false;
    }
    Register AllocRegister(RawValue *req) {//这个用来分配普通寄存器
            if(RegisterFull) {
            return this->LruAlloc();
            }
            else{
            while(tempReg >= 10 && tempReg <= 11 || RegisterAvail[tempReg].used)  tempReg++;
            if(tempReg == 31) {
                RegisterFull = true;
            }
            rawValueTable.push_back(req);
            RegisterAvail[tempReg].used = true;
            return RegisterAvail[tempReg];
         }
    }

    Register LruAlloc() {
        auto &minRaw = rawValueTable.at(0);
        for(auto &rawValue : rawValueTable) {
            if(minRaw->used_times >= rawValue->used_times) {
                minRaw = rawValue;
            }
        }
        minRaw
    }
};

class StackManager{
    public:
    int StackAvailable;//表示现在可用的栈的位置
    
}











































