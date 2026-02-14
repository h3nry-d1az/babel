/**
 * @author Kyryl Shyshko
 * @version 0.1.0
 * @note To compile a Forth program, run `./forthc file.f; gcc output.s output`.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __linked_list_node_struct linked_list_node_t;
typedef struct __linked_list_struct linked_list_t;

struct __linked_list_node_struct
{
    void *data;
    struct __linked_list_node_struct *next_node;
};

struct __linked_list_struct
{
    struct __linked_list_node_struct *start_node;
    size_t size;
    void (*data_free_func)(void *);
};

#define llist_create linked_list_create
#define llist_free linked_list_free
#define llist_append linked_list_append
#define llist_prepend linked_list_prepend
#define llist_get_at linked_list_get_at
#define llist_delete_at linked_list_delete_at
#define llist_foreach linked_list_foreach

linked_list_t *linked_list_create()
{
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));

    list->start_node = NULL;
    list->size = 0;
    list->data_free_func = NULL;

    return list;
}

void linked_list_free(linked_list_t *list)
{
    linked_list_node_t *curr_node = list->start_node;
    linked_list_node_t *next_node = 0;
    while ((list->size--) > 0)
    {
        next_node = curr_node->next_node;
        if (list->data_free_func != NULL)
            list->data_free_func(curr_node->data);

        free(curr_node);
        curr_node = next_node;
    }
    free(list);
}

uint8_t linked_list_append(linked_list_t *list, void *data)
{
    linked_list_node_t *new_node = malloc(sizeof(linked_list_node_t));

    if (new_node == NULL)
        return 1;

    new_node->next_node = NULL;
    new_node->data = data;

    if (list->size == 0)
    {
        list->start_node = new_node;
        list->size = 1;
        return 0;
    }

    linked_list_node_t *curr_node = list->start_node;
    size_t size_left = list->size;
    while ((size_left--) > 1)
    {
        curr_node = curr_node->next_node;
    }

    curr_node->next_node = new_node;

    list->size++;

    return 0;
};

uint8_t linked_list_prepend(linked_list_t *list, void *data)
{
    linked_list_node_t *new_node = malloc(sizeof(linked_list_node_t));
    if (new_node == NULL)
        return 1;

    new_node->next_node = list->start_node;
    new_node->data = data;

    list->start_node = new_node;

    list->size++;

    return 0;
}

void *linked_list_get_at(linked_list_t *list, size_t index)
{
    if (index >= list->size)
        return NULL;

    size_t curr_index = 0;
    linked_list_node_t *curr_node = list->start_node;

    while (curr_index != index)
    {
        curr_node = curr_node->next_node;
        curr_index += 1;
    }

    return curr_node->data;
}

uint8_t linked_list_delete_at(linked_list_t *list, size_t index)
{
    if (index >= list->size)
        return 1;

    if (list->size == 1)
    {
        if (list->data_free_func != NULL)
            list->data_free_func(list->start_node->data);
        free(list->start_node);
        list->start_node = NULL;
        list->size = 0;
    }

    size_t curr_index = 1;
    linked_list_node_t *prev_node = list->start_node;
    linked_list_node_t *curr_node = prev_node->next_node;

    while (curr_index != index)
    {
        prev_node = curr_node;
        curr_node = prev_node->next_node;
        curr_index += 1;
    }

    prev_node->next_node = curr_node->next_node;

    list->size--;

    if (list->data_free_func != NULL)
        list->data_free_func(curr_node->data);

    free(curr_node);

    return 0;
}

void linked_list_foreach(linked_list_t *list, void (*data_func)(void *, void *),
                         void *func_arg)
{
    size_t size_left = list->size;
    linked_list_node_t *curr_node = list->start_node;

    while ((size_left--) > 0)
    {
        data_func(curr_node->data, func_arg);
        curr_node = curr_node->next_node;
    }

    return;
}

struct __forth_word_struct
{
    char *name;
    char *source;
};

struct __forth_word_struct FORTH_WORDS_BUILTIN[] = {
    {.name = "+", .source = "fpop rax\nfpop rbx\nadd rax, rbx\nfpush rax\n"},
    {.name = "-", .source = "fpop rax\nfpop rbx\nsub rbx, rax\nfpush rbx\n"},
    {.name = "dup", .source = "fpull rax\nfpush rax\n"},
    {.name = "emit",
     .source = "fpop rax \n"
               "lea rdx, [rip + buffer] \n"
               "mov [rdx], al \n"
               "mov rax, 0 \n"
               "lea rdx, [rip + buffer + 1] \n"
               "mov [rdx + 1], al \n"
               "\n"
               "mov rax, 1 \n"
               "mov rdi, 1 \n"
               "lea rsi, [rip + buffer] \n"
               "mov rdx, 1 \n"
               "syscall \n"}

};

char FORTH_PROGRAM_START_CODE[] = ".intel_syntax noprefix\n\n"
                                  ".section .data\n"
                                  "buffer: .zero 200\n"
                                  ".section .text\n"
                                  ".set FORTH_STACK_REGISTER, r12\n"
                                  ".set FORTH_STACK_REGISTER_ORIGINAL, r13\n"
                                  ".macro fpush reg \n"
                                  "	mov [FORTH_STACK_REGISTER], \\reg \n"
                                  "	add FORTH_STACK_REGISTER, 8 \n"
                                  ".endm \n"
                                  "\n"
                                  ".macro fpop reg \n"
                                  "	sub FORTH_STACK_REGISTER, 8 \n"
                                  "	mov \\reg, [FORTH_STACK_REGISTER] \n"
                                  ".endm \n"
                                  ".macro fpull reg \n"
                                  "	mov \\reg, [FORTH_STACK_REGISTER - 8] \n"
                                  ".endm \n"
                                  " \n";

char FORTH_RUNTIME_START_CODE[] =
    "\n\n\n.global main \n"
    "main: \n"
    "movq rdi, 2048\n"
    "call malloc \n"
    "testq rax, rax \n"

    "jne malloc_successful \n"
    "// Handle unsuccesful malloc \n"
    "movq rdi, 1 \n"
    "call exit \n"

    "malloc_successful: \n"
    "movq FORTH_STACK_REGISTER_ORIGINAL, rax\n"
    "movq FORTH_STACK_REGISTER, FORTH_STACK_REGISTER_ORIGINAL \n"
    "// End of init\n\n";

char FORHT_RUNTIME_END_CODE[] = "\n\n//Begin exit\n"
                                "movq rdi, FORTH_STACK_REGISTER_ORIGINAL \n"
                                "call free \n"

                                "movq rdi, 0 \n"
                                "call exit\n";

char *get_source(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Provide a filename\n");
        return NULL;
    }

    FILE *f_ptr = fopen(argv[1], "r");

    if (f_ptr == NULL)
    {
        fclose(f_ptr);
        printf("File not found\n");
        return NULL;
    }

    fseek(f_ptr, 0L, SEEK_END);
    size_t filesize = ftell(f_ptr);
    fseek(f_ptr, 0L, SEEK_SET);

    char *src = (char *)malloc(filesize + 1);

    fread(src, 1, filesize, f_ptr);

    src[filesize] = '\0';

    fclose(f_ptr);

    return src;
}

void return_output(int /*argc*/, char ** /*argv*/, char *out)
{
    FILE *f_ptr = fopen("output.s", "w");
    fprintf(f_ptr, out);
    fclose(f_ptr);
}

