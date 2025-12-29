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

# =====================================================
# NOUVELLES VÉRIFICATIONS — Fonctions mathématiques
# =====================================================

# 10) Division par zéro
SOIT x dans R tel que x <- 10 / 0  # ERREUR à la compilation

# 11) SQRT de négatif (doit retourner C)
SOIT c dans C tel que c <- sqrt(-9)  # OK mais avertissement

# 12) LOG(0) indéfini
SOIT r1 dans R tel que r1 <- log(0)  # ERREUR

# 13) LOG(-5) indéfini
SOIT r2 dans R tel que r2 <- log(-5)  # ERREUR

# 14) ARG sur non-complexe
SOIT phase dans R tel que phase <- arg(3.14)  # ERREUR, arg() demande C


# 16) SIN, COS demandent nombres
SOIT angle dans R tel que angle <- sin(1.57)  # OK

# =====================================================
# OPÉRATIONS SUR CHAÎNES
# =====================================================

# 17) Concaténation de chaînes
SOIT str1 dans Sigma tel que str1 <- "Bonjour"
SOIT str2 dans Sigma tel que str2 <- "Monde"
SOIT result dans Sigma tel que result <- str1 + str2  # OK concaténation

# 18) MAJUSCULES sur chaîne
SOIT text dans Sigma tel que text <- "hello"
SOIT upper dans Sigma tel que upper <- majuscules(text)  # OK

# 19) MINUSCULES sur chaîne
SOIT lower dans Sigma tel que lower <- minuscules(upper)  # OK

# 20) MAJUSCULES sur non-chaîne (devrait être erreur)
SOIT num dans Z tel que num <- 42
SOIT bad dans Sigma tel que bad <- majuscules(num)  # ERREUR type Z != Sigma