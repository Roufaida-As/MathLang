# =====================================================
# TEST LONG — MathLang
# =====================================================

# ---------------------
# Déclarations simples
# ---------------------

SOIT x dans Z
SOIT y dans Z tel que y <- 10
SOIT z dans R tel que z <- 2.5
SOIT flag dans B tel que flag <- vrai

SOIT_CONST MAX dans Z tel que MAX <- 100


# ---------------------
# Expressions arithmétiques
# ---------------------

SOIT a dans Z tel que a <- 3 + 5 * 2
SOIT b dans Z tel que b <- (3 + 5) * 2
SOIT c dans Z tel que c <- -b + a
SOIT d dans Z tel que d <- c / 2


# ---------------------
# Condition SI / SINON
# ---------------------

SI a > b ALORS
    AFFICHER("a est plus grand que b")
SINON
    AFFICHER("a est inferieur ou egal a b")
FIN


# ---------------------
# Boucle TANT QUE
# ---------------------

SOIT i dans Z tel que i <- 0

TANT QUE i < 5 FAIRE
    AFFICHER("i =", i)
    i <- i + 1
FIN


# ---------------------
# Boucles POUR
# ---------------------

POUR j DE 1 A 5 FAIRE
    AFFICHER(j)
FIN


POUR k DE 0 A 10 PAR 2 FAIRE
    AFFICHER_LIGNE(k)
FIN


# ---------------------
# Lecture et affichage
# ---------------------

LIRE(x)
AFFICHER("Valeur entree pour x :", x)


# ---------------------
# Combinaisons logiques
# ---------------------

SI (x > 0) et (x < MAX) ALORS
    AFFICHER("x est dans l'intervalle")
SINON
    AFFICHER("x est hors intervalle")
FIN


# ---------------------
# Calculs imbriques
# ---------------------

SOIT res dans Z tel que res <- (a + b) * (c - d) + 10 / 2
AFFICHER("Resultat =", res)


# ---------------------
# Fin du test
# ---------------------

AFFICHER("Fin du test long")