typedef struct __string_struct
{
    char *data;
    size_t len;
} string_t;

void print_str(string_t str)
{

    char *buf = malloc(str.len + 1);
    memcpy(buf, str.data, str.len);

    buf[str.len] = '\0';

    printf(buf);

    printf("\n");

    free(buf);

    return;
}

_Bool string_is(string_t str1, char *str2)
{
    for (size_t i = str1.len; i > 0; i--)
    {
        if (!str2[i - 1])
            return 0;
        if (str2[i - 1] != str1.data[i - 1])
            return 0;
    }
    return 1;
}

string_t *string_copy(string_t *str)
{
    string_t *out = malloc(sizeof(string_t));
    out->len = str->len;
    out->data = malloc(out->len);
    memcpy(out->data, str->data, out->len);
    return out;
}

string_t *string_create(char *str)
{
    string_t *out = malloc(sizeof(string_t));
    out->len = strlen(str);
    out->data = malloc(out->len);
    memcpy(out->data, str, out->len);
    return out;
}

string_t *string_lowercase(string_t *str)
{
    char ch;

    for (size_t i = 0; i < str->len; i++)
    {
        ch = str->data[i];
        if (ch >= 'A' && ch <= 'Z')
        {
            ch += 'a' - 'A';
            str->data[i] = ch;
        }
    }

    return str;
}

