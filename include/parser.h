#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <map>

extern int CurTok;
extern std::map<char, int> BinopPrecedence;

int getNextTok();
int GetTokenPrecedence();

std::unique_ptr<ExprAST> LogError(const char *str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *str);

std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseIdentifierExpr();
std::unique_ptr<ExprAST> ParsePrimary();
std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
std::unique_ptr<ExprAST> ParseExpression();
std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<functionAST> ParseDefinition();
std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<functionAST>ParseTopLevelExpr();
#endif