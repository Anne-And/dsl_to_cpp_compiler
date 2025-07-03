
#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast/ast.h"
#include <memory>
#include <string>

class CodeGenerator {
public:

    static std::string generateCode(const AST::Contract& contract);
    static bool writeToFile(const std::string& code, const std::string& filename);
    
private:
    static std::string generateIncludes();
    static std::string generateMain();
};

#endif // CODEGEN_H