char string_is_digit(string_t *str)
{
    char uint8_t;
    for (size_t i = 0; i < str->len; i++)
    {
        uint8_t = str->data[i];
        if (uint8_t < '0' || uint8_t > '9')
            return 0;
    }
    return 1;
}

void string_free(string_t *str)
{
    free(str->data);
    free(str);
}

string_t *strings_join(string_t *s_arr[])
{
    string_t **ptr = s_arr;
    size_t total_size = 0;

    while (*ptr)
    {
        total_size += (*ptr)->len;
        ptr++;
    }

    string_t *out = malloc(sizeof(string_t));
    out->len = total_size;
    out->data = malloc(out->len);

    char *s_ptr = out->data;
    ptr = s_arr;

    while (*ptr)
    {
        memcpy(s_ptr, (*ptr)->data, (*ptr)->len);
        s_ptr += (*ptr)->len;
        ptr++;
    }

    return out;
}

enum __ast_node_type_enum;
union __ast_node_data_union;
struct __ast_node_struct;

typedef enum __ast_node_type_enum ast_node_type_t;
typedef union __ast_node_data_union ast_node_data_t;
typedef struct __ast_node_struct ast_node_t;

enum __ast_node_type_enum
{
    AST_TYPE_NODE_LIST,
    AST_TYPE_CALL_WORD,
    AST_TYPE_WORD_DEF,
    AST_TYPE_INLINE_ASM,
    AST_TYPE_IF
};

union __ast_node_data_union {
    linked_list_t *ast_node_list;
    string_t *word;
    struct __ast_node_word_def_data
    {
        string_t *word_name;
        ast_node_t *ast_def;
    } *word_def;
    string_t *inline_asm;
    ast_node_t *if_inside_ast;
};

struct __ast_node_struct
{
    ast_node_type_t type;
    ast_node_data_t data;
};

string_t get_next_word(char **pgm)
{

    string_t word = {.data = NULL, .len = 0};

    for (; **pgm; (*pgm)++)
    {
        if (**pgm == '\t' || **pgm == ' ' || **pgm == '\n')
        {
            if (word.len == 0)
                continue;
            break;
        }
        if (**pgm == '(')
        {
            while (**pgm != ')')
                (*pgm)++;
            if (word.len > 0)
            {
                (*pgm)++;
                break;
            }
            continue;
        }

        if (word.len == 0)
            word.data = *pgm;

        word.len += 1;
    }

    string_lowercase(&word);

    return word;
}

// Converts uint8_t string to hex string ("hash")
string_t *get_word_name_hash(string_t *word_name)
{
    char hexmap[17] = "0123456789ABCDEF";

    string_t *result = malloc(sizeof(string_t));
    result->len = word_name->len * 2;
    result->data = malloc(result->len);

    char *ptr = result->data;

    for (size_t uint8_t_i = 0; uint8_t_i < word_name->len; uint8_t_i++)
    {
        char uint8_t = word_name->data[uint8_t_i];
        *ptr = hexmap[uint8_t >> 4];
        ptr += 1;
        *ptr = hexmap[uint8_t & 0b1111];
        ptr += 1;
    }

    return result;
}

