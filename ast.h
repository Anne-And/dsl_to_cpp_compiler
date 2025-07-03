#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

namespace AST {

class Node {
public:
    virtual ~Node() = default;
};

class DataType : public Node {
public:
    enum Kind { UINT, INT, BOOL, ADDRESS };
    Kind kind;
    DataType(Kind k) : kind(k) {}
    std::string toCppType() const;
};

class Expression : public Node {
public:
    virtual std::string generateCode() = 0;
};

class Value : public Expression {
public:
    enum ValueType { INTEGER, BOOLEAN, ADDRESS_VAL };
    ValueType type;
    std::string value;
    Value(ValueType t, const std::string& v) : type(t), value(v) {}
    std::string generateCode() override;
};

class Variable : public Expression {
public:
    std::string name;
    Variable(const std::string& n) : name(n) {}
    std::string generateCode() override;
};

class BinaryOperation : public Expression {
public:
    enum OpType {
        ADD, SUB, MUL, DIV, MOD,
        EQUAL, NOTEQUAL, GREATERTHAN, LESSERTHAN, GREATEREQUAL, LESSEREQUAL,
        AND, OR
    };
    OpType op;
    std::unique_ptr<Expression> left, right;
    BinaryOperation(OpType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    std::string generateCode() override;
};

class UnaryOperation : public Expression {
public:
    enum OpType { NOT, NEGATE };
    OpType op;
    std::unique_ptr<Expression> operand;
    UnaryOperation(OpType o, std::unique_ptr<Expression> operand)
        : op(o), operand(std::move(operand)) {}
    std::string generateCode() override;
};

class Statement : public Node {
public:
    virtual std::string generateCode(int indent = 0) = 0;
};

class VariableDeclaration : public Statement {
public:
    std::unique_ptr<DataType> type;
    std::string name;
    std::unique_ptr<Expression> initializer;
    VariableDeclaration(std::unique_ptr<DataType> t, const std::string& n, std::unique_ptr<Expression> init = nullptr)
        : type(std::move(t)), name(n), initializer(std::move(init)) {}
    std::string generateCode(int indent = 0) override;
};

class Assignment : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> value;
    Assignment(const std::string& var, std::unique_ptr<Expression> val)
        : variable(var), value(std::move(val)) {}
    std::string generateCode(int indent = 0) override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenStmts, elseStmts;
    IfStatement(std::unique_ptr<Expression> cond) : condition(std::move(cond)) {}
    std::string generateCode(int indent = 0) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    ReturnStatement(std::unique_ptr<Expression> val) : value(std::move(val)) {}
    std::string generateCode(int indent = 0) override;
};

class Parameter {
public:
    std::unique_ptr<DataType> type;
    std::string name;
    Parameter(std::unique_ptr<DataType> t, const std::string& n)
        : type(std::move(t)), name(n) {}
    std::string generateCode() const;
};

class Function : public Node {
public:
    std::string name;
    std::vector<std::unique_ptr<Parameter>> parameters;
    std::unique_ptr<DataType> returnType;
    std::vector<std::unique_ptr<Statement>> body;
    Function(const std::string& n, std::unique_ptr<DataType> retType)
        : name(n), returnType(std::move(retType)) {}
    std::string generateCode() const;
};

class Contract : public Node {
public:
    std::string name;
    std::vector<std::unique_ptr<VariableDeclaration>> variables;
    std::vector<std::unique_ptr<Function>> functions;
    Contract(const std::string& n) : name(n) {}
    std::string generateCode() const;
};

}

#endif
