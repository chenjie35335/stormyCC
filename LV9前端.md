### LV9前端

#### 1.AST语法识别

```ebnf
ConstDef      ::= IDENT ["[" ConstExp "]"] "=" ConstInitVal;
ConstInitVal  ::= ConstExp | "{" [ConstExp {"," ConstExp}] "}";
VarDef        ::= IDENT ["[" ConstExp "]"]
                | IDENT ["[" ConstExp "]"] "=" InitVal;
InitVal       ::= Exp | "{" [Exp {"," Exp}] "}";

LVal          ::= IDENT ["[" Exp "]"];
```

这里主要有三部分，常量-变量，定义与初始化，左值定义

根据提示得到如下代码

```
ConstDecl
  : CONST Btype MulConstDef ';'{
    auto ast        = new ConstDeclAST();
    ast->Btype      = unique_ptr<BaseAST>($2);
    ast->MulConstDef= unique_ptr<BaseAST>($3);
    ast->type = 0;
    $$              = ast;
  } | CONST Btype ConstArrayDef ';'{
    auto ast = new ConstDeclAST();
    ast->Btype      = unique_ptr<BaseAST>($2);
    ast->arrDef = unique_ptr<BaseAST>($3);
    ast->type = 1;
    $$ = ast;
  }
  ;
```

给常量定义新补充内容，其中最关键的就是ConstArrayDef，但是这里复用了很多次，对于左值的情况不能赋值，因此后续还需要重新修改。

```
ConstArrayDef
  : IDENT ArrayDimen '=' ConstArrayVar {
    auto ast = new ConstArrayDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->dimension = unique_ptr<BaseAST>($2);
    ast->arrayVar = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  ;
```



这里多添加了两个，分别是对维度和初始化的定义

```
ArrayDimen
  : '[' ConstExp ']' {
    auto ast = new ArrayDimenAST();
    ast->constExp = unique_ptr<BaseAST>($2);
    ast->type = 0;
    $$ = ast;
  } | MulArrayDimen {
    auto ast = new ArrayDimenAST();
    ast->constExp = unique_ptr<BaseAST>($1);
    ast->type = 1;
    $$ = ast;
  }
  ;

MulArrayDimen 
  : '[' ConstExp ']' ArrayDimen {
    auto ast = new MulArrayDimenAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->dim = unique_ptr<BaseAST>($4);
    $$ = ast;
  } 
  ;
```

这个初始化应该是数组里面最难部分之一，特别绕，但是也只有这样才能够识别数组的全部定义：

```
ConstArrayVar
  : '{' ArrayContent '}' {
    auto ast = new ConstArrayVarAST();
    ast->content = unique_ptr<BaseAST>($2);
    ast->type = 0;
    $$ = ast;
  } | '{' '}' {
    auto ast = new ConstArrayVarAST();
    ast->type = 1;
    $$ = ast;
  }
  ;

ArrayContent 
  : ConstExp {
    auto ast = new ArrayContentAST();
    ast->var = unique_ptr<BaseAST>($1);
    ast->type = 0;
    $$ = ast;
  } | MulArraycontent {
    auto ast = new ArrayContentAST();
    ast->var = unique_ptr<BaseAST>($1);
    ast->type = 1;
    $$ = ast;
  } | ConstArrayVar {
    auto ast = new ArrayContentAST();
    ast->var = unique_ptr<BaseAST>($1);
    ast->type = 2;
    $$ = ast;
  } //multi dimension array
  ;

MulArraycontent 
  : ConstExp ',' ArrayContent {
    auto ast = new MulArrayContentAST();
    ast->sin = unique_ptr<BaseAST>($1);
    ast->mul = unique_ptr<BaseAST>($3);
    ast->type = 0;
    $$ = ast;
  } | ConstArrayVar ',' ArrayContent {
    auto ast = new MulArrayContentAST();
    ast->sin = unique_ptr<BaseAST>($1);
    ast->mul = unique_ptr<BaseAST>($3);
    ast->type = 1;
  }
  ;
```

上述只是常量定义，变量定义其实还需要重新复述一遍上述操作，只是前面没有const，并且常量是不用翻译出来的，但是要存储到内存or寄存器中。

来看变量定义，这里直接在单变量定义中补充即可，这里需要把赋值和不赋值的全部考虑进去：

