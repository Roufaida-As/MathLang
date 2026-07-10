#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "symbol_table.h"   /* pour DataType */

#define FT_MAX_FUNCTIONS 256
#define FT_MAX_PARAMS    32

typedef struct {
    DataType type;
    char name[64];
} FTParam;

typedef struct {
    char name[128];
    int is_function;        /* 1 = fonction (retourne une valeur), 0 = procedure */
    DataType return_type;   /* TYPE_VOID pour une procedure */
    FTParam params[FT_MAX_PARAMS];
    int param_count;
    int quad_start;         /* indice du premier quadruplet du corps */
    int quad_end;           /* indice juste apres le dernier quadruplet (exclusif) */
} FunctionInfo;

void ft_reset(void);

/* A appeler juste apres avoir vu "TOK_FONCTION/TOK_PROCEDURE TOK_ID (" */
void ft_begin(const char* name, int is_function);

/* A appeler pour chaque parametre, dans l'ordre */
void ft_add_param(DataType type, const char* name);

void ft_set_return_type(DataType type);
void ft_set_quad_start(int quad_start);

/* A appeler juste apres "bloc TOK_FIN" */
void ft_end(int quad_end);

/* Recherche par nom (pour la verification des appels) */
FunctionInfo* ft_find(const char* name);

/* La fonction en cours de definition (pile, utile si un jour on imbrique) */
FunctionInfo* ft_current(void);

/* Pour le generateur de code : tableau complet + nombre d'entrees */
FunctionInfo* ft_get_all(int* count);

#endif