#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mathlang.tab.h"  // Using Bison's token definitions instead of tokens.h
YYSTYPE yylval; 

/* Variables globales */
int current_token;
extern int yylex();
extern char* yytext;
extern int line_num;
extern FILE* yyin;

/* methods */
void error(const char* msg);
void match(int expected);
void programme();
void declarations();
void declaration();
void type();
void instructions();
void instruction();
void expr();
void expr_prime();
void term();
void term_prime();
void factor();

int indent = 0;

/* Fonction d'erreur */
void error(const char* msg) {
    fprintf(stderr, "\n ERREUR SYNTAXIQUE ligne %d: %s\n", line_num, msg);
    fprintf(stderr, "   Token trouvé: '%s'\n", yytext);
    exit(1);
}

/* Fonction match pour voir le token courant*/
void match(int expected) {
    if (current_token == expected) {
        printf("%*s %s\n", indent * 2, "", yytext);
        current_token = yylex(); // on le consomme et on lit le suivant 
    } else {
        char msg[100];
        sprintf(msg, "Attendu token %d, trouvé %d", expected, current_token);
        error(msg);
    }
}

/* <Programme> ::= <Declarations> <Instructions> */
void programme() {
    printf(" <Programme> \n");
    indent++;
    declarations();
    instructions();
    indent--;
    printf("Fin Programme \n");
}

/* <Declarations> ::= <Declaration> <Declarations> | ε */
void declarations() {
    printf("%*s<Declarations>\n", indent * 2, "");
    indent++;
    
    while (current_token == TOK_SOIT) {
        declaration();
    }
    
    indent--;
}

/* <Declaration> ::= SOIT ID DANS <Type> */
void declaration() {
    printf("%*s<Declaration>\n", indent * 2, "");
    indent++;  
    match(TOK_SOIT);
    
    if (current_token != TOK_ID) {
        error("Identificateur attendu après SOIT");
    }
    match(TOK_ID);
    
    match(TOK_IN);  // DANS
    type();
    
    indent--;
}

/* <Type> ::= Z | R | B */
void type() {
    printf("%*s<Type>\n", indent * 2, "");
    indent++;
    
    if (current_token == TOK_TYPE_Z) {
        match(TOK_TYPE_Z);
    } else if (current_token == TOK_TYPE_R) {
        match(TOK_TYPE_R);
    } else if (current_token == TOK_TYPE_B) {
        match(TOK_TYPE_B);
    } else {
        error("Type attendu (Z, R ou B)");
    }
    
    indent--;
}

/* <Instructions> ::= <Instruction> <Instructions> | ε */
void instructions() {
    printf("%*s<Instructions>\n", indent * 2, "");
    indent++;
    
    while (current_token == TOK_ID || current_token == TOK_AFFICHER) {
        instruction();
    }
    
    indent--;
}

/* <Instruction> ::= ID ASSIGN <Expr> | AFFICHER LPAREN <Expr> RPAREN */
void instruction() {
    printf("%*s<Instruction>\n", indent * 2, "");
    indent++;
    
    if (current_token == TOK_ID) {
        match(TOK_ID);
        match(TOK_ASSIGN);
        expr();
    } else if (current_token == TOK_AFFICHER) {
        match(TOK_AFFICHER);
        match(TOK_LPAREN);
        expr();
        match(TOK_RPAREN);
    } else {
        error("Instruction attendue (ID ou AFFICHER)");
    }
    
    indent--;
}

/* <Expr> ::= <Term> <Expr'> */
void expr() {
    printf("%*s<Expr>\n", indent * 2, "");
    indent++;
    
    term();
    expr_prime();
    
    indent--;
}

/* <Expr'> ::= PLUS <Term> <Expr'> | MINUS <Term> <Expr'> | ε */
void expr_prime() {
    if (current_token == TOK_PLUS) {
        match(TOK_PLUS);
        term();
        expr_prime();
    } else if (current_token == TOK_MINUS) {
        match(TOK_MINUS);
        term();
        expr_prime();
    }
    /* Sinon ε */
}

/* <Term> ::= <Factor> <Term'> */
void term() {
    printf("%*s<Term>\n", indent * 2, "");
    indent++;
    
    factor();
    term_prime();
    
    indent--;
}

/* <Term'> ::= MULT <Factor> <Term'> | ε */
void term_prime() {
    if (current_token == TOK_MULT) {
        match(TOK_MULT);
        factor();
        term_prime();
    }
    /* Sinon ε */
}

/* <Factor> ::= INT | FLOAT | ID | LPAREN <Expr> RPAREN */
void factor() {
    printf("%*s<Factor>\n", indent * 2, "");
    indent++;
    
    if (current_token == TOK_INT) {
        match(TOK_INT);
    } else if (current_token == TOK_FLOAT) {
        match(TOK_FLOAT);
    } else if (current_token == TOK_ID) {
        match(TOK_ID);
    } else if (current_token == TOK_LPAREN) {
        match(TOK_LPAREN);
        expr();
        match(TOK_RPAREN);
    } else {
        error("Facteur attendu (INT, FLOAT, ID ou expression)");
    }
    
    indent--;
}

/* Main */
int main(int argc, char** argv) {
    printf(" ANALYSEUR SYNTAXIQUE \n");
    
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", argv[1]);
            return 1;
        }
        printf("Fichier: %s\n\n", argv[1]);
    }
    
    current_token = yylex();
    programme();
    
    if (current_token != 0) {
        error("Tokens après la fin du programme");
    }
    
    printf("\n ANALYSE SYNTAXIQUE RÉUSSIE !\n\n");
    
    if (yyin && yyin != stdin) fclose(yyin);
    return 0;
}