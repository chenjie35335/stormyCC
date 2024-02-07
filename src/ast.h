#include <iostream>
#include <memory>
#include <string>
using namespace std;
#pragma once
// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const = 0;
  //virtual void koopa() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
  void Dump() const override {
    //cout <<  "CompUnitAST {" << endl;
    func_def->Dump();
    //cout << "}" <<endl;
  }
  //void koopa() const override {
  //    func_def->koopa();
  //}
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump() const override {
    //cout << "FuncDefAST { " << endl;
    //func_type->Dump();
    //cout << ", " << ident << ", " <<endl;
    //block->Dump();
    //cout << " }" << endl;
    cout << "fun ";
    cout << "@" << ident << "(): ";
    func_type->Dump();
    block->Dump();
  }
 // void koopa() const override {
 //   cout << "fun ";
 //   cout << "@" << ident << "():";
 //   func_type->koopa();
 //   block->koopa();
 // }
};

class FuncTypeAST : public BaseAST {
 public:
    std::string type = std::string("int");
    void Dump() const override {
      cout << "i32" << " ";
      //cout << "FuncTypeAST{" << endl;
      //cout << type <<endl;
      //cout << "}" << endl;
    }
  //  void koopa() const override {
  //    cout << "i32" << " ";
  //  }
};

class BlockAST : public BaseAST {
 public:
    std::unique_ptr<BaseAST> stmt;
    void Dump() const override {
      cout << "{" << endl;
      stmt->Dump();
      cout << "}" << endl;
      //cout << "BlockAST{" << endl;
      //stmt->Dump();
      //cout << "}" << endl;
}
  //  void koopa() const override {
  //    cout << "{" << endl;
  //    stmt->koopa();
  //    cout << "}" << endl;
  //  }
};

class StmtAST : public BaseAST {
 public:
    int num;
    void Dump() const override {
      // cout << "StmtAST{" << endl;
      // cout << num <<endl;
      // cout << "}" << endl;
      cout << "%" << "entry:" << endl;
      cout << "  " << "ret " << num << endl; 
    }
  //  void koopa() const override {
  //    cout << "%" << "entry:" << endl;
  //    cout << "  " << "ret" << num << endl; 
  //  }
};

// class NumberAST : public BaseAST {
//  public:
    // int number;
// };
// ...












