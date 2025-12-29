# =====================================================
# TEST FONCTIONS MATHÉMATIQUES — Cas corrects
# =====================================================

# Variables de test
SOIT angle dans R tel que angle <- 1.57

# Fonctions trigonométriques: prennent R → retournent R
SOIT sin_result dans R tel que sin_result <- sin(angle)
SOIT cos_result dans R tel que cos_result <- cos(angle)

# Exponentielle: R → R
SOIT exp_result dans R tel que exp_result <- exp(2.0)

# Logarithme: R+ → R
SOIT log_result dans R tel que log_result <- log(10.0)

# Racine carrée: R+ → R (normal) ou C (si négatif)
SOIT sqrt_pos dans R tel que sqrt_pos <- sqrt(25.0)
SOIT sqrt_neg dans C tel que sqrt_neg <- sqrt(-1.0)

# Valeur absolue: préserve le type
SOIT abs_z dans Z tel que abs_z <- abs(-42)
SOIT abs_r dans R tel que abs_r <- abs(-3.14)

# Floor, Ceil, Round: R → Z
SOIT floor_val dans Z tel que floor_val <- floor(3.7)
SOIT ceil_val dans Z tel que ceil_val <- ceil(3.2)
SOIT round_val dans Z tel que round_val <- round(3.5)

# Parties réelle et imaginaire: C → R
SOIT complexe dans C tel que complexe <- 3.0 + 4.0
SOIT re_part dans R tel que re_part <- re(complexe)
SOIT im_part dans R tel que im_part <- im(complexe)

# Argument (phase): C → R
SOIT phase dans R tel que phase <- arg(complexe)

# =====================================================
# OPÉRATIONS SUR CHAÎNES — Cas corrects
# =====================================================

# Déclarations de chaînes
SOIT greeting dans Sigma tel que greeting <- "Bonjour"
SOIT space dans Sigma tel que space <- " "
SOIT name dans Sigma tel que name <- "Monde"

# Concaténation
SOIT message dans Sigma tel que message <- greeting + space + name

# Transformation de casse
SOIT upper_msg dans Sigma tel que upper_msg <- majuscules(message)
SOIT lower_msg dans Sigma tel que lower_msg <- minuscules(message)

# =====================================================
# VÉRIFICATIONS DE TYPES
# =====================================================

# Promotions numériques correctes: Z → R → C
SOIT entier dans Z tel que entier <- 5
SOIT reel1 dans R tel que reel1 <- entier + 2.5      # Z + R → R
SOIT complexe_ok dans C tel que complexe_ok <- entier + (3.0 + 4.0)  # Z + C → C

# Pas d'erreurs de division (diviseur pas zéro)
SOIT safe_div dans R tel que safe_div <- 100.0 / 5.0
