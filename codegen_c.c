#include "codegen_c.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================================================= */
/*  PETITE TABLE : nom de temporaire -> DataType              */
/* ========================================================= */
/*
 * Vos Quadruplet ne stockent pas le type du résultat, donc on
 * doit le déduire nous-mêmes. On fait une première passe sur
 * la liste de quadruplets (dans l'ordre où ils ont été générés)
 * et on retient, pour chaque temporaire "Tn", quel type il a.
 */

typedef struct
{
    char *name;
    DataType type;
    int owner_quad; /* indice du quadruplet ou ce nom a ete defini en premier */
} TempEntry;

typedef struct
{
    TempEntry *entries;
    int count;
    int capacity;
} TempMap;

static void temp_map_init(TempMap *map)
{
    map->capacity = 64;
    map->count = 0;
    map->entries = (TempEntry *)malloc(sizeof(TempEntry) * map->capacity);
}

static void temp_map_free(TempMap *map)
{
    for (int i = 0; i < map->count; i++)
    {
        free(map->entries[i].name);
    }
    free(map->entries);
}

static DataType temp_map_get(TempMap *map, const char *name)
{
    for (int i = 0; i < map->count; i++)
    {
        if (strcmp(map->entries[i].name, name) == 0)
        {
            return map->entries[i].type;
        }
    }
    return TYPE_UNKNOWN;
}

static void temp_map_set(TempMap *map, const char *name, DataType type, int quad_index)
{
    for (int i = 0; i < map->count; i++)
    {
        if (strcmp(map->entries[i].name, name) == 0)
        {
            map->entries[i].type = type;
            return; /* on conserve owner_quad de la premiere apparition */
        }
    }
    if (map->count >= map->capacity)
    {
        map->capacity *= 2;
        map->entries = (TempEntry *)realloc(map->entries, sizeof(TempEntry) * map->capacity);
    }
    map->entries[map->count].name = stringDuplicate(name);
    map->entries[map->count].type = type;
    map->entries[map->count].owner_quad = quad_index;
    map->count++;
}

typedef struct
{
    char **items;
    int count;
    int capacity;
} ParamBuffer;

static void pb_init(ParamBuffer *pb)
{
    pb->capacity = 8;
    pb->count = 0;
    pb->items = (char **)malloc(sizeof(char *) * pb->capacity);
}
static void pb_clear(ParamBuffer *pb)
{
    for (int i = 0; i < pb->count; i++)
        free(pb->items[i]);
    pb->count = 0;
}
static void pb_push(ParamBuffer *pb, const char *s)
{
    if (pb->count >= pb->capacity)
    {
        pb->capacity *= 2;
        pb->items = (char **)realloc(pb->items, sizeof(char *) * pb->capacity);
    }
    pb->items[pb->count++] = stringDuplicate(s ? s : "");
}
static void pb_free(ParamBuffer *pb)
{
    pb_clear(pb);
    free(pb->items);
}

/* ========================================================= */
/*  UTILITAIRES DE RECONNAISSANCE D'ADRESSES                  */
/* ========================================================= */

static int is_numeric_literal(const char *s, int *has_dot)
{
    if (!s || !*s)
        return 0;
    const char *p = s;
    if (*p == '-')
        p++;
    if (!isdigit((unsigned char)*p))
        return 0;
    *has_dot = 0;
    for (; *p; p++)
    {
        if (*p == '.')
            *has_dot = 1;
        else if (!isdigit((unsigned char)*p))
            return 0;
    }
    return 1;
}

/* Reconnait un littéral imaginaire du style "3i", "4.0i", "-2.5j".
   Si trouvé, copie la partie numérique (sans le suffixe) dans out_num
   et renvoie 1. */
static int parse_complex_literal(const char *s, char *out_num, size_t out_size)
{
    if (!s)
        return 0;
    size_t len = strlen(s);
    if (len < 2)
        return 0;
    char suffix = s[len - 1];
    if (suffix != 'i' && suffix != 'j' && suffix != 'I' && suffix != 'J')
        return 0;
    if (len - 1 >= out_size)
        return 0;

    strncpy(out_num, s, len - 1);
    out_num[len - 1] = '\0';

    int has_dot;
    return is_numeric_literal(out_num, &has_dot);
}

