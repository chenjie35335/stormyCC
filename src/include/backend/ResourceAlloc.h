#include <string>
#include <unordered_map>
#include <ValueKind.h>
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
class RegisterManager{
    public:
    Register registerfile[32];//寄存器堆
    RawValue 
    RegisterManager() {
        
        
        
        
    }
};

class StackManager{
    public:
    int StackAvailable;//表示现在可用的栈的位置
    unordered_map <RawValue,Memory> StackAlloc;
}