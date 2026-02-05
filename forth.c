/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 */
#ifndef STACK_SIZE
#define STACK_SIZE (1 << 8)
#endif

#define RED "\e[0;31m"
#define BRED "\e[1;31m"
#define GRN "\e[0;32m"
#define BGRN "\e[1;32m"
#define BLU "\e[0;34m"
#define BBLU "\e[1;34m"
#define UWHT "\e[4;37m"
#define RES "\e[0m"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define panic(...)                                                             \
    do                                                                         \
    {                                                                          \
        printf(__VA_ARGS__);                                                   \
        exit(1);                                                               \
    } while (0);

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

int32_t top(const stack_t *s)
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

_Bool streq(const string_t *s, const char *other, _Bool cs)
{
    char c;
    for (uint8_t j = 0; j < s->p; j++)
    {
        c = (cs ? s->data[j] : lower(s->data[j]));
        if (c != other[j])
            return false;
    }
    return true;
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

void f_eq(stack_t *);
void f_gt(stack_t *);
void f_geq(stack_t *);
void f_lt(stack_t *);
void f_leq(stack_t *);

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
void f_swap(stack_t *);

// No operation
void f_nop(stack_t *)
{
}

f_function_t instructions[] = {
    {"+", 1, &f_add},       {"-", 1, &f_sub},         {"*", 1, &f_mul},
    {"/", 1, &f_div},       {"mod", 3, &f_mod},       {"and", 3, &f_and},
    {"or", 2, &f_or},       {"negate", 3, &f_negate}, {"xor", 3, &f_xor},
    {"=", 1, &f_eq},        {">", 1, &f_gt},          {">=", 2, &f_geq},
    {"<", 1, &f_lt},        {"<=", 2, &f_leq},        {".", 1, &f_print},
    {"emit", 4, &f_emit},   {"cr", 2, &f_cr},         {"drop", 4, &f_drop},
    {"2drop", 5, &f_2drop}, {"dup", 3, &f_dup},       {"nip", 3, &f_nip},
    {"swap", 4, &f_swap},   {"\0", 0, &f_nop}};

void next(string_t *instr, stack_t *dstack, uint32_t *t, char *pgm)
{
    instr->p = 0;
    for (; pgm[*t]; (*t)++)
    {
        if (pgm[*t] != ' ' && pgm[*t] != '\n' && pgm[*t] != '\t')
        {
            instr->data[instr->p++] = pgm[*t];
            continue;
        }

        if (instr->p == 1)
        {
            if (instr->data[0] == '\\')
            {
                for (; pgm[*t] != '\n'; (*t)++)
                    ;
                next(instr, dstack, t, pgm);
                goto cleanup;
            }
            else if (instr->data[0] == '(')
            {
                for (; pgm[*t] != ')'; (*t)++)
                    ;
                (*t)++;
                next(instr, dstack, t, pgm);
                goto cleanup;
            }
        }

        if (instr->p == 0)
        {
            (*t)++;
            next(instr, dstack, t, pgm);
            return;
        }

    cleanup:
        instr->data[instr->p] = '\0';
#ifdef DEBUG
        printf("\n");
        printf("INSTRUCTION: %s\tSIZE: %d\n", instr->data, instr->p);
        for (int32_t pp = 0; pp < dstack->p; pp++)
            printf("-> %d ", dstack->stack[pp]);
#endif
        break;
    }
}

_Bool execute(string_t *instr, stack_t *dstack, uint32_t *t, char *pgm)
{
    if (instr->p == 0)
        return true;

    if (instr->p == 2)
    {
        if (streq(instr, ".\"", false))
        {
            string_t s = {.p = 0};
            while (pgm[++(*t)] != '"')
                s.data[s.p++] = pgm[*t];
            printf("%s", s.data);
            (*t)++;
            return true;
        }
        else if (streq(instr, ".(", false))
        {
            string_t s = {.p = 0};
            while (pgm[++(*t)] != ')')
                s.data[s.p++] = pgm[*t];
            printf("%s", s.data);
            (*t)++;
            return true;
        }
    }

    if (instr->p == 2 && streq(instr, "if", false))
    {
        _Bool state = pop(dstack) != 0, else_ = true;
        int8_t l = 1;
        while (l)
        {
#ifdef DEBUG
            printf("\nSTATE: %d\tCAN ELSE? %d\n", state, else_);
#endif
            next(instr, dstack, t, pgm);

            if (streq(instr, "then", false))
                l--;

            if (l == 1 && streq(instr, "else", false))
            {
                if (!else_)
                    return 1;
                state = !state;
                else_ = false;
            }

            if (state)
            {
                execute(instr, dstack, t, pgm);
            }
            else
            {
                if (streq(instr, "if", false))
                    l++;
            }

            if (l && !pgm[*t])
            {
                panic("\n" BRED "PARSE ERROR:" RES " Expected \"" UWHT
                      "then" RES "\" but found EOF.");
            }
        }
        return true;
    }

    for (uint8_t i = 0; instructions[i].length; i++)
    {
        if (instructions[i].length != instr->p)
            continue;

        if (!streq(instr, instructions[i].identifier, false))
            continue;

        instructions[i].callable(dstack);
        return true;
    }

    if (instr->data[0] != '-' &&
        !('0' <= instr->data[0] && instr->data[0] <= '9'))
        return false;

    for (uint8_t i = 1; instr->data[i]; i++)
    {
        if ('0' > instr->data[i] || instr->data[i] > '9')
            return false;
    }

    push(dstack, atoi(instr->data));
    return true;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    stack_t dstack = {.p = 0}; //, rstack = {.p = 0};
    string_t instr = {.p = 0};

    char *pgm = argv[1];
    uint32_t t = 0;

    while (pgm[t])
    {
        next(&instr, &dstack, &t, pgm);

        if (!execute(&instr, &dstack, &t, pgm))
        {
            printf("\n" BRED "RUNTIME ERROR:" RES " Instruction \"" UWHT
                   "%s" RES "\" failed to execute.",
                   instr.data);
            return 1;
        }
    }

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

void f_eq(stack_t *s)
{
    push(s, (pop(s) == pop(s) ? 1 : 0));
}

void f_gt(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, (b > a ? 1 : 0));
}

void f_geq(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, (b >= a ? 1 : 0));
}

void f_lt(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, (b < a ? 1 : 0));
}

void f_leq(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, (b <= a ? 1 : 0));
}

// IO functions
void f_print(stack_t *s)
{
    printf("%d ", pop(s));
}

void f_emit(stack_t *s)
{
    printf("%c", (char)top(s));
}

void f_cr(stack_t *)
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

void f_swap(stack_t *s)
{
    int32_t a = pop(s), b = pop(s);
    push(s, a);
    push(s, b);
}