#include "ast.h"
#include <sstream>

namespace AST {

std::string Type::toCppType() const {
    switch (kind) {
        case UINT: return "uint32_t";
        case INT: return "int32_t";
        case BOOL: return "bool";
        case ADDRESS: return "uint64_t";
        default: return "uint32_t";
    }
}

std::string Literal::generateCode() {
    return value;
}

std::string Identifier::generateCode() {
    return name;
}

std::string BinaryOp::generateCode() {
    std::string leftCode = left->generateCode();
    std::string rightCode = right->generateCode();
    
    std::string opStr;
    switch (op) {
        case ADD: opStr = " + "; break;
        case SUB: opStr = " - "; break;
        case MUL: opStr = " * "; break;
        case DIV: opStr = " / "; break;
        case MOD: opStr = " % "; break;
        case EQ: opStr = " == "; break;
        case NE: opStr = " != "; break;
        case GT: opStr = " > "; break;
        case LT: opStr = " < "; break;
        case GE: opStr = " >= "; break;
        case LE: opStr = " <= "; break;
        case AND: opStr = " && "; break;
        case OR: opStr = " || "; break;
        default: opStr = " ? "; break;
    }
    
    return "(" + leftCode + opStr + rightCode + ")";
}

std::string UnaryOp::generateCode() {
    std::string operandCode = operand->generateCode();
    
    switch (op) {
        case NOT: return "(!" + operandCode + ")";
        case NEGATE: return "(-" + operandCode + ")";
        default: return operandCode;
    }
}

std::string VarDecl::generateCode(int indent) {
    std::string indentStr(indent * 4, ' ');
    std::string result = indentStr + type->toCppType() + " " + name;
    
    if (initializer) {
        result += " = " + initializer->generateCode();
    }
    
    result += ";\n";
    return result;
}

std::string Assignment::generateCode(int indent) {
    std::string indentStr(indent * 4, ' ');
    return indentStr + variable + " = " + value->generateCode() + ";\n";
}

std::string IfStatement::generateCode(int indent) {
    std::string indentStr(indent * 4, ' ');
    std::string result = indentStr + "if (" + condition->generateCode() + ") {\n";
    
    for (auto& stmt : thenStmts) {
        result += stmt->generateCode(indent + 1);
    }
    
    if (!elseStmts.empty()) {
        result += indentStr + "} else {\n";
        for (auto& stmt : elseStmts) {
            result += stmt->generateCode(indent + 1);
        }
    }
    
    result += indentStr + "}\n";
    return result;
}

std::string ReturnStatement::generateCode(int indent) {
    std::string indentStr(indent * 4, ' ');
    return indentStr + "return " + value->generateCode() + ";\n";
}

std::string Parameter::generateCode() const {
    return type->toCppType() + " " + name;
}

std::string Function::generateCode() const {
    std::string result = returnType->toCppType() + " " + name + "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        result += parameters[i]->generateCode();
        if (i < parameters.size() - 1) {
            result += ", ";
        }
    }
    
    result += ") {\n";
    
    for (auto& stmt : body) {
        result += stmt->generateCode(1);
    }
    
    result += "}\n\n";
    return result;
}

std::string Contract::generateCode() const {
    std::string result = "#include <cstdint>\n#include <iostream>\n\n";
    
    for (auto& var : variables) {
        result += var->generateCode(0);
    }
    
    result += "\n";
    
    for (auto& func : functions) {
        result += func->generateCode();
    }
    
    // Generate main function
    result += "int main() {\n";
    result += "    // Call some functions to demonstrate compilation\n";
    result += "    return 0;\n";
    result += "}\n";
    
    return result;
}

} // namespace AST