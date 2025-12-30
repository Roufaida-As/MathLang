#include "quadruplet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================= */
/*                   VARIABLES GLOBALES                       */
/* ========================================================= */

static int tempCounter = 0;
static int labelCounter = 0;

// Piles globales pour les structures de contrôle
IntStack ifStack;
IntStack elseStack;
IntStack whileStartStack;
IntStack whileExitStack;
IntStack forStartStack;
IntStack forExitStack;
IntStack forIncrStack;
IntStack repeatStartStack;

/* ========================================================= */
/*                   GESTION DES QUADRUPLETS                  */
/* ========================================================= */

QuadList* initQuadList(void) {
    QuadList* list = (QuadList*)malloc(sizeof(QuadList));
    if (!list) {
        perror("malloc QuadList");
        exit(EXIT_FAILURE);
    }
    
    list->capacity = 1000;
    list->count = 0;
    list->quads = (Quadruplet*)malloc(sizeof(Quadruplet) * list->capacity);
    if (!list->quads) {
        perror("malloc quads");
        exit(EXIT_FAILURE);
    }
    
    return list;
}

void freeQuadList(QuadList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
        free(list->quads[i].arg1);
        free(list->quads[i].arg2);
        free(list->quads[i].result);
    }
    free(list->quads);
    free(list);
}

int createQuad(QuadList* list, QuadOp op, const char* arg1,
               const char* arg2, const char* result) {
    if (!list) return -1;
    
    // Redimensionner si nécessaire
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->quads = (Quadruplet*)realloc(list->quads, 
                                           sizeof(Quadruplet) * list->capacity);
        if (!list->quads) {
            perror("realloc quads");
            exit(EXIT_FAILURE);
        }
    }
    
    Quadruplet* q = &list->quads[list->count];
    q->op = op;
    q->arg1 = arg1 ? stringDuplicate(arg1) : NULL;
    q->arg2 = arg2 ? stringDuplicate(arg2) : NULL;
    q->result = result ? stringDuplicate(result) : NULL;
    
    return list->count++;
}

void updateQuad(QuadList* list, int index, const char* result) {
    if (!list || index < 0 || index >= list->count) return;
    
    free(list->quads[index].result);
    list->quads[index].result = result ? stringDuplicate(result) : NULL;
}

int nextQuad(const QuadList* list) {
    return list ? list->count : 0;
}

/* ========================================================= */
/*                   AFFICHAGE                                */
/* ========================================================= */

const char* quadOpToString(QuadOp op) {
    switch (op) {
        case QUAD_ADD: return "+";
        case QUAD_SUB: return "-";
        case QUAD_MUL: return "*";
        case QUAD_DIV: return "/";
        case QUAD_MOD: return "MOD";
        case QUAD_POW: return "^";
        case QUAD_NEG: return "NEG";
        
        case QUAD_AND: return "AND";
        case QUAD_OR: return "OR";
        case QUAD_NOT: return "NOT";
        case QUAD_XOR: return "XOR";
        
        case QUAD_EQ: return "==";
        case QUAD_NEQ: return "!=";
        case QUAD_LT: return "<";
        case QUAD_GT: return ">";
        case QUAD_LEQ: return "<=";
        case QUAD_GEQ: return ">=";
        
        case QUAD_ASSIGN: return ":=";
        
        case QUAD_BR: return "BR";
        case QUAD_BZ: return "BZ";
        case QUAD_BNZ: return "BNZ";
        
        case QUAD_SIN: return "SIN";
        case QUAD_COS: return "COS";
        case QUAD_SQRT: return "SQRT";
        case QUAD_ABS: return "ABS";
        case QUAD_EXP: return "EXP";
        case QUAD_LOG: return "LOG";
        case QUAD_FLOOR: return "FLOOR";
        case QUAD_CEIL: return "CEIL";
        case QUAD_ROUND: return "ROUND";
        case QUAD_RE: return "RE";
        case QUAD_IM: return "IM";
        case QUAD_ARG: return "ARG";
        
        case QUAD_READ: return "READ";
        case QUAD_WRITE: return "WRITE";
        case QUAD_WRITELN: return "WRITELN";
        
        case QUAD_LABEL: return "LABEL";
        
        case QUAD_PARAM: return "PARAM";
        case QUAD_CALL: return "CALL";
        case QUAD_RETURN: return "RETURN";
        
        case QUAD_NOP: return "NOP";
        
        default: return "UNKNOWN";
    }
}

