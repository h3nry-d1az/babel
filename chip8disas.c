/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 */
#define BRED "\e[1;31m"
#define UWHT "\e[4;37m"
#define RES "\e[0m"

#define panic(...)                                                             \
    do                                                                         \
    {                                                                          \
        printf(__VA_ARGS__);                                                   \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#define Vx ((instr & 0x0F00) >> 8)
#define Vy ((instr & 0x00F0) >> 4)
#define addr (instr & 0x0FFF)
#define byte (instr & 0x00FF)
#define nibble (instr & 0x000F)

#define VALID_ADDRESS                                                          \
    (addr >= 0x200 && addr < fsize + 0x200 && get_label(labels, addr) &&       \
     !(addr & 1))

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __source
{
    uint16_t p;
    char data[1 << 15];
} source_t;

void append_line(source_t *f, char *line)
{
    for (uint16_t i = 0; line[i]; i++)
        f->data[f->p++] = line[i];
    f->data[f->p++] = '\n';
}

_Bool get_label(uint64_t *labels, uint16_t label)
{
    return (labels[label >> 6] & (1ull << (label & 63))) != 0;
}

void set_label(uint64_t *labels, uint16_t label)
{
    labels[label >> 6] |= (1ull << (label & 63));
}

void decompile(char *line, uint16_t instr, uint64_t *labels, size_t fsize)
{
    if (instr == 0x00E0)
    {
        strcpy(line, "cls");
        return;
    }

    else if (instr == 0x00EE)
    {
        strcpy(line, "ret");
        return;
    }

    switch (instr & 0xF000)
    {
    case 0x0000:
        sprintf(line, "sys x%03x", addr);
        return;

    case 0x1000:
        if (VALID_ADDRESS)
            sprintf(line, "jp @_i%03x", (addr - 0x200) >> 1);
        else
            sprintf(line, "jp x%03x", addr);
        return;

    case 0x2000:
        if (VALID_ADDRESS)
            sprintf(line, "call @_i%03x", (addr - 0x200) >> 1);
        else
            sprintf(line, "call x%03x", addr);
        return;

    case 0x3000:
        sprintf(line, "se v%x x%x", Vx, byte);
        return;

    case 0x4000:
        sprintf(line, "sne v%x x%x", Vx, byte);
        return;

    case 0x5000:
        sprintf(line, "se v%x v%x", Vx, Vy);
        return;

    case 0x6000:
        sprintf(line, "ld v%x x%x", Vx, byte);
        return;

    case 0x7000:
        sprintf(line, "add v%x x%x", Vx, byte);
        return;

    case 0x8000:
        switch (instr & 0x000F)
        {
        case 0:
            sprintf(line, "ld v%x v%x", Vx, Vy);
            return;

        case 1:
            sprintf(line, "or v%x v%x", Vx, Vy);
            return;

        case 2:
            sprintf(line, "and v%x v%x", Vx, Vy);
            return;

        case 3:
            sprintf(line, "xor v%x v%x", Vx, Vy);
            return;

        case 4:
            sprintf(line, "add v%x v%x", Vx, Vy);
            return;

        case 5:
            sprintf(line, "sub v%x v%x", Vx, Vy);
            return;

        case 6:
            sprintf(line, "shr v%x v%x", Vx, Vy);
            return;

        case 7:
            sprintf(line, "subn v%x v%x", Vx, Vy);
            return;

        case 0xE:
            sprintf(line, "shl v%x v%x", Vx, Vy);
            return;
        }
        break;

    case 0x9000:
        sprintf(line, "sne v%x v%x", Vx, Vy);
        return;

    case 0xA000:
        if (VALID_ADDRESS)
            sprintf(line, "ld I @_i%03x", (addr - 0x200) >> 1);
        else
            sprintf(line, "ld I x%03x", addr);
        return;

    case 0xB000:
        if (VALID_ADDRESS)
            sprintf(line, "jp v0 @_i%03x", (addr - 0x200) >> 1);
        else
            sprintf(line, "jp v0 x%03x", addr);
        return;

    case 0xC000:
        sprintf(line, "rnd v%x x%x", Vx, byte);
        return;

    case 0xD000:
        sprintf(line, "drw v%x v%x x%x", Vx, Vy, nibble);
        return;
    }

    switch (instr & 0xF0FF)
    {
    case 0xE09E:
        sprintf(line, "skp v%x", Vx);
        return;

    case 0xE0A1:
        sprintf(line, "sknp v%x", Vx);
        return;

    case 0xF007:
        sprintf(line, "ld v%x DT", Vx);
        return;

    case 0xF00A:
        sprintf(line, "ld v%x K", Vx);
        return;

    case 0xF015:
        sprintf(line, "ld DT v%x", Vx);
        return;

    case 0xF018:
        sprintf(line, "ld ST v%x", Vx);
        return;

    case 0xF01E:
        sprintf(line, "add I v%x", Vx);
        return;

    case 0xF029:
        sprintf(line, "ld F v%x", Vx);
        return;

    case 0xF033:
        sprintf(line, "ld B v%x", Vx);
        return;

    case 0xF055:
        sprintf(line, "ld [I] v%x", Vx);
        return;

    case 0xF065:
        sprintf(line, "ld v%x [I]", Vx);
        return;
    }

    sprintf(line, "x%04x", instr);
    return;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(BRED "FATAL ERROR:" RES " An input file must be provided.");
        return 1;
    }

    uint8_t rom[0x1000 - 0x200];
    uint16_t instr;
    static uint64_t labels[64];

    source_t disas = {.p = 0};
    char line[256];

    FILE *fptr = fopen(argv[1], "rb");
    size_t fsize = fread(rom, 1, 0x1000 - 0x200, fptr);
    if (!fsize)
        panic(BRED "RUNTIME ERROR:" RES " Could not read file \"%s\".",
              argv[1]);
    fclose(fptr);

    for (uint16_t i = 0; i < fsize; i += 2)
    {
        instr = (rom[i] << 8) + rom[i + 1];

        switch (instr & 0xF000)
        {
        case 0x1000:
        case 0x2000:
        case 0xA000:
        case 0xB000:
            set_label(labels, instr & 0x0FFF);
            break;
        }
    }

    for (uint16_t pc = 0x200; pc < fsize + 0x200; pc += 2)
    {
        if (get_label(labels, pc))
        {
            sprintf(line, "\n_i%03x:", (pc - 0x200) >> 1);
            append_line(&disas, line);
        }

        instr = (rom[pc - 0x200] << 8) + rom[pc - 0x1FF];

        decompile(line, instr, labels, fsize);

        disas.data[disas.p++] = ' ';
        disas.data[disas.p++] = ' ';
        disas.data[disas.p++] = ' ';
        disas.data[disas.p++] = ' ';
        append_line(&disas, line);
    }

    fptr = fopen((argc >= 3 ? argv[2] : "output.s"), "wb");
    fwrite(disas.data, 1, disas.p, fptr);
    fclose(fptr);

    return 0;
}