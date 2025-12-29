#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SemanticErrorMode error_mode = SEMANTIC_FATAL;

/* ========================================================= */
/* UTILITAIRES                                               */
/* ========================================================= */

char* string_duplicate(const char* str) {
    if (!str) return NULL;
    char* s = malloc(strlen(str) + 1);
    if (!s) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(s, str);
    return s;
}

unsigned int hash_function(const char* name) {
    unsigned int hash = 5381;
    int c;
    while ((c = *name++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

/* ========================================================= */
/* INITIALISATION / DESTRUCTION                               */
/* ========================================================= */

SymbolTable* init_symbol_table(void) {
    SymbolTable* table = calloc(1, sizeof(SymbolTable));
    if (!table) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    table->current_scope = 0;
    table->scopes = NULL;
    return table;
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    while (table->scopes)
        exit_scope(table);

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolEntry* e = table->entries[i];
        while (e) {
            SymbolEntry* next = e->next;
            free(e->name);
            free(e->details);
            free(e);
            e = next;
        }
    }
    free(table);
}

/* ========================================================= */
/* PORTÉES                                                   */
/* ========================================================= */

void enter_scope(SymbolTable* table) {
    ScopeStack* s = calloc(1, sizeof(ScopeStack));
    s->level = ++table->current_scope;
    s->parent = table->scopes;
    table->scopes = s;
}

void exit_scope(SymbolTable* table) {
    if (!table || !table->scopes) return;

    int level = table->current_scope;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolEntry *e = table->entries[i], *prev = NULL;
        while (e) {
            if (e->scope_level == level) {
                SymbolEntry* del = e;
                e = e->next;
                if (prev) prev->next = e;
                else table->entries[i] = e;
                free(del->name);
                free(del->details);
                free(del);
                table->count--;
            } else {
                prev = e;
                e = e->next;
            }
        }
    }

    ScopeStack* old = table->scopes;
    table->scopes = old->parent;
    free(old);
    table->current_scope--;
}

/* ========================================================= */
/* SYMBOLES                                                  */
/* ========================================================= */

bool add_symbol(SymbolTable* table,
                const char* name,
                SymbolCategory category,
                DataType type,
                NumericSubType subtype) {

    SymbolEntry* old = find_symbol_in_current_scope(table, name);
    if (old) {
        error_redeclared_symbol(name, 0, 0, 0);
        return false;
    }

    SymbolEntry* e = calloc(1, sizeof(SymbolEntry));
    e->name = string_duplicate(name);
    e->category = category;
    e->type = type;
    e->subtype = subtype;
    e->scope_level = table->current_scope;
    e->is_const = (category == SYMBOL_CONSTANT);

    unsigned idx = hash_function(name);
    e->next = table->entries[idx];
    table->entries[idx] = e;
    table->count++;
    return true;
}

SymbolEntry* find_symbol(SymbolTable* table, const char* name) {
    unsigned idx = hash_function(name);
    SymbolEntry* best = NULL;
    int best_scope = -1;

    for (SymbolEntry* e = table->entries[idx]; e; e = e->next) {
        if (!strcmp(e->name, name) && e->scope_level >= best_scope) {
            best = e;
            best_scope = e->scope_level;
        }
    }
    return best;
}

SymbolEntry* find_symbol_in_current_scope(SymbolTable* table, const char* name) {
    unsigned idx = hash_function(name);
    for (SymbolEntry* e = table->entries[idx]; e; e = e->next)
        if (!strcmp(e->name, name) && e->scope_level == table->current_scope)
            return e;
    return NULL;
}

bool remove_symbol(SymbolTable* table, const char* name) {
    unsigned idx = hash_function(name);
    SymbolEntry *e = table->entries[idx], *prev = NULL;
    while (e) {
        if (!strcmp(e->name, name)) {
            if (prev) prev->next = e->next;
            else table->entries[idx] = e->next;
            free(e->name);
            free(e->details);
            free(e);
            table->count--;
            return true;
        }
        prev = e;
        e = e->next;
    }
    return false;
}

void update_symbol_details(SymbolEntry* entry, TypeDetails* details) {
    if (!entry) return;
    free(entry->details);
    entry->details = details;
}

void mark_symbol_used(SymbolEntry* entry) {
    if (entry) entry->is_used = true;
}

void mark_symbol_initialized(SymbolEntry* entry) {
    if (entry) entry->is_initialized = true;
}

/* ========================================================= */
/* TYPES                                                     */
/* ========================================================= */

bool is_numeric_type(DataType t) {
    return t == TYPE_Z || t == TYPE_R || t == TYPE_C;
}

bool is_orderable_type(DataType t) {
    return t == TYPE_Z || t == TYPE_R || t == TYPE_SIGMA || t == TYPE_CHAR;
}

bool check_type_compatibility(DataType target, DataType source) {
    if (target == source) return true;
    if (target == TYPE_R && source == TYPE_Z) return true;
    if (target == TYPE_C && (source == TYPE_Z || source == TYPE_R)) return true;
    return false;
}

bool can_implicit_cast(DataType from, DataType to) {
    return check_type_compatibility(to, from);
}

DataType infer_binary_operation_type(DataType l, DataType r, SemanticOperator op) {
    if (l == TYPE_ERROR || r == TYPE_ERROR) return TYPE_ERROR;

    switch (op) {
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_POW:
            if (!is_numeric_type(l) || !is_numeric_type(r)) return TYPE_ERROR;
            if (l == TYPE_C || r == TYPE_C) return TYPE_C;
            if (l == TYPE_R || r == TYPE_R) return TYPE_R;
            return TYPE_Z;

        case OP_EQ: case OP_NEQ:
            return (check_type_compatibility(l, r) ||
                    check_type_compatibility(r, l)) ? TYPE_B : TYPE_ERROR;

        case OP_LT: case OP_GT: case OP_LEQ: case OP_GEQ:
            return (is_orderable_type(l) && is_orderable_type(r)) ? TYPE_B : TYPE_ERROR;

        case OP_AND: case OP_OR: case OP_XOR:
            return (l == TYPE_B && r == TYPE_B) ? TYPE_B : TYPE_ERROR;

        default:
            return TYPE_ERROR;
    }
}

DataType infer_unary_operation_type(DataType t, SemanticOperator op) {
    if (op == OP_SUB)
        return is_numeric_type(t) ? t : TYPE_ERROR;
    if (op == OP_NOT)
        return (t == TYPE_B) ? TYPE_B : TYPE_ERROR;
    return TYPE_ERROR;
}

/* ========================================================= */
/* ERREURS                                                   */
/* ========================================================= */

void semantic_error(const char* msg, int line, int col) {
    fprintf(stderr, "ERREUR [%d:%d] %s\n", line, col, msg);
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void semantic_warning(const char* msg, int line, int col) {
    fprintf(stderr, "AVERTISSEMENT [%d:%d] %s\n", line, col, msg);
}

void error_undeclared_symbol(const char* name, int line, int col) {
    fprintf(stderr, "ERREUR [%d:%d] symbole '%s' non déclaré\n", line, col, name);
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void error_redeclared_symbol(const char* name, int line, int col, int prev_line) {
    fprintf(stderr,
            "ERREUR [%d:%d] symbole '%s' déjà déclaré (ligne %d)\n",
            line, col, name, prev_line);
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void error_type_mismatch(DataType expected, DataType found, int line, int col) {
    fprintf(stderr,
            "ERREUR [%d:%d] type attendu %s, trouvé %s\n",
            line, col,
            type_to_string(expected),
            type_to_string(found));
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void error_uninitialized_variable(const char* name, int line, int col) {
    fprintf(stderr,
            "ERREUR [%d:%d] variable '%s' utilisée avant initialisation\n",
            line, col, name);
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void error_const_assignment(const char* name, int line, int col) {
    fprintf(stderr,
            "ERREUR [%d:%d] tentative de modification de la constante '%s'\n",
            line, col, name);
    if (error_mode == SEMANTIC_FATAL)
    exit(EXIT_FAILURE);
}

void set_semantic_error_mode(SemanticErrorMode mode) {
    error_mode = mode;
}


/* ========================================================= */
/* AFFICHAGE                                                 */
/* ========================================================= */

const char* type_to_string(DataType t) {
    switch (t) {
        case TYPE_Z: return "Z";
        case TYPE_R: return "R";
        case TYPE_B: return "B";
        case TYPE_C: return "C";
        case TYPE_SIGMA: return "SIGMA";
        case TYPE_CHAR: return "CHAR";
        case TYPE_ARRAY: return "ARRAY";
        case TYPE_SET: return "SET";
        case TYPE_FUNCTION: return "FUNCTION";
        case TYPE_VOID: return "VOID";
        case TYPE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

const char* category_to_string(SymbolCategory c) {
    switch (c) {
        case SYMBOL_VARIABLE: return "VAR";
        case SYMBOL_CONSTANT: return "CONST";
        case SYMBOL_FUNCTION: return "FUNC";
        case SYMBOL_PROCEDURE: return "PROC";
        case SYMBOL_PARAMETER: return "PARAM";
        case SYMBOL_TYPE: return "TYPE";
        default: return "UNKNOWN";
    }
}



static int cmp_entries(const void* a, const void* b) {
    const SymbolEntry* ea = *(const SymbolEntry* const*)a;
    const SymbolEntry* eb = *(const SymbolEntry* const*)b;
    if (ea->scope_level != eb->scope_level)
        return (ea->scope_level < eb->scope_level) ? -1 : 1;
    return strcmp(ea->name, eb->name);
}

void print_symbol_table(const SymbolTable* table) {
    if (!table) {
        printf("Symbol table: (null)\n");
        return;
    }

    printf("\n=== SYMBOL TABLE (%d symbols) ===\n", table->count);
    printf("%-20s %-10s %-10s %-5s %-6s %-6s %-6s\n",
           "Name", "Category", "Type", "Scope", "Const", "Init", "Used");
    printf("-----------------------------------------------------------------------\n");

    SymbolEntry** list = NULL;
    if (table->count > 0) {
        list = (SymbolEntry**)malloc(sizeof(SymbolEntry*) * table->count);
    }
    int idx = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (SymbolEntry* e = table->entries[i]; e; e = e->next) {
            if (list && idx < table->count) list[idx++] = e;
        }
    }
    if (list && idx > 0) qsort(list, idx, sizeof(SymbolEntry*), cmp_entries);

    for (int i = 0; i < idx; i++) {
        SymbolEntry* e = list[i];
        printf("%-20s %-10s %-10s %-5d %-6s %-6s %-6s\n",
               e->name,
               category_to_string(e->category),
               type_to_string(e->type),
               e->scope_level,
               e->is_const ? "yes" : "no",
               e->is_initialized ? "yes" : "no",
               e->is_used ? "yes" : "no");
    }

    free(list);
    printf("=======================================================================\n");
}