string_t *get_uid_hash(size_t uid)
{
    char hexmap[17] = "0123456789ABCDEF";

    string_t *result = malloc(sizeof(string_t));
    result->len = sizeof(uid) * 2;
    result->data = malloc(result->len);

    char *ptr = result->data;
    char *data_ptr = (char *)&uid;

    for (size_t uint8_t_i = 0; uint8_t_i < 8; uint8_t_i++)
    {
        char uint8_t = data_ptr[uint8_t_i];
        *ptr = hexmap[uint8_t >> 4];
        ptr += 1;
        *ptr = hexmap[uint8_t & 0b1111];
        ptr += 1;
    }

    return result;
}

ast_node_t *llist_to_ast(linked_list_t *llist)
{
    if (llist->size == 1)
    {
        return (ast_node_t *)llist_get_at(llist, 0);
    }

    ast_node_t *out = malloc(sizeof(ast_node_t));

    out->type = AST_TYPE_NODE_LIST;
    out->data.ast_node_list = llist;

    return out;
}

ast_node_t *get_next_ast_node(char **pgm)
{

    string_t word = get_next_word(pgm);

    if (word.len == 0)
        return NULL;

    ast_node_t *output_node = malloc(sizeof(ast_node_t));

    if (string_is(word, ":"))
    {
        string_t word_def_name = get_next_word(pgm);

        char *pgm2 = *pgm;

        linked_list_t *ast_def = llist_create();

        while (*pgm2)
        {
            if (string_is(get_next_word(&pgm2), ";"))
            {
                *pgm = pgm2;
                break;
            }

            ast_node_t *node = get_next_ast_node(pgm);

            llist_append(ast_def, (void *)node);

            pgm2 = *pgm;
        }

        output_node->type = AST_TYPE_WORD_DEF;

        output_node->data.word_def =
            malloc(sizeof(struct __ast_node_word_def_data));

        output_node->data.word_def->word_name =
            string_lowercase(string_copy(&word_def_name));
        output_node->data.word_def->ast_def = llist_to_ast(ast_def);

        return output_node;
    }

    if (string_is(word, "if"))
    {
        // size_t lvl = 1;

        linked_list_t *inside_ast = llist_create();

        char *pgm2 = *pgm;

        while (**pgm)
        {
            word = get_next_word(&pgm2);

            if (string_is(word, "then"))
            {
                *pgm = pgm2;
                break;
            }

            llist_append(inside_ast, get_next_ast_node(pgm));

            pgm2 = *pgm;
        }

        output_node->type = AST_TYPE_IF;
        output_node->data.if_inside_ast = llist_to_ast(inside_ast);

        return output_node;
    }

    // Call word

    output_node->type = AST_TYPE_CALL_WORD;
    output_node->data.word = string_lowercase(string_copy(&word));

    return output_node;
}

typedef struct __compilation_data_struct
{
    size_t word_uid;
} compilation_data_t;

void get_ast_node_asm_code(ast_node_t *node, linked_list_t *out,
                           compilation_data_t *comp_data)
{
    switch (node->type)
    {
    case AST_TYPE_CALL_WORD:
        llist_append(out, string_create("call WORD_"));
        llist_append(out, get_word_name_hash(node->data.word));
        llist_append(out, string_create("\n"));
        break;
    case AST_TYPE_WORD_DEF:
        llist_append(out, string_create("WORD_"));
        llist_append(out, get_word_name_hash(node->data.word_def->word_name));
        llist_append(out, string_create(":\n"));
        get_ast_node_asm_code(node->data.word_def->ast_def, out, comp_data);
        llist_append(out, string_create("ret\n"));
        break;
    case AST_TYPE_NODE_LIST:
        linked_list_node_t *llist_node = node->data.ast_node_list->start_node;
        while (llist_node)
        {
            get_ast_node_asm_code(llist_node->data, out, comp_data);
            llist_node = llist_node->next_node;
        }
        break;
    case AST_TYPE_INLINE_ASM:
        llist_append(out, node->data.inline_asm);
        break;
    case AST_TYPE_IF:
        size_t if_uid = comp_data->word_uid;
        comp_data->word_uid++;
        llist_append(out, string_create("fpop rdx\ncmp rdx, 0\nje IF_skip_"));
        llist_append(out, get_uid_hash(if_uid));
        llist_append(out, string_create("\n"));
        get_ast_node_asm_code(node->data.if_inside_ast, out, comp_data);
        llist_append(out, string_create("IF_skip_"));
        llist_append(out, get_uid_hash(if_uid));
        llist_append(out, string_create(":\n"));

        break;
    }
}

