# MathLang

MathLang est un langage de programmation pédagogique, à syntaxe française et orienté mathématiques, doté d'un compilateur complet écrit en C (lex/flex + yacc/bison) qui traduit le code MathLang en C, compile ce C avec `gcc`, puis exécute le binaire obtenu.

Le compilateur effectue les phases classiques :

1. **Analyse lexicale** (flex) — tokenisation avec suivi ligne/colonne
2. **Analyse syntaxique** (bison) — construction de l'AST implicite via les règles de grammaire
3. **Analyse sémantique** — vérification de types, gestion des portées, table des symboles, table des fonctions
4. **Génération de code intermédiaire** — quadruplets
5. **Génération de code C** — traduction des quadruplets en C, compilé avec `gcc -lm`

## Fonctionnalités du langage

- **Types** : `Z` (entier), `R` (réel), `B` (booléen), `Sigma` (chaîne), `Car` (caractère), `C` (complexe)
- **Déclarations** : `SOIT x dans Z tel que x <- 42`, `SOIT_CONST PI dans R tel que PI <- 3.14159`
- **Fonctions et procédures** : `FONCTION` (retourne une valeur), `PROCEDURE` (aucun retour), avec `RETOURNER`
- **Contrôle de flux** : `SI / ALORS / SINON`, `TANT QUE / FAIRE`, `POUR ... DE ... A ... [PAR ...] FAIRE`, `REPETER / JUSQUA`, `SORTIR`, `CONTINUER`
- **Opérateurs logiques** : `ET`, `OU`, `NON`, `XOR`, implication et équivalence
- **Fonctions mathématiques intégrées** : `sin`, `cos`, `exp`, `log`, `sqrt`, `abs`, `floor`, `ceil`, `round`, ainsi que `re`, `im`, `arg` pour les nombres complexes
- **Opérations sur chaînes** : concaténation, `majuscules`, `minuscules`
- **Entrées/sorties** : `AFFICHER`, `AFFICHER_LIGNE`, `LIRE`
- **Détection d'erreurs sémantiques** : types incompatibles, constantes modifiées, variables non déclarées, portées de boucle, division par zéro, domaines mathématiques invalides, etc.

## Exemple

```
FONCTION factorielle(n : Z) : Z
    SI n <= 1 ALORS
        RETOURNER 1
    SINON
        RETOURNER n * factorielle(n - 1)
    FIN
FIN

PROCEDURE afficher_titre(titre : Sigma)
    AFFICHER_LIGNE(titre)
    AFFICHER("-------------------------")
    AFFICHER_LIGNE("")
FIN
```

## Prérequis

- `gcc`
- `flex`
- `bison`
- `make`

Sous Ubuntu/Debian :
```bash
sudo apt-get update
sudo apt-get install -y build-essential flex bison
```

## Compilation

```bash
make
```

Cela génère l'exécutable `parser` (analyseur lexical/syntaxique + générateur de code C).

## Utilisation

```bash
./parser mon_programme.ml
```

Le compilateur affiche successivement :
- le résultat de l'analyse lexicale (table des tokens),
- le résultat de l'analyse syntaxique,
- la table des symboles,
- la liste des quadruplets générés,
- puis génère `output.c`, le compile (`gcc output.c -lm -o output`) et produit l'exécutable `output`.

S'il y a des erreurs sémantiques, la génération de code est annulée et les erreurs sont listées.

## Tests

```bash
make test
```

Ce qui exécute `scripts/run_tests.sh` : chaque fichier `.ml` du dossier `tests/` est compilé, le C généré est vérifié (compilation + exécution avec timeout), et un résumé pass/fail est affiché.

## Intégration continue

- **`.github/workflows/ci.yml`** — build + tests à chaque push/pull request sur `main`
- **`.github/workflows/release.yml`** — à chaque tag `v*` poussé, build le compilateur et publie le binaire Linux en tant que GitHub Release

Pour publier une nouvelle release :
```bash
git tag v1.1
git push origin v1.1
```

## Structure du projet

```
mathlang.y            # Grammaire bison (lexique + syntaxe + sémantique)
codegen_c.c            # Génération du code C à partir des quadruplets
function_table.c/.h    # Table des fonctions/procédures déclarées
symbol_table.c/.h       # Table des symboles (variables, types, portées)
quadruplet.c/.h         # Représentation et gestion des quadruplets
tests/                  # Programmes MathLang de test
scripts/run_tests.sh    # Script d'exécution des tests
.github/workflows/      # Pipelines CI/CD
```

## Licence

Ce projet est sous licence [MIT](LICENSE).