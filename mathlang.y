%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "quadruplet.h"
#include "expr_info.h"

extern int yylex();
extern int line_num;
extern int col_num;
extern FILE *yyin;

void yyerror(const char *s);
const char* token_name(int tok);

/* Table des symboles globale */
SymbolTable* global_symbol_table = NULL;
QuadList* quadList = NULL;


char* expr_to_addr(ExprInfo e) {
    // Si c'est une comparaison qui n'a pas encore généré son temporaire
    if (e.cmp_op != CMP_NONE && e.cmp_left && e.cmp_right) {
        // Générer le quadruplet de comparaison maintenant
        char* t = newTemp();
        QuadOp op;
        switch (e.cmp_op) {
            case CMP_EQ:  op = QUAD_EQ; break;
            case CMP_NEQ: op = QUAD_NEQ; break;
            case CMP_LT:  op = QUAD_LT; break;
            case CMP_GT:  op = QUAD_GT; break;
            case CMP_LEQ: op = QUAD_LEQ; break;
            case CMP_GEQ: op = QUAD_GEQ; break;
            default: op = QUAD_EQ; break;
        }
        createQuad(quadList, op, e.cmp_left, e.cmp_right, t);
        return t;
    }
    
    if (!e.addr) {
        fprintf(stderr, "Erreur: expr_to_addr appelé avec e.addr == NULL\n");
        return strdup("0");  // Retourner une valeur par défaut au lieu de NULL
    }
    
    // Always return a heap-allocated copy
    char* copy = malloc(strlen(e.addr) + 1);
    if (copy) strcpy(copy, e.addr);
    return copy;
}

/* Génère un branchement conditionnel optimisé 
 * Pour IF: on saute si la condition est FAUSSE (inverse de la comparaison)
 * Retourne l'indice du quadruplet généré
 */
int generate_inverse_branch(QuadList* list, ExprInfo cond) {
    if (cond.cmp_op != CMP_NONE && cond.cmp_left && cond.cmp_right) {
        // C'est une comparaison : générer le branchement inverse direct
        QuadOp branch_op;
        switch (cond.cmp_op) {
            case CMP_EQ:  branch_op = QUAD_BNE; break;  // si !=, sauter
            case CMP_NEQ: branch_op = QUAD_BE; break;   // si ==, sauter  
            case CMP_LT:  branch_op = QUAD_BGE; break;  // si >=, sauter
            case CMP_GT:  branch_op = QUAD_BLE; break;  // si <=, sauter
            case CMP_LEQ: branch_op = QUAD_BG; break;   // si >, sauter
            case CMP_GEQ: branch_op = QUAD_BL; break;   // si <, sauter
            default: branch_op = QUAD_BZ; break;
        }
        createQuad(list, branch_op, cond.cmp_left, cond.cmp_right, "");
        return nextQuad(list) - 1;
    } else {
        // Pas une comparaison simple : utiliser BZ classique
        char* addr = expr_to_addr(cond);
        // Standard layout: condition in arg1, arg2 unused
        createQuad(list, QUAD_BZ, addr, NULL, "");
        free(addr);
        return nextQuad(list) - 1;
    }
}

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;

#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
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
    ExprInfo expr_info;
}

%locations
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
%type <expr_info> expression expr_or expr_xor expr_and expr_cmp expr_add expr_mul expr_unary primaire expr_pow



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
                      SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
        }
    }
    | TOK_SOIT TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression {
        if (global_symbol_table) {
            if (strcmp($2, $6) == 0) {
                if (!check_type_compatibility($4, $8.type)) {
                    error_type_mismatch($4, $8.type, @8.first_line, @8.first_column);
                }
                add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, $4, 
                          SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
                SymbolEntry* entry = find_symbol(global_symbol_table, $2);
                if (entry) {
                    mark_symbol_initialized(entry);
                }
            } else {
                semantic_error("Le nom de variable ne correspond pas", @2.first_line, @2.first_column);
            }
        }
        free($2);
    }
    ;