void preprocess_ast(ast_node_t *ast_node)
{

    // while(llist_node){
    // 	ast_node_t* node = llist_node->data;
    switch (ast_node->type)
    {
    case AST_TYPE_CALL_WORD:
        if (string_is_digit(ast_node->data.word))
        {
            ast_node->type = AST_TYPE_INLINE_ASM;
            string_t *s1 = string_create("movq rax, ");
            string_t *s2 = string_create("\nfpush rax\n");
            string_t *strs[] = {s1, ast_node->data.word, s2, NULL};
            string_t *final = strings_join(strs);
            string_free(s1);
            string_free(s2);
            ast_node->data.inline_asm = final;
        }
        break;
    case AST_TYPE_WORD_DEF:
        preprocess_ast(ast_node->data.word_def->ast_def);
        break;
    case AST_TYPE_NODE_LIST:
        linked_list_node_t *llist_node =
            ast_node->data.ast_node_list->start_node;
        while (llist_node)
        {
            preprocess_ast(llist_node->data);
            llist_node = llist_node->next_node;
        }

        break;
    case AST_TYPE_INLINE_ASM:
        break;
    case AST_TYPE_IF:
        preprocess_ast(ast_node->data.if_inside_ast);
        break;
    }

    // 	llist_node = llist_node->next_node;
    // }
}

#define TABS(n)                                                                \
    for (size_t i = 0; i < n; i++)                                             \
    printf("\t")

void print_ast_node(ast_node_t *node, size_t indent)
{

    switch (node->type)
    {
    case AST_TYPE_CALL_WORD:
        TABS(indent);
        printf("Type: Call Word\n");
        TABS(indent);
        printf("Word: ");
        print_str(*(node->data.word));
        break;
    case AST_TYPE_WORD_DEF:
        TABS(indent);
        printf("Type: Def Word\n");
        TABS(indent);
        printf("Word: ");
        print_str(*(node->data.word_def->word_name));
        printf("Def:\n");
        print_ast_node(node->data.word_def->ast_def, indent + 1);
        break;
    case AST_TYPE_NODE_LIST:
        TABS(indent);
        printf("Type: Node List\n");
        TABS(indent);
        printf("List:\n");
        linked_list_node_t *llist_node = node->data.ast_node_list->start_node;
        while (llist_node)
        {
            print_ast_node(llist_node->data, indent + 1);
            llist_node = llist_node->next_node;
        }
        break;
    case AST_TYPE_INLINE_ASM:
        TABS(indent);
        printf("Type: Inline ASM\n");
        TABS(indent);
        printf("Assembly size: %lld\n", node->data.inline_asm->len);
        break;
    case AST_TYPE_IF:
        TABS(indent);
        printf("Type: IF\n");
        TABS(indent);
        printf("Inside AST:\n");
        print_ast_node(node->data.if_inside_ast, indent + 1);
        break;
    }

    printf("\n");
}

