
#include "codegen.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<IRBuilder<>> Builder;
std::unique_ptr<Module> TheModule;
std::map<std::string, Value *> NamedValues;

Value *LogErrorV(const char *str) {
  LogError(str);
  return nullptr;
}

Value *NumExprAST::codegen() {
  return ConstantFP::get(*TheContext, APFloat(val));
}

Value *VarExprAST::codegen() {
  Value *V = NamedValues[name];
  if (!V) {
    LogErrorV("Unknown variable name");
  }
  return V;
}

Value *BinExprAST::codegen() {
  Value *L = lhs->codegen();
  Value *R = rhs->codegen();

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
    return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");

  default:
    return LogErrorV("invalid binary operator");
  }
}

Value *CallExprAST::codegen() {
  Function *calleeF = TheModule->getFunction(callee);
  if (!calleeF) {
    return LogErrorV("Unknown function referenced");
  }
  if (calleeF->arg_size() != args.size()) {
    return LogErrorV("Incorrect # arguments passed");
  }
  std::vector<Value *> argsv;
  for (unsigned i = 0; i < args.size(); ++i) {
    argsv.push_back(args[i]->codegen());
    if (!argsv.back()) {
      return nullptr;
    }
  }
  return Builder->CreateCall(calleeF, argsv, "calltmp");
}

Function *PrototypeAST::codegen() {
  std::vector<Type *> Doubles(args.size(), Type::getDoubleTy(*TheContext));
  FunctionType *FT =
      FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
  Function *F =
      Function::Create(FT, Function::ExternalLinkage, name, TheModule.get());

  unsigned indx = 0;
  for (auto &arg : F->args()) {
    arg.setName(args[indx++]);
  }
  return F;
}

Function *FunctionAST::codegen() {
  Function *TheFunction = TheModule->getFunction(proto->getName());
  if (!TheFunction) {
    TheFunction = proto->codegen();
  }
  if (!TheFunction) {
    return nullptr;
  }
  if (!TheFunction->empty()) {
    return (Function *)LogErrorV("Function cannot be redefined.");
  }
  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &arg : TheFunction->args()) {
    NamedValues[std::string(arg.getName())] = &arg;
  }

  if (Value *RetVal = body->codegen()) {
    Builder->CreateRet(RetVal);
    verifyFunction(*TheFunction);
    return TheFunction;
  }
  TheFunction->eraseFromParent();
  return nullptr;
}
