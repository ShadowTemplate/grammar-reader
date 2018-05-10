#include <stdio.h>
#include <ctype.h> //Per isupper e islower
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100
#define MAX_PRODUCTIONS 100
#pragma warning (disable: 4996) //Direttive per offuscamento di warnings di VisualStudio
#pragma warning (disable: 4995) //Direttive per offuscamento di warnings di VisualStudio

// Definizione dei tipi ------------------------------------------------------

typedef char Symbol;

typedef struct
{
        Symbol word [ MAX_WORD_LENGTH ];
        int length;
} Word;

typedef struct
{
        Word left;
        Word right;
} Production;

typedef struct
{
        Production productions[MAX_PRODUCTIONS];
        int numprod;
} Grammar;


//Prototipi
int is_terminal ( Symbol );
int is_nonterminal ( Symbol );
int is_prodsym ( Symbol );
int is_prodsep ( Symbol );
Symbol read_sym ( FILE* );
Production* add_new_production ( Grammar* );
void add_symbol ( Word*, Symbol );
int there_is_S_symbol ( Grammar* );
int there_is_nonterminal ( Word* );
Grammar* load_grammar ( FILE*, Grammar* );

void print_sym ( Symbol );
void print_word ( Word* );
void print_production ( Production* );
void print_grammar ( Grammar* );

int is_CF ( Grammar* );
int is_CS ( Grammar* );
int is_MN ( Grammar* );
int is_RL ( Grammar* );

void write_grammar ( FILE*, Grammar* );
void write_productions ( Grammar*, FILE* );
void delete_productions ( Grammar* );
void convert_MN_to_CS ( Grammar*, Grammar* );
void union_grammar ( Grammar*, Grammar*, Grammar* );
void concatenation_grammar ( Grammar*, Grammar*, Grammar* );
void iteration_grammar ( Grammar*, Grammar* );
