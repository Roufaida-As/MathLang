#ifndef QUADRUPLET_H
#define QUADRUPLET_H

#include <stdbool.h>
#include "symbol_table.h"

/* ========================================================= */
/*                   TYPES DE QUADRUPLETS                     */
/* ========================================================= */

typedef enum {
    // Opérations arithmétiques
    QUAD_ADD,      // +
    QUAD_SUB,      // -
    QUAD_MUL,      // *
    QUAD_DIV,      // / (division réelle)
    QUAD_DIV_INT,  // div (division entière)
    QUAD_MOD,      // mod
    QUAD_POW,      // ^
    QUAD_NEG,      // négation unaire
    
    // Opérations logiques
    QUAD_AND,      // et
    QUAD_OR,       // ou
    QUAD_NOT,      // non
    QUAD_XOR,      // xor
    
    // Comparaisons
    QUAD_EQ,       // =
    QUAD_NEQ,      // !=
    QUAD_LT,       // <
    QUAD_GT,       // >
    QUAD_LEQ,      // <=
    QUAD_GEQ,      // >=
    
    // Affectation
    QUAD_ASSIGN,   // <-
    
    // Branchements
    QUAD_BR,       // branchement inconditionnel
    QUAD_BZ,       // branchement si zéro (faux)
    QUAD_BNZ,      // branchement si non-zéro (vrai)
    
    // Branchements conditionnels
    QUAD_BG,       // branchement si >
    QUAD_BGE,      // branchement si >=
    QUAD_BL,       // branchement si <
    QUAD_BLE,      // branchement si <=
    QUAD_BE,       // branchement si ==
    QUAD_BNE,      // branchement si !=
    
    // Fonctions mathématiques
    QUAD_SIN,
    QUAD_COS,
    QUAD_SQRT,
    QUAD_ABS,
    QUAD_EXP,
    QUAD_LOG,
    QUAD_FLOOR,
    QUAD_CEIL,
    QUAD_ROUND,
    QUAD_RE,       // partie réelle
    QUAD_IM,       // partie imaginaire
    QUAD_ARG,      // argument
    QUAD_MAJUSCULES, // majuscules(string)
    QUAD_MINUSCULES, // minuscules(string)
    
    // I/O
    QUAD_READ,     // LIRE
    QUAD_WRITE,    // AFFICHER
    QUAD_WRITELN,  // AFFICHER_LIGNE
    
    // Étiquettes
    QUAD_LABEL,    // étiquette
    
    // Fonctions
    QUAD_PARAM,    // passage de paramètre
    QUAD_CALL,     // appel de fonction
    QUAD_RETURN,   // retour de fonction
    
    // Spéciaux
    QUAD_NOP       // pas d'opération
} QuadOp;

/* ========================================================= */
/*                   STRUCTURE QUADRUPLET                     */
/* ========================================================= */

typedef struct {
    QuadOp op;         // Opérateur
    char* arg1;        // Premier argument
    char* arg2;        // Deuxième argument
    char* result;      // Résultat
} Quadruplet;

/* ========================================================= */
/*                   GESTION DES QUADRUPLETS                  */
/* ========================================================= */

#define MAX_QUADS 10000

typedef struct {
    Quadruplet* quads;
    int count;
    int capacity;
} QuadList;

// Initialisation et libération
QuadList* initQuadList(void);
void freeQuadList(QuadList* list);

// Création de quadruplets
int createQuad(QuadList* list, QuadOp op, const char* arg1, 
               const char* arg2, const char* result);

// Modification de quadruplets (pour compléter les sauts)
void updateQuad(QuadList* list, int index, const char* result);

// Affichage
void printQuadruplets(const QuadList* list);
void printQuadruplet(const Quadruplet* quad, int index);

// Récupération de l'index courant
int nextQuad(const QuadList* list);

/* ========================================================= */
/*                   TEMPORAIRES                              */
/* ========================================================= */

char* newTemp(void);
void resetTempCounter(void);

/* ========================================================= */
/*                   ÉTIQUETTES                               */
/* ========================================================= */

char* newLabel(void);
void resetLabelCounter(void);

/* ========================================================= */
/*                   PILES POUR STRUCTURES DE CONTRÔLE        */
/* ========================================================= */

#define MAX_STACK_SIZE 1000

// Pile d'entiers (pour les indices de quadruplets à compléter)
typedef struct {
    int data[MAX_STACK_SIZE];
    int top;
} IntStack;

void initIntStack(IntStack* stack);
bool pushInt(IntStack* stack, int value);
int popInt(IntStack* stack);
int peekInt(const IntStack* stack);
bool isIntStackEmpty(const IntStack* stack);
int intStackSize(const IntStack* stack);

// Pile de chaînes (pour les étiquettes)
typedef struct {
    char* data[MAX_STACK_SIZE];
    int top;
} StringStack;

void initStringStack(StringStack* stack);
bool pushString(StringStack* stack, const char* value);
char* popString(StringStack* stack);
char* peekString(const StringStack* stack);
bool isStringStackEmpty(const StringStack* stack);
void freeStringStack(StringStack* stack);

/* ========================================================= */
/*                   PILES GLOBALES                           */
/* ========================================================= */

// Piles pour IF/ELSE
extern IntStack ifStack;        // Pour les BZ à compléter
extern IntStack ifBrStack;      // Pour les BR du premier IF (avant SINSO)
extern IntStack elseStack;      // Pour les BR à compléter

// Piles pour WHILE
extern IntStack whileStartStack;    // Indices de début de boucle
extern IntStack whileExitStack;     // BZ de sortie à compléter

// Piles pour FOR
extern IntStack forStartStack;      // Indices de début de boucle
extern IntStack forExitStack;       // BZ de sortie à compléter
extern IntStack forIncrStack;       // Indices d'incrémentation
extern IntStack forBreakStack;      // BR générés par SORTIR
extern IntStack forContinueStack;   // BR générés par CONTINUER

// Piles pour REPEAT
extern IntStack repeatStartStack;   // Indices de début de boucle

// Initialisation des piles globales
void initControlStacks(void);

/* ========================================================= */
/*                   UTILITAIRES                              */
/* ========================================================= */

const char* quadOpToString(QuadOp op);
char* stringDuplicate(const char* str);
bool check_comparable_types(DataType left, DataType right);

#endif /* QUADRUPLET_H */