/* Déduit le type d'une "addr" (nom de variable, temporaire ou littéral) */
static DataType resolve_type(const char *addr, SymbolTable *table, TempMap *tmap,
                             FunctionInfo *current_fn)
{
    if (!addr)
        return TYPE_UNKNOWN;

    if (addr[0] == '\'')
        return TYPE_CHAR;
    if (addr[0] == '"')
        return TYPE_SIGMA;
    if (strcmp(addr, "true") == 0 || strcmp(addr, "false") == 0)
        return TYPE_B;

    int has_dot;
    if (is_numeric_literal(addr, &has_dot))
    {
        return has_dot ? TYPE_R : TYPE_Z;
    }

    char numbuf[64];
    if (parse_complex_literal(addr, numbuf, sizeof(numbuf)))
    {
        return TYPE_C;
    }

    /* PRIORITE MAXIMALE : un parametre de la fonction en cours de generation
       masque toute variable globale homonyme, exactement comme en C. */
    if (current_fn)
    {
        for (int p = 0; p < current_fn->param_count; p++)
        {
            if (strcmp(current_fn->params[p].name, addr) == 0)
            {
                return current_fn->params[p].type;
            }
        }
    }

    if (table)
    {
        SymbolEntry *e = find_symbol(table, addr);
        if (e)
            return e->type;
    }

    return temp_map_get(tmap, addr);
}

/* Opérations dont le champ "result" désigne une variable qui reçoit
   une valeur (par opposition à une cible de branchement, comme dans
   BR/BZ/BG..., ou un résultat non pertinent comme WRITE/WRITELN). */
static int is_assigning_op(QuadOp op)
{
    switch (op)
    {
    case QUAD_ADD:
    case QUAD_SUB:
    case QUAD_MUL:
    case QUAD_DIV:
    case QUAD_DIV_INT:
    case QUAD_MOD:
    case QUAD_POW:
    case QUAD_NEG:
    case QUAD_AND:
    case QUAD_OR:
    case QUAD_NOT:
    case QUAD_XOR:
    case QUAD_EQ:
    case QUAD_NEQ:
    case QUAD_LT:
    case QUAD_GT:
    case QUAD_LEQ:
    case QUAD_GEQ:
    case QUAD_ASSIGN:
    case QUAD_SIN:
    case QUAD_COS:
    case QUAD_EXP:
    case QUAD_LOG:
    case QUAD_SQRT:
    case QUAD_ABS:
    case QUAD_FLOOR:
    case QUAD_CEIL:
    case QUAD_ROUND:
    case QUAD_RE:
    case QUAD_IM:
    case QUAD_ARG:
    case QUAD_MAJUSCULES:
    case QUAD_MINUSCULES:
    case QUAD_READ:
    case QUAD_CALL:
        return 1;
    default:
        return 0;
    }
}

/* ========================================================= */
/*  PASSE 1 : INFÉRENCE DES TYPES MANQUANTS                    */
/* ========================================================= */
/*
 * Deux catégories de noms doivent être déclarées "à la main" en C
 * car elles n'apparaissent pas (ou plus) dans la table des symboles :
 *   1) Les temporaires générés par le compilateur (T0, T1, ...)
 *   2) Les variables MathLang dont le scope a déjà été fermé au
 *      moment où on génère le code (ex: la variable de boucle d'un
 *      POUR, retirée de la table par exit_scope() après le parsing).
 * On les traite de la même façon : si le nom n'est pas trouvable
 * dans la table des symboles, on déduit son type depuis l'opération
 * qui l'a produit et on le mémorise dans tmap pour le déclarer plus
 * tard.
 */
