#include <Ast.h>
#include <Backend.h>

void backend_f(unique_ptr<BaseAST> &ast) {
        midend* RawProgramme = nullptr;
        ast->generateRawProgramme(RawProgramme);
        RawProgramme->Visit();
}