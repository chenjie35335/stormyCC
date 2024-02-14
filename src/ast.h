#include <iostream>
#include <memory>
#include <string>
#include <cassert>
using namespace std;
#pragma once
// 所有 AST 的基类
//这个是所有类别的标记
//如果是这样的，做出以下约定：
/*
  1. 每种分类必须以**AST开头，下划线后表示具体类型;
  2. 对于分类的名称，为了方便，除PrimaryExp外，需要改动
  3. 取用每种匹配方式的前三个字符的大写作为分类方式
*/
// enum{
    // UNARYEXP,
    // NUMBER
// }PrimaryExpAST_Kind;
// 
// enum{
    // MULEXP,
    // ADDMUL
// }AddExpAST_Kind;
// 
// enum{
    // MULEXPAST_UNA,
    // MULEXPAST_MUL
// }MulExpAST_Kind;
// 
enum{
  UNARYEXP,
  NUMBER,
  MULEXP,
  ADDMUL,
  MULEXPAST_UNA,
  MULEXPAST_MUL,
  LOREXPAST_LAN,
  LOREXPAST_LOR,
  LANDEXPAST_EQE,
  LANDEXPAST_LAN,
  EQEXPAST_REL,
  EQEXPAST_EQE,
  EQOPAST_EQ,
  EQOPAST_NE,
  RELEXPAST_ADD,
  RELEXPAST_REL,
  RELOPAST_L,
  RELOPAST_G,
  RELOPAST_LE,
  RELOPAST_GE
}Kind;

