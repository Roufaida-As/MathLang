%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int line_num;
extern int col_num;

void yyerror(const char *s);
%}

/* ===================== */
/* TYPES SÉMANTIQUES     */
/* ===================== */
%union {
    int intval;
    double floatval;
    char charval;
    char* strval;
}

/* ===================== */
/* TOKENS AVEC TYPES     */
/* ===================== */
%token <intval>   TOK_INT
%token <floatval> TOK_FLOAT
%token <charval>  TOK_CHAR
%token <strval>   TOK_STRING TOK_COMPLEX TOK_ID

/* ===================== */
/* TOKENS SIMPLES        */
/* ===================== */
%token TOK_SOIT TOK_SOIT_CONST TOK_IN TOK_TEL_QUE TOK_TYPE TOK_ENREGISTREMENT
%token TOK_SI TOK_ALORS TOK_SINON TOK_FIN
%token TOK_TANT TOK_QUE TOK_FAIRE
%token TOK_POUR TOK_DE TOK_PAR TOK_A TOK_REPETER TOK_JUSQUA
%token TOK_SORTIR TOK_CONTINUER
%token TOK_AFFICHER TOK_AFFICHER_LIGNE TOK_LIRE
%token TOK_FONCTION TOK_PROCEDURE TOK_RETOURNER TOK_LAMBDA
%token TOK_TRUE TOK_FALSE
%token TOK_AND TOK_OR TOK_NOT TOK_XOR

%token TOK_TYPE_Z TOK_TYPE_R TOK_TYPE_B TOK_TYPE_C TOK_TYPE_SIGMA TOK_TYPE_CHAR
%token TOK_INT8 TOK_INT16 TOK_INT32 TOK_INT64 TOK_FLOAT_TYPE TOK_DOUBLE_TYPE

%token TOK_SIN TOK_COS TOK_EXP TOK_LOG TOK_SQRT TOK_ABS TOK_FLOOR TOK_CEIL TOK_ROUND
%token TOK_INT_FUNC TOK_REEL TOK_BOOLEEN TOK_CHAINE
%token TOK_RE TOK_IM TOK_ARG
%token TOK_MAJUSCULES TOK_MINUSCULES TOK_DIVISER
%token TOK_DET TOK_INV TOK_TRACE TOK_IDENTITE

%token TOK_ASSIGN TOK_ARROW TOK_IMPLIES TOK_EQUIV
%token TOK_PLUS TOK_MINUS TOK_MULT TOK_DIV_REAL TOK_DIV TOK_MOD TOK_POWER
%token TOK_EQ TOK_NEQ TOK_LT TOK_GT TOK_LEQ TOK_GEQ

%token TOK_UNION TOK_INTER TOK_DIFF TOK_SYMDIFF
%token TOK_NOT_IN TOK_SUBSET TOK_STRICT_SUBSET

%token TOK_LPAREN TOK_RPAREN TOK_LBRACKET TOK_RBRACKET
%token TOK_LBRACE TOK_RBRACE
%token TOK_COMMA TOK_COLON TOK_DOT TOK_PIPE TOK_SEMICOLON

/* ===================== */
/* PRIORITÉS             */
/* ===================== */
%right TOK_ASSIGN
%left TOK_EQUIV
%right TOK_IMPLIES
%left TOK_OR TOK_XOR
%left TOK_AND
%right TOK_NOT
%left TOK_EQ TOK_NEQ
%left TOK_LT TOK_GT TOK_LEQ TOK_GEQ
%left TOK_IN TOK_NOT_IN TOK_SUBSET TOK_STRICT_SUBSET
%left TOK_UNION TOK_DIFF TOK_SYMDIFF
%left TOK_INTER
%left TOK_PLUS TOK_MINUS
%left TOK_MULT TOK_DIV_REAL TOK_DIV TOK_MOD
%right TOK_POWER
%right UMINUS
%right TOK_ARROW
%left TOK_DOT TOK_LBRACKET TOK_LPAREN

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
    : TOK_SOIT TOK_ID TOK_IN type
    | TOK_SOIT TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression
    ;

declaration_constante
    : TOK_SOIT_CONST TOK_ID TOK_IN type TOK_TEL_QUE TOK_ID TOK_ASSIGN expression
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
    : type_arrow
    ;

type_arrow
    : type_base
    | type_base TOK_ARROW type_arrow
    ;

type_base
    : TOK_TYPE_Z | TOK_TYPE_R | TOK_TYPE_B | TOK_TYPE_C
    | TOK_TYPE_SIGMA | TOK_TYPE_CHAR
    | TOK_INT8 | TOK_INT16 | TOK_INT32 | TOK_INT64
    | TOK_FLOAT_TYPE | TOK_DOUBLE_TYPE
    | TOK_ID
    | TOK_LPAREN type TOK_RPAREN
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
    : TOK_ID TOK_ASSIGN expression
    ;

/* ===================== */
/* STRUCTURES            */
/* ===================== */
instruction_si
    : TOK_SI expression TOK_ALORS bloc TOK_FIN
    | TOK_SI expression TOK_ALORS bloc TOK_SINON bloc TOK_FIN
    ;

instruction_tant_que
    : TOK_TANT TOK_QUE expression TOK_FAIRE bloc TOK_FIN
    ;

instruction_pour
    : TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_FAIRE bloc TOK_FIN
    | TOK_POUR TOK_ID TOK_DE expression TOK_A expression TOK_PAR expression TOK_FAIRE bloc TOK_FIN
    | TOK_POUR TOK_ID TOK_IN expression TOK_FAIRE bloc TOK_FIN
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
    | TOK_LIRE TOK_LPAREN TOK_ID TOK_RPAREN
    ;

liste_expressions
    : expression
    | liste_expressions TOK_COMMA expression
    ;

/* ===================== */
/* EXPRESSIONS           */
/* ===================== */
expression
    : expression TOK_PLUS expression
    | expression TOK_MINUS expression
    | expression TOK_MULT expression
    | expression TOK_DIV_REAL expression
    | expression TOK_EQ expression
    | expression TOK_LT expression
    | expression TOK_GT expression
    | expression TOK_AND expression
    | expression TOK_OR expression
    | expression TOK_XOR expression
    | TOK_MINUS expression %prec UMINUS
    | TOK_NOT expression
    | primaire
    ;

primaire
    : TOK_INT
    | TOK_FLOAT
    | TOK_STRING
    | TOK_CHAR
    | TOK_COMPLEX
    | TOK_TRUE
    | TOK_FALSE
    | TOK_ID
    | TOK_LPAREN expression TOK_RPAREN
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr,
            "Erreur syntaxique ligne %d colonne %d : %s\n",
            line_num, col_num, s);
}

int main(int argc, char **argv) {
    extern FILE *yyin;

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr,
                    "Erreur: impossible d'ouvrir le fichier %s\n", argv[1]);
            return 1;
        }
    }

    printf("=== Début de l'analyse syntaxique ===\n");
    int result = yyparse();

    if (result == 0)
        printf("=== Analyse syntaxique réussie ! ===\n");
    else
        printf("=== Analyse syntaxique échouée ===\n");

    if (argc > 1) fclose(yyin);
    return result;
}