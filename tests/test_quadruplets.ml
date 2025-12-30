# =====================================================
# TEST GÉNÉRATION DE QUADRUPLETS - MATHLANG
# =====================================================

# --- Test 1: Affectations simples ---
SOIT x dans Z tel que x <- 5
SOIT y dans Z tel que y <- 10

# --- Test 2: Addition ---
SOIT z dans Z tel que z <- x + y

# --- Test 3: Soustraction ---
SOIT difference dans Z tel que difference <- x - y

# --- Test 4: Multiplication ---
SOIT prod dans Z tel que prod <- x * y

# --- Test 5: Division réelle ---
SOIT quot_reel dans R tel que quot_reel <- x / y

# --- Test 9: Expression complexe ---
SOIT resultat dans Z tel que resultat <- x + y * 2

# --- Test 10: Expression avec parenthèses ---
SOIT res2 dans Z tel que res2 <- (x + y) * 2

# --- Test 11: Négation ---
SOIT negatif dans Z tel que negatif <- -42

# --- Test 12: sin ---
SOIT s dans R tel que s <- sin(1.57)

# --- Test 13: cos ---
SOIT c dans R tel que c <- cos(0.0)

# --- Test 14: sqrt ---
SOIT racine dans R tel que racine <- sqrt(16.0)

# --- Test 15: exp ---
SOIT expo dans R tel que expo <- exp(1.0)

# --- Test 16: log ---
SOIT logarithme dans R tel que logarithme <- log(2.718)

# --- Test 17: abs ---
SOIT absolu dans R tel que absolu <- abs(-5.5)

# --- Test 18: floor ---
SOIT plancher dans Z tel que plancher <- floor(3.7)

# --- Test 19: ceil ---
SOIT plafond dans Z tel que plafond <- ceil(3.2)

# --- Test 20: round ---
SOIT arrondi dans Z tel que arrondi <- round(3.5)

# --- Test 21: Comparaison < ---
SOIT comp1 dans B tel que comp1 <- x < y

# --- Test 22: Comparaison > ---
SOIT comp2 dans B tel que comp2 <- x > y

# --- Test 23: Comparaison <= ---
SOIT comp3 dans B tel que comp3 <- x <= y

# --- Test 24: Comparaison >= ---
SOIT comp4 dans B tel que comp4 <- x >= y

# --- Test 25: Égalité = ---
SOIT comp5 dans B tel que comp5 <- x = y

# --- Test 26: Différent != ---
SOIT comp6 dans B tel que comp6 <- x != y

# --- Test 27: Booléens vrai/faux ---
SOIT b1 dans B tel que b1 <- vrai
SOIT b2 dans B tel que b2 <- faux

# --- Test 28: ET logique ---
SOIT et_log dans B tel que et_log <- b1 et b2

# --- Test 29: OU logique ---
SOIT ou_log dans B tel que ou_log <- b1 ou b2

# --- Test 30: NON logique ---
SOIT non_log dans B tel que non_log <- non b1

# --- Test 31: XOR logique ---
SOIT xor_log dans B tel que xor_log <- b1 xor b2

# --- Test 32: Expression complexe avec plusieurs opérations ---
SOIT complexe dans R tel que complexe <- (x + y) * (x - y) / 2.0

# --- Test 33: Constante ---
SOIT_CONST PI dans R tel que PI <- 3.14159

# --- Test 34: Expression avec constante ---
SOIT aire dans R tel que aire <- PI * 5.0 * 5.0

# =====================================================
# QUADRUPLETS ATTENDUS
# =====================================================

# Test 1:
0: ( :=  , 5    , -   , x )
1: ( :=  , 10   , -   , y )

# Test 2:
2: ( +   , x    , y   , T0 )
3: ( :=  , T0   , -   , z )

# Test 3:
4: ( -   , x    , y   , T1 )
5: ( :=  , T1   , -   , difference )

# Test 4:
6: ( *   , x    , y   , T2 )
7: ( :=  , T2   , -   , prod )

# Test 5:
8: ( /   , x    , y   , T3 )
9: ( :=  , T3   , -   , quot_reel )


# Test 9:
16: ( *   , y    , 2   , T7 )
17: ( +   , x    , T7  , T8 )
18: ( :=  , T8   , -   , resultat )

# Test 10:
19: ( +   , x    , y   , T9 )
20: ( *   , T9   , 2   , T10 )
21: ( :=  , T10  , -   , res2 )

# Test 11:
22: ( NEG , 42   , -   , T11 )
23: ( :=  , T11  , -   , negatif )

# Test 12:
24: ( SIN , 1.57 , -   , T12 )
25: ( :=  , T12  , -   , s )

# Test 13:
26: ( COS , 0.0  , -   , T13 )
27: ( :=  , T13  , -   , c )

# Test 14:
28: ( SQRT, 16.0 , -   , T14 )
29: ( :=  , T14  , -   , racine )

# Test 15:
30: ( EXP , 1.0  , -   , T15 )
31: ( :=  , T15  , -   , expo )

# Test 16:
32: ( LOG , 2.718, -   , T16 )
33: ( :=  , T16  , -   , logarithme )

# Test 17:
34: ( ABS , -5.5 , -   , T17 )
35: ( :=  , T17  , -   , absolu )

# Test 18:
36: ( FLOOR, 3.7 , -   , T18 )
37: ( :=  , T18  , -   , plancher )

# Test 19:
38: ( CEIL, 3.2  , -   , T19 )
39: ( :=  , T19  , -   , plafond )

# Test 20:
40: ( ROUND, 3.5 , -   , T20 )
41: ( :=  , T20  , -   , arrondi )

# Test 21:
42: ( <   , x    , y   , T21 )
43: ( :=  , T21  , -   , comp1 )

# Test 22:
44: ( >   , x    , y   , T22 )
45: ( :=  , T22  , -   , comp2 )

# Test 23:
46: ( <=  , x    , y   , T23 )
47: ( :=  , T23  , -   , comp3 )

# Test 24:
48: ( >=  , x    , y   , T24 )
49: ( :=  , T24  , -   , comp4 )

# Test 25:
50: ( =   , x    , y   , T25 )
51: ( :=  , T25  , -   , comp5 )

# Test 26:
52: ( !=  , x    , y   , T26 )
53: ( :=  , T26  , -   , comp6 )

# Test 27:
54: ( :=  , 1    , -   , b1 )
55: ( :=  , 0    , -   , b2 )

# Test 28:
56: ( AND , b1   , b2  , T27 )
57: ( :=  , T27  , -   , et_log )

# Test 29:
58: ( OR  , b1   , b2  , T28 )
59: ( :=  , T28  , -   , ou_log )

# Test 30:
60: ( NOT , b1   , -   , T29 )
61: ( :=  , T29  , -   , non_log )

# Test 31:
62: ( XOR , b1   , b2  , T30 )
63: ( :=  , T30  , -   , xor_log )

# Test 32:
64: ( +   , x    , y   , T31 )
65: ( -   , x    , y   , T32 )
66: ( *   , T31  , T32 , T33 )
67: ( /   , T33  , 2.0 , T34 )
68: ( :=  , T34  , -   , complexe )

# Test 33:
69: ( :=  , 3.14159, - , PI )

# Test 34:
70: ( *   , PI   , 5.0 , T35 )
71: ( *   , T35  , 5.0 , T36 )
72: ( :=  , T36  , -   , aire )