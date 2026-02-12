/**
 * @author Henry Díaz Bordón
 * @version 0.1.0
 */
#define BRED "\e[1;31m"
#define UWHT "\e[4;37m"
#define RES "\e[0m"

#define panic(i, fn, ...)                                                      \
    do                                                                         \
    {                                                                          \
        printf(BRED "FATAL ERROR" RES " in file " UWHT "%s" RES                \
                    " at line " UWHT "%d" RES ": ",                            \
               (fn), (i));                                                     \
        printf(__VA_ARGS__);                                                   \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#define UNDEFINED_INSTANCE                                                     \
    panic(lineno, filename,                                                    \
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

uint16_t fetch_label(uint16_t lineno, const char *filename,
                     const asm_label_list_t *l, const char *identifier)
{
    for (uint8_t i = 0; i < l->p; i++)
    {
        if (!strcmp(identifier, l->labels[i].name))
            return l->labels[i].addr;
    }
    panic(lineno, filename,
          "Label \"" UWHT "%s" RES "\" has not been declared.", identifier);
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

char *advance(char *tok)
{
    while (*tok && *(tok++) != '\n')
        ;

    if (!(*tok))
        return NULL;
    return tok;
}

uint16_t __hex_digit(uint16_t lineno, const char *filename, char c)
{
    switch (lower(c))
    {
    case '0' ... '9':
        return c - '0';
    case 'a' ... 'f':
        return lower(c) - 'a' + 10;
    default:
        panic(lineno, filename,
              UWHT "%c" RES " is not a valid hexadecimal digit.", c);
    }
}

uint16_t __bin_digit(uint16_t lineno, const char *filename, char c)
{
    if (c == '0')
        return 0;
    if (c == '1')
        return 1;
    panic(lineno, filename, UWHT "%c" RES " is not a valid binary digit.", c);
}

uint16_t __dec_digit(uint16_t lineno, const char *filename, char c)
{
    if (c < '0' || c > '9')
        panic(lineno, filename, UWHT "%c" RES " is not a valid base-10 digit.",
              c);
    return c - '0';
}

uint16_t parse_number(uint16_t lineno, const char *filename,
                      const struct __word *lit, uint32_t max, const char *err)
{
    uint32_t n = 0;

    switch (lower(lit->name[0]))
    {
    case 'x':
        if (lit->p < 2)
            panic(lineno, filename, "Hexadecimal literals must be non-empty.");
        for (uint8_t i = 1; i < lit->p; i++)
            n = (n << 4) + __hex_digit(lineno, filename, lit->name[i]);
        break;

    case 'b':
        if (lit->p < 2)
            panic(lineno, filename, "Binary literals must be non-empty.");
        for (uint8_t i = 1; i < lit->p; i++)
            n = (n << 1) + __bin_digit(lineno, filename, lit->name[i]);
        break;

    default:
        for (uint8_t i = 0; i < lit->p; i++)
            n = 10 * n + __dec_digit(lineno, filename, lit->name[i]);
        break;
    }

    if (n > max)
        panic(lineno, filename, "%s", err);

    return (uint16_t)n;
}

#define parse_instr(lineno, filename, lit)                                     \
    parse_number(lineno, filename, lit, 0xFFFF,                                \
                 "Raw data must be divided into 16-bit "                       \
                 "chunks, i.e. of the form xNNNN")
#define parse_address(lineno, filename, lit)                                   \
    parse_number(lineno, filename, lit, 0xFFF,                                 \
                 "Addresses must be 12-bits long, i.e. of "                    \
                 "the form xNNN.")
#define parse_byte(lineno, filename, lit)                                      \
    parse_number(lineno, filename, lit, 0xFF,                                  \
                 "Bytes must be 8-bits long, i.e. of the form xNN.")
#define parse_nibble(lineno, filename, lit)                                    \
    parse_number(lineno, filename, lit, 0xF,                                   \
                 "Nibbles must be 4-bits long, i.e. of the form xN.")

uint16_t fetch_address(uint16_t lineno, const char *filename,
                       const struct __word *lit, const struct __word *instr,
                       const asm_label_list_t *l)
{
    if (lit->p < 2)
        panic(lineno, filename,
              "The instruction \"" UWHT "%s" RES
              "\" must receive an address or subroutine as "
              "argument.",
              instr->name);

    if (lit->name[0] == '@')
        return fetch_label(lineno, filename, l, lit->name + 1);
    else
        return parse_address(lineno, filename, lit);
}

uint16_t fetch_register(uint16_t lineno, const char *filename,
                        const struct __word *lit)
{
    if (lower(lit->name[0]) != 'v')
        panic(lineno, filename,
              "Registers must start with " UWHT "'V'" RES ", e.g: V0, VA, ...");
    if (lit->p != 2)
        panic(lineno, filename,
              "Registers must be of the form" UWHT "'VX'" RES
              ", where X ranges from 0 to F.");
    return __hex_digit(lineno, filename, lit->name[1]);
}

#define Vx (fetch_register(lineno, filename, instr + 1) << 8)
#define Vy (fetch_register(lineno, filename, instr + 2) << 4)

uint16_t compile(uint16_t lineno, const char *filename, instruction_t instr,
                 const asm_label_list_t *l)
{
    if (streqlc(instr, "cls"))
        return 0x00E0;

    else if (streqlc(instr, "ret"))
        return 0x00EE;

    else if (streqlc(instr, "sys"))
        return fetch_address(lineno, filename, instr + 1, instr, l);

    else if (streqlc(instr, "jp"))
    {
        if (streqlc(instr + 1, "v0"))
            return 0xB000 +
                   fetch_address(lineno, filename, instr + 2, instr, l);
        return 0x1000 + fetch_address(lineno, filename, instr + 1, instr, l);
    }

    else if (streqlc(instr, "call"))
        return 0x2000 + fetch_address(lineno, filename, instr + 1, instr, l);

    else if (streqlc(instr, "se"))
    {
        if (lower(instr[2].name[0]) == 'v')
            return 0x5000 + Vx + Vy;
        return 0x3000 + Vx + parse_byte(lineno, filename, instr + 2);
    }

    else if (streqlc(instr, "sne"))
    {
        if (lower(instr[2].name[0]) == 'v')
            return 0x9000 + Vx + Vy;
        return 0x4000 + Vx + parse_byte(lineno, filename, instr + 2);
    }

    else if (streqlc(instr, "add"))
    {
        if (lower(instr[1].name[0]) == 'i')
        {
            if (instr[1].p == 1)
                return 0xF01E +
                       (fetch_register(lineno, filename, instr + 2) << 8);
            UNDEFINED_INSTANCE;
        }

        if (lower(instr[2].name[0]) == 'v')
            return 0x8004 + Vx + Vy;

        return 0x7000 + Vx + parse_byte(lineno, filename, instr + 2);
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
        return 0xC000 + Vx + parse_byte(lineno, filename, instr + 2);

    else if (streqlc(instr, "drw"))
        return 0xD000 + Vx + Vy + parse_nibble(lineno, filename, instr + 3);

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
                return 0xA000 +
                       fetch_address(lineno, filename, instr + 2, instr, l);
            case 'f':
                return 0xF029 +
                       (fetch_register(lineno, filename, instr + 2) << 8);
            case 'b':
                return 0xF033 +
                       (fetch_register(lineno, filename, instr + 2) << 8);
            }
            break;
        case 2:
            switch (lower(instr[1].name[0]))
            {
            case 'd':
                if (!streqlc(instr + 1, "dt"))
                    goto undefined_ld;
                return 0xF015 +
                       (fetch_register(lineno, filename, instr + 2) << 8);
            case 's':
                if (!streqlc(instr + 1, "st"))
                    goto undefined_ld;
                return 0xF018 +
                       (fetch_register(lineno, filename, instr + 2) << 8);
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
                    return 0xF065 + Vx;
                default:
                    return 0x6000 + Vx +
                           parse_byte(lineno, filename, instr + 2);
                }
            }
            break;
        case 3:
            if (streqlc(instr + 1, "[i]"))
                return 0xF055 +
                       (fetch_register(lineno, filename, instr + 2) << 8);
        }

    undefined_ld:
        UNDEFINED_INSTANCE;
    }

    else if (lower(instr[0].name[0]) == 'x' || lower(instr[0].name[0]) == 'b')
        return parse_instr(lineno, filename, instr);

    panic(lineno, filename,
          "Unknown instruction: \"" UWHT "%s" RES
          "\". Cannot proceed with compilation.",
          instr[0].name);
}

