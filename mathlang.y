%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

extern int yylex();
extern int line_num;
extern int col_num;
extern FILE *yyin;

void yyerror(const char *s);
const char* token_name(int tok);

/* Table des symboles globale */
SymbolTable* global_symbol_table = NULL;
%}

/* ===================== */
/* UNION                 */
/* ===================== */
%union {
    int intval;
    double floatval;
    char charval;
    char* strval;
    DataType datatype;
    struct {
        DataType type;
        SymbolEntry* symbol;
    } expr_info;
}

/* ===================== */
/* TOKENS AVEC VALEURS   */
/* ===================== */
%token <intval>   TOK_INT
%token <floatval> TOK_FLOAT
%token <charval>  TOK_CHAR
%token <strval>   TOK_STRING TOK_COMPLEX TOK_ID

/* ===================== */
/* MOTS-CLÉS             */
/* ===================== */
%token TOK_SOIT TOK_SOIT_CONST TOK_IN TOK_TEL_QUE TOK_TYPE TOK_ENREGISTREMENT
%token TOK_SI TOK_ALORS TOK_SINON TOK_FIN
%token TOK_TANT TOK_QUE TOK_FAIRE
%token TOK_POUR TOK_DE TOK_PAR TOK_A TOK_REPETER TOK_JUSQUA
%token TOK_SORTIR TOK_CONTINUER
%token TOK_AFFICHER TOK_AFFICHER_LIGNE TOK_LIRE
%token TOK_FONCTION TOK_PROCEDURE TOK_RETOURNER TOK_LAMBDA

/* ===================== */
/* BOOLÉENS / LOGIQUE    */
/* ===================== */
%token TOK_TRUE TOK_FALSE
%token TOK_AND TOK_OR TOK_NOT TOK_XOR
%token TOK_IMPLIES TOK_EQUIV

/* ===================== */
/* TYPES                 */
/* ===================== */
%token TOK_TYPE_Z TOK_TYPE_R TOK_TYPE_B TOK_TYPE_C
%token TOK_TYPE_SIGMA TOK_TYPE_CHAR
%token TOK_INT8 TOK_INT16 TOK_INT32 TOK_INT64
%token TOK_FLOAT_TYPE TOK_DOUBLE_TYPE
%token TOK_INT_FUNC TOK_REEL TOK_BOOLEEN TOK_CHAINE

/* Types non-terminaux */
%type <datatype> type type_base type_arrow
%type <expr_info> expression expr_or expr_xor expr_and expr_cmp expr_add expr_mul expr_unary primaire

/* ===================== */
/* MATHS                 */
/* ===================== */
%token TOK_SIN TOK_COS TOK_EXP TOK_LOG TOK_SQRT
%token TOK_ABS TOK_FLOOR TOK_CEIL TOK_ROUND
%token TOK_RE TOK_IM TOK_ARG

/* ===================== */
/* ENSEMBLES / MATRICES  */
/* ===================== */
%token TOK_UNION TOK_INTER TOK_DIFF TOK_SYMDIFF
%token TOK_SUBSET TOK_STRICT_SUBSET TOK_NOT_IN
%token TOK_DET TOK_INV TOK_TRACE TOK_IDENTITE

/* ===================== */
/* OPÉRATEURS            */
/* ===================== */
%token TOK_ASSIGN TOK_ARROW
%token TOK_PLUS TOK_MINUS TOK_MULT TOK_DIV_REAL TOK_DIV TOK_MOD TOK_POWER
%token TOK_EQ TOK_NEQ TOK_LT TOK_GT TOK_LEQ TOK_GEQ

/* ===================== */
/* SYMBOLES              */
/* ===================== */
%token TOK_LPAREN TOK_RPAREN
%token TOK_LBRACKET TOK_RBRACKET
%token TOK_LBRACE TOK_RBRACE
%token TOK_COMMA TOK_COLON TOK_DOT TOK_PIPE TOK_SEMICOLON

/* ===================== */
/* FONCTIONS / CHAÎNES   */
/* ===================== */
%token TOK_MAJUSCULES TOK_MINUSCULES TOK_DIVISER

/* ===================== */
/* PRIORITÉS             */
/* ===================== */
%right TOK_ASSIGN
%right TOK_ARROW
%left TOK_OR TOK_XOR
%left TOK_AND
%left TOK_EQ TOK_NEQ TOK_LT TOK_GT TOK_LEQ TOK_GEQ
%left TOK_PLUS TOK_MINUS
%left TOK_MULT TOK_DIV_REAL TOK_DIV TOK_MOD
%right TOK_POWER
%right UMINUS