declaration_constante
    : TOK_SOIT_CONST TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression {
        if (global_symbol_table) {
            if (strcmp($2, $6) == 0) {
                if (!check_type_compatibility($4, $8.type)) {
                    error_type_mismatch($4, $8.type, @8.first_line, @8.first_column);
                }
                add_symbol(global_symbol_table, $2, SYMBOL_CONSTANT, $4, 
                          SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
                SymbolEntry* entry = find_symbol(global_symbol_table, $2);
                if (entry) {
                    mark_symbol_initialized(entry);
                }
            } else {
                semantic_error("Le nom de constante ne correspond pas", @2.first_line, @2.first_column);
            }
        }
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
    | TOK_RETOURNER expression {
        // Générer quadruplet de retour
        char* ret_addr = expr_to_addr($2);
        createQuad(quadList, QUAD_RETURN, ret_addr, NULL, NULL);
        free(ret_addr);
    }
    | TOK_SORTIR {
        // BREAK : sortir de la boucle courante
        // Générer un BR avec destination inconnue
        createQuad(quadList, QUAD_BR, NULL, NULL, "");
        // Empiler dans la pile dédiée au break
        if (!isIntStackEmpty(&whileExitStack)) {
            // Dans une boucle WHILE
            pushInt(&whileExitStack, nextQuad(quadList) - 1);
        } else if (!isIntStackEmpty(&forStartStack)) {
            // Dans une boucle FOR : empiler dans forBreakStack
            pushInt(&forBreakStack, nextQuad(quadList) - 1);
        } else if (!isIntStackEmpty(&repeatStartStack)) {
            // Dans une boucle REPEAT
            pushInt(&repeatStartStack, nextQuad(quadList) - 1);
        }
    }
    | TOK_CONTINUER {
        // CONTINUE : revenir au début de la boucle
        // Générer un BR vers le début de la boucle
        if (!isIntStackEmpty(&whileStartStack)) {
            // Dans une boucle WHILE
            int start_index = peekInt(&whileStartStack);
            char start_addr[16];
            sprintf(start_addr, "%d", start_index);
            createQuad(quadList, QUAD_BR, NULL, NULL, start_addr);
        } else if (!isIntStackEmpty(&forStartStack)) {
            // Dans une boucle FOR : empiler dans forContinueStack
            createQuad(quadList, QUAD_BR, NULL, NULL, "");
            pushInt(&forContinueStack, nextQuad(quadList) - 1);
        } else if (!isIntStackEmpty(&repeatStartStack)) {
            // Dans une boucle REPEAT
            int start_index = peekInt(&repeatStartStack);
            char start_addr[16];
            sprintf(start_addr, "%d", start_index);
            createQuad(quadList, QUAD_BR, NULL, NULL, start_addr);
        }
    }

/* ===================== */
/* AFFECTATION           */
/* ===================== */
affectation
    : TOK_ID TOK_ASSIGN expression {
        int semantic_error = 0;
        if (global_symbol_table) {
            SymbolEntry* entry = find_symbol(global_symbol_table, $1);
            if (!entry) {
                error_undeclared_symbol($1, @1.first_line, @1.first_column);
                semantic_error = 1;
            } else {
                if (entry->is_const) {
                    error_const_assignment($1, @1.first_line, @1.first_column);
                    semantic_error = 1;
                }
                if (!check_type_compatibility(entry->type, $3.type)) {
                    error_type_mismatch(entry->type, $3.type, @3.first_line, @3.first_column);
                    semantic_error = 1;
                }
                if (!semantic_error) {
                    // GÉNÉRATION DE QUADRUPLET
                    char* addr = expr_to_addr($3);
                    createQuad(quadList, QUAD_ASSIGN, addr, NULL, $1);
                    free(addr);
                    mark_symbol_initialized(entry);
                    mark_symbol_used(entry);
                }
            }
        }
    }
    ;

/* ===================== */
/* STRUCTURES            */
/* ===================== */
instruction_si
    : TOK_SI expression {
        // Vérification de type
        if ($2.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", @2.first_line, @2.first_column);
        }
        // Générer branchement conditionnel inversé (saute si faux)
        int branch_index = generate_inverse_branch(quadList, $2);
        // Libérer les adresses de comparaison si nécessaire
        if ($2.cmp_left) free($2.cmp_left);
        if ($2.cmp_right) free($2.cmp_right);
        // Empiler l'indice pour le compléter plus tard
        pushInt(&ifStack, branch_index);
    } TOK_ALORS bloc partie_sinon_opt TOK_FIN
    ;

partie_sinon_opt
    : /* vide */ {
        // Pas de ELSE : compléter le BZ pour pointer ici (après le bloc THEN)
        int bz_index = popInt(&ifStack);
        char next_addr[16];
        sprintf(next_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, next_addr);
    }
    | TOK_SINON TOK_SI {
        // SINON SI : chaînage d'alternatives (else-if)
        // Générer BR pour sauter la fin depuis le THEN précédent
        createQuad(quadList, QUAD_BR, NULL, NULL, "");
        int br_index = nextQuad(quadList) - 1;
        pushInt(&ifBrStack, br_index);
        
        // Compléter le BZ précédent : il pointe vers ce nouveau SI
        int bz_index = popInt(&ifStack);
        char else_addr[16];
        sprintf(else_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, else_addr);
    } expression {
        // Vérification de type pour la nouvelle condition
        if ($4.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", @4.first_line, @4.first_column);
        }
        // Générer branchement conditionnel inversé pour cette condition
        int branch_index = generate_inverse_branch(quadList, $4);
        if ($4.cmp_left) free($4.cmp_left);
        if ($4.cmp_right) free($4.cmp_right);
        // Empiler ce nouveau BZ
        pushInt(&ifStack, branch_index);
    } TOK_ALORS bloc partie_sinon_opt {
        // Compléter tous les BR de la chaîne else-if
        // partie_sinon_opt recursive a déjà complété son propre BZ
        // On doit maintenant compléter notre BR (celui qu'on a généré au début)
        if (!isIntStackEmpty(&ifBrStack)) {
            int br_index = popInt(&ifBrStack);
            char end_addr[16];
            sprintf(end_addr, "%d", nextQuad(quadList));
            updateQuad(quadList, br_index, end_addr);
        }
    }
    | TOK_SINON {
        // SINON simple (dernier cas, pas de condition)
        createQuad(quadList, QUAD_BR, NULL, NULL, "");
        int br_index = nextQuad(quadList) - 1;
        
        // Compléter le BZ du IF : il pointe vers le début du ELSE
        int bz_index = popInt(&ifStack);
        char else_addr[16];
        sprintf(else_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, else_addr);
        
        pushInt(&ifBrStack, br_index);
    } bloc {
        // Compléter le BR : il pointe vers ici (après le bloc ELSE)
        if (!isIntStackEmpty(&ifBrStack)) {
            int br_index = popInt(&ifBrStack);
            char end_addr[16];
            sprintf(end_addr, "%d", nextQuad(quadList));
            updateQuad(quadList, br_index, end_addr);
        }
    }
    ;

instruction_tant_que
    : TOK_TANT TOK_QUE {
        // Mémoriser l'indice de début de boucle (pour y revenir)
        pushInt(&whileStartStack, nextQuad(quadList));
    } expression {
        // Vérification de type
        if ($4.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", @4.first_line, @4.first_column);
        }
        // Générer branchement conditionnel inversé (saute si faux)
        int branch_index = generate_inverse_branch(quadList, $4);
        // Libérer les adresses de comparaison si nécessaire
        if ($4.cmp_left) free($4.cmp_left);
        if ($4.cmp_right) free($4.cmp_right);
        // Empiler l'indice pour le compléter plus tard
        pushInt(&whileExitStack, branch_index);
    } TOK_FAIRE bloc TOK_FIN {
        // Fin du corps de la boucle : générer BR pour revenir au début
        int start_index = popInt(&whileStartStack);
        char start_addr[16];
        sprintf(start_addr, "%d", start_index);
        createQuad(quadList, QUAD_BR, NULL, NULL, start_addr);
        
        // Compléter le BZ de sortie : il pointe vers ici (après la boucle)
        int bz_index = popInt(&whileExitStack);
        char exit_addr[16];
        sprintf(exit_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, exit_addr);
    }
    ;

instruction_pour
    : TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_Z,
                       SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
        
        // Initialiser la variable de boucle
        char* start_addr = expr_to_addr($4);
        createQuad(quadList, QUAD_ASSIGN, start_addr, NULL, $2);
        free(start_addr);
        
        // Mémoriser le début de la boucle (test de condition)
        pushInt(&forStartStack, nextQuad(quadList));
        
        // Générer le branchement : BG i, fin, sortie (si i > fin, sortir)
        char* end_addr = expr_to_addr($6);
        createQuad(quadList, QUAD_BG, $2, end_addr, "");
        free(end_addr);
        pushInt(&forExitStack, nextQuad(quadList) - 1);
    } bloc {
        // Incrémentation : variable = variable + 1
        int continue_target = nextQuad(quadList);  // Calculer AVANT l'incrémentation
        char* temp_incr = newTemp();
        createQuad(quadList, QUAD_ADD, $2, "1", temp_incr);
        createQuad(quadList, QUAD_ASSIGN, temp_incr, NULL, $2);
        free(temp_incr);
        
        // Compléter les BR de CONTINUER (ils pointent vers l'incrémentation)
        while (!isIntStackEmpty(&forContinueStack)) {
            int br_index = popInt(&forContinueStack);
            char target_addr[16];
            sprintf(target_addr, "%d", continue_target);
            updateQuad(quadList, br_index, target_addr);
        }
        
        // Retour au début de la boucle (test)
        int start_index = popInt(&forStartStack);
        char start_addr[16];
        sprintf(start_addr, "%d", start_index);
        createQuad(quadList, QUAD_BR, NULL, NULL, start_addr);
        
        // Compléter le BZ de sortie : il pointe vers APRÈS cette boucle
        int bz_index = popInt(&forExitStack);
        char exit_addr[16];
        sprintf(exit_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, exit_addr);

        // Compléter les BR de SORTIR (ils pointent vers la fin de la boucle)
        while (!isIntStackEmpty(&forBreakStack)) {
            int br_index = popInt(&forBreakStack);
            updateQuad(quadList, br_index, exit_addr);
        }
        
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    | TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_PAR expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_Z,
                       SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
        
        // Initialiser la variable de boucle
        char* start_addr = expr_to_addr($4);
        createQuad(quadList, QUAD_ASSIGN, start_addr, NULL, $2);
        free(start_addr);
        
        // Mémoriser le début de la boucle (test de condition)
        pushInt(&forStartStack, nextQuad(quadList));
        
        // Générer le branchement : BG i, fin, sortie (si i > fin, sortir)
        char* end_addr = expr_to_addr($6);
        createQuad(quadList, QUAD_BG, $2, end_addr, "");
        free(end_addr);
        pushInt(&forExitStack, nextQuad(quadList) - 1);
    } bloc {
        // Incrémentation : variable = variable + pas
        char* step_addr = expr_to_addr($8);
        char* temp_incr = newTemp();
        createQuad(quadList, QUAD_ADD, $2, step_addr, temp_incr);
        createQuad(quadList, QUAD_ASSIGN, temp_incr, NULL, $2);
        free(step_addr);
        free(temp_incr);
        
        // Compléter les BR de SORTIR (ils pointent vers la fin de la boucle)
        while (!isIntStackEmpty(&forBreakStack)) {
            int br_index = popInt(&forBreakStack);
            char exit_addr[16];
            sprintf(exit_addr, "%d", nextQuad(quadList));
            updateQuad(quadList, br_index, exit_addr);
        }
        
        // Compléter les BR de CONTINUER (ils pointent vers le retour au début)
        int continue_target = nextQuad(quadList);
        while (!isIntStackEmpty(&forContinueStack)) {
            int br_index = popInt(&forContinueStack);
            char target_addr[16];
            sprintf(target_addr, "%d", continue_target);
            updateQuad(quadList, br_index, target_addr);
        }
        
        // Retour au début de la boucle (test)
        int start_index = popInt(&forStartStack);
        char start_addr[16];
        sprintf(start_addr, "%d", start_index);
        createQuad(quadList, QUAD_BR, NULL, NULL, start_addr);
        
        // Compléter le BZ de sortie
        int bz_index = popInt(&forExitStack);
        char exit_addr[16];
        sprintf(exit_addr, "%d", nextQuad(quadList));
        updateQuad(quadList, bz_index, exit_addr);
        
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    | TOK_POUR TOK_ID TOK_IN expression TOK_FAIRE {
        if (global_symbol_table) {
            enter_scope(global_symbol_table);
            add_symbol(global_symbol_table, $2, SYMBOL_VARIABLE, TYPE_UNKNOWN,
                       SUBTYPE_DEFAULT, @2.first_line, @2.first_column);
            SymbolEntry* it = find_symbol(global_symbol_table, $2);
            if (it) mark_symbol_initialized(it);
        }
        // Version simplifiée pour FOR...IN (parcours d'ensemble)
        // Cette version nécessiterait des quadruplets spécifiques pour l'itération
    } bloc {
        if (global_symbol_table) exit_scope(global_symbol_table);
        free($2);
    } TOK_FIN
    ;

instruction_repeter
    : TOK_REPETER {
        // Mémoriser l'indice de début de boucle
        pushInt(&repeatStartStack, nextQuad(quadList));
    } bloc TOK_JUSQUA expression {
        // Vérification de type de la condition
        if ($5.type != TYPE_B) {
            semantic_error("La condition doit être de type B (booléen)", @5.first_line, @5.first_column);
        }
        
        int start_index = popInt(&repeatStartStack);
        char start_addr[16];
        sprintf(start_addr, "%d", start_index);
        
        // REPEAT...UNTIL continue tant que la condition est FAUSSE
        // Donc on génère un branchement inversé qui saute au début si faux
        if ($5.cmp_op != CMP_NONE && $5.cmp_left && $5.cmp_right) {
            // C'est une comparaison : générer le branchement inverse direct
            QuadOp branch_op;
            switch ($5.cmp_op) {
                case CMP_EQ:  branch_op = QUAD_BNE; break;  // si !=, reboucler
                case CMP_NEQ: branch_op = QUAD_BE; break;   // si ==, reboucler
                case CMP_LT:  branch_op = QUAD_BGE; break;  // si >=, reboucler
                case CMP_GT:  branch_op = QUAD_BLE; break;  // si <=, reboucler
                case CMP_LEQ: branch_op = QUAD_BG; break;   // si >, reboucler
                case CMP_GEQ: branch_op = QUAD_BL; break;   // si <, reboucler
                default: branch_op = QUAD_BZ; break;
            }
            createQuad(quadList, branch_op, $5.cmp_left, $5.cmp_right, start_addr);
            free($5.cmp_left);
            free($5.cmp_right);
        } else {
            // Pas une comparaison simple : utiliser BZ classique
            char* cond_addr = expr_to_addr($5);
            // Standard layout: condition in arg1, arg2 unused
            createQuad(quadList, QUAD_BZ, cond_addr, NULL, start_addr);
            free(cond_addr);
        }
    }
    ;

/* ===================== */
/* I/O                   */
/* ===================== */
instruction_io
    : TOK_AFFICHER TOK_LPAREN liste_expressions TOK_RPAREN {
        // Les quadruplets sont générés dans liste_expressions
    }
    | TOK_AFFICHER_LIGNE TOK_LPAREN liste_expressions TOK_RPAREN {
        // Générer un quadruplet pour le saut de ligne
        createQuad(quadList, QUAD_WRITELN, NULL, NULL, NULL);
    }
    | TOK_LIRE TOK_LPAREN TOK_ID TOK_RPAREN {
        if (global_symbol_table) {
            SymbolEntry* e = find_symbol(global_symbol_table, $3);
            if (!e) {
                error_undeclared_symbol($3, @3.first_line, @3.first_column);
            } else {
                // Générer quadruplet READ
                createQuad(quadList, QUAD_READ, NULL, NULL, $3);
                mark_symbol_initialized(e);
                mark_symbol_used(e);
            }
        }
        free($3);
    }
    ;

liste_expressions
    : expression {
        // Générer quadruplet WRITE pour cette expression
        char* addr = expr_to_addr($1);
        if (addr) {
            createQuad(quadList, QUAD_WRITE, addr, NULL, NULL);
            free(addr);
        }
    }
    | liste_expressions TOK_COMMA expression {
        // Générer quadruplet WRITE pour cette expression
        char* addr = expr_to_addr($3);
        if (addr) {
            createQuad(quadList, QUAD_WRITE, addr, NULL, NULL);
            free(addr);
        }
    }
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
        $$.is_literal = 0;
    }
    | expr_xor { $$ = $1; }
    ;

expr_xor
    : expr_xor TOK_XOR expr_and {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_XOR);
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_and { $$ = $1; }
    ;

expr_and
    : expr_and TOK_AND expr_cmp {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_AND);
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_cmp { $$ = $1; }
    ;

expr_cmp
    : expr_cmp TOK_EQ expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_EQ;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;  // Sera généré si nécessaire
    }
    | expr_cmp TOK_NEQ expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_NEQ;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;
    }
    | expr_cmp TOK_LT expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_LT;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;
    }
    | expr_cmp TOK_GT expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_GT;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;
    }
    | expr_cmp TOK_LEQ expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_LEQ;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;
    }
    | expr_cmp TOK_GEQ expr_add {
        if (!check_comparable_types($1.type, $3.type)) {
            error_type_mismatch($1.type, $3.type, @2.first_line, @2.first_column);
        }
        
        // Stocker les informations de comparaison pour optimisation
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.cmp_op = CMP_GEQ;
        $$.cmp_left = expr_to_addr($1);
        $$.cmp_right = expr_to_addr($3);
        $$.addr = NULL;
    }
    | expr_add {
        $$ = $1;
        $$.cmp_op = CMP_NONE;  // Pas une comparaison
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    ;
    ;


expr_add
    : expr_add TOK_PLUS expr_mul {
        /* Vérifier si c'est concaténation de chaînes */
        if ($1.type == TYPE_SIGMA || $3.type == TYPE_SIGMA) {
            $$.type = infer_string_operation_type($1.type, $3.type, OP_ADD);
        } else {
            $$.type = infer_binary_operation_type($1.type, $3.type, OP_ADD);
        }
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        if (addr1 && addr2) {
            createQuad(quadList, QUAD_ADD, addr1, addr2, t);
        }
        $$.addr = t;
        if (addr1) free(addr1);
        if (addr2) free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = ($1.is_literal && $3.is_literal) ? 1 : 0;
        if ($$.is_literal) {
            $$.literal_int = $1.literal_int + $3.literal_int;
            $$.literal_float = $1.literal_float + $3.literal_float;
        }
    }
    | expr_add TOK_MINUS expr_mul {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_SUB);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_SUB, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = ($1.is_literal && $3.is_literal) ? 1 : 0;
        if ($$.is_literal) {
            $$.literal_int = $1.literal_int - $3.literal_int;
            $$.literal_float = $1.literal_float - $3.literal_float;
        }
    }
    | expr_mul { $$ = $1; }
    ;