static void infer_types_pass(const QuadList *list, SymbolTable *table, TempMap *tmap,
                             FunctionInfo *functions, const int *owner)
{
    for (int i = 0; i < list->count; i++)
    {
        const Quadruplet *q = &list->quads[i];
        if (!q->result || !is_assigning_op(q->op))
            continue;

        FunctionInfo *current_fn = (owner[i] >= 0) ? &functions[owner[i]] : NULL;

        /* Un parametre de la fonction courante ne doit jamais etre re-declare
           (meme s'il existe une variable globale homonyme dans la table). */
        int already_known = 0;
        if (current_fn)
        {
            for (int p = 0; p < current_fn->param_count; p++)
            {
                if (strcmp(current_fn->params[p].name, q->result) == 0)
                {
                    already_known = 1;
                    break;
                }
            }
        }
        if (!already_known && table && find_symbol(table, q->result))
            continue;

        DataType t1 = resolve_type(q->arg1, table, tmap, current_fn);
        DataType t2 = resolve_type(q->arg2, table, tmap, current_fn);
        DataType result_type;

        switch (q->op)
        {
        case QUAD_ADD:
            if (t1 == TYPE_SIGMA || t2 == TYPE_SIGMA)
            {
                result_type = TYPE_SIGMA; /* concaténation de chaînes */
            }
            else if (t1 == TYPE_C || t2 == TYPE_C)
            {
                result_type = TYPE_C;
            }
            else
            {
                result_type = (t1 == TYPE_R || t2 == TYPE_R) ? TYPE_R : TYPE_Z;
            }
            break;

        case QUAD_SUB:
        case QUAD_MUL:
        case QUAD_MOD:
        case QUAD_POW:
            if (t1 == TYPE_C || t2 == TYPE_C)
            {
                result_type = TYPE_C;
            }
            else
            {
                result_type = (t1 == TYPE_R || t2 == TYPE_R) ? TYPE_R : TYPE_Z;
            }
            break;

        case QUAD_DIV:
            result_type = (t1 == TYPE_C || t2 == TYPE_C) ? TYPE_C : TYPE_R;
            break;

        case QUAD_DIV_INT:
            result_type = TYPE_Z;
            break;

        case QUAD_NEG:
        case QUAD_ABS:
            result_type = (t1 == TYPE_UNKNOWN) ? TYPE_R : t1;
            break;

        case QUAD_ASSIGN:
            result_type = (t1 == TYPE_UNKNOWN) ? TYPE_R : t1;
            break;

        case QUAD_AND:
        case QUAD_OR:
        case QUAD_NOT:
        case QUAD_XOR:
        case QUAD_EQ:
        case QUAD_NEQ:
        case QUAD_LT:
        case QUAD_GT:
        case QUAD_LEQ:
        case QUAD_GEQ:
            result_type = TYPE_B;
            break;

        case QUAD_SIN:
        case QUAD_COS:
        case QUAD_EXP:
        case QUAD_LOG:
        case QUAD_SQRT:
        case QUAD_FLOOR:
        case QUAD_CEIL:
        case QUAD_ROUND:
        case QUAD_RE:
        case QUAD_IM:
        case QUAD_ARG:
            result_type = TYPE_R;
            break;

        case QUAD_MAJUSCULES:
        case QUAD_MINUSCULES:
            result_type = TYPE_SIGMA;
            break;

        case QUAD_READ:
            /* On ne peut rien déduire d'un READ isolé : on garde
               ce qu'on connaît déjà de cette variable si on l'a
               déjà vue, sinon on retombe sur R par défaut. */
            result_type = temp_map_get(tmap, q->result);
            if (result_type == TYPE_UNKNOWN)
                result_type = TYPE_R;
            break;
        case QUAD_CALL:
        {
            FunctionInfo *callee = ft_find(q->arg1); /* q->arg1 = nom de la fonction */
            result_type = callee ? callee->return_type : TYPE_R;
            break;
        }

        default:
            /* Choix prudent par défaut pour ne rien casser à la compilation C */
            result_type = TYPE_R;
            break;
        }

        temp_map_set(tmap, q->result, result_type, i);
    }
}

/* Convertit un opérande en texte C valide. Le seul cas à traiter est
   le littéral complexe ("3i" -> "(3*I)") : tout le reste passe tel quel. */
static char *format_operand(const char *addr)
{
    if (!addr)
        return NULL;
    char numbuf[64];
    if (parse_complex_literal(addr, numbuf, sizeof(numbuf)))
    {
        char *out = (char *)malloc(strlen(numbuf) + 16);
        sprintf(out, "(%s*I)", numbuf);
        return out;
    }
    return stringDuplicate(addr);
}

/* ========================================================= */
/*  TYPES C CORRESPONDANTS                                    */
/* ========================================================= */

