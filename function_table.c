#include "function_table.h"
#include <string.h>

static FunctionInfo g_functions[FT_MAX_FUNCTIONS];
static int g_function_count = 0;

#define FT_STACK_SIZE 64
static int g_stack[FT_STACK_SIZE];   /* indices dans g_functions */
static int g_stack_top = -1;

void ft_reset(void) {
    g_function_count = 0;
    g_stack_top = -1;
}

void ft_begin(const char* name, int is_function) {
    if (g_function_count >= FT_MAX_FUNCTIONS) return;
    if (g_stack_top + 1 >= FT_STACK_SIZE) return;

    int idx = g_function_count++;
    FunctionInfo* fi = &g_functions[idx];
    memset(fi, 0, sizeof(*fi));
    strncpy(fi->name, name, sizeof(fi->name) - 1);
    fi->is_function = is_function;
    fi->return_type = TYPE_VOID;

    g_stack_top++;
    g_stack[g_stack_top] = idx;
}

static FunctionInfo* current(void) {
    if (g_stack_top < 0) return NULL;
    return &g_functions[g_stack[g_stack_top]];
}

void ft_add_param(DataType type, const char* name) {
    FunctionInfo* fi = current();
    if (!fi || fi->param_count >= FT_MAX_PARAMS) return;
    fi->params[fi->param_count].type = type;
    strncpy(fi->params[fi->param_count].name, name, sizeof(fi->params[0].name) - 1);
    fi->param_count++;
}

void ft_set_return_type(DataType type) {
    FunctionInfo* fi = current();
    if (fi) fi->return_type = type;
}

void ft_set_quad_start(int quad_start) {
    FunctionInfo* fi = current();
    if (fi) fi->quad_start = quad_start;
}

void ft_end(int quad_end) {
    FunctionInfo* fi = current();
    if (fi) fi->quad_end = quad_end;
    if (g_stack_top >= 0) g_stack_top--;
}

FunctionInfo* ft_find(const char* name) {
    for (int i = 0; i < g_function_count; i++) {
        if (strcmp(g_functions[i].name, name) == 0) return &g_functions[i];
    }
    return NULL;
}

FunctionInfo* ft_current(void) { return current(); }

FunctionInfo* ft_get_all(int* count) {
    if (count) *count = g_function_count;
    return g_functions;
}