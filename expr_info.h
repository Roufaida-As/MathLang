#ifndef EXPR_INFO_H
#define EXPR_INFO_H

#include "symbol_table.h"

typedef struct {
    char* addr;
    DataType type;
    SymbolEntry* symbol;
    int is_literal;
    int literal_int;
    double literal_float;
} ExprInfo;

#endif