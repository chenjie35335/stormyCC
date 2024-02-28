#include "BaseAST.h"

class BlockAST : public BaseAST {
 public:
    std::unique_ptr<BaseAST> MulBlockItem;
    void Dump() const override {
      cout << "{" << endl;
      MulBlockItem->Dump();
      cout << "}" << endl;
}
    void Dump(string &sign) const override {}
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class MulBlockItemAST : public BaseAST {
  public:
    vector <unique_ptr<BaseAST>> SinBlockItem;
    void Dump() const override{
      cout << "%" << "entry:" << endl;
      for(auto &sinBlockItem : SinBlockItem) {
        sinBlockItem->Dump();
      }
    }
    void Dump(string &sign) const override {} 
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class SinBlockItemAST : public BaseAST {
  public:
    unique_ptr<BaseAST> decl;
    unique_ptr<BaseAST> stmt;
    uint32_t type;
    void Dump() const override{
      switch(type) {
        case SINBLOCKITEM_DEC: decl->Dump();break;
        case SINBLOCKITEM_STM: stmt->Dump();break;
        default:assert(0);
      }
    }
    void Dump(string &sign) const override {} 
    void Dump(int value) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};
