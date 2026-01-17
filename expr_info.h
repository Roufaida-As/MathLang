#ifndef EXPR_INFO_H
#define EXPR_INFO_H

#include "symbol_table.h"

// Type d'opération de comparaison (pour optimisation des branchements)
typedef enum {
    CMP_NONE,   // Pas une comparaison
    CMP_EQ,     // ==
    CMP_NEQ,    // !=
    CMP_LT,     // <
    CMP_GT,     // >
    CMP_LEQ,    // <=
    CMP_GEQ     // >=
} ComparisonOp;

typedef struct {
    char* addr;
    DataType type;
    SymbolEntry* symbol;
    int is_literal;
    int literal_int;
    double literal_float;
    
    // Pour les comparaisons : permet de générer directement les branchements
    ComparisonOp cmp_op;
    char* cmp_left;   // Opérande gauche de la comparaison
    char* cmp_right;  // Opérande droite de la comparaison
} ExprInfo;

#endif