
#include "codegen.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "llvm/ADT/APFloat.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value *> NamedValues;

llvm::Value *LogErrorV(const char *str) {
  LogError(str);
  return nullptr;
}

llvm::Value *NumExprAST::codegen() {
  return llvm::ConstantFP::get(*TheContext, llvm::APFloat(val));
}

llvm::Value *VarExprAST::codegen() {
  llvm::Value *V = NamedValues[name];
  if (!V) {
    LogErrorV("Unknown variable name");
  }
  return V;
}

llvm::Value *BinExprAST::codegen() {
  llvm::Value *L = lhs->codegen();
  llvm::Value *R = rhs->codegen();

  if (!L || !R) {
    return nullptr;
  }
  switch (op) {
  case '+':
    return Builder->CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder->CreateFSub(L, R, "subtmp");
  case '*':
    return Builder->CreateFMul(L, R, "multmp");
  case '<':
    L = Builder->CreateFCmpULT(L, R, "cmtmp");
    return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext),
                                 "booltmp");

  default:
    return LogErrorV("invalid binary operator");
  }
}

llvm::Value *CallExprAST::codegen() {
  llvm::Function *calleeF = TheModule->getFunction(callee);
  if (!calleeF) {
    return LogErrorV("Unknown function referenced");
  }
  if (calleeF->arg_size() != args.size()) {
    return LogErrorV("Incorrect # arguments passed");
  }
  std::vector<llvm::Value *> argsv;
  for (unsigned i = 0; i < args.size(); ++i) {
    argsv.push_back(args[i]->codegen());
    if (!argsv.back()) {
      return nullptr;
    }
  }
  return Builder->CreateCall(calleeF, argsv, "calltmp");
}

llvm::Function *PrototypeAST::codegen() {
  std::vector<llvm::Type *> Doubles(args.size(),
                                    llvm::Type::getDoubleTy(*TheContext));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*TheContext), Doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, name, TheModule.get());

  unsigned indx = 0;
  for (auto &arg : F->args()) {
    arg.setName(args[indx++]);
  }
  return F;
}
