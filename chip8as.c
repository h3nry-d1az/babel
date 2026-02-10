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

#define UNDEFINED_INSTANCE                                                     \
    panic(lineno,                                                              \
          "Undefined instance of the instruction \"" UWHT "%s" RES "\".",      \
          instr[0].name)

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
#define MAX_PARAMS 4
typedef struct __word instruction_t[MAX_PARAMS];

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

uint16_t __dec_digit(uint16_t lineno, char c)
{
    if (c < '0' || c > '9')
        panic(lineno, UWHT "%c" RES " is not a valid base-10 digit.", c);
    return c - '0';
}

uint16_t parse_number(uint16_t lineno, const struct __word *lit, uint32_t max,
                      const char *err)
{
    uint32_t n = 0;

    if (lower(lit->name[0]) == 'x')
    {
        if (lit->p < 2)
            panic(lineno, "Hexadecimal literals must be non-empty.");
        for (uint8_t i = 1; i < lit->p; i++)
            n = (n << 4) + __hex_digit(lineno, lit->name[i]);
    }
    else
    {
        for (uint8_t i = 0; i < lit->p; i++)
            n = 10 * n + __dec_digit(lineno, lit->name[i]);
    }

    if (n > max)
        panic(lineno, "%s", err);

    return (uint16_t)n;
}

#define parse_instr(lineno, lit)                                               \
    parse_number(lineno, lit, 0xFFFF,                                          \
                 "Raw data must be divided into 16-bit "                       \
                 "chunks, i.e. of the form xNNNN")
#define parse_address(lineno, lit)                                             \
    parse_number(lineno, lit, 0xFFF,                                           \
                 "Addresses must be 12-bits long, i.e. of "                    \
                 "the form xNNN.")
#define parse_byte(lineno, lit)                                                \
    parse_number(lineno, lit, 0xFF,                                            \
                 "Bytes must be 8-bits long, i.e. of the form xNN.")
#define parse_nibble(lineno, lit)                                              \
    parse_number(lineno, lit, 0xF,                                             \
                 "Nibbles must be 4-bits long, i.e. of the form xN.")

uint16_t fetch_address(uint16_t lineno, const struct __word *lit,
                       const struct __word *instr, const asm_label_list_t *l)
{
    if (lit->p < 2)
        panic(lineno,
              "The instruction \"" UWHT "%s" RES
              "\" must receive an address or subroutine as "
              "argument.",
              instr->name);

    if (lit->name[0] == '@')
        return fetch_label(lineno, l, lit->name + 1);
    else
        return parse_address(lineno, lit);
}

uint16_t fetch_register(uint16_t lineno, const struct __word *lit)
{
    if (lower(lit->name[0]) != 'v')
        panic(lineno,
              "Registers must start with " UWHT "'V'" RES ", e.g: V0, VA, ...");
    if (lit->p != 2)
        panic(lineno, "Registers must be of the form" UWHT "'VX'" RES
                      ", where X ranges from 0 to F.");
    return __hex_digit(lineno, lit->name[1]);
}

#define Vx (fetch_register(lineno, instr + 1) << 8)
#define Vy (fetch_register(lineno, instr + 2) << 4)

