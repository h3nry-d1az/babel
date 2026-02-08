/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 * @cite See "Cowgod's Chip-8 Technical Reference v1.0"
 * (http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).
 */
#define RED "\e[0;31m"
#define BRED "\e[1;31m"
#define GRN "\e[0;32m"
#define BGRN "\e[1;32m"
#define BLU "\e[0;34m"
#define BBLU "\e[1;34m"
#define UWHT "\e[4;37m"
#define RES "\e[0m"

#define panic(...)                                                             \
    do                                                                         \
    {                                                                          \
        printf(__VA_ARGS__);                                                   \
        return 1;                                                              \
    } while (0);

#ifndef ROM_MAX_SIZE
#define ROM_MAX_SIZE ((1 << 16) - 1)
#endif

#define Vx (reg[(instr & 0x0F00) >> 8])
#define Vy (reg[(instr & 0x00F0) >> 4])
#define VF (reg[0xF])

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint16_t pop(uint16_t *s, uint8_t *sp)
{
    if (*sp == 0)
        panic("\n" BRED "RUNTIME ERROR:" RES " Stack empty.");
    return s[--(*sp)];
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    uint16_t pc = 0;
    uint16_t instr;
    uint8_t reg[16] = {0};
    uint16_t I = 0;
    uint8_t ram[4096] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };
    uint8_t sp = 0;
    uint16_t stack[16] = {0};
    uint8_t dt = 0; //, st = 0;
    uint64_t screen[32] = {0};

    uint16_t rom[ROM_MAX_SIZE] = {0};

    FILE *fptr = fopen(argv[1], "rb");
    if (!fread(rom, 2, ROM_MAX_SIZE, fptr))
        panic(RED "RUNTIME ERROR:" RES " Could not read file \"%s\".", argv[1]);
    fclose(fptr);

    srand(time(NULL));

    for (pc = 0; pc < ROM_MAX_SIZE; pc++)
    {
#ifdef DEBUG
        printf("\n%x\n", rom[pc]);
        for (uint8_t y = 0; y < 32; y++)
        {
            for (uint8_t x = 0; x < 64; x++)
                printf("%d",
                       (uint8_t)((screen[y] & (1ll << (63 - x))) >> (63 - x)));
            printf("\n");
        }
#endif
        instr = rom[pc];

        // 00E0 - CLS
        if (instr == 0x00E0)
        {
            for (uint8_t i = 0; i < 32; i++)
                screen[i] = 0;
        }

        // 00EE - RET
        else if (instr == 0x00EE)
            pc = pop(stack, &sp);

        switch (instr & 0xF000)
        {
        // 0nnn - SYS addr
        case 0:
            break;

        // 1nnn - JP addr
        case 0x1000:
            pc = instr & 0x0FFF;
            break;

        // 2nnn - CALL addr
        case 0x200:
            if (sp == 16)
                panic(BRED "RUNTIME ERROR:" RES " Stack limit exceeded (16).");
            stack[sp++] = pc;
            pc = instr & 0x0FFF;
            break;

        // 3xkk - SE Vx, byte
        case 0x3000:
            if (Vx == (instr & 0x00FF))
                pc++;
            break;

        // 4xkk - SNE Vx, byte
        case 0x4000:
            if (Vx != (instr & 0x00FF))
                pc++;
            break;

        // 5xy0 - SE Vx, Vy
        case 0x5000:
            if (Vx == Vy)
                pc++;
            break;

        // 6xkk - LD Vx, byte
        case 0x6000:
            Vx = (instr & 0x00FF);
            break;

        // 7xkk - ADD Vx, byte
        case 0x7000:
            Vx += (instr & 0x00FF);
            break;

        case 0x8000:
            switch (instr & 0x000F)
            {
            // 8xy0 - LD Vx, Vy
            case 0:
                Vx = Vy;
                break;

            // 8xy1 - OR Vx, Vy
            case 1:
                Vx |= Vy;
                break;

            // 8xy2 - AND Vx, Vy
            case 2:
                Vx &= Vy;
                break;

            // 8xy3 - XOR Vx, Vy
            case 3:
                Vx ^= Vy;
                break;

            // 8xy4 - ADD Vx, Vy
            case 4:
                VF = Vy > (255 - Vx);
                Vx += Vy;
                break;

            // 8xy5 - SUB Vx, Vy
            case 5:
                VF = Vx > Vy;
                Vx -= Vy;
                break;

            // 8xy6 - SHR Vx {, Vy}
            case 6:
                VF = Vx & 1;
                Vx >>= 1;
                break;

            // 8xy7 - SUBN Vx, Vy
            case 7:
                VF = Vy > Vx;
                Vx = Vy - Vx;
                break;

            // 8xyE - SHL Vx {, Vy}
            case 0xE:
                VF = (Vx & 0x80 ? 1 : 0);
                Vx <<= 1;
                break;
            }
            break;

        // 9xy0 - SNE Vx, Vy
        case 0x9000:
            if (Vx != Vy)
                pc++;
            break;

        // Annn - LD I, addr
        case 0xA000:
            I = instr & 0x0FFF;
            break;

        // Bnnn - JP V0, addr
        case 0xB000:
            pc = 0x0FFF + reg[0];
            break;

        // Cxkk - RND Vx, byte
        case 0xC000:
            Vx = ((uint8_t)rand()) & (instr & 0x00FF);
            break;

        // Dxyn - DRW Vx, Vy, nibble
        case 0xD000:
            uint8_t px = Vx & 0x3F, py = Vy & 0x1F;
            uint8_t k = px + 8 - 64;
            uint64_t b, b1, b2;
            for (uint8_t i = 0; i < (instr & 0x000F); i++)
            {
                b = ram[I + i];
                b1 = b << (64 - 8) >> px;
                if (px > 64 - 8)
                    b2 = (b & ((1 << k) - 1)) << (64 - k);
                else
                    b2 = 0;
                b = b1 ^ b2;

                if (screen[py] & b)
                    VF = 1;

                screen[py] ^= (b1 ^ b2);
                py = (py + 1) & 0x1F;
            }
            // TODO:
            //     Paint this onto the screen.
            break;
        }

        switch (instr & 0xF0FF)
        {
        // Ex9E - SKP Vx
        case 0xE09E:
            // TODO: implement this
            break;

        // ExA1 - SKNP Vx
        case 0xE0A1:
            // TODO: implement this
            break;

        // Fx07 - LD Vx, DT
        case 0xF007:
            Vx = dt;
            break;

        // Fx0A - LD Vx, K
        case 0xF00A:
            // TODO: implement this
            break;

        // Fx15 - LD DT, Vx
        case 0xF015:
            dt = Vx;
            break;

        // Fx18 - LD ST, Vx
        case 0xF018:
            // st = Vx;
            break;

        // Fx1E - ADD I, Vx
        case 0xF01E:
            I += Vx;
            break;

        // Fx29 - LD F, Vx
        case 0xF029:
            I = 5 * (Vx & 0xF);
            break;

        // Fx33 - LD B, Vx
        case 0xF033:
            ram[I] = Vx / 100;
            ram[I + 1] = (Vx / 10) % 10;
            ram[I + 2] = Vx % 10;
            break;

        // Fx55 - LD [I], Vx
        case 0xF055:
            for (uint8_t i = 0; i <= ((instr & 0x0F00) >> 8); i++)
                ram[I + i] = reg[i];
            break;

        // Fx65 - LD Vx, [I]
        case 0xF065:
            for (uint8_t i = 0; i <= ((instr & 0x0F00) >> 8); i++)
                reg[i] = ram[I + i];
            break;
        }
    }

    return 0;
}