static const char *get_c_type(DataType type)
{
    switch (type)
    {
    case TYPE_Z:
        return "long";
    case TYPE_R:
        return "double";
    case TYPE_B:
        return "int";
    case TYPE_CHAR:
        return "char";
    case TYPE_SIGMA:
        return "char*";
    case TYPE_C:
        return "double complex";
    case TYPE_VOID:
        return "void";
    default:
        return "double"; /* valeur par défaut prudente */
    }
}

/* ========================================================= */
/*  ÉMISSION D'UN AFFICHAGE (WRITE) SELON LE TYPE              */
/* ========================================================= */

static void emit_write(FILE *out, const char *addr, DataType type)
{
    switch (type)
    {
    case TYPE_Z:
        fprintf(out, "    printf(\"%%ld\", %s);\n", addr);
        break;
    case TYPE_R:
        fprintf(out, "    printf(\"%%g\", %s);\n", addr);
        break;
    case TYPE_B:
        fprintf(out, "    printf(\"%%s\", (%s) ? \"true\" : \"false\");\n", addr);
        break;
    case TYPE_CHAR:
        fprintf(out, "    printf(\"%%c\", %s);\n", addr);
        break;
    case TYPE_SIGMA:
        fprintf(out, "    printf(\"%%s\", %s);\n", addr);
        break;
    default:
        fprintf(out, "    printf(\"%%g\", (double)(%s));\n", addr);
        break;
    }
}

/* ========================================================= */
/*  TRADUCTION D'UN QUADRUPLET EN C                            */
/* ========================================================= */

