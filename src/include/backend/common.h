enum {
/// Not equal to.
    RBO_NOT_EQ,
/// Equal to.
    RBO_EQ,
/// Greater than.
    RBO_GT,
/// Less than.
    RBO_LT,
/// Greater than or equal to
    RBO_GE,
/// Less than or equal to.
    RBO_LE,
/// Addition.
    RBO_ADD,
/// Subtraction.    
    RBO_SUB,
/// Multiplication.
    RBO_MUL,
/// Division.
    RBO_DIV,
/// Modulo.
    RBO_MOD,
/// Bitwise AND.
    RBO_AND,
/// Bitwise OR.
    RBO_OR,
/// Bitwise XOR.
    RBO_XOR,
/// Shift left logical.
    RBO_SHL,
/// Shift right logical.
    RBO_SHR,
/// Shift right arithmetic.
    RBO_SAR,
}BinaryOp;

enum {
    /// @brief 寄存器变量
    REGISTER,
    /// @brief 内存变量
    MEMORY,
    /// @brief 未分配
    UNALLOC
}RawValueStatus;