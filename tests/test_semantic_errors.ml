# =====================================================
# TEST SEMANTIQUE — erreurs intentionnelles
# =====================================================

# 1) Utilisation avant déclaration
AFFICHER(x)

# 2) Déclaration puis réaffectation constante
SOIT_CONST PI dans R tel que PI <- 3.14
PI <- 3.14159  # devrait déclencher erreur de modification constante

# 3) Incompatibilité de types
SOIT n dans Z tel que n <- 42
n <- 2.5       # type R assigné à Z

# 4) Expression booléenne invalide en arithmétique
SOIT b dans B tel que b <- vrai
SOIT k dans Z tel que k <- b + 1  # bool + int

# 5) Condition non booléenne
SI 123 ALORS
    AFFICHER("condition non booléenne")
FIN

# 6) Redéclaration dans la même portée
SOIT y dans Z
SOIT y dans Z  # redéclaration refusée

# 7) Variable de boucle hors portée
POUR i DE 1 A 3 FAIRE
    AFFICHER(i)
FIN
AFFICHER(i)    # i n'est plus déclaré ici

# 8) Lecture d'une variable non déclarée
LIRE(z)        # z n'existe pas

# 9) Affectation d'une chaîne à un entier
SOIT a dans Z
a <- "hello"