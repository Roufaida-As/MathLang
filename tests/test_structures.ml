# Test des structures de contrôle

SOIT x dans Z tel que x <- 5

# Test IF simple
SI x > 0 ALORS
    AFFICHER("positif")
FIN

# Test IF-ELSE
SI x < 0 ALORS
    AFFICHER("negatif")
SINON
    AFFICHER("positif ou nul")
FIN

# Test WHILE
TANT QUE x > 0 FAIRE
    x <- x - 1
FIN

# Test FOR
POUR i DE 0 A 10 FAIRE
    AFFICHER(i)
FIN
