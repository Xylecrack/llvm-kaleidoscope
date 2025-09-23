#include "ast.h"
#include "lexer.h"
#include "parser.h"

static void HandleDefinition() {
  if (ParseDefinition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    getNextTok();
  }
}
static void HandleExtern() {
  if (ParseExtern()) {
    fprintf(stderr, "Parsed an extern.\n");
  } else {
    getNextTok();
  }
}
static void HandleTopLevelExpression() {
  if (ParseTopLevelExpr()) {
    fprintf(stderr, "Parsed a top-level expression.\n");
  } else {
    getNextTok();
  }
}

static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';':
      getNextTok();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}
int main() {

  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;

  fprintf(stderr, "ready> ");
  getNextTok();

  MainLoop();

  return 0;
}