```
SinVarDef
  : IDENT {
     auto ast   = new SinVarDefAST();
     ast->type  =  SINVARDEFAST_UIN;
     ast->ident = *unique_ptr<string>($1);
     $$         = ast;
  } | IDENT '=' InitVal {
    auto ast   = new SinVarDefAST();
    ast->type  =  SINVARDEFAST_INI;
    ast->ident = *unique_ptr<string>($1);
    ast->InitVal= unique_ptr<BaseAST>($3);
    $$         = ast;
  } | IDENT ArrayDimen {
    auto ast = new SinVarDefAST();
    ast->type = SINVARDEFAST_UNI_ARR;
    ast->ident = *unique_ptr<string>($1);
    ast->dimen = unique_ptr<BaseAST>($2);
    $$ = ast;
  } | IDENT ArrayDimen '=' ConstArrayVar {
    auto ast = new SinVarDefAST();
    ast->type = SINVARDEFAST_INI_ARR;
    ast->ident = *unique_ptr<string>($1);
    ast->dimen = unique_ptr<BaseAST>($2);
    ast->ConstInit = unique_ptr<BaseAST>($4);
    $$ = ast;
  } | IDENT ArrayDimen '=' Number {
    auto ast = new SinVarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->dimen = unique_ptr<BaseAST>($2);
    ast->type = SINVARDEFAST_ASSIGNMENT;
    ast->number = $4;
  }
  ;
```

这样就解决了变量定义的问题，其实还有个全局的部分：

这里稍作解释，global要到最前面去处理，由于前面函数定义好的，我们只需要加一个标志位判断是不是全局变量，然后做对应的处理就行，这个在后面的AST中进一步描述：



左值和右值也是比较麻烦的一部分

```
Stmt
  : RETURN SinExp ';' {
    auto ast = new StmtAST();
    ast->SinExp = unique_ptr<BaseAST>($2);
    ast->type= STMTAST_RET;
    $$       = ast;
  } | LValL '=' Exp ';' {
    auto ast = new StmtAST();
    ast->Exp = unique_ptr<BaseAST> ($3);
    ast->Lval= unique_ptr<BaseAST> ($1);
    ast->type= STMTAST_LVA;
    $$       = ast;
  }
```

LVAL要大改：

```
LValL 
  : IDENT {
      auto ast = new LValLAST();
      ast->ident = *unique_ptr<string>($1);
      ast->type = 0;
      $$ = ast;
  } | MulLValL {
      auto ast = new LValLAST();
      ast->array = unique_ptr<BaseAST>($1);
      ast->type = 1;
      $$ = ast;
  }
  ;

MulLValL 
  : IDENT ArrPara {
      auto ast = new MulLValLAST();
      ast->ident = *unique_ptr<string>($1);
      ast->exp = unique_ptr<BaseAST>($2);
      $$ = ast;
  } 
  ;
```

这里MulLValL就是有数组的左值

右值也是一样，但是这里就不能用数组维度了，而是数组参数（<=）

```
LValR
  : IDENT {
    auto ast   = new LValRAST();
    ast->ident = *unique_ptr<string>($1);
    ast->type = 0;
    $$ = ast;
  } | IDENT ArrPara {
    auto ast = new LValRAST();
    ast->ident = *unique_ptr<string>($1);
    ast->array = unique_ptr<BaseAST>($2);
    ast->type = 1;
    $$ = ast;
  }
  ;
```

这里的Para给出类似的定理，但是AST的处理有所区别

```
ArrPara 
  : '[' Exp ']' {
    auto ast = new ArrParaAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->type = 0;
    $$ = ast;
  } | MulArrPara {
    auto ast = new ArrParaAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->type = 1;
    $$ = ast;
  }
  ;

MulArrPara
  : '[' Exp ']' ArrPara {
    auto ast = new MulArrParaAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->para = unique_ptr<BaseAST>($4);
    $$ = ast;
  } 
  ;
```

#### 2.AST设计

这里开始的一个大头就是constDef：

