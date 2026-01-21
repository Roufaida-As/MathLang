# MathLang - Compilateur pour Langage Mathématique

##  Description

**MathLang** est un compilateur complet pour un langage de programmation orienté vers les mathématiques. Le projet implémente les trois phases principales d'un compilateur :

1. **Analyse Lexicale** - Reconnaissance des tokens
2. **Analyse Syntaxique** - Vérification de la grammaire
3. **Analyse Sémantique** - Vérification des types et génération de code intermédiaire

Le compilateur génère du code intermédiaire sous forme de **quadruplets** pour une exécution ultérieure.

---

##  Fonctionnalités

### Types de Données Supportés
- **Z** - Entiers (int8, int16, int32, int64)
- **R** - Nombres réels (float, double)
- **B** - Booléens (vrai/faux)
- **C** - Nombres complexes
- **Σ (Sigma)** - Chaînes de caractères
- **Char** - Caractères

### Structures de Contrôle
- **IF/ELSE/ELSE-IF** - Conditionnelles avec branchements
- **WHILE** - Boucles indéterminées
- **FOR** - Boucles déterminées (3 variantes)
- **REPEAT/UNTIL** - Boucles post-testées
- **BREAK/CONTINUE** - Contrôle de flux

### Opérations Mathématiques
- Arithmétique : `+`, `-`, `*`, `/`, `div`, `mod`, `^`
- Comparaisons : `=`, `≠`, `<`, `>`, `≤`, `≥`
- Logiques : `et`, `ou`, `xor`, `non`
- Fonctions : `sin`, `cos`, `exp`, `log`, `sqrt`, `abs`, `floor`, `ceil`, `round`
- Complexes : `re` (partie réelle), `im` (partie imaginaire), `arg` (argument)
- Chaînes : `majuscules`, `minuscules`

### I/O
- `AFFICHER(expr)` - Affichage simple
- `AFFICHER_LIGNE(expr)` - Affichage avec nouvelle ligne
- `LIRE(var)` - Lecture depuis l'entrée

### Déclarations
```mathlang
SOIT x dans Z tel que x <- 5
SOIT_CONST PI dans R tel que PI <- 3.14159
```

---

##  Structure du Projet

```
MathLang/
├── mathlang.y                    # Grammaire Bison (syntaxe + sémantique)
├── mathlang.l                    # Lexique Flex
├── symbol_table.h/c              # Gestion de la table des symboles
├── quadruplet.h/c                # Génération de code intermédiaire
├── expr_info.h                   # Structure info expression
├── Analyse_lexicale_manuelle.c   # Analyseur lexical manuel (AFN/AFD)
├── Makefile                      # Compilation du projet
├── tests/                        # Suite de tests
│   ├── test_b_simple.ml
│   ├── test_bc_complet.ml
│   ├── TEST_FINAL_COMPLET.ml
│   └── ...
├── Analyse_syntaxique_man/       # Analyse syntaxique manuelle
│   ├── Analyse_syntaxique_manuelle.c
│   ├── test1.ml - test4.ml
│   └── ...
└── README.md              
```

---

##  Compilation et Exécution

### Prérequis
- GCC ou Clang
- Flex (scanner lexical)
- Bison (analyseur syntaxique)
- Make


---

##  Analyse Détaillée

### Phase 1 : Analyse Lexicale (`mathlang.l`)

Le lexeur reconnaît :
- **Mots-clés** : SOIT, ALORS, POUR, etc.
- **Identificateurs** : [a-zA-Z_][a-zA-Z0-9_]*
- **Littéraux** : entiers, réels, complexes, chaînes, caractères
- **Opérateurs** : arithmétiques, logiques, de comparaison
- **Commentaires** : # ...

### Phase 2 : Analyse Syntaxique (`mathlang.y`)

La grammaire Bison valide :
- Déclarations de variables et constantes
- Types (simples et complexes)
- Expressions (avec priorité correcte)
- Instructions (if, while, for, repeat)
- Appels de fonction

### Phase 3 : Analyse Sémantique

Vérifications effectuées :
- **Symboles** : déclaration, redéclaration, utilisation
- **Types** : compatibilité d'affectation, opérations valides
- **Initialisation** : variables avant utilisation
- **Constantes** : modification interdite

### Code Intermédiaire

Génération de **quadruplets** (opérateur, arg1, arg2, résultat) :

```
   0: ( :=     , 5         , -         , x        )
   1: ( :=     , 3.14      , -         , y        )
   2: ( +      , x         , 1         , T0       )
   3: ( :=     , T0        , -         , x        )
   4: ( BZ     , x         , 0         , 2        )
```

---

##  Table des Symboles

La table des symboles gère :
- **Catégories** : variable, constante, fonction, procédure
- **Types** : Z, R, B, C, Σ, Char
- **Portées** : scope globale et locale
- **Attributs** : initialisé, constant, utilisé

Sortie exemple :
```
=== SYMBOL TABLE (5 symbols) ===
Name                 Category   Type       Scope  Const  Init   Used
────────────────────────────────────────────────────────────────
x                    VAR        Z          0      no     yes    yes
PI                   CONST      R          0      yes    yes    no
nom                  VAR        SIGMA      0      no     yes    yes
```

---

##  Tests

### Fichier de Test

- **TEST_FINAL_COMPLET.ml** - Suite complète

### Exécution des Tests

```bash
./parser tests/TEST_FINAL_COMPLET.ml
```

Sortie complète incluant :
1. Analyse lexicale (tous les tokens)
2. Analyse syntaxique
3. Table des symboles finale
4. Code intermédiaire (quadruplets)


---

##  Gestion des Erreurs

Le compilateur détecte et rapporte :

### Erreurs Lexicales
```
Erreur lexicale à la ligne 1, colonne 5: caractère inconnu '§'
```

### Erreurs Syntaxiques
```
Erreur syntaxique ligne 2 colonne 10 : ...
```

### Erreurs Sémantiques
```
ERREUR [3:15] symbole 'z' non déclaré
ERREUR [4:10] type attendu Z, trouvé R
ERREUR [5:8] tentative de modification de la constante 'PI'
```

---

##  Références Techniques

### Grammaire BNF Simplifiée

```
programme       := liste_declarations
declaration     := declaration_variable | instruction
type            := TOK_TYPE_Z | TOK_TYPE_R | TOK_TYPE_B | ...
expression      := expr_or
expr_or         := expr_or TOK_OR expr_xor | expr_xor
instruction_si  := TOK_SI expression TOK_ALORS bloc partie_sinon_opt TOK_FIN
instruction_pour := TOK_POUR TOK_ID TOK_DE expr TOK_A expr TOK_FAIRE bloc TOK_FIN
```

### Priorité des Opérateurs

1. `^` (puissance) - droite
2. `-u`, `non` (unaire) - droite
3. `*`, `/`, `div`, `mod` - gauche
4. `+`, `-` - gauche
5. `=`, `≠`, `<`, `>`, `≤`, `≥` - gauche
6. `et` - gauche
7. `xor` - gauche
8. `ou` - gauche
9. `<-` (affectation) - droite

---

##  Contribution

Pour améliorer le compilateur :

1. Étendre les types de données supportés
2. Ajouter de nouvelles structures de contrôle
3. Optimiser la génération de code
4. Ajouter des passes d'optimisation
5. Implémenter un back-end (LLVM, x86-64, etc.)

---

##  Licence

Ce projet est un travail académique de compilation et traduction.

---

##  Auteur

ASBAR ROUFAIDA
MAHMOUDI SAMEH
ROUABHI MOUNIA HIBET ERRAHMANE

---

**Dernière mise à jour** : 21/01/2026