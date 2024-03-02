#include <iostream>
#include <stdint.h>
#include <string>
#include "IrGraph.h"
#include "ResourceAlloc.h"
using namespace std;
//原始值，这个是生成asm的关键部分
enum {
    /// Not equal to.
    RBO_NOT_EQ,
    /// Equal to.
    RBO_EQ,
/// Greater than.
    RBO_GT,
/// Less than.
    RBO_LT,
/// Greater than or equal to
    RBO_GE,
/// Less than or equal to.
    RBO_LE,
/// Addition.
    RBO_ADD,
/// Subtraction.    
    RBO_SUB,
/// Multiplication.
    RBO_MUL,
/// Division.
    RBO_DIV,
/// Modulo.
    RBO_MOD,
/// Bitwise AND.
    RBO_AND,
/// Bitwise OR.
    RBO_OR,
/// Bitwise XOR.
    RBO_XOR,
/// Shift left logical.
    RBO_SHL,
/// Shift right logical.
    RBO_SHR,
/// Shift right arithmetic.
    RBO_SAR,
}BinaryOp;

enum {
    /// @brief 寄存器变量
    REGISTER,
    /// @brief 内存变量
    MEMORY,
    /// @brief 未分配
    UNALLOC
}RawValueStatus;

RegisterManager registerManager;
StackManager    stackManager;
//肯定要建立的是原始值和寄存器，内存之间的对应关系
//建立的方法是
class RawValue : public midend {
    public:
        char *name;
        RawSlice used_by;
        ValueKind* value;
        int used_times = 0;
        Register reg;
        Memory memory;
    private:
        int Status;
        bool lock;
        //RawType ty;
        void Load() {
            setStatus(REGISTER);
            cout << "  lw   " << reg.name << ", " <<memory.offset << "(sp)" <<endl; 
        }

        Register Visit() {
            switch(Status) {
                case REGISTER://如果该值存在寄存器中，则直接返回Register
                    break;
                case MEMORY://如果该值直接存在内存当中，需要分配内存
                    reg = registerManager.AllocRegister(this);
                    this->Load();
                    break;
                case UNALLOC:{
                    reg    = value->Visit(this);
                    memory = StackManager.AllocStack();
                    this->setStatus(REGISTER);
                    break;
                }
                default: assert(0);
            }
            used_times++;
            return reg;
        }
        void addLock() { lock = true};
        void resetlock() { lock = false};
        bool getlock() { return lock; }
        void setStatus(int status) { Status = status;}
        int getStatus() { return Status;}
        void store() {
            this->setStatus(MEMORY);
            cout << "  sw   " << reg.name << ", " <<memory.offset << "(sp)" <<endl; 
        }
};
//各类变量的总父类，作为接口
class ValueKind {
    public:
       virtual Register Visit(RawValue *rawValue) const = 0;
};
//整型变量
class RawInteger  : public ValueKind {
    public:
    int32_t value;
    Register Visit(RawValue *rawValue) const override {
        if(value == 0) {
            return RegisterManager.X0Alloc();
        }
        else {
            auto reg = RegisterManager.AllocRegister(rawValue);
            cout << "  li   " << reg.name << " " << integer.value << endl;
            return reg;
        }
    }
};
//浮点型变量
class RawFloat : public ValueKind {
    public:
    float value;
    void Visit() const override {

    }
}
//返回值
class RawReturn : public ValueKind {
    public:
    RawValue *value;
    Register Visit(RawValue *rawValue) const override {
        auto reg = value->Visit();
        cout << "  mv   a0, " + reg.name << endl;
        cout << "  ret" << endl;
    }
};

