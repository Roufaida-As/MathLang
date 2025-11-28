/* ============================================ */
/* MATHLANG LEXER TEST - Programme de Test    */
/* ============================================ */
/* Ce programme teste l'analyseur lexical     */
/* en affichant tous les tokens reconnus      */
/* ============================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

/* Déclarations externes (fournies par Flex) */
extern int yylex();
extern FILE *yyin;
extern char *yytext;
extern int line_num;
extern int col_num;
extern int yyleng; 

/* Variable globale pour stocker les valeurs des tokens */
YYSTYPE yylval;

/* ============================================ */
/* FONCTION : token_name                       */
/* Retourne le nom lisible d'un token          */
/* ============================================ */
const char *token_name(int token)
{
    switch (token)
    {
    /* Déclarations */
    case TOK_SOIT:
        return "SOIT";
    case TOK_SOIT_CONST:
        return "SOIT_CONST";
    case TOK_DANS:
        return "DANS";
    case TOK_TEL_QUE:
        return "TEL_QUE";
    case TOK_TYPE:
        return "TYPE";
    case TOK_ENREGISTREMENT:
        return "ENREGISTREMENT";

    /* Structures de contrôle */
    case TOK_SI:
        return "SI";
    case TOK_ALORS:
        return "ALORS";
    case TOK_SINON:
        return "SINON";
    case TOK_FIN:
        return "FIN";
    case TOK_TANT:
        return "TANT";
    case TOK_QUE:
        return "QUE";
    case TOK_FAIRE:
        return "FAIRE";
    case TOK_POUR:
        return "POUR";
    case TOK_DE:
        return "DE";
    case TOK_A:
        return "A";
    case TOK_PAR:
        return "PAR";
    case TOK_REPETER:
        return "REPETER";
    case TOK_JUSQUA:
        return "JUSQU'A";

    /* Contrôle de flux */
    case TOK_SORTIR:
        return "SORTIR";
    case TOK_CONTINUER:
        return "CONTINUER";

    /* I/O */
    case TOK_AFFICHER:
        return "AFFICHER";
    case TOK_AFFICHER_LIGNE:
        return "AFFICHER_LIGNE";
    case TOK_LIRE:
        return "LIRE";

    /* Fonctions */
    case TOK_FONCTION:
        return "FONCTION";
    case TOK_PROCEDURE:
        return "PROCEDURE";
    case TOK_RETOURNER:
        return "RETOURNER";
    case TOK_LAMBDA:
        return "LAMBDA";

    /* Booléens */
    case TOK_TRUE:
        return "TRUE";
    case TOK_FALSE:
        return "FALSE";

    /* Opérateurs logiques */
    case TOK_AND:
        return "AND";
    case TOK_OR:
        return "OR";
    case TOK_NOT:
        return "NOT";
    case TOK_XOR:
        return "XOR";

    /* Types de base */
    case TOK_TYPE_Z:
        return "TYPE_Z";
    case TOK_TYPE_R:
        return "TYPE_R";
    case TOK_TYPE_B:
        return "TYPE_B";
    case TOK_TYPE_C:
        return "TYPE_C";
    case TOK_TYPE_SIGMA:
        return "TYPE_SIGMA";
    case TOK_TYPE_CHAR:
        return "TYPE_CHAR";

    /* Sous-types */
    case TOK_INT8:
        return "INT8";
    case TOK_INT16:
        return "INT16";
    case TOK_INT32:
        return "INT32";
    case TOK_INT64:
        return "INT64";
    case TOK_FLOAT_TYPE:
        return "FLOAT_TYPE";
    case TOK_DOUBLE_TYPE:
        return "DOUBLE_TYPE";

    /* Affectation */
    case TOK_ASSIGN:
        return "ASSIGN";

    /* Opérateurs arithmétiques */
    case TOK_PLUS:
        return "PLUS";
    case TOK_MINUS:
        return "MINUS";
    case TOK_MULT:
        return "MULT";
    case TOK_DIV_REAL:
        return "DIV_REAL";
    case TOK_DIV:
        return "DIV";
    case TOK_MOD:
        return "MOD";
    case TOK_POWER:
        return "POWER";

    /* Comparaison */
    case TOK_EQ:
        return "EQ";
    case TOK_NEQ:
        return "NEQ";
    case TOK_LT:
        return "LT";
    case TOK_GT:
        return "GT";
    case TOK_LEQ:
        return "LEQ";
    case TOK_GEQ:
        return "GEQ";

    /* Ensembles */
    case TOK_UNION:
        return "UNION";
    case TOK_INTER:
        return "INTER";
    case TOK_DIFF:
        return "DIFF";
    case TOK_SYMDIFF:
        return "SYMDIFF";

    /* Appartenance */
    case TOK_IN:
        return "IN";
    case TOK_NOT_IN:
        return "NOT_IN";
    case TOK_SUBSET:
        return "SUBSET";
    case TOK_STRICT_SUBSET:
        return "STRICT_SUBSET";

    /* Délimiteurs */
    case TOK_LPAREN:
        return "LPAREN";
    case TOK_RPAREN:
        return "RPAREN";
    case TOK_LBRACKET:
        return "LBRACKET";
    case TOK_RBRACKET:
        return "RBRACKET";
    case TOK_LBRACE:
        return "LBRACE";
    case TOK_RBRACE:
        return "RBRACE";
    case TOK_COMMA:
        return "COMMA";
    case TOK_COLON:
        return "COLON";
    case TOK_DOT:
        return "DOT";
    case TOK_PIPE:
        return "PIPE";
    case TOK_SEMICOLON:
        return "SEMICOLON";

    /* Symboles spéciaux */
    case TOK_ARROW:
        return "ARROW";
    case TOK_IMPLIES:
        return "IMPLIES";
    case TOK_EQUIV:
        return "EQUIV";

    /* Littéraux */
    case TOK_INT:
        return "INT";
    case TOK_FLOAT:
        return "FLOAT";
    case TOK_STRING:
        return "STRING";
    case TOK_CHAR:
        return "CHAR";
    case TOK_COMPLEX:
        return "COMPLEX";
    case TOK_ID:
        return "IDENTIFIER";

    /* Fonctions mathématiques */
    case TOK_SIN:
        return "SIN";
    case TOK_COS:
        return "COS";
    case TOK_SQRT:
        return "SQRT";
    case TOK_ABS:
        return "ABS";

    /* Spéciaux */
    case TOK_EOF:
        return "EOF";
    case TOK_ERROR:
        return "ERROR";

    default:
        return "UNKNOWN";
    }
}

/* ============================================ */
/* FONCTION PRINCIPALE                         */
/* ============================================ */
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (!yyin)
        {
            fprintf(stderr, "Cannot open file '%s'\n", argv[1]);
            return 1;
        }
    }
    else
    {
        yyin = stdin;
        printf("Enter MathLang code (Ctrl+D to finish):\n");
    }

    int token;
    while ((token = yylex()) != 0)
    {
        if (token != TOK_ERROR)
        {
            printf("Accepted: %s at line %d, col %d\n",
                   token_name(token),
                   line_num,
                   col_num - yyleng); // <-- adjust column
        }
        else
        {
            printf("Not accepted: '%s' at line %d, col %d\n",
                   yytext,
                   line_num,
                   col_num - yyleng);
        }
    }

    if (yyin != stdin)
        fclose(yyin);
    return 0;
}