expr_mul
    : expr_mul TOK_MULT expr_pow {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_MUL);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_MUL, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = ($1.is_literal && $3.is_literal) ? 1 : 0;
        if ($$.is_literal) {
            $$.literal_int = $1.literal_int * $3.literal_int;
            $$.literal_float = $1.literal_float * $3.literal_float;
        }
    }
    | expr_mul TOK_DIV_REAL expr_pow {
        /* Vérifier division par zéro littéral */
        if ($3.is_literal && $3.literal_float == 0.0) {
            semantic_error("Division par zéro - impossible à la compilation", 
                          @3.first_line, @3.first_column);
        }
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_DIV);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_DIV, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_mul TOK_DIV expr_pow {
        /* Vérifier division entière par zéro littéral */
        if ($3.is_literal && $3.literal_int == 0) {
            semantic_error("Division entière par zéro - impossible à la compilation", 
                          @3.first_line, @3.first_column);
        }
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_DIV);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_DIV_INT, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_mul TOK_MOD expr_pow {
        /* Vérifier modulo par zéro littéral */
        if ($3.is_literal && $3.literal_int == 0) {
            semantic_error("Modulo par zéro - impossible à la compilation", 
                          @3.first_line, @3.first_column);
        }
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_MOD);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_MOD, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_pow { $$ = $1; }
    ;