%%

/* ===================== */
/* PROGRAMME             */
/* ===================== */
programme
    : liste_declarations
    | /* vide */
    ;

liste_declarations
    : liste_declarations declaration
    | declaration
    ;

declaration
    : declaration_variable
    | declaration_constante
    | declaration_type
    | declaration_fonction
    | declaration_procedure
    | instruction
    ;

/* ===================== */
/* DÉCLARATIONS          */
/* ===================== */
declaration_variable
    : TOK_SOIT TOK_ID TOK_IN type {
        if (global_symbol_table) {
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, $4, 
                      SUBTYPE_DEFAULT);
        }
        free($2);
    }
    | TOK_SOIT TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression {
        if (global_symbol_table) {
            if (strcmp($2, $6) == 0) {
                if (!check_type_compatibility($4, $8.type)) {
                    error_type_mismatch($4, $8.type, line_num, col_num);
                }
                add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, $4, 
                          SUBTYPE_DEFAULT);
                SymbolEntry* entry = find_symbol(global_symbol_table, $2);
                if (entry) {
                    mark_symbol_initialized(entry);
                }
            } else {
                semantic_error("Le nom de variable ne correspond pas", line_num, col_num);
            }
        }
        free($2);
        free($6);
    }
    ;

declaration_constante
    : TOK_SOIT_CONST TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression {
        if (global_symbol_table) {
            if (strcmp($2, $6) == 0) {
                if (!check_type_compatibility($4, $8.type)) {
                    error_type_mismatch($4, $8.type, line_num, col_num);
                }
                add_symbol(global_symbol_table, $2, SYMBOL_CONSTANT, $4, 
                          SUBTYPE_DEFAULT);
                SymbolEntry* entry = find_symbol(global_symbol_table, $2);
                if (entry) {
                    mark_symbol_initialized(entry);
                }
            } else {
                semantic_error("Le nom de constante ne correspond pas", line_num, col_num);
            }
        }
        free($2);
        free($6);
    }
    ;

declaration_type
    : TOK_TYPE TOK_ID TOK_EQ type
    | TOK_TYPE TOK_ID TOK_EQ TOK_ENREGISTREMENT liste_champs TOK_FIN
    ;

liste_champs
    : champ
    | liste_champs champ
    ;

champ
    : TOK_ID TOK_COLON type
    ;

/* ===================== */
/* TYPES                 */
/* ===================== */
type
    : type_arrow { $$ = $1; }
    ;

type_arrow
    : type_base { $$ = $1; }
    | type_base TOK_ARROW type_arrow { $$ = TYPE_FUNCTION; /* Fonction */ }
    ;

type_base
    : TOK_TYPE_Z { $$ = TYPE_Z; }
    | TOK_TYPE_R { $$ = TYPE_R; }
    | TOK_TYPE_B { $$ = TYPE_B; }
    | TOK_TYPE_C { $$ = TYPE_C; }
    | TOK_TYPE_SIGMA { $$ = TYPE_SIGMA; }
    | TOK_TYPE_CHAR { $$ = TYPE_CHAR; }
    | TOK_INT8 { $$ = TYPE_Z; }
    | TOK_INT16 { $$ = TYPE_Z; }
    | TOK_INT32 { $$ = TYPE_Z; }
    | TOK_INT64 { $$ = TYPE_Z; }
    | TOK_FLOAT_TYPE { $$ = TYPE_R; }
    | TOK_DOUBLE_TYPE { $$ = TYPE_R; }
    | TOK_ID { $$ = TYPE_UNKNOWN; /* Type personnalisé */ }
    | TOK_LPAREN type TOK_RPAREN { $$ = $2; }
    ;

/* ===================== */
/* FONCTIONS             */
/* ===================== */
declaration_fonction
    : TOK_FONCTION TOK_ID TOK_LPAREN TOK_RPAREN TOK_COLON type bloc TOK_FIN
    | TOK_FONCTION TOK_ID TOK_LPAREN parametres TOK_RPAREN TOK_COLON type bloc TOK_FIN
    ;