void printQuadruplet(const Quadruplet* quad, int index) {
    if (!quad) return;
    
    printf("%4d: ( %-8s , ", index, quadOpToString(quad->op));
    
    if (quad->arg1) printf("%-10s", quad->arg1);
    else printf("%-10s", "-");
    
    printf(" , ");
    
    if (quad->arg2) printf("%-10s", quad->arg2);
    else printf("%-10s", "-");
    
    printf(" , ");
    
    if (quad->result) printf("%-10s", quad->result);
    else printf("%-10s", "-");
    
    printf(" )\n");
}

void printQuadruplets(const QuadList* list) {
    if (!list) {
        printf("Liste de quadruplets vide\n");
        return;
    }
    
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           CODE INTERMÉDIAIRE - QUADRUPLETS                   ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Total: %d quadruplets                                        ║\n", list->count);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf(" Idx  Operation    Arg1         Arg2         Result\n");
    printf("────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < list->count; i++) {
        printQuadruplet(&list->quads[i], i);
    }
    
    printf("\n");
}

/* ========================================================= */
/*                   TEMPORAIRES ET ÉTIQUETTES                */
/* ========================================================= */

char* newTemp(void) {
    char* temp = (char*)malloc(16);
    if (!temp) {
        perror("malloc temp");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "T%d", tempCounter++);
    return temp;
}

void resetTempCounter(void) {
    tempCounter = 0;
}

char* newLabel(void) {
    char* label = (char*)malloc(16);
    if (!label) {
        perror("malloc label");
        exit(EXIT_FAILURE);
    }
    sprintf(label, "L%d", labelCounter++);
    return label;
}

void resetLabelCounter(void) {
    labelCounter = 0;
}

/* ========================================================= */
/*                   PILE D'ENTIERS                           */
/* ========================================================= */

void initIntStack(IntStack* stack) {
    if (!stack) return;
    stack->top = -1;
}

bool pushInt(IntStack* stack, int value) {
    if (!stack || stack->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Erreur: débordement de pile (IntStack)\n");
        return false;
    }
    stack->data[++stack->top] = value;
    return true;
}

int popInt(IntStack* stack) {
    if (!stack || stack->top < 0) {
        fprintf(stderr, "Erreur: pile vide (IntStack)\n");
        return -1;
    }
    return stack->data[stack->top--];
}

int peekInt(const IntStack* stack) {
    if (!stack || stack->top < 0) {
        return -1;
    }
    return stack->data[stack->top];
}

bool isIntStackEmpty(const IntStack* stack) {
    return !stack || stack->top < 0;
}

int intStackSize(const IntStack* stack) {
    return stack ? stack->top + 1 : 0;
}

/* ========================================================= */
/*                   PILE DE CHAÎNES                          */
/* ========================================================= */

void initStringStack(StringStack* stack) {
    if (!stack) return;
    stack->top = -1;
}

bool pushString(StringStack* stack, const char* value) {
    if (!stack || stack->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Erreur: débordement de pile (StringStack)\n");
        return false;
    }
    stack->data[++stack->top] = value ? stringDuplicate(value) : NULL;
    return true;
}

char* popString(StringStack* stack) {
    if (!stack || stack->top < 0) {
        fprintf(stderr, "Erreur: pile vide (StringStack)\n");
        return NULL;
    }
    return stack->data[stack->top--];
}

char* peekString(const StringStack* stack) {
    if (!stack || stack->top < 0) {
        return NULL;
    }
    return stack->data[stack->top];
}

bool isStringStackEmpty(const StringStack* stack) {
    return !stack || stack->top < 0;
}

void freeStringStack(StringStack* stack) {
    if (!stack) return;
    
    while (stack->top >= 0) {
        free(stack->data[stack->top--]);
    }
}

/* ========================================================= */
/*                   INITIALISATION DES PILES GLOBALES        */
/* ========================================================= */

void initControlStacks(void) {
    initIntStack(&ifStack);
    initIntStack(&elseStack);
    initIntStack(&whileStartStack);
    initIntStack(&whileExitStack);
    initIntStack(&forStartStack);
    initIntStack(&forExitStack);
    initIntStack(&forIncrStack);
    initIntStack(&repeatStartStack);
}

/* ========================================================= */
/*                   UTILITAIRES                              */
/* ========================================================= */

char* stringDuplicate(const char* str) {
    if (!str) return NULL;
    
    char* dup = (char*)malloc(strlen(str) + 1);
    if (!dup) {
        perror("malloc stringDuplicate");
        exit(EXIT_FAILURE);
    }
    strcpy(dup, str);
    return dup;
}