expr_pow
    : expr_unary TOK_POWER expr_pow {
        $$.type = infer_binary_operation_type($1.type, $3.type, OP_POW);
        
        // GÉNÉRATION DE QUADRUPLET
        char* t = newTemp();
        char* addr1 = expr_to_addr($1);
        char* addr2 = expr_to_addr($3);
        createQuad(quadList, QUAD_POW, addr1, addr2, t);
        $$.addr = t;
        free(addr1);
        free(addr2);
        
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | expr_unary { $$ = $1; }
    ;
expr_unary
    : TOK_MINUS expr_unary %prec UMINUS {
        $$.type = infer_unary_operation_type($2.type, OP_SUB);
        $$.symbol = NULL;
        $$.is_literal = $2.is_literal;
        $$.literal_int = -$2.literal_int;
        $$.literal_float = -$2.literal_float;
    }
    | TOK_NOT expr_unary {
        $$.type = infer_unary_operation_type($2.type, OP_NOT);
        $$.symbol = NULL;
        $$.is_literal = 0;
    }
    | primaire { $$ = $1; }
    ;

primaire
    : TOK_INT {
        $$.type = TYPE_Z;
        $$.symbol = NULL;
        $$.is_literal = 1;
        $$.literal_int = $1;
        $$.literal_float = (double)$1;

         // GÉNÉRATION ADRESSE
        char* addr = malloc(32);
        sprintf(addr, "%d", $1);
        $$.addr = addr;
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_FLOAT {
        $$.type = TYPE_R;
        $$.symbol = NULL;
        $$.is_literal = 1;
        $$.literal_int = (int)$1;
        $$.literal_float = $1;

         // GÉNÉRATION ADRESSE
        char* addr = malloc(32);
        sprintf(addr, "%g", $1);
        $$.addr = addr;
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_STRING {
        $$.type = TYPE_SIGMA;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.literal_int = 0;
        $$.literal_float = 0.0;
        $$.addr = strdup($1);
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_CHAR {
        $$.type = TYPE_CHAR;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.literal_int = 0;
        $$.literal_float = 0.0;
        char* addr = malloc(4);
        sprintf(addr, "'%c'", $1);
        $$.addr = addr;
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_COMPLEX {
        $$.type = TYPE_C;
        $$.symbol = NULL;
        $$.is_literal = 0;
        $$.literal_int = 0;
        $$.literal_float = 0.0;
        $$.addr = strdup($1);
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_TRUE {
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 1;
        $$.literal_int = 1;
        $$.literal_float = 1.0;
        char* addr = malloc(8);
        sprintf(addr, "true");
        $$.addr = addr;
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
    | TOK_FALSE {
        $$.type = TYPE_B;
        $$.symbol = NULL;
        $$.is_literal = 1;
        $$.literal_int = 0;
        $$.literal_float = 0.0;
        char* addr = malloc(8);
        sprintf(addr, "false");
        $$.addr = addr;
        $$.cmp_op = CMP_NONE;
        $$.cmp_left = NULL;
        $$.cmp_right = NULL;
    }
       | TOK_ID {
        if (global_symbol_table) {
            SymbolEntry* entry = find_symbol(global_symbol_table, $1);
            if (!entry) {
                error_undeclared_symbol($1, @1.first_line, @1.first_column);
                $$.type = TYPE_ERROR;
                $$.symbol = NULL;
                $$.addr = NULL;
                $$.cmp_op = CMP_NONE;
                $$.cmp_left = NULL;
                $$.cmp_right = NULL;
            } else {
                if (!entry->is_initialized && entry->category == SYMBOL_VARIABLE) {
                    error_uninitialized_variable($1, @1.first_line, @1.first_column);
                }
                mark_symbol_used(entry);
                $$.type = entry->type;
                $$.symbol = entry;
                $$.addr = strdup($1);
                $$.cmp_op = CMP_NONE;
                $$.cmp_left = NULL;
                $$.cmp_right = NULL;
            }
        } else {
            $$.type = TYPE_UNKNOWN;
            $$.symbol = NULL;
            $$.addr = NULL;
            $$.cmp_op = CMP_NONE;
            $$.cmp_left = NULL;
            $$.cmp_right = NULL;
        }
        $$.is_literal = 0;
        free($1);
    }
    | TOK_LPAREN expression TOK_RPAREN {
        $$ = $2;
    }
    | TOK_SIN TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_SIN, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        check_math_function_constraints(FUNC_SIN, $3.type, @1.first_line, @1.first_column);
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_SIN, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_COS TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_COS, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        check_math_function_constraints(FUNC_COS, $3.type, @1.first_line, @1.first_column);
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_COS, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_EXP TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_EXP, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        check_math_function_constraints(FUNC_EXP, $3.type, @1.first_line, @1.first_column);
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_EXP, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_LOG TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_LOG, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        if ($3.is_literal && $3.literal_float <= 0.0) {
            semantic_error("LOG(x) indéfini pour x inférieur ou egal à 0", @1.first_line, @1.first_column);
        }
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_LOG, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_SQRT TOK_LPAREN expression TOK_RPAREN {
        $$.type = TYPE_R;
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_SQRT, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_ABS TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_ABS, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_ABS, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_FLOOR TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_FLOOR, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_FLOOR, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_CEIL TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_CEIL, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_CEIL, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_ROUND TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_ROUND, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_ROUND, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_RE TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_RE, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_RE, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_IM TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_IM, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_IM, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_ARG TOK_LPAREN expression TOK_RPAREN {
        $$.type = infer_math_function_type(FUNC_ARG, $3.type);
        $$.symbol = NULL;
        $$.is_literal = 0;
        check_math_function_constraints(FUNC_ARG, $3.type, line_num, col_num);
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_ARG, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_MAJUSCULES TOK_LPAREN expression TOK_RPAREN {
        if ($3.type != TYPE_SIGMA) {
            error_type_mismatch(TYPE_SIGMA, $3.type, @3.first_line, @3.first_column);
            $$.type = TYPE_ERROR;
        } else {
            $$.type = TYPE_SIGMA;
        }
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_MAJUSCULES, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
    }
    | TOK_MINUSCULES TOK_LPAREN expression TOK_RPAREN {
        if ($3.type != TYPE_SIGMA) {
            error_type_mismatch(TYPE_SIGMA, $3.type, @3.first_line, @3.first_column);
            $$.type = TYPE_ERROR;
        } else {
            $$.type = TYPE_SIGMA;
        }
        $$.symbol = NULL;
        $$.is_literal = 0;
        char* t = newTemp();
        char* arg_addr = expr_to_addr($3);
        createQuad(quadList, QUAD_MINUSCULES, arg_addr, NULL, t);
        $$.addr = t;
        free(arg_addr);
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
        case TOK_TYPE_SIGMA: return "TOK_TYPE_SIGMA";
        case TOK_TYPE_CHAR: return "TOK_TYPE_CHAR";

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
        case TOK_DIV: return "TOK_DIV";
        case TOK_MOD: return "TOK_MOD";
        case TOK_POWER: return "TOK_POWER";

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

        /* Fonctions mathématiques */
        case TOK_SIN: return "TOK_SIN";
        case TOK_COS: return "TOK_COS";
        case TOK_EXP: return "TOK_EXP";
        case TOK_LOG: return "TOK_LOG";
        case TOK_SQRT: return "TOK_SQRT";
        case TOK_ABS: return "TOK_ABS";
        case TOK_FLOOR: return "TOK_FLOOR";
        case TOK_CEIL: return "TOK_CEIL";
        case TOK_ROUND: return "TOK_ROUND";
        case TOK_RE: return "TOK_RE";
        case TOK_IM: return "TOK_IM";
        case TOK_ARG: return "TOK_ARG";

        case TOK_FONCTION: return "TOK_FONCTION";

        /* Fonctions chaînes */
        case TOK_MAJUSCULES: return "TOK_MAJUSCULES";
        case TOK_MINUSCULES: return "TOK_MINUSCULES";

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

    /* INITIALISER LES QUADRUPLETS */
    quadList = initQuadList();
    initControlStacks();

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

     /* AFFICHER LES QUADRUPLETS */
    printQuadruplets(quadList);

    /* Libérer la mémoire */
    free_symbol_table(global_symbol_table);
     freeQuadList(quadList); 
    return 0;
}