declaration_procedure
    : TOK_PROCEDURE TOK_ID TOK_LPAREN TOK_RPAREN bloc TOK_FIN
    | TOK_PROCEDURE TOK_ID TOK_LPAREN parametres TOK_RPAREN bloc TOK_FIN
    ;

parametres
    : parametre
    | parametres TOK_COMMA parametre
    ;

parametre
    : TOK_ID TOK_COLON type
    ;

/* ===================== */
/* INSTRUCTIONS          */
/* ===================== */
bloc
    : liste_instructions
    | /* vide */
    ;

liste_instructions
    : liste_instructions instruction
    | instruction
    ;

instruction
    : affectation
    | instruction_si
    | instruction_tant_que
    | instruction_pour
    | instruction_repeter
    | instruction_io
    | TOK_RETOURNER expression
    | TOK_SORTIR
    | TOK_CONTINUER
    ;

/* ===================== */
/* AFFECTATION           */
/* ===================== */
affectation
    : TOK_ID TOK_ASSIGN expression {
        if (global_symbol_table) {
            SymbolEntry* entry = find_symbol(global_symbol_table, $1);
            if (!entry) {
                error_undeclared_symbol($1, line_num, col_num);
            } else {
                if (entry->is_const) {
                    error_const_assignment($1, line_num, col_num);
                }
                if (!check_type_compatibility(entry->type, $3.type)) {
                    error_type_mismatch(entry->type, $3.type, line_num, col_num);
                }
                mark_symbol_initialized(entry);
                mark_symbol_used(entry);
            }
        }
        free($1);
    }
    ;

/* ===================== */
/* STRUCTURES            */
/* ===================== */
instruction_si
    : TOK_SI expression TOK_ALORS bloc TOK_FIN {
        if ($2.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", line_num, col_num);
        }
    }
    | TOK_SI expression TOK_ALORS bloc TOK_SINON bloc TOK_FIN {
        if ($2.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", line_num, col_num);
        }
    }
    ;

instruction_tant_que
    : TOK_TANT TOK_QUE expression TOK_FAIRE bloc TOK_FIN {
        if ($3.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", line_num, col_num);
        }
    }
    ;

instruction_pour
    : TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_Z,
                       SUBTYPE_DEFAULT);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
    } bloc {
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    | TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_PAR expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_Z,
                       SUBTYPE_DEFAULT);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
    } bloc {
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    | TOK_POUR TOK_ID TOK_IN expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_UNKNOWN,
                       SUBTYPE_DEFAULT);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
    } bloc {
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    ;

instruction_repeter
    : TOK_REPETER bloc TOK_JUSQUA expression
    ;

/* ===================== */
/* I/O                   */
/* ===================== */
instruction_io
    : TOK_AFFICHER TOK_LPAREN liste_expressions TOK_RPAREN
    | TOK_AFFICHER_LIGNE TOK_LPAREN liste_expressions TOK_RPAREN
    | TOK_LIRE TOK_LPAREN TOK_ID TOK_RPAREN {
        if (global_symbol_table) {
            SymbolEntry* e = find_symbol(global_symbol_table, $3);
            if (!e) {
                error_undeclared_symbol($3, line_num, col_num);
            } else {
                mark_symbol_initialized(e);
                mark_symbol_used(e);
            }
        }
        free($3);
    }
    ;

liste_expressions
    : expression
    | liste_expressions TOK_COMMA expression
    ;

/* ===================== */
/* EXPRESSIONS (SANS CONFLITS) */
/* ===================== */

expression
    : expr_or { $$ = $1; }
    ;

expr_or
    : expr_or TOK_OR expr_xor {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_OR);
        $$.symbol = NULL;
    }
    | expr_xor { $$ = $1; }
    ;

expr_xor
    : expr_xor TOK_XOR expr_and {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_XOR);
        $$.symbol = NULL;
    }
    | expr_and { $$ = $1; }
    ;

expr_and
    : expr_and TOK_AND expr_cmp {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_AND);
        $$.symbol = NULL;
    }
    | expr_cmp { $$ = $1; }
    ;

expr_cmp
    : expr_cmp TOK_EQ expr_add {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_EQ);
        $$.symbol = NULL;
    }
    | expr_cmp TOK_LT expr_add {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_LT);
        $$.symbol = NULL;
    }
    | expr_cmp TOK_GT expr_add {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_GT);
        $$.symbol = NULL;
    }
    | expr_add { $$ = $1; }
    ;

