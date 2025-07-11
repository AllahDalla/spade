#ifndef SPADE_SEM_H
#define SPADE_SEM_H

#include "spade.parser.h"
#include "spade.symbol.h"

void analyze_AST(ASTNode *tree, SymbolTable *symbol_table);

#endif