void compile_source(char* src, char* main_filename, chip8_rom_t *rom,
                  asm_label_list_t *labels, uint16_t *pc)
{

	char* filename;
	
    uint16_t lineno;
    instruction_t instr;
    char* tok = src;

    uint8_t indentation;
    
    uint16_t label_pc = *pc;
	
	char* filename_stack_arr[256];
	char** filename_stack = filename_stack_arr;

	size_t lineno_stack_arr[256];
	size_t* lineno_stack = lineno_stack_arr;

	*(filename_stack) = main_filename;
	filename = *filename_stack;
	
    for (lineno = 1; tok; lineno++, tok = advance(tok))
    {
        for (indentation = 0;
         tok[indentation] == ' ' || tok[indentation] == '\t'; indentation++)
        ;
		
		tok += indentation;
		
        if (tok[0] == ';' || tok[0] == '\n')
            continue;
        if(strncmp(tok, "%__file ", 8) == 0)
			continue;
        
        char* word_end_ptr = advance(tok);
        if(word_end_ptr == NULL){
            word_end_ptr = tok;
            while(*(word_end_ptr++));
        }
		word_end_ptr--;
        while(*word_end_ptr == '\t' || *word_end_ptr == ' ' || *word_end_ptr == '\n') word_end_ptr--;
        if(*word_end_ptr == ':'){
            struct __asm_label lb = {.addr = label_pc};
            uint8_t idx;
            for (idx = 0; tok[idx] != ' ' && tok[idx] != ':' && tok[idx] != '\n' &&
                          tok[idx] != '\r';
                 idx++)
                lb.name[idx] = tok[idx];
            lb.name[idx + 1] = '\0';
            *(tok) = ';'; // Cheeky comment the label (for example, Stop: -> ;top: )
            append_label(labels, lb);
            continue;
        }
        // Instruction line
        label_pc += 2;
    }
    
	#ifdef DEBUG
	printf("Labels:\n");
    for(size_t i = 0; i < labels->p; i++){
        printf("%s - %p\n", labels->labels[i].name, (void*)(size_t)labels->labels[i].addr);
    }
	printf("\n");
	#endif

    tok = src;
    for (lineno = 1; tok; lineno++, tok = advance(tok))
    {
        for (indentation = 0;
             tok[indentation] == ' ' || tok[indentation] == '\t'; indentation++)
            ;
			
		tok += indentation;
        
        if (tok[0] == ';' || tok[0] == '\n')
            continue;
			
		// Handle filename changes
		if(strncmp(tok, "%__file ", 8) == 0){
			if(tok[8] == '-'){
				filename = *(--filename_stack);
				lineno = *(--lineno_stack);
				lineno -= 1;
				continue;
			}
			size_t s = 0;
			while(tok[8 + s] != '\n') s++;
			filename = malloc(sizeof(char) * s);
			memcpy(filename, tok + 8, s);
			*(++filename_stack) = filename;
			
			*(lineno_stack++) = lineno;
			
			lineno = 0;
			
			continue;
		}
        
        instr[0].p = 0, instr[1].p = 0, instr[2].p = 0, instr[3].p = 0;
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

        append_instr(rom, compile(lineno, filename, instr, labels));
        *pc += 2;
    }
	
	
	while(filename_stack != (char**)filename_stack_arr){
		free(*filename_stack);
		filename_stack--;
	}
	
}




