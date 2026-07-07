#ifndef CODEGEN_C_H
#define CODEGEN_C_H

#include <stdio.h>
#include "quadruplet.h"
#include "symbol_table.h"

/* ========================================================= */
/*  GÉNÉRATION DE CODE C À PARTIR DES QUADRUPLETS             */
/* ========================================================= */
void generate_c_code(FILE* out, QuadList* list, SymbolTable* table);

#endif