
#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

// base class with virtual destructor
class ExprAST {
public:
  virtual ~ExprAST() = default;
};
// expression class for numbers

class NumExprAST : public ExprAST {
  double val;

public:
  NumExprAST(double val) : val(val) {}
};
// expression class for variables
class VarExprAST : public ExprAST {
  std::string name;

public:
  VarExprAST(const std::string &name) : name(name) {}
};
// expression class for binary operators
class BinExprAST : public ExprAST {
  char op;
  std::unique_ptr<ExprAST> lhs, rhs;

public:
  BinExprAST(char op, std::unique_ptr<ExprAST> lhs,
             std::unique_ptr<ExprAST> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// expression class for function calls
class CallExprAST : public ExprAST {
  std::string callee;
  std::vector<std::unique_ptr<ExprAST>> args;

public:
  CallExprAST(const std::string &callee,
              std::vector<std::unique_ptr<ExprAST>> args)
      : callee(callee), args(std::move(args)) {}
};

class PrototypeAST {
  std::string name;
  std::vector<std::string> args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {}
};
class functionAST {
  std::unique_ptr<PrototypeAST> proto;
  std::unique_ptr<ExprAST> body;

public:
  functionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body)
      : proto(std::move(proto)), body(std::move(body)) {}
};
#endif
