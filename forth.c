/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 */
#ifndef STACK_SIZE
#define STACK_SIZE (1 << 8)
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __stack
{
    int32_t stack[STACK_SIZE];
    int32_t p;
} stack_t;

void push(stack_t *s, int32_t v)
{
    s->stack[s->p++] = v;
}

int32_t pop(stack_t *s)
{
    return s->stack[--s->p];
}

int32_t top(stack_t *s)
{
    return s->stack[s->p - 1];
}

typedef struct __string
{
    char data[256];
    uint8_t p;
} string_t;

char lower(char c)
{
    if ('A' <= c && c <= 'Z')
        return c + ('a' - 'A');
    return c;
}

typedef struct __f_function
{
    char identifier[256];
    uint8_t length;
    void (*callable)(stack_t *);
} f_function_t;

// Arithmetic/logic functions
void f_add(stack_t *);
void f_sub(stack_t *);
void f_mul(stack_t *);
void f_div(stack_t *);
void f_mod(stack_t *);

void f_and(stack_t *);
void f_or(stack_t *);
void f_negate(stack_t *);
void f_xor(stack_t *);

// IO functions
void f_print(stack_t *);
void f_emit(stack_t *);
void f_cr(stack_t *);

// Stack manipulation
void f_drop(stack_t *);
void f_2drop(stack_t *);
void f_dup(stack_t *);
// void f_rot(stack_t *);
void f_nip(stack_t *);
// void f_tuck(stack_t *);

// No operation
void f_nop(stack_t *)
{
}

f_function_t instructions[] = {
    {"+", 1, &f_add},     {"-", 1, &f_sub},         {"*", 1, &f_mul},
    {"/", 1, &f_div},     {"mod", 3, &f_mod},       {"and", 3, &f_and},
    {"or", 2, &f_or},     {"negate", 3, &f_negate}, {"xor", 3, &f_xor},
    {".", 1, &f_print},   {"emit", 4, &f_emit},     {"cr", 2, &f_cr},
    {"drop", 4, &f_drop}, {"2drop", 5, &f_2drop},   {"dup", 3, &f_dup},
    {"nip", 3, &f_nip},   {"\0", 0, &f_nop}};

void execute(string_t instr, stack_t *dstack)
{
    for (uint8_t i = 0; instructions[i].length; i++)
    {
        if (instructions[i].length != instr.p)
            continue;

        for (uint8_t j = 0; j < instr.p; j++)
        {
            if (lower(instr.data[j]) != instructions[i].identifier[j])
                goto next_instruction;
        }

        instructions[i].callable(dstack);
        return;

    next_instruction:;
    }

    instr.data[instr.p] = '\0';
    push(dstack, atoi(instr.data));
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    stack_t dstack = {.p = 0}; //, rstack = {.p = 0};
    string_t instr = {.p = 0}; //, ptext = {.p = 0};

    char *pgm = argv[1];

    for (uint32_t t = 0; pgm[t]; t++)
    {
        if (pgm[t] != ' ' && pgm[t] != '\n' && pgm[t] != '\t')
        {
            instr.data[instr.p++] = pgm[t];
            continue;
        }

        if (instr.p == 0)
            continue;

#ifdef DEBUG
        printf("\n");
        for (int32_t pp = 0; pp < dstack.p; pp++)
            printf("-> %d ", dstack.stack[pp]);
#endif

        if (instr.p == 1)
        {
            if (instr.data[0] == '\\')
            {
                for (; pgm[t] != '\n'; t++)
                    ;
                instr.p = 0;
                continue;
            }
            else if (instr.data[0] == '(')
            {
                for (; pgm[t] != ')'; t++)
                    ;
                t++;
                instr.p = 0;
                continue;
            }
        }

        execute(instr, &dstack);
        instr.p = 0;
    }

    execute(instr, &dstack);

    return 0;
}

// Arithmetic/logic functions
void f_add(stack_t *s)
{
    push(s, pop(s) + pop(s));
}

void f_sub(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, b - a);
}

void f_mul(stack_t *s)
{
    push(s, pop(s) * pop(s));
}

void f_div(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, b / a);
}

void f_mod(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, b % a);
}

void f_and(stack_t *s)
{
    push(s, pop(s) & pop(s));
}

void f_or(stack_t *s)
{
    push(s, pop(s) | pop(s));
}

void f_negate(stack_t *s)
{
    push(s, -pop(s));
}

void f_xor(stack_t *s)
{
    push(s, pop(s) ^ pop(s));
}

// IO functions
void f_print(stack_t *s)
{
    printf("%d\n", top(s));
}

void f_emit(stack_t *s)
{
    printf("%c", (char)top(s));
}

void f_cr(stack_t *s)
{
    printf("\n");
}

// Stack manipulation
void f_drop(stack_t *s)
{
    pop(s);
}

void f_2drop(stack_t *s)
{
    pop(s);
    pop(s);
}

void f_dup(stack_t *s)
{
    push(s, top(s));
}

// void f_rot(stack_t *s)
// {
// }

void f_nip(stack_t *s)
{
    int32_t x = pop(s);
    pop(s);
    push(s, x);
}

// void f_tuck(stack_t *s)
// {
// }