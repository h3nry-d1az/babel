/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 */
#define BRED "\e[1;31m"
#define UWHT "\e[4;37m"
#define RES "\e[0m"

#define panic(i, ...)                                                          \
    do                                                                         \
    {                                                                          \
        printf(BRED "FATAL ERROR" RES " at line " UWHT "%d" RES ": ", (i));    \
        printf(__VA_ARGS__);                                                   \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct __asm_label
{
    char name[256];
    uint16_t addr;
};

typedef struct __asm_label_list
{
    struct __asm_label labels[256];
    uint8_t p;
} asm_label_list_t;

uint16_t fetch_label(uint16_t lineno, const asm_label_list_t *l,
                     const char *identifier)
{
    for (uint8_t i = 0; i < l->p; i++)
    {
        if (!strcmp(identifier, l->labels[i].name))
            return l->labels[i].addr;
    }
    panic(lineno, "Label \"" UWHT "%s" RES "\" has not been declared.",
          identifier);
}

void append_label(asm_label_list_t *l, struct __asm_label lb)
{
    l->labels[l->p++] = lb;
}

typedef struct __chip8_rom
{
    uint8_t rom[0x1000 - 0x200];
    uint16_t p;
} chip8_rom_t;

void append_instr(chip8_rom_t *c8r, uint16_t instr)
{
    c8r->rom[c8r->p++] = (instr & 0xFF00) >> 8;
    c8r->rom[c8r->p++] = (instr & 0x00FF);
}

struct __word
{
    char name[256];
    uint8_t p;
};
typedef struct __word instruction_t[3];

char lower(char c)
{
    if ('A' <= c && c <= 'Z')
        return c + ('a' - 'A');
    return c;
}

_Bool streqlc(const struct __word *w, char *s)
{
    for (uint8_t i = 0; i < w->p; i++)
    {
        if (lower(w->name[i]) != s[i])
            return false;
    }
    return true;
}

uint16_t __hex_digit(uint16_t lineno, char c)
{
    switch (c)
    {
    case '0' ... '9':
        return c - '0';
    case 'A' ... 'F':
        return c - 'A' + 10;
    default:
        panic(lineno, UWHT "%c" RES " is not a valid hexadecimal digit.", c);
    }
}

uint16_t hex(uint16_t lineno, const struct __word *lit)
{
    if (lit->p != 3)
        panic(lineno,
              "Addresses must be 12-bits long, i.e. of the form 0xNNN.");
    uint16_t n = 0;
    n = __hex_digit(lineno, lit->name[0]);
    n = (n << 4) + __hex_digit(lineno, lit->name[1]);
    n = (n << 4) + __hex_digit(lineno, lit->name[2]);
    return n;
}

uint16_t compile(uint16_t lineno, instruction_t instr,
                 const asm_label_list_t *l)
{
    if (streqlc(instr, "cls"))
        return 0x00E0;

    else if (streqlc(instr, "ret"))
        return 0x00EE;

    else if (streqlc(instr, "sys"))
    {
        if (instr[1].p < 2)
            panic(lineno,
                  "The instruction \"" UWHT "%s" RES
                  "\" must receive an address or subroutine as argument.",
                  instr[0].name);

        if (instr[1].name[0] == '@')
            return fetch_label(lineno, l, instr[1].name + 1);
        else
            return hex(lineno, instr + 1);
    }

    panic(lineno,
          "Unknown instruction: \"" UWHT "%s" RES
          "\". Cannot proceed with compilation.",
          instr[0].name);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(BRED "FATAL ERROR:" RES " An input file must be provided.");
        return 1;
    }

    chip8_rom_t rom = {.p = 0};
    asm_label_list_t labels = {.p = 0};

    char *pgm = (char *)malloc(1 << 14), *tok;
    instruction_t instr;
    uint16_t lineno, pc = 0x200;

    FILE *fp = fopen(argv[1], "r");
    size_t _nr = fread(pgm, 1, 1 << 14, fp);
    pgm[_nr] = '\0';
    fclose(fp);

    uint8_t indentation;
    for (lineno = 1, tok = strtok(pgm, "\n"); tok;
         lineno++, tok = strtok(NULL, "\n"))
    {
        for (indentation = 0;
             tok[indentation] == ' ' || tok[indentation] == '\t'; indentation++)
            ;

        if (indentation == 0 && tok[0] != ';')
        {
            // TODO: raw bytes

            struct __asm_label lb = {.addr = pc};
            uint8_t idx;
            for (idx = 0; tok[idx] != ' ' && tok[idx] != ':'; idx++)
                lb.name[idx] = tok[idx];
            lb.name[idx + 1] = '\0';
            append_label(&labels, lb);
            continue;
        }

        instr[0].p = 0, instr[1].p = 0, instr[2].p = 0;
        uint8_t wordno = 0;
        for (uint8_t i = 0; tok[i] && tok[i] != '\n' && wordno < 3; i++)
        {
            if (tok[i] == '\r')
                continue;

            if (tok[i] == ' ' || tok[i] == '\t' || tok[i] == ';')
            {
                if (instr[wordno].p != 0)
                {
                    instr[wordno].name[instr[wordno].p++] = '\0';
                    wordno++;
                }

                if (tok[i] == ';')
                    goto next_line;
                else
                    continue;
            }
            instr[wordno].name[instr[wordno].p++] = tok[i];
        }
        instr[wordno].name[instr[wordno].p] = '\0';

    next_line:
        if (instr[0].p == 0)
            continue;

#ifdef DEBUG
        printf("Line %d: [%s | %s | %s]\n", lineno, instr[0].name,
               instr[1].name, instr[2].name);
#endif

        append_instr(&rom, compile(lineno, instr, &labels));
        pc += 2;
    }

    fp = fopen((argc > 2 ? argv[2] : "output.ch8"), "wb");
    fwrite(rom.rom, 1, rom.p, fp);
    fclose(fp);

    return 0;
}