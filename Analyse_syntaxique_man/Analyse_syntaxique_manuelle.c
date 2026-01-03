#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

/* Declare external variables needed by lex.yy.c */
extern int yylex(void);
extern FILE* yyin;
extern char* yytext;
extern int line_num;

/* Define yylval (declared as extern in tokens.h) */
YYSTYPE yylval;

/* Define yylloc that the lexer expects */
typedef struct {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;

YYLTYPE yylloc;

#define MAX_RHS 10
#define MAX_SYMBOLS 50
#define MAX_RULES 50
#define MAX_TERMINALS 30
#define MAX_NON_TERMINALS 20
#define MAX_STACK 100

typedef enum { false, true } bool;

/* Symboles */
typedef struct {
    char name[20];
    bool is_terminal;
    int token_code; // Code du token de tokens.h
} Symbol;

/* Règle de grammaire */
typedef struct {
    int lhs;               // index du non-terminal
    int rhs[MAX_RHS];      // indices des symboles à droite
    int rhs_len;
} Rule;

/* Grammaire */
Symbol symbols[MAX_SYMBOLS];
int nb_symbols = 0;
int terminals[MAX_TERMINALS];
int nb_terminals = 0;
int non_terminals[MAX_NON_TERMINALS];
int nb_non_terminals = 0;

Rule rules[MAX_RULES];
int nb_rules = 0;

int FIRST[MAX_SYMBOLS][MAX_TERMINALS];
int FOLLOW[MAX_NON_TERMINALS][MAX_TERMINALS];

/* Indicateur epsilon pour chaque symbole */
bool has_epsilon[MAX_SYMBOLS];

/* Table LL(1) : table[non_terminal_index][terminal_index] = numéro de règle */
int table[MAX_NON_TERMINALS][MAX_TERMINALS]; 

/* Ajouter un symbole et retourner son index */
int add_symbol(const char* name, bool is_terminal) {
    for (int i = 0; i < nb_symbols; i++)
        if (strcmp(symbols[i].name, name) == 0) return i;
    
    strcpy(symbols[nb_symbols].name, name);
    symbols[nb_symbols].is_terminal = is_terminal;
    symbols[nb_symbols].token_code = -1; // Par défaut
    
    if (is_terminal) terminals[nb_terminals++] = nb_symbols;
    else non_terminals[nb_non_terminals++] = nb_symbols;
    
    return nb_symbols++;
}

/* Mapper un code de token à un index de symbole */
int token_to_symbol(int token_code) {
    if (token_code == 0) {
        // EOF
        for (int i = 0; i < nb_symbols; i++) {
            if (strcmp(symbols[i].name, "#") == 0) return i;
        }
    }
    
    // Chercher uniquement dans les terminaux
    for (int i = 0; i < nb_symbols; i++) {
        if (symbols[i].is_terminal && symbols[i].token_code == token_code) {
            return i;
        }
    }
    
    // Debug: afficher ce qu'on cherche
    printf("DEBUG token_to_symbol: token_code %d non trouvé!\n", token_code);
    printf("DEBUG: Symboles terminaux disponibles:\n");
    for (int i = 0; i < nb_symbols; i++) {
        if (symbols[i].is_terminal) {
            printf("  [%d] %s -> token_code=%d\n", i, symbols[i].name, symbols[i].token_code);
        }
    }
    
    return -1;
}

/* Ajouter une règle */
void add_rule(int lhs, int* rhs, int rhs_len) {
    rules[nb_rules].lhs = lhs;
    rules[nb_rules].rhs_len = rhs_len;
    for (int i = 0; i < rhs_len; i++) rules[nb_rules].rhs[i] = rhs[i];
    nb_rules++;
}

/* Affiche les symboles */
void print_symbols() {
    printf("\n");
    printf("========================================\n");
    printf("     SYMBOLES DE LA GRAMMAIRE          \n");
    printf("========================================\n\n");
    
    printf("Non-Terminaux:\n");
    printf("------------------\n");
    for (int i = 0; i < nb_non_terminals; i++) {
        int idx = non_terminals[i];
        printf("  [%2d] %s\n", idx, symbols[idx].name);
    }
    
    printf("\nTerminaux:\n");
    printf("------------------\n");
    for (int i = 0; i < nb_terminals; i++) {
        int idx = terminals[i];
        printf("  [%2d] %-15s", idx, symbols[idx].name);
        if ((i + 1) % 3 == 0) printf("\n");
    }
    if (nb_terminals % 3 != 0) printf("\n");
}

/* Initialise FIRST et FOLLOW */
void init_first_follow() {
    for (int i = 0; i < nb_symbols; i++) {
        has_epsilon[i] = false;
        for (int j = 0; j < nb_terminals; j++)
            FIRST[i][j] = 0;
    }
    for (int i = 0; i < nb_non_terminals; i++)
        for (int j = 0; j < nb_terminals; j++)
            FOLLOW[i][j] = 0;
}

/* Vérifier si un non-terminal peut dériver epsilon */
bool can_derive_epsilon(int nt_symbol) {
    return has_epsilon[nt_symbol];
}

/* Calcul des ensembles FIRST (itératif) */
void compute_FIRST() {
    // Initialiser FIRST pour les terminaux : FIRST(a) = {a}
    for (int t = 0; t < nb_terminals; t++) {
        int term_sym = terminals[t];
        FIRST[term_sym][t] = 1;
        has_epsilon[term_sym] = false;
    }
    
    // Marquer les règles epsilon directes
    for (int r = 0; r < nb_rules; r++) {
        if (rules[r].rhs_len == 0) {
            has_epsilon[rules[r].lhs] = true;
        }
    }
    
    // Calculer FIRST pour les non-terminaux
    bool changed;
    do {
        changed = false;
        for (int r = 0; r < nb_rules; r++) {
            int lhs = rules[r].lhs;
            
            // Si la règle est vide (epsilon), déjà traité
            if (rules[r].rhs_len == 0) {
                continue;
            }
            
            // Vérifier si tous les symboles peuvent dériver epsilon
            bool all_epsilon = true;
            
            // Pour chaque symbole de la partie droite
            for (int i = 0; i < rules[r].rhs_len; i++) {
                int sym = rules[r].rhs[i];
                
                // Ajouter FIRST(sym) à FIRST(lhs)
                for (int t = 0; t < nb_terminals; t++) {
                    if (!FIRST[lhs][t] && FIRST[sym][t]) {
                        FIRST[lhs][t] = 1;
                        changed = true;
                    }
                }
                
                // Si sym ne peut pas dériver epsilon, on s'arrête
                if (!has_epsilon[sym]) {
                    all_epsilon = false;
                    break;
                }
            }
            
            // Si tous les symboles peuvent dériver epsilon, lhs peut aussi
            if (all_epsilon && !has_epsilon[lhs]) {
                has_epsilon[lhs] = true;
                changed = true;
            }
        }
    } while(changed);
}

/* Calcul simplifié FOLLOW pour le symbole de départ */
void compute_FOLLOW(int start_symbol) {
    bool changed;
    
    int eof_idx = -1;
    for (int t = 0; t < nb_terminals; t++) {
        if (strcmp(symbols[terminals[t]].name, "#") == 0) {
            eof_idx = t;
            break;
        }
    }
    if (eof_idx != -1) {
        FOLLOW[start_symbol][eof_idx] = 1;
    }
    
    do {
        changed = false;
        for (int r = 0; r < nb_rules; r++) {
            int lhs = rules[r].lhs;
            int lhs_nt_idx = -1;
            for (int i = 0; i < nb_non_terminals; i++) {
                if (non_terminals[i] == lhs) {
                    lhs_nt_idx = i;
                    break;
                }
            }
            if (lhs_nt_idx == -1) continue;
            
            for (int i = 0; i < rules[r].rhs_len; i++) {
                int sym = rules[r].rhs[i];
                
                if (!symbols[sym].is_terminal) {
                    int sym_nt_idx = -1;
                    for (int j = 0; j < nb_non_terminals; j++) {
                        if (non_terminals[j] == sym) {
                            sym_nt_idx = j;
                            break;
                        }
                    }
                    if (sym_nt_idx == -1) continue;
                    
                    int j = i + 1;
                    bool all_epsilon = true;
                    
                    while (j < rules[r].rhs_len) {
                        int next_sym = rules[r].rhs[j];
                        if (symbols[next_sym].is_terminal) {
                            int t_idx = -1;
                            for (int t = 0; t < nb_terminals; t++) {
                                if (terminals[t] == next_sym) {
                                    t_idx = t;
                                    break;
                                }
                            }
                            if (t_idx != -1 && !FOLLOW[sym_nt_idx][t_idx]) {
                                FOLLOW[sym_nt_idx][t_idx] = 1;
                                changed = true;
                            }
                            all_epsilon = false;
                            break;
                        } else {
                            for (int t = 0; t < nb_terminals; t++) {
                                if (FIRST[next_sym][t] && !FOLLOW[sym_nt_idx][t]) {
                                    FOLLOW[sym_nt_idx][t] = 1;
                                    changed = true;
                                }
                            }
                        }
                        j++;
                    }
                    
                    if (all_epsilon) {
                        for (int t = 0; t < nb_terminals; t++) {
                            if (FOLLOW[lhs_nt_idx][t] && !FOLLOW[sym_nt_idx][t]) {
                                FOLLOW[sym_nt_idx][t] = 1;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while(changed);
}

/* Générer la table LL(1) */
void generate_LL1_table() {
    for (int i = 0; i < nb_non_terminals; i++)
        for (int j = 0; j < nb_terminals; j++)
            table[i][j] = -1; // -1 = erreur

    for (int r = 0; r < nb_rules; r++) {
        int lhs = rules[r].lhs;
        int lhs_nt_idx = -1;
        for (int i = 0; i < nb_non_terminals; i++) {
            if (non_terminals[i] == lhs) {
                lhs_nt_idx = i;
                break;
            }
        }
        if (lhs_nt_idx == -1) continue;
        
        if (rules[r].rhs_len == 0) {
            // Règle epsilon : ajouter dans FOLLOW(lhs)
            for (int t = 0; t < nb_terminals; t++) {
                if (FOLLOW[lhs_nt_idx][t]) {
                    table[lhs_nt_idx][t] = r;
                }
            }
        } else {
            // Pour chaque terminal dans FIRST de la partie droite
            bool all_can_be_epsilon = true;
            
            for (int i = 0; i < rules[r].rhs_len; i++) {
                int sym = rules[r].rhs[i];
                
                // Ajouter FIRST(sym) à la table
                for (int t = 0; t < nb_terminals; t++) {
                    if (FIRST[sym][t]) {
                        if (table[lhs_nt_idx][t] == -1) {
                            table[lhs_nt_idx][t] = r;
                        }
                    }
                }
                
                // Si sym ne peut pas dériver epsilon, on s'arrête
                if (symbols[sym].is_terminal || !can_derive_epsilon(sym)) {
                    all_can_be_epsilon = false;
                    break;
                }
            }
            
            // Si tous les symboles peuvent dériver epsilon, ajouter FOLLOW(lhs)
            if (all_can_be_epsilon) {
                for (int t = 0; t < nb_terminals; t++) {
                    if (FOLLOW[lhs_nt_idx][t]) {
                        if (table[lhs_nt_idx][t] == -1) {
                            table[lhs_nt_idx][t] = r;
                        }
                    }
                }
            }
        }
    }
}

/* Afficher les ensembles FIRST */
void print_FIRST() {
    printf("\n\n");
    printf("========================================\n");
    printf("      ENSEMBLES FIRST (DÉBUTS)         \n");
    printf("========================================\n\n");
    
    for (int i = 0; i < nb_non_terminals; i++) {
        int nt = non_terminals[i];
        printf("FIRST(%-10s) = { ", symbols[nt].name);
        
        bool first_printed = false;
        
        // Afficher epsilon si le non-terminal peut dériver epsilon
        if (can_derive_epsilon(nt)) {
            printf("ε");
            first_printed = true;
        }
        
        for (int t = 0; t < nb_terminals; t++) {
            if (FIRST[nt][t]) {
                if (first_printed) printf(", ");
                printf("%s", symbols[terminals[t]].name);
                first_printed = true;
            }
        }
        printf(" }\n");
    }
}

/* Afficher les ensembles FOLLOW */
void print_FOLLOW() {
    printf("\n\n");
    printf("========================================\n");
    printf("     ENSEMBLES FOLLOW (SUIVANTS)       \n");
    printf("========================================\n\n");
    
    for (int i = 0; i < nb_non_terminals; i++) {
        printf("FOLLOW(%-10s) = { ", symbols[non_terminals[i]].name);
        
        bool first_printed = false;
        for (int t = 0; t < nb_terminals; t++) {
            if (FOLLOW[i][t]) {
                if (first_printed) printf(", ");
                printf("%s", symbols[terminals[t]].name);
                first_printed = true;
            }
        }
        printf(" }\n");
    }
}

/* Afficher la table */
void print_LL1_table() {
    printf("\n\n");
    printf("========================================\n");
    printf("        TABLE D'ANALYSE LL(1)          \n");
    printf("========================================\n\n");
    
    printf("%-20s", "Non-Terminal");
    for (int t = 0; t < nb_terminals; t++) {
        printf(" %-6s", symbols[terminals[t]].name);
    }
    printf("\n");
    
    printf("%-20s", "--------------------");
    for (int t = 0; t < nb_terminals; t++) {
        printf(" ------");
    }
    printf("\n");
    
    for (int i = 0; i < nb_non_terminals; i++) {
        printf("%-20s", symbols[non_terminals[i]].name);
        for (int t = 0; t < nb_terminals; t++) {
            int rule_num = table[i][t];
            if (rule_num == -1) {
                printf(" %-6s", "--");
            } else {
                printf(" R%-5d", rule_num);
            }
        }
        printf("\n");
    }
    
    printf("\n\nLégende des règles:\n");
    printf("-------------------\n");
    for (int r = 0; r < nb_rules; r++) {
        printf("R%-2d: %s -> ", r, symbols[rules[r].lhs].name);
        if (rules[r].rhs_len == 0) {
            printf("ε");
        } else {
            for (int i = 0; i < rules[r].rhs_len; i++) {
                printf("%s", symbols[rules[r].rhs[i]].name);
                if (i < rules[r].rhs_len - 1) printf(" ");
            }
        }
        printf("\n");
    }
}

/* Exemple : définition de la grammaire MathLang */
void define_grammar() {
    int EOF_SYM = add_symbol("#", true);
    symbols[EOF_SYM].token_code = 0; // EOF
        
    int P = add_symbol("Programme", false);
    int Decls = add_symbol("Declarations", false);
    int Decl = add_symbol("Declaration", false);
    int Type = add_symbol("Type", false);
    int Instrs = add_symbol("Instructions", false);
    int Instr = add_symbol("Instruction", false);
    int Expr = add_symbol("Expr", false);
    int Exprp = add_symbol("Expr'", false);
    int Term = add_symbol("Term", false);
    int Termp = add_symbol("Term'", false);
    int Factor = add_symbol("Factor", false);

    int TOK_SOIT_SYM = add_symbol("SOIT", true);
    symbols[TOK_SOIT_SYM].token_code = TOK_SOIT;  // 100
    
    int TOK_ID_SYM = add_symbol("ID", true);
    symbols[TOK_ID_SYM].token_code = TOK_ID;  // 310
    
    int TOK_IN_SYM = add_symbol("DANS", true);
    symbols[TOK_IN_SYM].token_code = TOK_IN;  // 240 (le lexer retourne TOK_IN pour "dans")
    
    int TOK_Z = add_symbol("Z", true);
    symbols[TOK_Z].token_code = TOK_TYPE_Z;  // 180
    
    int TOK_R = add_symbol("R", true);
    symbols[TOK_R].token_code = TOK_TYPE_R;
    
    int TOK_B = add_symbol("B", true);
    symbols[TOK_B].token_code = TOK_TYPE_B;
    
    int TOK_AFF = add_symbol("AFFICHER", true);
    symbols[TOK_AFF].token_code = TOK_AFFICHER;
    
    int TOK_INT_SYM = add_symbol("INT", true);
    symbols[TOK_INT_SYM].token_code = TOK_INT;
    
    int TOK_FLOAT_SYM = add_symbol("FLOAT", true);
    symbols[TOK_FLOAT_SYM].token_code = TOK_FLOAT;
    
    int TOK_PLUS_SYM = add_symbol("+", true);
    symbols[TOK_PLUS_SYM].token_code = TOK_PLUS;
    
    int TOK_MINUS_SYM = add_symbol("-", true);
    symbols[TOK_MINUS_SYM].token_code = TOK_MINUS;
    
    int TOK_MULT_SYM = add_symbol("*", true);
    symbols[TOK_MULT_SYM].token_code = TOK_MULT;
    
    int TOK_LPAREN_SYM = add_symbol("(", true);
    symbols[TOK_LPAREN_SYM].token_code = TOK_LPAREN;
    
    int TOK_RPAREN_SYM = add_symbol(")", true);
    symbols[TOK_RPAREN_SYM].token_code = TOK_RPAREN;
    
    int TOK_ASSIGN_SYM = add_symbol("<-", true);
    symbols[TOK_ASSIGN_SYM].token_code = TOK_ASSIGN;

    // Ajouter les règles
    int rhs1[] = {Decls, Instrs}; add_rule(P, rhs1, 2);
    int rhs2[] = {Decl, Decls}; add_rule(Decls, rhs2, 2);
    int rhs3[] = {}; add_rule(Decls, rhs3, 0);
    int rhs4[] = {TOK_SOIT_SYM, TOK_ID_SYM, TOK_IN_SYM, Type}; add_rule(Decl, rhs4, 4);
    int rhs5[] = {TOK_Z}; add_rule(Type, rhs5, 1);
    int rhs6[] = {TOK_R}; add_rule(Type, rhs6, 1);
    int rhs7[] = {TOK_B}; add_rule(Type, rhs7, 1);
    int rhs8[] = {Instr, Instrs}; add_rule(Instrs, rhs8, 2);
    int rhs9[] = {}; add_rule(Instrs, rhs9, 0);
    int rhs10[] = {TOK_ID_SYM, TOK_ASSIGN_SYM, Expr}; add_rule(Instr, rhs10, 3);
    int rhs11[] = {TOK_AFF, TOK_LPAREN_SYM, Expr, TOK_RPAREN_SYM}; add_rule(Instr, rhs11, 4);
    int rhs12[] = {Term, Exprp}; add_rule(Expr, rhs12, 2);
    int rhs13[] = {}; add_rule(Exprp, rhs13, 0);
    int rhs14[] = {TOK_PLUS_SYM, Term, Exprp}; add_rule(Exprp, rhs14, 3);
    int rhs15[] = {TOK_MINUS_SYM, Term, Exprp}; add_rule(Exprp, rhs15, 3);
    int rhs16[] = {Factor, Termp}; add_rule(Term, rhs16, 2);
    int rhs17[] = {}; add_rule(Termp, rhs17, 0);
    int rhs18[] = {TOK_MULT_SYM, Factor, Termp}; add_rule(Termp, rhs18, 3);
    int rhs19[] = {TOK_ID_SYM}; add_rule(Factor, rhs19, 1);
    int rhs20[] = {TOK_INT_SYM}; add_rule(Factor, rhs20, 1);
    int rhs21[] = {TOK_FLOAT_SYM}; add_rule(Factor, rhs21, 1);
    int rhs22[] = {TOK_LPAREN_SYM, Expr, TOK_RPAREN_SYM}; add_rule(Factor, rhs22, 3);
}

/* Simple LL(1) parser using the table - CORRIGÉ */
void parse_file(const char* filename) {
    yyin = fopen(filename, "r");
    if (!yyin) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", filename);
        return;
    }
    
    printf("\n\n");
    printf("========================================\n");
    printf(" ANALYSE LL(1) DU FICHIER: %s\n", filename);
    printf("========================================\n\n");
    
    int stack[MAX_STACK];
    int stack_ptr = 0;
    
    // Trouver l'axiome
    int axiom = -1;
    for (int i = 0; i < nb_symbols; i++) {
        if (strcmp(symbols[i].name, "Programme") == 0) {
            axiom = i;
            break;
        }
    }
    
    // Trouver le token EOF
    int eof_token = -1;
    for (int t = 0; t < nb_terminals; t++) {
        if (strcmp(symbols[terminals[t]].name, "#") == 0) {
            eof_token = terminals[t];
            break;
        }
    }
    
    // Initialiser la pile avec # et l'axiome
    stack[stack_ptr++] = eof_token;
    stack[stack_ptr++] = axiom;
    

    int raw_token = yylex();
    
    int current_token = token_to_symbol(raw_token);
    
    if (current_token == -1) {
        current_token = eof_token;
    }
    printf("\n");
    
    int parse_success = 1;
    int step = 1;
    
    printf("%-5s | %-50s | %-15s | %-30s\n", "Étape", "Pile", "Token lu", "Action");
    printf("------|----------------------------------------------------|-----------------|---------------------------------\n");
    
    while (stack_ptr > 0) {
        int top = stack[stack_ptr - 1];
        
        // Afficher l'étape
        printf("%-5d | ", step);
        
        // Afficher la pile (sommet à droite)
        char pile_str[60] = "";
        for (int i = 0; i < stack_ptr && i < 8; i++) {
            if (i > 0) strcat(pile_str, " ");
            strcat(pile_str, symbols[stack[i]].name);
        }
        if (stack_ptr > 8) strcat(pile_str, " ...");
        printf("%-50s | %-15s | ", pile_str, symbols[current_token].name);
        
        if (symbols[top].is_terminal) {
            // Le sommet est un terminal
            if (top == current_token) {
                if (strcmp(symbols[top].name, "#") == 0) {
                    printf("# acceptation\n");
                } else {
                    printf("Accepter '%s' et avancer\n", symbols[top].name);
                }
                stack_ptr--;
                
        
                raw_token = yylex();
                
                current_token = token_to_symbol(raw_token);
                if (current_token == -1) {
                    current_token = eof_token;
                }
            } else {
                printf("ERREUR: attendu '%s', reçu '%s'\n", 
                       symbols[top].name, symbols[current_token].name);
                parse_success = 0;
                break;
            }
        } else {
            // Le sommet est un non-terminal
            int nt_idx = -1;
            for (int i = 0; i < nb_non_terminals; i++) {
                if (non_terminals[i] == top) {
                    nt_idx = i;
                    break;
                }
            }
            
            int t_idx = -1;
            for (int t = 0; t < nb_terminals; t++) {
                if (terminals[t] == current_token) {
                    t_idx = t;
                    break;
                }
            }
            
            if (nt_idx != -1 && t_idx != -1 && table[nt_idx][t_idx] != -1) {
                int rule_num = table[nt_idx][t_idx];
                
                // Afficher la règle appliquée
                printf("Appliquer R%d: %s -> ", rule_num, symbols[rules[rule_num].lhs].name);
                if (rules[rule_num].rhs_len == 0) {
                    printf("ε");
                } else {
                    for (int i = 0; i < rules[rule_num].rhs_len; i++) {
                        printf("%s", symbols[rules[rule_num].rhs[i]].name);
                        if (i < rules[rule_num].rhs_len - 1) printf(" ");
                    }
                }
                printf("\n");
                
                // Dépiler le non-terminal
                stack_ptr--;
                
                // Empiler les symboles de la partie droite (en ordre inverse)
                for (int i = rules[rule_num].rhs_len - 1; i >= 0; i--) {
                    if (stack_ptr < MAX_STACK) {
                        stack[stack_ptr++] = rules[rule_num].rhs[i];
                    }
                }
            } else {
                printf("ERREUR: pas de règle pour [%s, %s]\n", 
                       symbols[top].name, symbols[current_token].name);
                parse_success = 0;
                break;
            }
        }
        step++;
        
        // Limiter l'affichage pour éviter trop de lignes
        if (step > 100) {
            printf("... (analyse tronquée après 100 étapes)\n");
            parse_success = 0;
            break;
        }
    }
    
    printf("\n========================================\n");
    if (parse_success && current_token == eof_token) {
        printf("SUCCÈS: Programme accepté!\n");
    } else {
        printf("ERREUR: Analyse syntaxique échouée\n");
    }
    printf("========================================\n\n");
    
    fclose(yyin);
}

int main(int argc, char** argv) {
    printf("\n");
    printf("******************************************\n");
    printf("*   ANALYSEUR SYNTAXIQUE MANUEL LL(1)    *\n");
    printf("*            MATHLANG                    *\n");
    printf("******************************************\n");
    
    define_grammar();
    print_symbols();
    init_first_follow();
    compute_FIRST();
    compute_FOLLOW(0);
    print_FIRST();
    print_FOLLOW();
    generate_LL1_table();
    print_LL1_table();
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            parse_file(argv[i]);
        }
    } else {
        printf("\n\nUtilisation: %s <fichier1.ml> [fichier2.ml] ...\n", argv[0]);
    }
    
    printf("\n\n");
    return 0;
}