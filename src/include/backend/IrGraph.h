#include <string>
#include <iostream>
#include <vector>
#include "ValueKind.h"
using namespace std;
//这个是所有中端表示的接口
enum {
    RegisterValue,
    MemoryValue,
    Unally
}StorageType;
//中端代码需要使用！
class midend {
    public:
        virtual void Visit() const = 0;
};  
//这个用来表示源程序
class RawProgramme : public midend{
    public:
        RawSlice Value;
        RawSlice Funcs;
        void Visit() const override{
            cout << "  .text" << endl;
            Funcs.Visit();
        }
};
//这个表示每一个程序块
class RawSlice : public midend{
    public:
        vector<midend *> buffer;
        void Visit() const override{
            for(auto &ptr : buffer) {
                ptr->Visit();
            }
        }
};
//表示一个函数，一个函数多个基本块组成
class RawFunction : public midend{
    public:
        char * name;
        RawSlice params;
        RawSlice bbs;
        //RawType ty;
        void Visit() const override {
            cout << "  .global " << name+1 << endl;
            cout << name+1 << ":" << endl;
            cout << "  addi sp, sp, -256" <<  endl;
            bbs.Visit();
        }
};
//表示基本块，一个基本块由多个
class RawBasicBlock : public midend {
    public:
    char * name;
    RawSlice params;
    RawSlice insts;
    //RawType ty;
    void Visit() const override {
        insts.Visit();
    }
};