static void translate_quad(FILE *out, const Quadruplet *q, SymbolTable *table,
                           TempMap *tmap, ParamBuffer *pb, FunctionInfo *current_fn)
{
    char *a1 = format_operand(q->arg1);
    char *a2 = format_operand(q->arg2);

    switch (q->op)
    {

    /* --- Arithmétique --- */
    case QUAD_ADD:
    {
        DataType t1 = resolve_type(q->arg1, table, tmap, current_fn);
        DataType t2 = resolve_type(q->arg2, table, tmap, current_fn);
        if (t1 == TYPE_SIGMA || t2 == TYPE_SIGMA)
        {
            /* Concaténation de chaînes : on alloue un nouveau buffer */
            fprintf(out,
                    "    %s = malloc(strlen(%s) + strlen(%s) + 1);\n"
                    "    strcpy(%s, %s);\n"
                    "    strcat(%s, %s);\n",
                    q->result, a1, a2,
                    q->result, a1,
                    q->result, a2);
        }
        else
        {
            fprintf(out, "    %s = %s + %s;\n", q->result, a1, a2);
        }
        break;
    }
    case QUAD_SUB:
        fprintf(out, "    %s = %s - %s;\n", q->result, a1, a2);
        break;
    case QUAD_MUL:
        fprintf(out, "    %s = %s * %s;\n", q->result, a1, a2);
        break;
    case QUAD_DIV:
    {
        DataType t1 = resolve_type(q->arg1, table, tmap, current_fn);
        DataType t2 = resolve_type(q->arg2, table, tmap, current_fn);
        if (t1 == TYPE_C || t2 == TYPE_C)
        {
            /* Pas de cast (double) ici : ça tronquerait la partie imaginaire */
            fprintf(out, "    %s = %s / %s;\n", q->result, a1, a2);
        }
        else
        {
            fprintf(out, "    %s = (double)(%s) / (double)(%s);\n", q->result, a1, a2);
        }
        break;
    }
    case QUAD_DIV_INT:
        fprintf(out, "    %s = (long)(%s) / (long)(%s);\n", q->result, a1, a2);
        break;
    case QUAD_MOD:
        fprintf(out, "    %s = (long)(%s) %% (long)(%s);\n", q->result, a1, a2);
        break;
    case QUAD_POW:
        fprintf(out, "    %s = pow((double)(%s), (double)(%s));\n", q->result, a1, a2);
        break;
    case QUAD_NEG:
        fprintf(out, "    %s = -(%s);\n", q->result, a1);
        break;

    /* --- Logique --- */
    case QUAD_AND:
        fprintf(out, "    %s = (%s) && (%s);\n", q->result, a1, a2);
        break;
    case QUAD_OR:
        fprintf(out, "    %s = (%s) || (%s);\n", q->result, a1, a2);
        break;
    case QUAD_NOT:
        fprintf(out, "    %s = !(%s);\n", q->result, a1);
        break;
    case QUAD_XOR:
        fprintf(out, "    %s = (!!(%s)) != (!!(%s));\n", q->result, a1, a2);
        break;

    /* --- Comparaisons (rarement matérialisées, cf expr_to_addr) --- */
    case QUAD_EQ:
        fprintf(out, "    %s = (%s) == (%s);\n", q->result, a1, a2);
        break;
    case QUAD_NEQ:
        fprintf(out, "    %s = (%s) != (%s);\n", q->result, a1, a2);
        break;
    case QUAD_LT:
        fprintf(out, "    %s = (%s) < (%s);\n", q->result, a1, a2);
        break;
    case QUAD_GT:
        fprintf(out, "    %s = (%s) > (%s);\n", q->result, a1, a2);
        break;
    case QUAD_LEQ:
        fprintf(out, "    %s = (%s) <= (%s);\n", q->result, a1, a2);
        break;
    case QUAD_GEQ:
        fprintf(out, "    %s = (%s) >= (%s);\n", q->result, a1, a2);
        break;

    /* --- Affectation --- */
    case QUAD_ASSIGN:
        fprintf(out, "    %s = %s;\n", q->result, a1);
        break;

    /* --- Branchements inconditionnels et conditionnels --- */
    case QUAD_BR:
        fprintf(out, "    goto L%s;\n", q->result);
        break;
    case QUAD_BZ:
        fprintf(out, "    if (!(%s)) goto L%s;\n", a1, q->result);
        break;
    case QUAD_BNZ:
        fprintf(out, "    if (%s) goto L%s;\n", a1, q->result);
        break;
    case QUAD_BG:
        fprintf(out, "    if ((%s) > (%s)) goto L%s;\n", a1, a2, q->result);
        break;
    case QUAD_BGE:
        fprintf(out, "    if ((%s) >= (%s)) goto L%s;\n", a1, a2, q->result);
        break;
    case QUAD_BL:
        fprintf(out, "    if ((%s) < (%s)) goto L%s;\n", a1, a2, q->result);
        break;
    case QUAD_BLE:
        fprintf(out, "    if ((%s) <= (%s)) goto L%s;\n", a1, a2, q->result);
        break;
    case QUAD_BE:
        fprintf(out, "    if ((%s) == (%s)) goto L%s;\n", a1, a2, q->result);
        break;
    case QUAD_BNE:
        fprintf(out, "    if ((%s) != (%s)) goto L%s;\n", a1, a2, q->result);
        break;

    /* --- Fonctions mathématiques --- */
    case QUAD_SIN:
        fprintf(out, "    %s = sin((double)(%s));\n", q->result, a1);
        break;
    case QUAD_COS:
        fprintf(out, "    %s = cos((double)(%s));\n", q->result, a1);
        break;
    case QUAD_EXP:
        fprintf(out, "    %s = exp((double)(%s));\n", q->result, a1);
        break;
    case QUAD_LOG:
        fprintf(out, "    %s = log((double)(%s));\n", q->result, a1);
        break;
    case QUAD_SQRT:
    {
        DataType t1 = resolve_type(q->arg1, table, tmap, current_fn);
        if (t1 == TYPE_C)
        {
            fprintf(out, "    %s = csqrt(%s);\n", q->result, a1);
        }
        else
        {
            fprintf(out, "    %s = sqrt((double)(%s));\n", q->result, a1);
        }
        break;
    }
    case QUAD_ABS:
    {
        DataType t1 = resolve_type(q->arg1, table, tmap, current_fn);
        if (t1 == TYPE_C)
        {
            fprintf(out, "    %s = cabs(%s);\n", q->result, a1);
        }
        else
        {
            fprintf(out, "    %s = fabs((double)(%s));\n", q->result, a1);
        }
        break;
    }
    case QUAD_FLOOR:
        fprintf(out, "    %s = floor((double)(%s));\n", q->result, a1);
        break;
    case QUAD_CEIL:
        fprintf(out, "    %s = ceil((double)(%s));\n", q->result, a1);
        break;
    case QUAD_ROUND:
        fprintf(out, "    %s = round((double)(%s));\n", q->result, a1);
        break;

    case QUAD_RE:
        fprintf(out, "    %s = creal(%s);\n", q->result, a1);
        break;
    case QUAD_IM:
        fprintf(out, "    %s = cimag(%s);\n", q->result, a1);
        break;
    case QUAD_ARG:
        fprintf(out, "    %s = carg(%s);\n", q->result, a1);
        break;

    /* --- Fonctions chaînes --- */
    case QUAD_MAJUSCULES:
        fprintf(out,
                "    %s = strdup(%s);\n"
                "    for (char* p = %s; *p; p++) *p = (char)toupper((unsigned char)*p);\n",
                q->result, a1, q->result);
        break;
    case QUAD_MINUSCULES:
        fprintf(out,
                "    %s = strdup(%s);\n"
                "    for (char* p = %s; *p; p++) *p = (char)tolower((unsigned char)*p);\n",
                q->result, a1, q->result);
        break;

    /* --- Entrées / sorties --- */
    case QUAD_READ:
    {
        DataType t = resolve_type(q->result, table, tmap, current_fn);
        if (t == TYPE_Z)
            fprintf(out, "    scanf(\"%%ld\", &%s);\n", q->result);
        else if (t == TYPE_R)
            fprintf(out, "    scanf(\"%%lf\", &%s);\n", q->result);
        else if (t == TYPE_CHAR)
            fprintf(out, "    scanf(\" %%c\", &%s);\n", q->result);
        else
            fprintf(out, "    /* TODO : lecture non geree pour ce type */\n");
        break;
    }
    case QUAD_WRITE:
    {
        DataType t = resolve_type(q->arg1, table, tmap, current_fn);
        emit_write(out, a1, t);
        break;
    }
    case QUAD_WRITELN:
        fprintf(out, "    printf(\"\\n\");\n");
        break;

    case QUAD_RETURN:
        fprintf(out, "    return %s;\n", a1 ? a1 : "0");
        break;

    case QUAD_LABEL:
    case QUAD_NOP:
        fprintf(out, "    ;\n");
        break;
    case QUAD_PARAM:
        pb_push(pb, a1);
        break;

    case QUAD_CALL:
    {
        fprintf(out, "    ");
        if (q->result)
            fprintf(out, "%s = ", q->result);
        fprintf(out, "%s(", q->arg1 ? q->arg1 : "");
        for (int k = 0; k < pb->count; k++)
        {
            fprintf(out, "%s%s", (k > 0) ? ", " : "", pb->items[k]);
        }
        fprintf(out, ");\n");
        pb_clear(pb);
        break;
    }

    default:
        fprintf(out, "    /* TODO : quadruplet non traduit (%s) */\n", quadOpToString(q->op));
        break;
    }

    free(a1);
    free(a2);
}