//运算
class RawBinary : public ValueKind {
    public:
    uint32_t op;
    RawValue *lhs;
    RawValue *rhs;
    Register Visit(RawValue *rawValue) const override {
        lhs->addlock();
        rhs->addlock();
        auto signl = lhs->Visit();
        auto signr = rhs->Visit();
        auto sign = RegisterManager.AllocRegister(rawValue);//给这个值分配了寄存器
        lhs->resetlock(); rhs->resetlock();
        switch(op) {
            case RBO_ADD: 
            //printf("parsing add\n");
            cout << "  add  " +sign.name+", "+ signl.name + ", " +signr.name <<endl; 
            break;
            case RBO_SUB: 
            //printf("parsing sub\n");
            cout << "  sub  " +sign.name+", "+ signl.name + ", " +signr.name <<endl;
            break;
            case RBO_EQ : 
            //printf("parsing eq\n");
            cout << "  xor  " +sign.name+", "+ signl.name + ", " +signr.name <<endl;
            cout << "  seqz " +sign.name+", "+ sign.name  <<endl;
            break;
            case RBO_NOT_EQ : 
            //("parsing eq\n");
            cout << "  xor  " +sign.name+", "+ signl.name + ", " +signr.name <<endl;
            cout << "  snez " +sign.name+", "+ sign.name  <<endl;
            break;
            case RBO_MUL :
            cout << "  mul  " + sign.name+", "+ signl.name + ", " +signr.name<<endl;
            break;
            case RBO_DIV :
            cout << "  div  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break; 
            case RBO_MOD :
            cout << "  rem  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break; 
            case RBO_LT :
            cout << "  slt  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break;
            case RBO_GT :
            cout << "  slt  " + sign.name + ", " + signr.name + ", "+signl.name << endl;
            break;
            case RBO_GE:
            cout << "  slt  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            cout << "  seqz " + sign.name + ", " + sign.name  <<endl;     
            break;
            case RBO_LE:
            cout << "  sgt  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            cout << "  seqz " + sign.name + ", " + sign.name  <<endl;     
            break;
            case RBO_OR:
            cout << "  or  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break;
            case RBO_XOR:
            cout << "  xor  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break;
            case RBO_AND:
            cout << "  and  " + sign.name+", "+ signl.name + ", " +signr.name << endl;
            break;
            default:assert(false);            
        }
    }
};
//读内存(这个和之前的rawValue的load不同)
/*
    rawValue之后load的结果是将对应的内存load到对应的寄存器上
    而这里的load是将load的结果load到其他的寄存器上
*/
class RawLoad : public ValueKind {
    public:
    RawValue *src;
    Register Visit(RawValue *rawValue) const override {
        assert(src->Status == MEMORY);
        auto SrcSign = src->Memory;
        src->addlock();
        auto sign = RegisterManager.AllocRegister(rawValue);
        src->resetlock();
        cout << "  lw   " << sign.name << ", " <<sign2.offset << "(sp)" <<endl;
    }
};
//写内存
class RawStore : public ValueKind {
    public:
    RawValue *value;
    RawValue *dest;
    Register Visit(RawValue *rawValue) const override {
        assert(dest->Status == MEMORY);
        auto sign1 = value->Visit();
        auto sign2 = dest->Memory;
        cout << "  sw   " << sign1.name << ", " <<sign2.offset << "(sp)" <<endl;
    }
}

/*
    考虑在一个基本块中的情况：
        1、对于一个原始值，既可以存放在内存栈中，也可以存放在寄存器中
        2、如果寄存器需要腾出来，我从原始值表中找到最近使用最少的原始值（LRU）
        3、一个寄存器可以存储多个值，但是一个值只能存在一个寄存器和一个内存当中
    结论：
        也就是说我需要在原始值内部声明一个寄存器和一个内存的对象，这个用来表示分配
        给当前原始值的寄存器和内存。但是真实情况下到底是使用内存还是寄存器，需要寄存器分配器
        和内存分配器来决定，内存分配器会调用原始值表，动态地分配。也就是说，这两个分配器来决定
        原始值的类型
        所以说：
        1、 原始值中需要两个变量来存储内存信息和寄存器信息
        2、 原始值中需要一个变量来存储原始值状态
        3、 初始时刻原始值状态为unalloc,然后每次遍历改变状态
        4、 寄存器分配器和栈分配器可能需要维护一个列表，用于存储所有原始值的引用，
        5、 原始值应该也要定义一个函数用来完成保存功能的实现
        6、 我的方法是： 铁打的内存， 不断变化的寄存器
*/