char* read_file(char filename[], size_t* filesize_ptr){
    FILE *f_ptr = fopen(filename, "r");
    
    if(f_ptr == NULL){
        return NULL;
    }
    
    fseek(f_ptr, 0L, SEEK_END);
    size_t filesize = ftell(f_ptr);
    fseek(f_ptr, 0L, SEEK_SET);
    
    char* src = (char*)malloc(filesize + 1);
    
    fread(src, 1, filesize, f_ptr);

    src[filesize] = '\0';
    
    if(filesize_ptr) *filesize_ptr = filesize;
    
    fclose(f_ptr);
    
    return src;
}



char* preprocess_source(char* src, char* filename){
    size_t len = 0;
    while(src[len]) len++;
    
    uint8_t indentation;
    uint16_t lineno;
    char* tok = src;

    
    for (lineno = 1; tok; lineno++, tok = advance(tok)){
        
        for (indentation = 0; tok[indentation] == ' ' || tok[indentation] == '\t'; indentation++)
			;
        
		tok += indentation;
        
        if (tok[0] == '#'){
            for (uint8_t i = 1; i <= 8; i++)
            {
                if (lower(tok[i]) != "#include "[i])
                    panic(lineno, "",
                          "Import statements must begin with" UWHT
                          "\"#include\"" RES ".");

                if (tok[9] != '\"')
                    panic(lineno, "",
                          "Files to include must be enclosed within quotes.");
            }

            char import_filename[256];
            uint8_t i;
            for (i = 10; tok[i] != '\"'; i++)
                import_filename[i - 10] = tok[i];
            
			size_t import_filename_size = i - 10;
			
            import_filename[i - 10] = '\0';
            
            size_t imported_size = 0;
            size_t skip_size = i;
            char* file_contents = read_file(import_filename, &imported_size);
            
			if(file_contents == NULL){
        		panic(lineno, filename,
              		"File " UWHT "%s" RES " is either empty or non-existent.",
              		import_filename);				
			}
			
			file_contents = preprocess_source(file_contents, import_filename);
			
			// __filename\n at the start of the file
			// __-\n at the end of the file
			size_t overhead_size = 8 + import_filename_size + 1 + 11;
			
            char* new_src_ptr = malloc(len + imported_size + overhead_size - skip_size);
            
            size_t n_read = tok - src + 1;
            
			char* cpy_ptr = new_src_ptr;
			
            memcpy(cpy_ptr, src, n_read - 1);
			cpy_ptr += n_read - 1;
			
			memcpy(cpy_ptr, "%__file ", 8);
			cpy_ptr += 8;
			memcpy(cpy_ptr, import_filename, import_filename_size);
			cpy_ptr += import_filename_size;
			*(cpy_ptr++) = '\n';
			
            memcpy(cpy_ptr, file_contents, imported_size);
			cpy_ptr += imported_size;
			
			memcpy(cpy_ptr, "\n%__file -\n", 11);
			cpy_ptr += 11;
			
            memcpy(cpy_ptr, src + n_read + skip_size, len - n_read);
            
            free(file_contents);
            free(src);
            
            len += imported_size;
            tok = new_src_ptr + n_read;
            src = new_src_ptr;
            
            continue;
        }        
    }
	
    return src;
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

    uint16_t pc = 0x200;

    _Bool jp_start = false;
    if ((argc == 3 && !strcmp(argv[2], "--start-as-entry")) ||
        (argc == 4 && !strcmp(argv[3], "--start-as-entry")))
    {
        jp_start = true;
        pc += 2;
        append_instr(&rom, 0);
    }

    
    char* pgm = read_file(argv[1], NULL);
	
	if(pgm == NULL){
		printf(BRED "FATAL ERROR:" RES " Input file not found.");
		return 1;
	}
	
    pgm = preprocess_source(pgm, argv[1]);

	#ifdef DEBUG
	printf("Post-process source:\n\n%s\n\n", pgm);
	#endif
	
    compile_source(pgm, argv[1], &rom, &labels, &pc);

	free(pgm);

    if (jp_start)
    {
        instruction_t instr;
        instr[0] = (struct __word){.name = "jp", .p = 2};
        instr[1] = (struct __word){.name = "@_start", .p = 7};
        instr[2].p = 0;
        uint16_t jp =
            0x1000 + fetch_address(0, argv[1], instr + 1, instr, &labels);
        rom.rom[0] = (jp & 0xFF00) >> 8;
        rom.rom[1] = jp & 0x00FF;
    }

    FILE *fp;
    if (argc < 3 || (argc == 3 && jp_start))
        fp = fopen("output.ch8", "wb");
    else if (argc == 4 && !strcmp(argv[2], "--start-as-entry"))
        fp = fopen(argv[3], "wb");
    else
        fp = fopen(argv[2], "wb");

    fwrite(rom.rom, 1, rom.p, fp);
    fclose(fp);

    return 0;
}