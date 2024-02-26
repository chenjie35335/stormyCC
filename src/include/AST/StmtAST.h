#include "BaseAST.h"

class StmtAST : public BaseAST {
 public:
    //int num;
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Lval;
    uint32_t type;
    void Dump() const override {
      string sign;
      switch(type) {
          case STMTAST_RET:
              Exp->Dump(sign);
              cout << "  " << "ret " << sign << endl; 
              break;
          case STMTAST_LVA:{
            string sign1,sign2;
            //Lval->Dump(sign1);
            Exp->Dump(sign2);
            cout << "  " << "store "<< sign2 << ", "; //这里使用一种比较蠢的方法：用Dump输出Lval的信息，不换行，有点凑的方式
            int value = Exp->calc();
            Lval->Dump(value);
            break;
          }
          default:
              assert(0);
      }
      
    }
    void Dump(string &sign) const override{}
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};