/* ========================================================= */
/*  POINT D'ENTRÉE PRINCIPAL                                  */
/* ========================================================= */

static int *build_quad_owner_array(const QuadList *list, FunctionInfo *functions, int function_count)
{
    int n = (list->count > 0) ? list->count : 1;
    int *owner = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < list->count; i++)
        owner[i] = -1;
    for (int f = 0; f < function_count; f++)
    {
        int start = functions[f].quad_start;
        int end = functions[f].quad_end;
        if (start < 0)
            start = 0;
        if (end > list->count)
            end = list->count;
        for (int i = start; i < end; i++)
            owner[i] = f;
    }
    return owner;
}

static void emit_signature(FILE *out, FunctionInfo *fi)
{
    fprintf(out, "%s %s(", fi->is_function ? get_c_type(fi->return_type) : "void", fi->name);
    if (fi->param_count == 0)
    {
        fprintf(out, "void");
    }
    else
    {
        for (int p = 0; p < fi->param_count; p++)
        {
            fprintf(out, "%s%s %s", (p > 0) ? ", " : "",
                    get_c_type(fi->params[p].type), fi->params[p].name);
        }
    }
    fprintf(out, ")");
}

void generate_c_code(FILE *out, QuadList *list, SymbolTable *table,
                     FunctionInfo *functions, int function_count)
{
    if (!out || !list)
        return;

    TempMap tmap;
    temp_map_init(&tmap);

    int *owner = build_quad_owner_array(list, functions, function_count); /* AVANT infer_types_pass maintenant */
    infer_types_pass(list, table, &tmap, functions, owner);


    fprintf(out, "/* ===================================================== */\n");
    fprintf(out, "/* Fichier genere automatiquement par le compilateur      */\n");
    fprintf(out, "/* MathLang -> C. Ne pas modifier a la main.              */\n");
    fprintf(out, "/* ===================================================== */\n\n");
    fprintf(out, "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n");
    fprintf(out, "#include <math.h>\n#include <ctype.h>\n#include <stdbool.h>\n");
    fprintf(out, "#include <complex.h>\n\n");

    /* --- Variables globales MathLang (portee programme) : declarees a
       l'echelle du fichier pour rester visibles depuis les fonctions --- */
    fprintf(out, "/* --- Variables globales declarees dans MathLang --- */\n");
    if (table)
    {
        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            SymbolEntry *e = table->entries[i];
            while (e)
            {
                if (e->category == SYMBOL_VARIABLE || e->category == SYMBOL_CONSTANT)
                {
                    if (e->type == TYPE_SIGMA)
                    {
                        fprintf(out, "%s %s = NULL;\n", get_c_type(e->type), e->name);
                    }
                    else
                    {
                        fprintf(out, "%s %s;\n", get_c_type(e->type), e->name);
                    }
                }
                e = e->next;
            }
        }
    }
    fprintf(out, "\n/* --- Prototypes --- */\n");
    for (int f = 0; f < function_count; f++)
    {
        emit_signature(out, &functions[f]);
        fprintf(out, ";\n");
    }
    fprintf(out, "\n");

    ParamBuffer pb;
    pb_init(&pb);

    /* --- Corps de chaque fonction/procedure --- */
    for (int f = 0; f < function_count; f++)
    {
        FunctionInfo *fi = &functions[f];
        emit_signature(out, fi);
        fprintf(out, " {\n    /* --- Temporaires / variables locales --- */\n");

        for (int t = 0; t < tmap.count; t++)
        {
            if (owner[tmap.entries[t].owner_quad] != f)
                continue;
            int is_param = 0;
            for (int p = 0; p < fi->param_count; p++)
            {
                if (strcmp(fi->params[p].name, tmap.entries[t].name) == 0)
                {
                    is_param = 1;
                    break;
                }
            }
            if (is_param)
                continue;
            fprintf(out, "    %s %s;\n", get_c_type(tmap.entries[t].type), tmap.entries[t].name);
        }
        fprintf(out, "\n");

        for (int i = fi->quad_start; i < fi->quad_end && i < list->count; i++)
        {
            fprintf(out, "L%d:;\n", i);
            translate_quad(out, &list->quads[i], table, &tmap, &pb,fi);
        }
        fprintf(out, "L%d:;\n", fi->quad_end);

        if (fi->is_function)
        {
            fprintf(out, "    /* filet de securite si aucun RETOURNER n'est atteint */\n");
            fprintf(out, "    return (%s)0;\n", get_c_type(fi->return_type));
        }
        else
        {
            fprintf(out, "    return;\n");
        }
        fprintf(out, "}\n\n");
    }

    /* --- main() : uniquement les quadruplets hors de toute fonction --- */
    fprintf(out, "int main(void) {\n    /* --- Temporaires / variables locales --- */\n");
    for (int t = 0; t < tmap.count; t++)
    {
        if (owner[tmap.entries[t].owner_quad] != -1)
            continue;
        fprintf(out, "    %s %s;\n", get_c_type(tmap.entries[t].type), tmap.entries[t].name);
    }
    fprintf(out, "\n");

    for (int i = 0; i < list->count; i++)
    {
        if (owner[i] != -1)
            continue;
        fprintf(out, "L%d:;\n", i);
        translate_quad(out, &list->quads[i], table, &tmap, &pb,NULL);
    }
    fprintf(out, "L%d:;\n", list->count);
    fprintf(out, "    return 0;\n}\n");

    pb_free(&pb);
    free(owner);
    temp_map_free(&tmap);
}