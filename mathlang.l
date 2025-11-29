%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

/* Variables globales pour le tracking */
int line_num = 1;
int col_num = 1;

/* Fonction pour mettre à jour la position dans le fichier */
void update_position() {
    col_num += yyleng;
}

/* Fonction pour gérer les nouvelles lignes */
void handle_newline() {
    line_num++;
    col_num = 1;
}
%}

/* ============================================ */
/* OPTIONS FLEX                                */
/* ============================================ */
%option noyywrap
%option yylineno

/* ============================================ */
/* DÉFINITIONS DE PATTERNS RÉUTILISABLES       */
/* ============================================ */
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID_START    [a-zA-Z_]
ID_CHAR     [a-zA-Z0-9_]
WHITESPACE  [ \t\r]
NEWLINE     \n



/* ============================================ */
/* SECTION PERSONNE A                          */
/* Mots-clés, Structures de Contrôle, Types    */
/* ============================================ */
/* 
 * TODO PERSONNE A : Ajouter vos règles ici
 * 
 * Exemples à implémenter :
 * "SOIT"      { update_position(); return TOK_SOIT; }
 */

/* === DÉCLARATIONS === */
/* === STRUCTURES DE CONTRÔLE === */
/* === CONTRÔLE DE FLUX === */
/* === ENTRÉES/SORTIES === */
/* === FONCTIONS === */
/* === BOOLÉENS === */
/* === OPÉRATEURS LOGIQUES TEXTUELS === */
/* === TYPES DE BASE === */
/* === SOUS-TYPES === */


/* ============================================ */
/* SECTION PERSONNE B                          */
/* Opérateurs et Symboles                      */
/* ============================================ */
/* 
 * TODO PERSONNE B : Ajouter vos règles ici
 * 
 * Exemples à implémenter :
 * "<-"        { update_position(); return TOK_ASSIGN; }
 * "!="        { update_position(); return TOK_NEQ; }
 */

/* === AFFECTATION === */
/* === OPÉRATEURS ARITHMÉTIQUES === */
/* === OPÉRATEURS DE COMPARAISON === */
/* === OPÉRATEURS ENSEMBLISTES === */
/* === OPÉRATEURS D'APPARTENANCE === */
/* === DÉLIMITEURS === */
/* === SYMBOLES SPÉCIAUX === */

/* 3. Littéraux, Identificateurs, Gestion Erreurs */

/* === COMMENTAIRES === */
"#".*                          { update_position(); /* Commentaire ignoré */ }

/* === ESPACES BLANCS === */
{WHITESPACE}+                  { update_position(); /* Ignoré */ }

/* === NOUVELLES LIGNES === */
{NEWLINE}+                     { handle_newline(); }

/* === NOMBRES COMPLEXES === */

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?i   { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}
{DIGIT}+[eE][+-]?{DIGIT}+i                { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}
{DIGIT}+\.([eE][+-]?{DIGIT}+)?i           { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}
\.{DIGIT}+([eE][+-]?{DIGIT}+)?i           { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}
{DIGIT}+i                                  { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

/* === NOMBRES RÉELS === */

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?    { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
{DIGIT}+[eE][+-]?{DIGIT}+                 { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
{DIGIT}+\.([eE][+-]?{DIGIT}+)?            { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
\.{DIGIT}+([eE][+-]?{DIGIT}+)?            { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}

/* === NOMBRES ENTIERS === */

{DIGIT}+                                   { 
    yylval.intval = atoi(yytext); 
    update_position();
    return TOK_INT; 
}

/* === CHAÎNES DE CARACTÈRES === */

\"([^\"\\]|\\.)*\"                         { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_STRING; 
}

/* === CARACTÈRES === */

\'([^\'\\]|\\.)\'                          { 
    /* Gestion des caractères échappés */
    if (yytext[1] == '\\') {
        switch(yytext[2]) {
            case 'n':  yylval.charval = '\n'; break;
            case 't':  yylval.charval = '\t'; break;
            case 'r':  yylval.charval = '\r'; break;
            case '\\': yylval.charval = '\\'; break;
            case '\'': yylval.charval = '\''; break;
            case '0':  yylval.charval = '\0'; break;
            default:   yylval.charval = yytext[2]; break;
        }
    } else {
        yylval.charval = yytext[1];
    }
    update_position();
    return TOK_CHAR; 
}

/* === IDENTIFICATEURS === */

{ID_START}{ID_CHAR}{0,62}                  { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_ID; 
}

/* === GESTION DES ERREURS === */
.                                          { 
    fprintf(stderr, "ERREUR LEXICALE ligne %d, col %d: caractère non reconnu '%s'\n", 
            line_num, col_num, yytext);
    update_position();
    return TOK_ERROR; 
}




/* ============================================ */
/* RÈGLE TEMPORAIRE POUR TESTER                */
/* (À SUPPRIMER une fois les sections remplies) */
/* ============================================ */
%%
"SOIT"      { update_position(); return TOK_SOIT; }
.   { 
    fprintf(stderr, "ERREUR LEXICALE ligne %d, col %d: caractère non reconnu '%s'\n", 
            line_num, col_num, yytext);
    update_position();
    return TOK_ERROR;
}

%%