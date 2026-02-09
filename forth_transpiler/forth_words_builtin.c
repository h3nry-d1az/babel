// Necessary strings for the Forth compiler:
// Built-in words, and initialization / end procedures

struct __forth_word_struct {
	char* name;
	char* source;
};


struct __forth_word_struct FORTH_WORDS_BUILTIN[] = {
	
	// {.name = "test", .source = "test instr\n"},
	
	{
		.name = "+",
		.source = "fpop rax\nfpop rbx\nadd rax, rbx\nfpush rax\n"
	},
	{
		.name = "-",
		.source = "fpop rax\nfpop rbx\nsub rbx, rax\nfpush rbx\n"
	},
	{
		.name = "dup",
		.source = "fpull rax\nfpush rax\n"
	},
	{
		.name = "emit",
		.source = 
			"fpop rax \n"
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
			"syscall \n"
	}

};


char FORTH_PROGRAM_START_CODE[] =
".intel_syntax noprefix\n\n"
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
" \n"
;


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
"// End of init\n\n"
;

char FORHT_RUNTIME_END_CODE[] =
"\n\n//Begin exit\n"
"movq rdi, FORTH_STACK_REGISTER_ORIGINAL \n"
"call free \n"

"movq rdi, 0 \n"
"call exit\n"
;

