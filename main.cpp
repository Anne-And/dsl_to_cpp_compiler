
#include <iostream>
#include <fstream>
#include "ast/ast.h"
#include "generator/codegen.h"

extern "C" {
    void* create_contract(const char* name);
    void add_variable_to_contract(void* contract, const char* type, const char* name, const char* value);
    void add_function_to_contract(void* contract, const char* name, const char* return_type);
    void write_contract_to_file(void* contract, const char* filename);
}
static AST::Contract* current_contract = nullptr;

void* create_contract(const char* name) {
    current_contract = new AST::Contract(name);
    return current_contract;
}

void add_variable_to_contract(void* contract_ptr, const char* type_str, const char* name, const char* value) {
    AST::Contract* contract = static_cast<AST::Contract*>(contract_ptr);
    AST::Type::TypeKind kind = AST::Type::UINT;
    if (strcmp(type_str, "int") == 0) kind = AST::Type::INT;
    else if (strcmp(type_str, "bool") == 0) kind = AST::Type::BOOL;
    else if (strcmp(type_str, "address") == 0) kind = AST::Type::ADDRESS;
    
    auto type = std::make_unique<AST::Type>(kind);
    auto literal = std::make_unique<AST::Literal>(AST::Literal::INTEGER, value);
    auto varDecl = std::make_unique<AST::VarDecl>(std::move(type), name, std::move(literal));
    
    contract->variables.push_back(std::move(varDecl));
}

void add_function_to_contract(void* contract_ptr, const char* name, const char* return_type) {
    AST::Contract* contract = static_cast<AST::Contract*>(contract_ptr);
    
    AST::Type::TypeKind kind = AST::Type::UINT;
    if (strcmp(return_type, "int") == 0) kind = AST::Type::INT;
    else if (strcmp(return_type, "bool") == 0) kind = AST::Type::BOOL;
    else if (strcmp(return_type, "address") == 0) kind = AST::Type::ADDRESS;
    
    auto retType = std::make_unique<AST::Type>(kind);
    auto function = std::make_unique<AST::Function>(name, std::move(retType));
    
    contract->functions.push_back(std::move(function));
}

void write_contract_to_file(void* contract_ptr, const char* filename) {
    AST::Contract* contract = static_cast<AST::Contract*>(contract_ptr);
    std::string code = CodeGenerator::generateCode(*contract);
    CodeGenerator::writeToFile(code, filename);
}

int main_ast_based(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.dsl> [output.cpp]" << std::endl;
        return 1;
    }
    
    std::cout << "AST-based compiler (demonstration only)" << std::endl;
    return 0;
}
