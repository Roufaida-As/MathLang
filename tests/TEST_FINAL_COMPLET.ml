# TEST COMPLET

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

# --- Mathematical functions ---
SOIT s1 dans R tel que s1 <- sin(x)
SOIT s2 dans R tel que s2 <- cos(x)
SOIT s3 dans R tel que s3 <- exp(y)
SOIT s4 dans R tel que s4 <- log(10)
SOIT s5 dans R tel que s5 <- sqrt(16)
SOIT s6 dans R tel que s6 <- abs(-5)
SOIT s7 dans R tel que s7 <- floor(3.7)
SOIT s8 dans R tel que s8 <- ceil(3.1)
SOIT s9 dans R tel que s9 <- round(2.5)

# --- End of test ---
AFFICHER_LIGNE("All token tests completed")