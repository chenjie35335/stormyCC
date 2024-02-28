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
            //cout << "parsing Lval" << endl;
            string sign1,sign2;
            Lval->Dump(sign1);
            Exp->Dump(sign2);
            //cout << "  " << "store "<< sign2 << ", "; //这里使用一种比较蠢的方法：用Dump输出Lval的信息，不换行，有点凑的方式
            int value = Exp->calc();
            //cout << sign1 << endl;
           // cout << "value=" << value << endl;
            if(ValueTable.find(sign1) != ValueTable.end()) {
              cerr << '"' << sign1 << "is a constant and can't be altered" << endl;
              exit(-1);
            }
            if(VarTable.find(sign1) != VarTable.end()){
              cout << "  " << "store " << sign2 << "," << "@"+sign1 << endl;
              VarTable[sign1] = value;
              //exit(-1);
            }
            //Lval->Dump(value);
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


//非终结符不存在类
class SinIfstmt : public BaseAST{
  public:
     std::unique_ptr<BaseAST> exp;
     std::unique_ptr<BaseAST> stmt;

     void Dump() const override{
      //add your content
     }

    void Dump(string &sign) const override{}
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class MultElseStmt : public BaseAST{
   std::unique_ptr<BaseAST> exp;
   std::unique_ptr<BaseAST> if_stmt;
   std::unique_ptr<BaseAST> else_stmt;
   void Dump() const override{
      //add your content
  }
    void Dump(string &sign) const override{}
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}

};