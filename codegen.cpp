#include "codegen.h"
#include <fstream>

std::string CodeGenerator::generateCode(const AST::Contract& contract) {
    return contract.generateCode();
}

bool CodeGenerator::writeToFile(const std::string& code, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << code;
    file.close();
    return true;
}

std::string CodeGenerator::generateIncludes() {
    return "#include <cstdint>\n#include <iostream>\n\n";
}

std::string CodeGenerator::generateMain() {
    return "int main() {\n    return 0;\n}\n";
}