linked_list_t *get_builtin_forth_word_def()
{
    size_t n_words =
        sizeof(FORTH_WORDS_BUILTIN) / sizeof(FORTH_WORDS_BUILTIN[0]);

    linked_list_t *word_defs = llist_create();

    for (size_t i = 0; i < n_words; i++)
    {
        struct __forth_word_struct word = FORTH_WORDS_BUILTIN[i];

        ast_node_t *def_node = malloc(sizeof(ast_node_t));

        def_node->type = AST_TYPE_WORD_DEF;
        def_node->data.word_def =
            malloc(sizeof(struct __ast_node_word_def_data));
        def_node->data.word_def->word_name = string_create(word.name);

        ast_node_t *asm_node = malloc(sizeof(ast_node_t));
        asm_node->type = AST_TYPE_INLINE_ASM;
        asm_node->data.inline_asm = string_create(word.source);

        def_node->data.word_def->ast_def = asm_node;

        llist_append(word_defs, def_node);
    }

    return word_defs;
}

int main(int argc, char **argv)
{

    char *original_src_ptr = get_source(argc, argv);

    char *src = original_src_ptr;

    if (src == NULL)
    {
        return 1;
    }

    char **pgm = &src;

    linked_list_t *llist = get_builtin_forth_word_def();
    // linked_list_t *word_defs = llist_create();

    while (**pgm)
    {
        ast_node_t *node = get_next_ast_node(pgm);

        if (node == NULL)
            break;

        preprocess_ast(node);

        llist_append(llist, node);
    }

    // preprocess_ast(llist);

    linked_list_node_t *list_node = llist->start_node;

    while (list_node)
    {
        ast_node_t *node = list_node->data;
        print_ast_node(node, 0);
        list_node = list_node->next_node;
    }

    ast_node_t *init_runtime_node = malloc(sizeof(ast_node_t));
    init_runtime_node->type = AST_TYPE_INLINE_ASM;
    init_runtime_node->data.inline_asm =
        string_create(FORTH_RUNTIME_START_CODE);
    llist_prepend(llist, init_runtime_node);

    ast_node_t *end_runtime_node = malloc(sizeof(ast_node_t));
    end_runtime_node->type = AST_TYPE_INLINE_ASM;
    end_runtime_node->data.inline_asm = string_create(FORHT_RUNTIME_END_CODE);
    llist_append(llist, end_runtime_node);

    list_node = llist->start_node;
    linked_list_t *word_defs_src_code = llist_create();
    linked_list_t *runtime_src_code = llist_create();

    compilation_data_t comp_data = {.word_uid = 1};
    compilation_data_t *comp_data_ptr = &comp_data;

    while (list_node)
    {
        ast_node_t *node = list_node->data;
        switch (node->type)
        {
        case AST_TYPE_WORD_DEF:
            get_ast_node_asm_code(node, word_defs_src_code, comp_data_ptr);
            break;
        default:
            get_ast_node_asm_code(node, runtime_src_code, comp_data_ptr);
        }
        list_node = list_node->next_node;
    }

    linked_list_t *src_code_list = llist_create();

    list_node = word_defs_src_code->start_node;
    while (list_node)
    {
        llist_append(src_code_list, list_node->data);
        list_node = list_node->next_node;
    }

    list_node = runtime_src_code->start_node;
    while (list_node)
    {
        llist_append(src_code_list, list_node->data);
        list_node = list_node->next_node;
    }

    llist_prepend(src_code_list, string_create(FORTH_PROGRAM_START_CODE));

    size_t src_code_len = 0;

    list_node = src_code_list->start_node;

    while (list_node)
    {
        src_code_len += ((string_t *)list_node->data)->len;
        list_node = list_node->next_node;
    }

    char *src_code = malloc(src_code_len + 1);
    size_t i = 0;

    list_node = src_code_list->start_node;

    while (list_node)
    {
        string_t *str = list_node->data;
        memcpy(src_code + i, str->data, str->len);
        i += str->len;
        list_node = list_node->next_node;
    }

    src_code[i] = '\0';

    printf(src_code);

    return_output(argc, argv, src_code);

    free(src_code);

    free(original_src_ptr);
}
