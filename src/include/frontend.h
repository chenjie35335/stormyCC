#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <unordered_map>
#include <vector>
using namespace std;
#ifndef FRONTEND_STORMY
#define FRONTENT_STORMY
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const {}//这个用来无返回值遍历
  virtual void Dump(string &sign) const {};//这个用来带有单个返回值的遍历
  virtual void Dump(string &sign1,string &sign2,string &sign) const{};
  //这个用来带有双目运算符的遍历
  virtual void Dump(string &sign,vector<string> &Para,vector<int> &ParaValue) const{};
  [[nodiscard]] virtual int calc() const {return 0;}//计算表达式的值
  virtual void Dump(int sign) const {};//这个用于函数时候判断参数
};


class IdentTableNode {
    public:
        IdentTableNode *father; //父节点
        IdentTableNode *child; //子节点
        //IdentTableNode *child2;
        int level;
        unordered_map<string,int> ConstTable; //常量表
        unordered_map<string,int> VarTable;  //变量表
        //构造函数 
        IdentTableNode() {
            father = nullptr;
            child  = nullptr;
            //child2 = nullptr;
        }  
};

class FuncTable{
    public:
    unordered_map<string,bool> FuncTable; //bool表示函数是否存在返回值
    unordered_map<string,vector<int>> FuncParams;
    unordered_map<string,unique_ptr<BaseAST>> Funcdef;

    void init() {
        FuncTable["getch"] = true;
        FuncTable["getint"] = true;
        FuncTable["getarrary"] = true;
        FuncTable["putint"] = false;
        FuncTable["putch"] = false;
        FuncTable["putarrary"] = false;
        FuncTable["starttime"] = false;
        FuncTable["stoptime"] = false;
    }
};

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
  SINVARDEFAST_FUNC,
  STMTAST_RET,
  STMTAST_LVA,
  STMTAST_SINE,
  STMTAST_BLO,
  STMTAST_IF,
  STMTAST_WHILE,
  STMTAST_BREAK,
  STMTAST_CONTINUE,
  STMTAST_INWHILE,
  SinIfAST_BE,
  SinIFAST_NO,
  LVALAST_LEFT,
  LVALAST_RIGHT,
  SINEXPAST_EXP,
  SINEXPAST_NULL,
  FUNC_SIN,
  FUNC_MUL,
  FUNC_EXP,
  COMP_FUNC,
  COMP_CON,
  COMP_VAR,
  FUNCTYPE_INT,
  FUNCTYPE_VOID,
  DECL_LOC,
  DECL_GLOB
}Kind;

extern int ScopeLevel;
extern IdentTableNode* IdentTable;
extern FuncTable funcTable;
extern int end_br[100];
extern int alloc_now;
extern int if_flag_level[200];
extern int if_level;
extern int ret_cnt;
extern int record_while[100];
extern int while_level;
extern int break_cnt;
extern int continue_cnt;


#endif