uint16_t compile(uint16_t lineno, instruction_t instr,
                 const asm_label_list_t *l)
{
    if (streqlc(instr, "cls"))
        return 0x00E0;

    else if (streqlc(instr, "ret"))
        return 0x00EE;

    else if (streqlc(instr, "sys"))
        return fetch_address(lineno, instr + 1, instr, l);

    else if (streqlc(instr, "jp"))
    {
        if (streqlc(instr + 1, "v0"))
            return 0xB000 + fetch_address(lineno, instr + 2, instr, l);
        return 0x1000 + fetch_address(lineno, instr + 1, instr, l);
    }

    else if (streqlc(instr, "call"))
        return 0x2000 + fetch_address(lineno, instr + 1, instr, l);

    else if (streqlc(instr, "se"))
    {
        if (lower(instr[2].name[0]) == 'v')
            return 0x5000 + Vx + Vy;
        return 0x3000 + Vx + parse_byte(lineno, instr + 2);
    }

    else if (streqlc(instr, "sne"))
    {
        if (lower(instr[2].name[0]) == 'v')
            return 0x9000 + Vx + Vy;
        return 0x4000 + Vx + parse_byte(lineno, instr + 2);
    }

    else if (streqlc(instr, "add"))
    {
        if (lower(instr[1].name[0]) == 'i')
        {
            if (instr[1].p == 1)
                return 0xF01E + (fetch_register(lineno, instr + 2) << 8);
            UNDEFINED_INSTANCE;
        }

        if (lower(instr[2].name[0]) == 'v')
            return 0x8004 + Vx + Vy;

        return 0x7000 + Vx + parse_byte(lineno, instr + 2);
    }

    else if (streqlc(instr, "or"))
        return 0x8001 + Vx + Vy;

    else if (streqlc(instr, "and"))
        return 0x8002 + Vx + Vy;

    else if (streqlc(instr, "xor"))
        return 0x8003 + Vx + Vy;

    else if (streqlc(instr, "sub"))
        return 0x8005 + Vx + Vy;

    else if (streqlc(instr, "shr"))
        return 0x8006 + Vx;

    else if (streqlc(instr, "subn"))
        return 0x8007 + Vx + Vy;

    else if (streqlc(instr, "shl"))
        return 0x800E + Vx;

    else if (streqlc(instr, "rnd"))
        return 0xC000 + Vx + parse_byte(lineno, instr + 2);

    else if (streqlc(instr, "drw"))
        return 0xD000 + Vx + Vy + parse_nibble(lineno, instr + 3);

    else if (streqlc(instr, "skp"))
        return 0xE09E + Vx;

    else if (streqlc(instr, "sknp"))
        return 0xE0A1 + Vx;

    else if (streqlc(instr, "ld"))
    {
        switch (instr[1].p)
        {
        case 1:
            switch (lower(instr[1].name[0]))
            {
            case 'i':
                return 0xA000 + fetch_address(lineno, instr + 2, instr, l);
            case 'f':
                return 0xF029 + (fetch_register(lineno, instr + 2) << 8);
            case 'b':
                return 0xF033 + (fetch_register(lineno, instr + 2) << 8);
            }
            break;
        case 2:
            switch (lower(instr[1].name[0]))
            {
            case 'd':
                if (!streqlc(instr + 1, "dt"))
                    goto undefined_ld;
                return 0xF015 + (fetch_register(lineno, instr + 2) << 8);
            case 's':
                if (!streqlc(instr + 1, "st"))
                    goto undefined_ld;
                return 0xF018 + (fetch_register(lineno, instr + 2) << 8);
            case 'v':
                switch (lower(instr[2].name[0]))
                {
                case 'v':
                    return 0x8000 + Vx + Vy;
                case 'd':
                    if (!streqlc(instr + 2, "dt"))
                        goto undefined_ld;
                    return 0xF007 + Vx;
                case 'k':
                    if (!streqlc(instr + 2, "k"))
                        goto undefined_ld;
                    return 0xF00A + Vx;
                case '[':
                    if (!streqlc(instr + 2, "[i]"))
                        goto undefined_ld;
                    return 0xF055 + Vx;
                default:
                    return 0x6000 + Vx + parse_byte(lineno, instr + 2);
                }
            }
            break;
        case 3:
            if (streqlc(instr + 1, "[i]"))
                return 0xF055 + (fetch_register(lineno, instr + 2) << 8);
        }

    undefined_ld:
        UNDEFINED_INSTANCE;
    }

    else if (lower(instr[0].name[0]) == 'x')
        return parse_instr(lineno, instr);

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
            struct __asm_label lb = {.addr = pc};
            uint8_t idx;
            for (idx = 0; tok[idx] != ' ' && tok[idx] != ':' &&
                          tok[idx] != '\n' && tok[idx] != '\r';
                 idx++)
                lb.name[idx] = tok[idx];
            lb.name[idx + 1] = '\0';
            append_label(&labels, lb);
            continue;
        }

        instr[0].p = 0, instr[1].p = 0, instr[2].p = 0;
        uint8_t wordno = 0;
        for (uint8_t i = 0; tok[i] && tok[i] != '\n' && wordno < MAX_PARAMS;
             i++)
        {
            if (tok[i] == '\r')
                continue;

            if (tok[i] == ' ' || tok[i] == '\t' || tok[i] == ';')
            {
                if (instr[wordno].p != 0)
                {
                    instr[wordno].name[instr[wordno].p] = '\0';
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
        printf("Line %d: [%s | %s | %s | %s]\n", lineno, instr[0].name,
               instr[1].name, instr[2].name, instr[3].name);
#endif

        append_instr(&rom, compile(lineno, instr, &labels));
        pc += 2;
    }

    fp = fopen((argc > 2 ? argv[2] : "output.ch8"), "wb");
    fwrite(rom.rom, 1, rom.p, fp);
    fclose(fp);

    return 0;
}