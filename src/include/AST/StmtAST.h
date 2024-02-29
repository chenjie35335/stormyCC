#include "BaseAST.h"
//无论是左值还是右值，都需要在全体作用域中寻找
class StmtAST : public BaseAST {
 public:
    //int num;
    std::unique_ptr<BaseAST> SinExp;
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Lval;
    std::unique_ptr<BaseAST> Block;
    uint32_t type;
    void Dump() const override {
      auto p = IdentTable;
      //auto& ValueTable = IdentTable->ConstTable;
      //auto& VarTable   = IdentTable->VarTable;
      
      string sign;
      switch(type) {
          case STMTAST_RET:
              SinExp->Dump(sign);
              cout << "  " << "ret " << sign << endl; 
              break;
          case STMTAST_LVA:{
            //cout << "parsing Lval" << endl;
            string sign1,sign2;
            Lval->Dump(sign1);
            Exp->Dump(sign2);
            int value = Exp->calc();
            while(p != nullptr) {
              auto &ValueTable = p->ConstTable;
              auto &VarTable   = p->VarTable; 
              int dep = p->level;
            if(ValueTable.find(sign1) != ValueTable.end()) {
              cerr << '"' << sign1 << "is a constant and can't be altered" << endl;
              exit(-1);
            }
            if(VarTable.find(sign1) != VarTable.end()){
              cout << "  " << "store " << sign2 << "," << "@"+sign1+"_"+to_string(dep) << endl;
              VarTable[sign1] = value;
              break;
              //exit(-1);
            }
              p = p->father;
            }
            if(p == nullptr) {
              cerr << '"' << sign1 << "is not defined" << endl;
              exit(-1);
            }
            break;
          }
        case STMTAST_SINE: break;
        case STMTAST_BLO: Block->Dump(); break;
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