```c
class ConstArrayDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> dimension;
    std::unique_ptr<BaseAST> arrayVar;
    string ident;
    void Dump() const override{
        //deal with res
        int dep = IdentTable->level;
        cout<<"  @"<<ident<<"_"<<dep<<" = ";
        string sign;
        vector<string> Para;
        dimension->Dump(sign,Para);
        //cout<<",";
        cout<<"alloc ";
        string res;

        for(auto it = Para.rbegin(); it < Para.rend(); ++it){
          if(it == Para.rbegin()){
            res.append("[");
            res.append("i32");
            res.append(", ");
            res.append(*it);
            res.append("]");
          } else {
            res.insert(0,"[");
            res.append(", ");
            res.append(*it);
            res.append("]");
          }
        }
        
        cout<<res;
        string sign1 = ident;
        arrayTable.ident = sign1;
        arrayVar->Dump(sign1,Para);
        arrayTable.ident = "";
        //cout<<sign1<<endl;
    }
};
```



dimen很好处理，也比较简单：

```C
class ArrayDimenAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> constExp;
    int type;
    void Dump(string &sign,vector<string> &Para) const override{
      if(type == 0){
        //cout<<"666";
        int tmp = constExp->calc();
        sign = to_string(tmp);
        Para.push_back(sign);
      }else {
        constExp->Dump(sign,Para);
      }
    }
};

class MulArrayDimenAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> dim;
    void Dump(string &sign,vector<string> &Para) const override {
      int tmp = exp->calc();
      sign = to_string(tmp);
      Para.push_back(sign);
      dim->Dump(sign,Para);
    }
};
```

因为主要输出koopa的功能都在def里面实现了，这里只是做了简单的识别和存储功能。

##### 数组初始化内部的处理-三部曲

```c
class ArrayContentAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var;
    int type;
    //three situation
    void Dump(string &sign, vector<string> &Para) const override{
        //this Para contain dimension
        if(type == 0){
          arrayTable.ArrTable[arr_seq_cnt] = var->calc();
          arrayTable.ArrLevel[arr_seq_cnt] = arr_dep;
          arr_seq_cnt++;

        } else if(type == 1){
          var->Dump(sign,Para);
          
        } else if(type == 2){
          arr_dep++;
          var->Dump(sign,Para);
          arrayTable.ArrLevel[arr_seq_cnt] = -1;
          arr_dep--;
        } 
    }
};

class MulArrayContentAST : public BaseAST {
  public: 
    std::unique_ptr<BaseAST> sin;
    std::unique_ptr<BaseAST> mul;
    int type;
    void Dump(string &sign,vector<string> &Para) const override {
      if(type == 0){
        sign = to_string(sin->calc());
        arrayTable.ArrTable[arr_seq_cnt] = sin->calc();
        //ArrayTable.ArrTable.push_back(arr_seq_cnt,var->calc());
        arr_seq_cnt++;
        Para.push_back(sign);
        mul->Dump(sign,Para);
      } else {
        vector<string> Para1;
        arr_dep++;
        sin->Dump(sign,Para1);
        arrayTable.ArrLevel[arr_seq_cnt] = -1;
        arr_dep--;
        Para.push_back(sign);
        mul->Dump(sign,Para);
      }
    }
};


```

定义的左值处理，这里用了个指针的while循环找到dep的最外层

```C
class MulLValLAST : public BaseAST {
  public:
    std::string ident;
    std::unique_ptr<BaseAST> exp;
    void Dump(string &sign) const override {
      vector<string> Para;
        exp->Dump(sign,Para);
        if(alloc_now<0) alloc_now = 0;
        auto p = IdentTable;
        int dep = p->level;
        while(p != nullptr){
          auto &VarTable   = p->VarTable;
          if(VarTable.find(ident) != VarTable.end() && VarTable[ident] == -1) { //array
            dep = p->level;
            break;
          }
          p = p->father;
        }
        //cout<<"dep"<<dep<<endl;
        if(dep<0) dep = 0;
        //int dep = IdentTable->level;
        //cout<<"dep"<<dep<<endl;
        string s0 = "@";
        s0.append(ident);
        s0.append("_"+to_string(dep));
        for(auto it = Para.begin(); it !=Para.end() ; it++){
          cout<<"\t%ptr_"<<alloc_now <<" = getelemptr "<< s0;
          cout<<", "<<*it<<endl;
          s0 = "%ptr_";
          s0.append(to_string(alloc_now));
          //cout<<"dep"<<dep<<endl;
          alloc_now++;
        }
        auto s1 = Para.begin();
        sign = "%" + to_string(alloc_now);
        lval_is_arr = 1;
        //sign.append(to_string(s1));
        cout<<"\t%"<<alloc_now<<" = "<<"load "<<s0<<endl;
    }
    //int calc() const override {return 38;}
};
```



