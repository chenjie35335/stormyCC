#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <unordered_map>
#include <vector>
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
enum{
  UNARYEXP,
  LVAL,
  NUMBER,
  MULEXP,
  ADDMUL,
  DECLAST_CON,
  DECLAST_VAR,
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
  RELOPAST_GE,
  SINBLOCKITEM_DEC,
  SINBLOCKITEM_STM,
  SINVARDEFAST_UIN,
  SINVARDEFAST_INI,
  STMTAST_RET,
  STMTAST_LVA
}Kind;
extern unordered_map<string,int> ValueTable;
static int alloc_now = -1;
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const = 0;//这个用来无返回值遍历
  virtual void Dump(string &sign) const = 0;//这个用来带有返回值的遍历
  virtual void Dump(string &sign1,string &sign2,string &sign) const = 0;
  [[nodiscard]] virtual int calc() const = 0;//计算表达式的值
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
  [[nodiscard]] int calc() const override{return 0;}
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
  [[nodiscard]] int calc() const override{return 0;}
};

class FuncTypeAST : public BaseAST {
 public:
    std::string type = std::string("int");
    void Dump() const override {
      cout << "i32" << " ";
    }
    void Dump(string &sign) const override {}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class DeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> ConstDecl;
    std::unique_ptr<BaseAST> VarDecl;
    uint32_t type;
    void Dump() const override {
        switch(type) {
            case DECLAST_CON: ConstDecl->Dump(); break;
            case DECLAST_VAR: VarDecl->Dump(); break;
            default: assert(0);
        }
    }
    void Dump(string &sign) const override {
      
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class ConstDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> Btype;
    std::unique_ptr<BaseAST> MulConstDef;
    void Dump() const override {
       MulConstDef->Dump();
    }
    void Dump(string &sign) const override {
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class BtypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
    }
    void Dump(string &sign) const override {}
    void Dump(string &sign,string  &sign1,string &sign2) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};
//这里使用mulConstDef递归嵌套的方式实现多个，而且使用vector存储可以提高树的平衡性
class MulConstDefAST : public BaseAST {
  public:
    vector <unique_ptr<BaseAST>> SinConstDef;
    void Dump() const override{
        for(auto &sinConstDef : SinConstDef) {
          sinConstDef->Dump();
        }
    }
    void Dump(string &sign) const override {

    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class SinConstDefAST : public BaseAST{
  public:
    string ident;
    unique_ptr<BaseAST>ConstExp;
    void Dump() const override{
      if(ValueTable.find(ident) != ValueTable.end()) {
        assert(0);
      } 
      int value = ConstExp->calc();
      ValueTable.insert(pair<string,int>(ident,value));
    }
    void Dump(string &sign) const override {} 
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class VarDeclAST : public BaseAST {
public:
     unique_ptr <BaseAST> MulVarDef;
     void Dump() const override{}
     void Dump(string & sign) const override {}
     void Dump(string &sign1, string &sign2,string &sign)const override{}
     [[nodiscard]] int calc() const override {return 0;}
};

class MulVarDefAST : public BaseAST {
public:
    vector <unique_ptr<BaseAST>> SinValDef;
    void Dump() const override{}
    void Dump(string & sign) const override {}
    void Dump(string &sign1, string &sign2,string &sign)const override{}
    [[nodiscard]] int calc() const override {return 0;}
};

class SinVarDefAST : public BaseAST {
public:
    string ident;
    unique_ptr<BaseAST>InitVal;
    uint32_t type;
    void Dump() const override{}
    void Dump(string & sign) const override {}
    void Dump(string &sign1, string &sign2,string &sign)const override{}
    [[nodiscard]] int calc() const override {return 0;}
};

class InitValAST : public BaseAST {
public:
    unique_ptr<BaseAST>Exp;
    void Dump() const override{}
    void Dump(string & sign) const override {}
    void Dump(string & sign1, string & sign2,string & sign)const override{}
    [[nodiscard]] int calc() const override {return 0;}
};

class ConstExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST>Exp;
    void Dump() const override{}
    void Dump(string &sign) const override {} 
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{
        return Exp->calc();
    }
};

class BlockAST : public BaseAST {
 public:
    std::unique_ptr<BaseAST> MulBlockItem;
    void Dump() const override {
      cout << "{" << endl;
      MulBlockItem->Dump();
      cout << "}" << endl;
}
    void Dump(string &sign) const override {}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class MulBlockItemAST : public BaseAST {
  public:
    vector <unique_ptr<BaseAST>> SinBlockItem;
    void Dump() const override{
      for(auto &sinBlockItem : SinBlockItem) {
        sinBlockItem->Dump();
      }
    }
    void Dump(string &sign) const override {} 
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
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};

class StmtAST : public BaseAST {
 public:
    //int num;
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Lval;
    uint32_t type;
    void Dump() const override {
      cout << "%" << "entry:" << endl;
      string sign;
      switch(type) {
          case STMTAST_RET:
              Exp->Dump(sign);
              break;
          case STMTAST_LVA:
              break;
          default:
              assert(0);
      }
      cout << "  " << "ret " << sign << endl; 
    }
    void Dump(string &sign) const override{}
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{return 0;}
};
//这里就是返回值的问题，但是这里考虑可以把返回值设为string,直接将常数改为string返回就可以了
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> LOrExp;
    void Dump() const override {}
    void Dump(string &sign) const override{
      //cout << "enter Exp" << endl;
        LOrExp->Dump(sign);
    }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{
        return LOrExp->calc();
    }
};

class LOrExpAST : public BaseAST {
  public:
   std::unique_ptr<BaseAST> LAndExp;
   std::unique_ptr<BaseAST> LOrExp;
   uint32_t type;
   void Dump() const override{}
   void Dump(string &sign) const override{
        //cout << "enter lorexp" << endl;
        string sign1;
        string sign2;
        switch(type) {
          case LOREXPAST_LAN:
          //cout << "enter lorexp2" << endl;
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
   [[nodiscard]] int calc() const override{
       int value = 0;
       switch(type) {
           case LOREXPAST_LAN:
               value = LAndExp->calc();break;
           case LOREXPAST_LOR:
           {
               int value1 = LOrExp->calc();
               int value2 = LAndExp->calc();
               value = value1 || value2;
               break;
           }
       }
       return value;
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
    string s1,s2;
    switch(type) {
      case LANDEXPAST_EQE:
      //cout << "enter landexp1" << endl;
          EqExp->Dump(sign);break;
      case LANDEXPAST_LAN:
          {
            //cout << "enter landexp1" << endl;
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
   [[nodiscard]] int calc() const override{
       int value = 0;
       switch(type) {
           case LANDEXPAST_EQE:
               value = EqExp->calc();break;
           case LANDEXPAST_LAN:
           {
               int value1 = EqExp->calc();
               int value2 = LAndExp->calc();
               value = value1 && value2;
               break;
           }
       }
       return value;
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
     string s1,s2;
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
   [[nodiscard]] int calc() const override{
       int value = 0;
       switch(type) {
           case EQEXPAST_REL:
               value = RelExp->calc();break;
           case EQEXPAST_EQE:
           {
               int value1 = EqExp->calc();
               int value2 = RelExp->calc();
               int OpType = EqOp->calc();
               switch(OpType) {
                   case EQOPAST_EQ: value = value1 == value2; break;
                   case EQOPAST_NE: value = value1 != value2; break;
                   default: break;
               }
               break;
           }
       }
       return value;
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
    [[nodiscard]] int calc() const override{return (int)type;}
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
      string s1,s2;
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
   [[nodiscard]] int calc() const override{
       int value;
       int value1,value2;
       switch(type) {
           case RELEXPAST_ADD: {
               value = AddExp->calc();break;
           }
           case RELEXPAST_REL: {
               value1 = RelExp->calc();
               value2 = AddExp->calc();
               int OpRel = RelOp->calc();
               switch(OpRel) {
                   case RELOPAST_GE: value = value1 >= value2; break;
                   case RELOPAST_LE: value = value1 <= value2; break;
                   case RELOPAST_G:  value = value1 > value2; break;
                   case RELOPAST_L:  value = value1 < value2; break;
                   default: assert(0);
               }
               break;
           }
           default:
               assert(0);
       }
       return value;
   }

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
    [[nodiscard]] int calc() const override{return type;}
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
            string sign1;
            string sign2;
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
    [[nodiscard]] int calc() const override{
        int value;
        switch(type) {
            case MULEXP:
                value = MulExp->calc();break;
            case ADDMUL:
            {
                int value1,value2;
                value1 = AddExp->calc();
                value2 = MulExp->calc();
                int OpAdd = AddOp->calc();
                switch(OpAdd) {
                    case '+' : value = value1 + value2; break;
                    case '-' : value = value1 - value2; break;
                    default: assert(0);
                }
                break;
            }
            default:
                assert(0);
        }
        return value;
    }
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
          string sign1;
          string sign2;
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
    [[nodiscard]] int calc() const override {
        int value;
        switch(type) {
            case MULEXPAST_MUL: {
                int value1,value2;
                value1 = MulExp->calc();
                value2 = UnaryExp->calc();
                int OpMul = MulOp->calc();
                switch(OpMul) {
                    case '*': value = value1 * value2;break;
                    case '/': value = value1 / value2;break;
                    case '%': value = value1 % value2;break;
                    default: assert(0);
                }
                break;
            }
            case MULEXPAST_UNA: value = UnaryExp->calc();break;
            default: assert(0);
        }
        return value;
    }
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
    [[nodiscard]] int calc() const override{
        return PrimaryExp->calc();
    }
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
    [[nodiscard]] int calc() const override{
        int value;
        int OpUnary = UnaryOp->calc();
        int value1  = UnaryExp->calc();
        switch(OpUnary) {
            case '+' : value = value1; break;
            case '-' : value = -value1; break;
            case '!' : value = !value1; break;
            default :  assert(0);
        }
        return value;
    }
};

class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Lval;
    int number;
  uint32_t kind;
    /*这里不带参数的不实现*/
  void Dump() const override{}
    /*如果遍历结果为常数，直接返回，如果不是，继续遍历*/
  void Dump(string &sign) const override{
      //if(kind == UNARYEXP)
        //  Exp->Dump(sign);
      //else 
        //  sign = to_string(number);
      switch(kind) {
        case UNARYEXP: Exp->Dump(sign); break;
        case LVAL: Lval->Dump(sign);break;
        case NUMBER:sign = to_string(number);break;
      }
  }
    void Dump(string &sign1,string &sign2,string &sign) const override{}
    [[nodiscard]] int calc() const override{
        int value;
        switch(kind) {
            case UNARYEXP: value = Exp->calc(); break;
            case LVAL:     value = Lval->calc();break;
            case NUMBER:   value = number;break;
        }
      return value;
  }
};

class LValAST : public BaseAST {
    public:
      string ident;
      int value;
      void Dump() const override {}
      void Dump(string &sign) const override {
          int CalValue = ValueTable.at(ident);
          sign = to_string(CalValue);
      }
      void Dump(string &sign1,string &sign2,string &sign3) const override{}
      [[nodiscard]] int calc() const override{
          int CalValue = ValueTable.at(ident);
          return CalValue;
      }
};
//对于OP类型的，如果是enum表示的type,返回type值，如果直接存储运算符，则返回运算符的值
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
    [[nodiscard]] int calc() const override{
        return op;
    }
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
    [[nodiscard]] int calc() const override{return (int)op;}
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
    [[nodiscard]] int calc() const override{return op;}
};











//现在存在两个问题：一个问题是是否使用共用体来完成这个过程
//op其实不需要这么弄，因为可以直接比较来判断，但是如果是其他的话
//需要添加enum数据结构，可以考虑弄一下，然后使用共用体。










