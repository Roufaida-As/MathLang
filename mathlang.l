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


/* ============================================ */
/* SECTION PERSONNE C                          */
/* Littéraux, Identificateurs, Gestion Erreurs */
/* ============================================ */
/* 
 * TODO PERSONNE C : Ajouter vos règles ici
 * 
 * ORDRE CRUCIAL pour les nombres :
 *    1. Complexes (2+3i, 5i)
 *    2. Réels avec notation scientifique (2.5e-3)
 *    3. Réels simples (3.14)
 *    4. Entiers (42)
 * 
 * Sinon "2+3i" sera reconnu comme "2", "+", "3", "i" !
 */

/* === NOMBRES COMPLEXES === */
/* === NOMBRES RÉELS === */
/* === NOMBRES ENTIERS === */
/* === CHAÎNES DE CARACTÈRES === */
/* === CARACTÈRES === */
/* === IDENTIFICATEURS === */
/* === COMMENTAIRES === */
/* === ESPACES BLANCS === */
/* === NOUVELLES LIGNES === */
/* === GESTION DES ERREURS === */




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