static int alloc_now = -1;
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const = 0;//这个用来无返回值遍历
  virtual void Dump(string &sign) const = 0;//这个用来带有返回值的遍历
  virtual void Dump(string &sign1,string &sign2,string &sign) const = 0;
};
// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
  void Dump() const override {
    alloc_now = -1;
    func_def->Dump();
  }
  void Dump(string &sign) const override {}//这两个不需要在此处重载
  void Dump(string &sign1,string &sign2,string &sign) const override{}
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump() const override {
    cout << "fun ";
    cout << "@" << ident << "(): ";
    func_type->Dump();
    block->Dump();
  }
  void Dump(string &sign) const override {}
  void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class FuncTypeAST : public BaseAST {
 public:
    std::string type = std::string("int");
    void Dump() const override {
      cout << "i32" << " ";
    }
    void Dump(string &sign) const override {}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class BlockAST : public BaseAST {
 public:
    std::unique_ptr<BaseAST> stmt;
    void Dump() const override {
      cout << "{" << endl;
      stmt->Dump();
      cout << "}" << endl;
}
    void Dump(string &sign) const override {}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class StmtAST : public BaseAST {
 public:
    //int num;
    std::unique_ptr<BaseAST> Exp;
    void Dump() const override {
      cout << "%" << "entry:" << endl;
      string sign = "";
      Exp->Dump(sign);
      cout << "  " << "ret " << sign << endl; 
    }
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};
//这里就是返回值的问题，但是这里考虑可以把返回值设为string,直接将常数改为string返回就可以了
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> LOrExp;
    void Dump() const override {}
    void Dump(string &sign) const override{
     // cout << "enter Exp" << endl;
        LOrExp->Dump(sign);
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class LOrExpAST : public BaseAST {
  public:
   std::unique_ptr<BaseAST> LAndExp;
   std::unique_ptr<BaseAST> LOrExp;
   uint32_t type;
   void Dump() const override{}
   void Dump(string &sign) const override{
        string sign1 = "";
        string sign2 = "";
        switch(type) {
          case LOREXPAST_LAN:
            LAndExp->Dump(sign);break;
          case LOREXPAST_LOR:
            {
              LOrExp->Dump(sign1);
              LAndExp->Dump(sign2);
              Dump(sign1,sign2,sign);
              break;
            }
        }
   }
   void Dump(string &sign1,string &sign2,string &sign)const override{
        alloc_now++;
        sign = "%"+to_string(alloc_now);
        cout << "  "+sign << " = " << "or " << sign1 << ", " << sign2 << endl;
        alloc_now++;
        cout << "  %"+to_string(alloc_now) << " = " << "ne " << sign  << ", " << 0 << endl;
        sign = "%"+to_string(alloc_now);
   }
}
;

class LAndExpAST : public BaseAST {
  public:
   std::unique_ptr<BaseAST> EqExp;
   std::unique_ptr<BaseAST> LAndExp;
   uint32_t type;
   void Dump() const override{}
   void Dump(string &sign) const override{
    string s1 = "",s2 = "";
    switch(type) {
      case LANDEXPAST_EQE:
          EqExp->Dump(sign);break;
      case LANDEXPAST_LAN:
          {
            LAndExp->Dump(s1);
            EqExp->Dump(s2);
            Dump(s1,s2,sign);
            break;
          }
      default: 
          assert(0);
    }

   }
   void Dump(string &sign1,string &sign2,string &sign)const override{
    alloc_now++;
    cout << "  %"+to_string(alloc_now) << " = " << "ne " << sign1 << ", " << 0 << endl;
    sign1 = "%"+to_string(alloc_now);
    alloc_now++;
    cout << "  %"+to_string(alloc_now) << " = " << "ne " << sign2 << ", " << 0 << endl;
    sign2 = "%"+to_string(alloc_now);
    alloc_now++;
    cout << "  %"+to_string(alloc_now) << " = " << "and " << sign1 << ", " << sign2 << endl;
    sign = "%"+to_string(alloc_now);
   }
}
;

class EqExpAST : public BaseAST {
  public:
   std::unique_ptr<BaseAST> EqExp;
   std::unique_ptr<BaseAST> RelExp;
   std::unique_ptr<BaseAST> EqOp;
   uint32_t type;
   void Dump() const override{}
   void Dump(string &sign) const override{
     string s1 = "",s2 = "";
     switch(type) {
        case EQEXPAST_REL: {
          RelExp->Dump(sign);break;
        }
        case EQEXPAST_EQE: {
          EqExp->Dump(s1);
          RelExp->Dump(s2);
          EqOp->Dump(s1,s2,sign);
          break;
        }
        default:
          assert(0);
     }
   }
   void Dump(string &sign1,string &sign2,string &sign)const override{

   }
}
;

class EqOpAST : public BaseAST {
  public:
    uint32_t type;
    void Dump() const override{}
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign)const override{
        alloc_now++;
        switch(type) {
          case EQOPAST_EQ :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "eq " << sign1 << ", " << sign2 << endl;
            break;
          case EQOPAST_NE :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "ne " << sign1 << ", " << sign2 << endl;
            break; 
          default:
            assert(0);
          }
        sign = "%"+to_string(alloc_now);
    }
} 
;

class RelExpAST : public BaseAST {
  public:
   std::unique_ptr<BaseAST> AddExp;
   std::unique_ptr<BaseAST> RelExp;
   std::unique_ptr<BaseAST> RelOp;
   uint32_t type;
   void Dump() const override{}
   void Dump(string &sign) const override{
      string s1 = "",s2 = "";
      switch(type) {
      case RELEXPAST_ADD: {
        AddExp->Dump(sign);break;
      }
      case RELEXPAST_REL: {
        RelExp->Dump(s1);
        AddExp->Dump(s2);
        RelOp->Dump(s1,s2,sign);
        break;
      }
      default:
        assert(0);
      }
   }
   void Dump(string &sign1,string &sign2,string &sign)const override{}
}
;

class RelOpAST : public BaseAST {
  public:
    uint32_t type;
    void Dump() const override{}
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign)const override{
        alloc_now++;
        switch(type) {
          case RELOPAST_GE :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "ge " << sign1 << ", " << sign2 << endl;
            break;
          case RELOPAST_LE :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "le " << sign1 << ", " << sign2 << endl;
            break; 
          case RELOPAST_L :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "lt " << sign1 << ", " << sign2 << endl;
            break; 
          case RELOPAST_G :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "gt " << sign1 << ", " << sign2 << endl;
            break;
          default:
            assert(0);
        }
        sign = "%"+to_string(alloc_now);
    }
} 
;

class AddExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> MulExp;
    std::unique_ptr<BaseAST> AddExp;
    uint32_t type;
    std::unique_ptr<BaseAST> AddOp;
    void Dump() const override{}
    void Dump(string &sign) const override{
      //cout << "enter AddExp" << endl;
      switch(type) {
        case MULEXP:  
            MulExp->Dump(sign);break;
        case ADDMUL:
        {
            string sign1 = "";
            string sign2 = "";
            AddExp->Dump(sign1);
            MulExp->Dump(sign2);
            AddOp->Dump(sign1,sign2,sign);
            break;
        }
        default:
            assert(0);
      }
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class MulExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> UnaryExp;
    std::unique_ptr<BaseAST> MulExp;
    uint32_t type;
    std::unique_ptr<BaseAST> MulOp;
    void Dump() const override{}
    void Dump(string &sign) const override{
      switch(type) {
        case MULEXPAST_MUL: {
          string sign1 = "";
          string sign2 = "";
          MulExp->Dump(sign1);
          UnaryExp->Dump(sign2);
          MulOp->Dump(sign1,sign2,sign);
          break;
        }
        case MULEXPAST_UNA: UnaryExp->Dump(sign);break;
        default: assert(0);
      }
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class UnaryExpAST_P : public BaseAST {
  public:
  //UnaryExp第一种情况
    std::unique_ptr<BaseAST> PrimaryExp;
    void Dump() const override {}
    void Dump(string &sign) const override{
      PrimaryExp->Dump(sign);
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class UnaryExpAST_U : public BaseAST {
  public:
  //UnaryExp的递归第二种情况
    std::unique_ptr<BaseAST> UnaryOp;
    std::unique_ptr<BaseAST> UnaryExp;
    void Dump() const override{}
    void Dump(string &sign) const override{
        UnaryExp->Dump(sign);
        UnaryOp->Dump(sign);
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> Exp;
    int number;
  uint32_t kind;
    /*这里不带参数的不实现*/
  void Dump() const override{}
    /*如果遍历结果为常数，直接返回，如果不是，继续遍历*/
  void Dump(string &sign) const override{
      if(kind == UNARYEXP)
          Exp->Dump(sign);
      else 
          sign = to_string(number);
  }
  void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class UnaryOpAST : public BaseAST {
  public:
    char op;
    void Dump() const override {}
    void Dump(string &sign) const override {
      //在运算符处生成中间表示的语句
      //if(sign.at(0) == '%') {
        alloc_now++;
      //}
      switch(op) {
        case '+': break;
        case '-':
        cout << "  %" << (alloc_now) << ' '<< '=' << ' ' <<"sub " <<0<< ", " << sign << endl;
        sign = "%"+to_string(alloc_now);break;
        case '!':
        cout << "  %" << (alloc_now) << ' '<< '=' << ' ' <<"eq " <<sign<< ", " << 0 << endl;
        sign = "%"+to_string(alloc_now);break;
        default: assert(false);
      }
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
};

class AddOpAST : public BaseAST {
  public:
    char op;
    void Dump() const override {}
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{
        alloc_now++;
        switch(op) {
          case '+' :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "add " << sign1 << ", " << sign2 << endl;
            break;
          case '-' :
            cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "sub " << sign1 << ", " << sign2 << endl;
            break; 
          default:
            assert(0);
        }
        sign = "%"+to_string(alloc_now);
    }
};

class MulOpAST : public BaseAST {
  public:
    char op;
    void Dump() const override {}
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override {
        alloc_now++;
        switch(op) {
         case '*' :
           cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "mul " << sign1 << ", " << sign2 << endl;
           break;
         case '/' :
           cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "div " << sign1 << ", " << sign2 << endl;
           break; 
         case '%' :
           cout << "  %" << (alloc_now) << ' '<< '=' << ' ' << "mod " << sign1 << ", " << sign2 << endl;
           break; 
         default:
           assert(0); 
        }   
        sign = "%"+to_string(alloc_now);
    }
};











//现在存在两个问题：一个问题是是否使用共用体来完成这个过程
//op其实不需要这么弄，因为可以直接比较来判断，但是如果是其他的话
//需要添加enum数据结构，可以考虑弄一下，然后使用共用体。