expr_add
    : expr_add TOK_PLUS expr_mul {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_ADD);
        $$.symbol = NULL;
    }
    | expr_add TOK_MINUS expr_mul {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_SUB);
        $$.symbol = NULL;
    }
    | expr_mul { $$ = $1; }
    ;

expr_mul
    : expr_mul TOK_MULT expr_unary {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_MUL);
        $$.symbol = NULL;
    }
    | expr_mul TOK_DIV_REAL expr_unary {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_DIV);
        $$.symbol = NULL;
    }
    | expr_unary { $$ = $1; }
    ;

expr_unary
    : TOK_MINUS expr_unary %prec UMINUS {
        $$.type = infer_unary_operation_type($2.type, OP_SUB);
        $$.symbol = NULL;
    }
    | TOK_NOT expr_unary {
        $$.type = infer_unary_operation_type($2.type, OP_NOT);
        $$.symbol = NULL;
    }
    | primaire { $$ = $1; }
    ;

primaire
    : TOK_INT {
        $$.type = TYPE_Z;
        $$.symbol = NULL;
    }
    | TOK_FLOAT {
        $$.type = TYPE_R;
        $$.symbol = NULL;
    }
    | TOK_STRING {
        $$.type = TYPE_SIGMA;
        $$.symbol = NULL;
    }
    | TOK_CHAR {
        $$.type = TYPE_CHAR;
        $$.symbol = NULL;
    }
    | TOK_COMPLEX {
        $$.type = TYPE_C;
        $$.symbol = NULL;
    }
    | TOK_TRUE {
        $$.type = TYPE_B;
        $$.symbol = NULL;
    }
    | TOK_FALSE {
        $$.type = TYPE_B;
        $$.symbol = NULL;
    }
    | TOK_ID {
        if (global_symbol_table) {
            SymbolEntry* entry = find_symbol(global_symbol_table, $1);
            if (!entry) {
                error_undeclared_symbol($1, line_num, col_num);
                $$.type = TYPE_ERROR;
                $$.symbol = NULL;
            } else {
                if (!entry->is_initialized && entry->category == SYMBOL_VARIABLE) {
                    error_uninitialized_variable($1, line_num, col_num);
                }
                mark_symbol_used(entry);
                $$.type = entry->type;
                $$.symbol = entry;
            }
        } else {
            $$.type = TYPE_UNKNOWN;
            $$.symbol = NULL;
        }
        free($1);
    }
    | TOK_LPAREN expression TOK_RPAREN {
        $$ = $2;
    }
    ;


%%

void yyerror(const char *s) {
    fprintf(stderr,
        "Erreur syntaxique ligne %d colonne %d : %s\n",
        line_num, col_num, s);
}

/* ===================== */
/* AFFICHAGE DES TOKENS  */
/* ===================== */

