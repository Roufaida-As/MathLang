%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int line_num;
extern int col_num;
extern FILE *yyin;

void yyerror(const char *s);
const char* token_name(int tok);
%}

/* ===================== */
/* UNION                 */
/* ===================== */
%union {
    int intval;
    double floatval;
    char charval;
    char* strval;
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

programme
    : /* vide */
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

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", argv[1]);
            return 1;
        }
    }

    printf("========================================\n");
    printf("        ANALYSE SYNTAXIQUE (BISON)\n");
    printf("========================================\n\n");

    printf("%-6s %-6s %-22s %s\n",
           "Ligne", "Col", "Token", "Valeur");
    printf("------------------------------------------------------------\n");

    while ((tok = yylex()) != 0) {
        printf("%-6d %-6d %-22s ",
               line_num,
               col_num,
               token_name(tok));

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

            case TOK_TRUE:
                printf("true");
                break;

            case TOK_FALSE:
                printf("false");
                break;

            default:
                printf("-");
        }

        printf("\n");
    }

    printf("\n========================================\n");
    printf("        FIN DE L'ANALYSE SYNTAXIQUE\n");
    printf("========================================\n");

    if (argc > 1) fclose(yyin);
    return 0;
}