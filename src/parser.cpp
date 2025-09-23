#include "parser.h"
#include "ast.h"
#include "lexer.h"

int CurTok;
int getNextTok() { return CurTok = gettok(); };

std::unique_ptr<ExprAST> LogError(const char *str) {
  fprintf(stderr, "Error:%s\n", str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *str) {
  LogError(str);
  return nullptr;
}

std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto result = std::make_unique<NumExprAST>(NumVal);
  getNextTok();
  return std::move(result);
}

std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextTok();
  auto InnerExpr = ParseExpression();
  if (!InnerExpr) {
    return nullptr;
  }
  if (CurTok != ')') {
    return LogError("expected ')'");
  }
  getNextTok();
  return InnerExpr;
}

std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;
  getNextTok();
  if (CurTok != '(') {
    return std::make_unique<VarExprAST>(IdName);
  }
  getNextTok();
  std::vector<std::unique_ptr<ExprAST>> args;
  if (CurTok != ')') {
    while (true) {
      if (auto arg = ParseExpression()) {
        args.push_back(std::move(arg));
      } else {
        return nullptr;
      }
      if (CurTok == ')') {
        break;
      }
      if (CurTok != ',') {
        return LogError("Expected ')' or ',' in argument list ");
      }
      getNextTok();
    }
  }
  getNextTok();
  return std::make_unique<CallExprAST>(IdName, std::move(args));
}

std::map<char, int> BinopPrecedence;

int GetTokenPrecedence() {
  if (!isascii(CurTok)) {
    return -1;
  }
  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) {
    return -1;
  }
  return TokPrec;
}

std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS) {
    return nullptr;
  }
  return ParseBinOpRHS(0, std::move(LHS));
}
std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                       std::unique_ptr<ExprAST> LHS) {
  while (true) {
    int TokPrec = GetTokenPrecedence();
    if (TokPrec < ExprPrec) {
      return LHS;
    }
    int BinOp = CurTok;
    getNextTok();
    auto RHS = ParsePrimary();
    if (!RHS) {
      return nullptr;
    }

    int nextPrec = GetTokenPrecedence();
    if (TokPrec < nextPrec) {
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
      if (!RHS) {
        return nullptr;
      }
    }
    LHS = std::make_unique<BinExprAST>(BinOp, std::move(LHS), std::move(RHS));
  }
}
std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurTok != tok_identifier) {
    return LogErrorP("Expected funtion name in prototype");
  }
  std::string FnName = IdentifierStr;
  getNextTok();
  if (CurTok != '(') {
    return LogErrorP("Expected '( in prototype");
  }
  std::vector<std::string> ArgNames;
  while (getNextTok() == tok_identifier) {
    ArgNames.push_back(IdentifierStr);
  }
  if (CurTok != ')') {
    return LogErrorP("Expected ')' in prototype");
  }
  getNextTok();
  return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}
std::unique_ptr<functionAST> ParseDefinition() {
  getNextTok();
  auto Proto = ParsePrototype();
  if (!Proto) {
    return nullptr;
  }
  if (auto exp = ParseExpression()) {
    return std::make_unique<functionAST>(std::move(Proto), std::move(exp));
  }
  return nullptr;
}
std::unique_ptr<PrototypeAST> ParseExtern() {
  getNextTok();
  return ParsePrototype();
}
std::unique_ptr<functionAST> ParseTopLevelExpr() {
  if (auto expr = ParseExpression()) {
    auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
    return std::make_unique<functionAST>(std::move(Proto), std::move(expr));
  }
  return nullptr;
}

std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  case tok_identifier:
    return ParseIdentifierExpr();
    break;
  case tok_number:
    return ParseNumberExpr();
    break;
  default:
    return LogError("unknown token when expecting an expression");
  }
}
