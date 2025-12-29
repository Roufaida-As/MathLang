#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

/* ========================================================= */
/*                   TYPES DE DONNÉES                         */
/* ========================================================= */

typedef enum {
    TYPE_UNKNOWN = 0,

    TYPE_Z,
    TYPE_R,
    TYPE_B,
    TYPE_C,
    TYPE_SIGMA,
    TYPE_CHAR,

    TYPE_ARRAY,
    TYPE_SET,
    TYPE_FUNCTION,

    TYPE_VOID,
    TYPE_ERROR
} DataType;

typedef enum {
    SUBTYPE_DEFAULT = 0,
    SUBTYPE_INT8,
    SUBTYPE_INT16,
    SUBTYPE_INT32,
    SUBTYPE_INT64,
    SUBTYPE_FLOAT,
    SUBTYPE_DOUBLE
} NumericSubType;

/* ========================================================= */
/*                 CATÉGORIES DE SYMBOLES                     */
/* ========================================================= */

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_CONSTANT,
    SYMBOL_FUNCTION,
    SYMBOL_PROCEDURE,
    SYMBOL_PARAMETER,
    SYMBOL_TYPE
} SymbolCategory;

/* ========================================================= */
/*                 OPÉRATEURS SÉMANTIQUES                     */
/* ========================================================= */

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LEQ, OP_GEQ,
    OP_AND, OP_OR, OP_XOR, OP_NOT
} SemanticOperator;

typedef enum {
    FUNC_SIN, FUNC_COS, FUNC_EXP, FUNC_LOG,
    FUNC_SQRT, FUNC_ABS, FUNC_FLOOR, FUNC_CEIL,
    FUNC_ROUND, FUNC_RE, FUNC_IM, FUNC_ARG,
    FUNC_MAJUSCULES, FUNC_MINUSCULES, FUNC_LENGTH,
    FUNC_CONCAT
} MathFunction;

/* ========================================================= */
/*                 DÉTAILS DE TYPES                           */
/* ========================================================= */

typedef struct {
    DataType element_type;
    int dimensions;
    int* sizes;
} ArrayDetails;

typedef struct {
    DataType element_type;
    bool is_bounded;
    int max_size;
} SetDetails;

typedef struct FunctionParam {
    char* name;
    DataType type;
    bool is_const;
    struct FunctionParam* next;
} FunctionParam;

typedef struct {
    DataType return_type;
    FunctionParam* parameters;
    int param_count;
    bool is_lambda;
    int defined_line;
} FunctionDetails;

typedef struct {
    DataType kind;
    union {
        ArrayDetails array;
        SetDetails set;
        FunctionDetails function;
    };
} TypeDetails;

/* ========================================================= */
/*                 ENTRÉE DE SYMBOLE                           */
/* ========================================================= */

typedef struct SymbolEntry {
    char* name;
    SymbolCategory category;

    DataType type;
    NumericSubType subtype;

    bool is_initialized;
    bool is_const;
    bool is_used;

    int scope_level;

    TypeDetails* details;

    struct SymbolEntry* next;
} SymbolEntry;

/* ========================================================= */
/*                 GESTION DES PORTÉES                        */
/* ========================================================= */

typedef struct ScopeStack {
    int level;
    struct ScopeStack* parent;
} ScopeStack;

/* ========================================================= */
/*                 TABLE DES SYMBOLES                         */
/* ========================================================= */

#define HASH_TABLE_SIZE 211

typedef struct {
    SymbolEntry* entries[HASH_TABLE_SIZE];
    int count;
    int current_scope;
    ScopeStack* scopes;
} SymbolTable;

/* ========================================================= */
/*                 API PRINCIPALE                             */
/* ========================================================= */

SymbolTable* init_symbol_table(void);
void free_symbol_table(SymbolTable* table);

/* Portées */
void enter_scope(SymbolTable* table);
void exit_scope(SymbolTable* table);

/* Symboles */
bool add_symbol(SymbolTable* table,
                const char* name,
                SymbolCategory category,
                DataType type,
                NumericSubType subtype,
                int line,
                int col);

SymbolEntry* find_symbol(SymbolTable* table, const char* name);
SymbolEntry* find_symbol_in_current_scope(SymbolTable* table, const char* name);

bool remove_symbol(SymbolTable* table, const char* name);
void update_symbol_details(SymbolEntry* entry, TypeDetails* details);

void mark_symbol_used(SymbolEntry* entry);
void mark_symbol_initialized(SymbolEntry* entry);

/* Debug */
void print_symbol_table(const SymbolTable* table);

/* ========================================================= */
/*                 SÉMANTIQUE / TYPES                         */
/* ========================================================= */

bool check_type_compatibility(DataType target, DataType source);
bool can_implicit_cast(DataType from, DataType to);

DataType infer_binary_operation_type(DataType left,
                                     DataType right,
                                     SemanticOperator op);

DataType infer_unary_operation_type(DataType operand,
                                    SemanticOperator op);

bool is_numeric_type(DataType type);
bool is_orderable_type(DataType type);

/* Fonctions mathématiques et chaînes */
DataType infer_math_function_type(MathFunction func, DataType arg_type);
void check_math_function_constraints(MathFunction func, DataType arg_type,
                                     int line, int col);
DataType infer_string_operation_type(DataType left, DataType right,
                                     SemanticOperator op);
void check_division_by_zero(int is_literal_divisor, int divisor_value,
                           int line, int col);

/* ========================================================= */
/*                 ERREURS / WARNINGS                         */
/* ========================================================= */

void semantic_error(const char* msg, int line, int col);
void semantic_warning(const char* msg, int line, int col);

void error_undeclared_symbol(const char* name, int line, int col);
void error_redeclared_symbol(const char* name, int line, int col, int prev_line);
void error_type_mismatch(DataType expected, DataType found, int line, int col);
void error_uninitialized_variable(const char* name, int line, int col);
void error_const_assignment(const char* name, int line, int col);

/* ========================================================= */
/*                 UTILITAIRES                                */
/* ========================================================= */

const char* type_to_string(DataType type);
const char* category_to_string(SymbolCategory category);

char* string_duplicate(const char* str);
unsigned int hash_function(const char* name);

/* ========================================================= */
/*                 MODE D’ERREUR                             */
/* ========================================================= */

typedef enum {
    SEMANTIC_FATAL,
    SEMANTIC_NON_FATAL
} SemanticErrorMode;

void set_semantic_error_mode(SemanticErrorMode mode);

#endif /* SYMBOL_TABLE_H */