数组参数很好处理，主要是在LValR里面去做进一步处理：

```C
class ArrParaAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    int type;
    void Dump(string &sign,vector<string> &Para) const override{
      if(type == 0){
        exp->Dump(sign);
        Para.push_back(sign);
      }else {
        exp->Dump(sign,Para);
      }
    }
};

class MulArrParaAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> para;
    void Dump(string &sign,vector<string> &Para) const override {
      exp->Dump(sign);
      Para.push_back(sign);
      para->Dump(sign,Para);
    }
};
```



#### 3.其他文件的修改（LVALR LVALL）

这里说实话右值找到了就要加载指针，获取变量，用vartable的-1标记他是一个数组类型的，但是仍然存在数组表里面，如果是函数参数，他也可能是-2

```C
class LValRAST : public BaseAST {
  public:
    string ident;
    std::unique_ptr<BaseAST> array;
    int type;
    void Dump() const override {}
    void Dump(string &sign) const override {
      auto p = IdentTable;
      while(p != nullptr) {
      auto &ValueTable = p->ConstTable;
      auto &VarTable   = p->VarTable;
      int dep = p->level;
      //cout << "value = " << VarTable.at(ident) << endl;
      if( ValueTable.find(ident) != ValueTable.end()){
          int CalValue = ValueTable.at(ident);
          sign = to_string(CalValue);
          break;
       }
      else if(VarTable.find(ident) != VarTable.end() && (VarTable.at(ident) == -1 || VarTable.at(ident) == -2) ){ //array
          vector<string> Para;
          string s0 = "@";
          s0.append(ident);
          s0.append("_"+to_string(dep));
          if(type == 1){
            array->Dump(sign,Para);
            for(auto it = Para.begin(); it !=Para.end() ; it++){
              cout<<"\t%ptr_"<<alloc_now <<" = getelemptr "<< s0;
              cout<<", "<<*it<<endl;
              s0 = "%ptr_";
              s0.append(to_string(alloc_now));
              alloc_now++;
            }
            cout<<"\t%"<<alloc_now<<" = "<<"load "<<s0<<endl; 
            //return;
          }
          break;
       }
      else if(VarTable.find(ident) != VarTable.end() ) { // common var
          alloc_now++;
          sign = "%"+to_string(alloc_now);
          cout << "  "<<sign << " = " << "load " << "@"+ident+"_"+to_string(dep)<<endl;
          break;
       }
       p = p->father;
      }
       if(p == nullptr) {
         cerr << "Error: " << '"' << ident << '"' << " is not defined" << endl;
         exit(-1);
       }
    }
    int calc() const override {
      auto p           = IdentTable;
      while(p != nullptr) {
      auto &ValueTable = p->ConstTable;
      auto &VarTable   = p->VarTable;
      if (ValueTable.find(ident) != ValueTable.end())
        {
          int CalValue = ValueTable.at(ident);
          return CalValue;
        }
        else if(VarTable.find(ident) != VarTable.end()) {
          return VarTable.at(ident);
        }
        p = p->father;
      }
        if(p == nullptr)
        {
          cerr << "Error: " << '"' << ident << '"' << "is not defined" << endl;
          exit(-1);
        } 
      }
};
```

LVAL左值的DECL操作，只用改一改单变量定义即可：

说实话，应该用面向对象编程实现，这里太多的冗余操作

多添加了***SINVARDEFAST_UNI_ARR   SINVARDEFAST_ASSIGNMENT  SINVARDEFAST_INI_ARR***三种类型的处理

这里发现了巨多恶心的同名变量，一个是从上层往下传的，一个下层算的结果要传上来，同名，我新定义了一个变量，同时要把旧的clear掉，否则程序容易发生意想不到的错乱