const char* token_name(int tok) {
    switch (tok) {

        /* Mots-clés */
        case TOK_SOIT: return "TOK_SOIT";
        case TOK_SOIT_CONST: return "TOK_SOIT_CONST";
        case TOK_IN: return "TOK_IN";
        case TOK_TEL_QUE: return "TOK_TEL_QUE";
        case TOK_TYPE: return "TOK_TYPE";
        case TOK_ENREGISTREMENT: return "TOK_ENREGISTREMENT";

        case TOK_SI: return "TOK_SI";
        case TOK_ALORS: return "TOK_ALORS";
        case TOK_SINON: return "TOK_SINON";
        case TOK_FIN: return "TOK_FIN";

        case TOK_TANT: return "TOK_TANT";
        case TOK_QUE: return "TOK_QUE";
        case TOK_FAIRE: return "TOK_FAIRE";

        case TOK_POUR: return "TOK_POUR";
        case TOK_DE: return "TOK_DE";
        case TOK_A: return "TOK_A";
        case TOK_PAR: return "TOK_PAR";

        case TOK_REPETER: return "TOK_REPETER";
        case TOK_JUSQUA: return "TOK_JUSQUA";

        case TOK_RETOURNER: return "TOK_RETOURNER";
        case TOK_SORTIR: return "TOK_SORTIR";
        case TOK_CONTINUER: return "TOK_CONTINUER";

        case TOK_AFFICHER: return "TOK_AFFICHER";
        case TOK_AFFICHER_LIGNE: return "TOK_AFFICHER_LIGNE";
        case TOK_LIRE: return "TOK_LIRE";

        /* Types */
        case TOK_TYPE_Z: return "TOK_TYPE_Z";
        case TOK_TYPE_R: return "TOK_TYPE_R";
        case TOK_TYPE_B: return "TOK_TYPE_B";
        case TOK_TYPE_C: return "TOK_TYPE_C";

        /* Identificateurs & constantes */
        case TOK_ID: return "TOK_ID";
        case TOK_INT: return "TOK_INT";
        case TOK_FLOAT: return "TOK_FLOAT";
        case TOK_STRING: return "TOK_STRING";
        case TOK_CHAR: return "TOK_CHAR";
        case TOK_COMPLEX: return "TOK_COMPLEX";
        case TOK_TRUE: return "TOK_TRUE";
        case TOK_FALSE: return "TOK_FALSE";

        /* Opérateurs */
        case TOK_ASSIGN: return "TOK_ASSIGN";
        case TOK_PLUS: return "TOK_PLUS";
        case TOK_MINUS: return "TOK_MINUS";
        case TOK_MULT: return "TOK_MULT";
        case TOK_DIV_REAL: return "TOK_DIV_REAL";

        case TOK_EQ: return "TOK_EQ";
        case TOK_NEQ: return "TOK_NEQ";
        case TOK_LT: return "TOK_LT";
        case TOK_GT: return "TOK_GT";
        case TOK_LEQ: return "TOK_LEQ";
        case TOK_GEQ: return "TOK_GEQ";

        case TOK_AND: return "TOK_AND";
        case TOK_OR: return "TOK_OR";
        case TOK_NOT: return "TOK_NOT";
        case TOK_XOR: return "TOK_XOR";

        /* Séparateurs */
        case TOK_LPAREN: return "TOK_LPAREN";
        case TOK_RPAREN: return "TOK_RPAREN";
        case TOK_COMMA: return "TOK_COMMA";
        case TOK_COLON: return "TOK_COLON";
        case TOK_SEMICOLON: return "TOK_SEMICOLON";

        default: return "TOKEN_INCONNU";
    }
}

/* ===================== */
/* MAIN                  */
/* ===================== */

int main(int argc, char **argv) {
    extern FILE *yyin;
    int tok;

    /* Initialiser la table des symboles */
    global_symbol_table = init_symbol_table();
    set_semantic_error_mode(SEMANTIC_NON_FATAL);

    if (argc < 2) {
        fprintf(stderr, "Usage : %s <fichier>\n", argv[0]);
        free_symbol_table(global_symbol_table);
        return 1;
    }

    /* ===================== */
    /* ANALYSE LEXICALE      */
    /* ===================== */
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", argv[1]);
        free_symbol_table(global_symbol_table);
        return 1;
    }

    /* Réinitialiser les positions avant la première passe */
    line_num = 1;
    col_num = 1;

    printf("=== ANALYSE LEXICALE ===\n\n");
    printf("%-6s %-6s %-22s %s\n",
           "Ligne", "Col", "Token", "Valeur");
    printf("------------------------------------------------------------\n");

    while ((tok = yylex()) != 0) {
        printf("%-6d %-6d %-22s ",
               line_num, col_num, token_name(tok));

        switch (tok) {
            case TOK_ID:
            case TOK_STRING:
            case TOK_COMPLEX:
                printf("%s", yylval.strval);
                break;

            case TOK_INT:
                printf("%d", yylval.intval);
                break;

            case TOK_FLOAT:
                printf("%f", yylval.floatval);
                break;

            case TOK_CHAR:
                printf("'%c'", yylval.charval);
                break;

            default:
                printf("-");
        }
        printf("\n");
    }

    fclose(yyin);

    /* ===================== */
    /* ANALYSE SYNTAXIQUE    */
    /* ===================== */
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Erreur : impossible de rouvrir %s\n", argv[1]);
        free_symbol_table(global_symbol_table);
        return 1;
    }

    /* Réinitialiser les positions avant la seconde passe (syntaxe) */
    line_num = 1;
    col_num = 1;

    printf("\n=== ANALYSE SYNTAXIQUE ===\n\n");

    if (yyparse() == 0)
        printf("Analyse syntaxique réussie\n");
    else
        printf("Analyse syntaxique échouée\n");

    fclose(yyin);

    /* Afficher la table des symboles finale */
    print_symbol_table(global_symbol_table);

    /* Libérer la mémoire */
    free_symbol_table(global_symbol_table);
    return 0;
}

