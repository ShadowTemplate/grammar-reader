#include "Header.h"

/* Funzioni di stampa a video ------------------------------------*/

void print_sym (Symbol s)
{
     printf("%c ",s);
}

void print_word (Word* w)
{
     int i;
     
     for (i=0; i<w->length; i++)
         print_sym(w->word[i]);
}

void print_production (Production* p) 
{
     print_word(&p->left);
     printf (" --> ");
     print_word(&p->right);
}

void print_grammar(Grammar* g)
{
     int i;
     
	 if (g->numprod == 0)
        printf ("Errore! Grammatica non valida!\n\n");
     else
     {
        printf ("Numero di produzioni: %d\n\n", g->numprod);
        for (i=0; i<g->numprod; i++)
        {
            printf("%d)  ", i+1); //Inserito il numero della produzione sulla sinistra
			print_production(&g->productions[i]);
            printf ("\n");
        }
		printf ("\n");
     }
}