```C
class SinVarDefAST : public BaseAST {
public:
    string ident;
    unique_ptr<BaseAST>InitVal;
    unique_ptr<BaseAST>func_exp;
    unique_ptr<BaseAST>ConstInit;
    unique_ptr<BaseAST>dimen;
    int number;
    uint32_t type;
    void Dump(int sign) const override{
      //cout<< "sinconstdef" << endl;
      arrayTable.ident = ident;
      int value;
      auto &ValueTable = IdentTable->ConstTable;
      auto &VarTable = IdentTable->VarTable;
      int dep = IdentTable->level;
      if(ValueTable.find(ident) != ValueTable.end()){
          cerr << '"' << ident << '"' << " has been defined as constant" <<endl;
          exit(-1);
      } 
       if(VarTable.find(ident) != VarTable.end()){
          cerr << '"' << ident << '"' << " redefined" <<endl;
          exit(-1);
        }
      string sign1;
      switch(type) {
        //we need add type
        case SINVARDEFAST_UIN: {
          if(sign == DECL_GLOB) 
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc i32," <<  " zeroinit" << endl;
          else {
            cout << "  @"<<ident+"_"+to_string(dep) <<" = " << "alloc i32" << endl;
          }
          value = 0;
          break;
        }
        case SINVARDEFAST_INI:
        {
          if(sign == DECL_GLOB) {
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc i32," <<  " "<<InitVal->calc() << endl;  
            cout<<endl;
          }else {
            cout << "  @"+ident+"_"+to_string(dep) <<" = " << "alloc i32" << endl;
            InitVal->Dump(sign1);
            cout << "  store " << sign1<< ", " << "@"+ident+"_"+to_string(dep)<<endl;
          }
          value = 0;
          break;
        }
        case SINVARDEFAST_UNI_ARR:
        {
          string sign2;
          vector<string> Para;
          dimen->Dump(sign2,Para);
          string res;

          for(auto it = Para.rbegin(); it < Para.rend(); ++it){
            //cout<<"520";
            if(it == Para.rbegin()){
              res.append("[");
              res.append("i32");
              res.append(", ");
              res.append(*it);
              res.append("]");
            } else {
              res.insert(0,"[");
              res.append(", ");
              res.append(*it);
              res.append("]");
            }
          }
          //cout<<res<<" ";

          if(sign == DECL_GLOB){
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc " << res <<"," <<" zeroinit" << endl;
            cout<<endl;
          } else {
            cout<< "  @" + ident +"_" + to_string(dep) <<" = " << "alloc " << res <<endl;
          }
          value = -1;
          break;
        }
        case SINVARDEFAST_INI_ARR:
        {
          string sign3,sign4;
          vector<string> Para;
          dimen->Dump(sign3,Para);
          vector<string> Para1 = Para;
          string res;
          for(auto it = Para.rbegin(); it < Para.rend(); ++it){
            //cout<<"520";
            if(it == Para.rbegin()){
              res.append("[");
              res.append("i32");
              res.append(", ");
              res.append(*it);
              res.append("]");
            } else {
              res.insert(0,"[");
              res.append(", ");
              res.append(*it);
              res.append("]");
            }
          }
          //cout<<res<<" ";
          Para.clear();
          //ConstInit->Dump(sign4,Para);
          if(sign == DECL_GLOB) {
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc " << res <<endl; 
            ConstInit->Dump(sign4,Para1);
            cout<<endl;
          }else {
            cout << "  @"+ident+"_"+to_string(dep) <<" = " << "alloc "<< res << endl;
            //ConstInit->Dump(sign1);
            //cout << "  store " << sign4<< ", " << "@"+ident+"_"+to_string(dep)<<endl;
            string sign5 = ident;
            //cout<<sign5;
            ConstInit->Dump(sign5,Para1);
          }
          value = -1;
          break;
        }
        case SINVARDEFAST_ASSIGNMENT:
        {
          string sign5;
          vector<string> PARA3;
          dimen->Dump(sign5,PARA3);
          string s0 = "@" + ident;
          for(auto it = PARA3.begin(); it != PARA3.end(); it++){
            cout<<"  %"<<alloc_now << " = "<<"getelemptr " <<  s0 << *it<<endl;
            s0 = "%" + to_string(alloc_now); 
            alloc_now++;
          }
          cout<<"  store "<< number << ", "<<"%" <<alloc_now<<endl;
          value = -1;
          break;
        }
        default: assert(0);
      }
      VarTable.insert(pair<string,int>(ident,value));
      arrayTable.ident = "";
    }
};
```



#### 4.数组函数参数（最难）

前提肯定要去修改识别的前端：

因为数组参数传进去可能是变量，一维和多维，三种类型顺便实现递归

