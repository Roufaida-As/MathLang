# ============================================
# TEST COMPLET - MATHLANG LEXER
# Sections A + B + C
# ============================================

# === SECTION A : MOTS-CLÉS ET DÉCLARATIONS ===

SOIT x dans Z tel que x <- 5
SOIT y dans R tel que y <- 3.14159
SOIT flag dans B tel que flag <- vrai
SOIT nom dans Sigma tel que nom <- "Alice"
SOIT lettre dans Char tel que lettre <- 'A'

SOIT_CONST PI dans R tel que PI <- 3.14159265359

TYPE Point = ENREGISTREMENT
    x : R
    y : R
FIN

# === STRUCTURES DE CONTRÔLE ===

SI x > 0 ALORS
    AFFICHER("x est positif")
SINON SI x < 0 ALORS
    AFFICHER("x est negatif")
SINON
    AFFICHER("x est nul")
FIN

# === BOUCLES ===

POUR i DE 0 A 10 PAR 2 FAIRE
    AFFICHER(i)
FIN

TANT QUE x > 0 FAIRE
    x <- x - 1
FIN
SOIT n dans Z 
REPETER
    AFFICHER("Entrez un nombre positif")
    LIRE(n)
JUSQUA n > 0

# === CONTRÔLE DE FLUX ===

POUR i DE 0 A 100 FAIRE
    SI i = 50 ALORS
        SORTIR
    FIN
    SI i mod 2 = 0 ALORS
        CONTINUER
    FIN
    AFFICHER(i)
FIN