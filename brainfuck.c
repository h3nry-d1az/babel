/**
 * @author Henry Díaz Bordón
 * @version 1.0.0
 * @note Huge thanks to Kyryl Shyshko for his clever insights and bug-fixing.
 */
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 30000
#endif

#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    uint8_t mem[MEMORY_SIZE] = {0};
    uint32_t p = 0;

    uint32_t stack[1 << 8];
    uint32_t sp = 0;

    char *pgm = argv[1];

    for (uint32_t pc = 0; pgm[pc]; pc++)
    {
        switch (pgm[pc])
        {
        case '+':
            mem[p]++;
            break;

        case '-':
            mem[p]--;
            break;

        case '>':
            p++;
            break;

        case '<':
            p--;
            break;

        case '.':
            printf("%c", mem[p]);
            break;

        case ',':
            scanf("%c", mem + p);
            break;

        case '[':
            if (!mem[p])
            {
                uint32_t l = 0;
                do
                {
                    if (pgm[pc] == '[')
                        l++;
                    if (pgm[pc] == ']')
                        l--;
                    pc++;
                } while (l);
                pc--;
            }
            else
                stack[sp++] = pc - 1;
            break;

        case ']':
            sp--;
            if (mem[p])
                pc = stack[sp];
            break;
        }
    }

    return 0;
}