```
SinFuncFParam
  : ParaType IDENT {
    auto ast = new SinFuncFParamAST();
    ast->ident = *unique_ptr<string>($2);
    ast->ParaType = unique_ptr<BaseAST>($1);
    ast->type = PARA_VAR;
    $$ = ast;
  } | ParaType IDENT '[' ']'  {
    auto ast = new SinFuncFParamAST();
    ast->ident = *unique_ptr<string>($2);
    ast->ParaType = unique_ptr<BaseAST>($1);
    ast->type = PARA_ARR_SIN;
    $$ = ast;
  } | ParaType IDENT '[' ']' ArrayDimen {
    auto ast = new SinFuncFParamAST();
    ast->ident = *unique_ptr<string>($2);
    ast->ParaType = unique_ptr<BaseAST>($1);
    ast->Dimen = unique_ptr<BaseAST>($5);
    ast->type = PARA_ARR_MUL;
    $$ = ast;
  }
  ;
```

函数的右值识别也需要同样处理，但是这里更复杂，数组名作为首地址传参指针相对困难

```
SinParams 
  : Exp {
    auto ast = new SinParamsAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->type = 1;
    $$  = ast;
  } | IDENT '[' ']' {
    auto ast = new SinParamsAST();
    ast->ident = *unique_ptr<string>($1);
    ast->type  = 2;
    $$ = ast;
  } | IDENT '[' ']' ArrayDimen {
    auto ast = new SinParamsAST();
    ast->ident = *unique_ptr<string>($1);
    ast->dimension = unique_ptr<BaseAST>($4);
    ast->type = 3;
    $$ = ast;
  } | IDENT {
    auto ast = new SinParamsAST();
    ast->ident = *unique_ptr<string>($1);
    ast->type = 4;
    $$ = ast;
  }
  ;
```

所以这里的exp是数组名还是变量名需要我们用value= 0 || -1 去分别处理



这里也多加了两种类型的判断和初始化：分别用0，-1，-2插入初值去表示类型，这里做的有些不好，但是也是一种判断类型的方式。

**PARA_ARR_SIN**和**PARA_ARR_MUL**

```C
class SinFuncFParamAST : public BaseAST
{
public:
  std::string ident;
  std::unique_ptr<BaseAST> ParaType;
  std::unique_ptr<BaseAST> Dimen;
  int type;
  void Dump() const override
  {
    switch(type){
      case PARA_VAR:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout << "i32";
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, 0));
        break;
      } 
      case PARA_ARR_SIN:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout<< "*i32";
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, -1));
        break;
      }
      case PARA_ARR_MUL:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout<< "*";
        string res,sign;
        vector<string> Para;
        Dimen->Dump(sign,Para);
        //cout<<sign;
         for(auto it = Para.rbegin(); it < Para.rend(); ++it){
          if(it == Para.rbegin()){
            res.append("[");
            res.append("i32");
            res.append(", ");
            res.append(*it);
            res.append("]");
          } else {
            res.insert(0,"[");
            res.append(", ");
            res.append(*it);
            res.append("]");
          }
        }
        cout<<res;
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, -2));
        break;
      }
        break;
    }
    
  }
};
```

调用参数处理：（这里最好把exp删掉，递归的层数太深了，估计卡的bug也在这里）

```C
class SinParamsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp; 
    int type;
    std::string ident;
    unique_ptr<BaseAST> dimension;
    void Dump(string &sign) const override{
      int dep = IdentTable->level;
      if(type == 1){
        exp->Dump(sign);
      } else if(type == 2){
        cout<<"  %ptr_"<<alloc_now<<" = "<<"getelemptr @"<<ident<<"_"<<dep<<", 0"<<endl;
        sign = "%ptr_"+ to_string(alloc_now);
      }else if(type == 3){
        vector<string> Para;
        dimension->Dump(sign,Para);
        string s0 = "@";
        s0.append(ident);
        s0.append("_"+to_string(dep));
        //cout<<"666";
        for(auto it = Para.begin(); it !=Para.end() ; it++){
          cout<<"\t%ptr_"<<alloc_now <<" = getelemptr "<< s0;
          cout<<", "<<*it<<endl;
          s0 = "%ptr_";
          s0.append(to_string(alloc_now));
          alloc_now++;
        }
        sign = "%ptr_" + to_string(alloc_now);
      }else {
          auto &varTable = IdentTable->VarTable;
          if(varTable.find(ident) != varTable.end() || varTable.at(ident) == -1){
            cout<<"  %"<<alloc_now<<" = getelemptr @"<<ident<<", 0"<<endl;
            alloc_now++;
            sign = "%" + to_string(alloc_now);
          }
      }
    }
};
```

后面就无了，设计的很冗余，后面重构需要大改。

